const zigbeeHerdsmanConverters = require('zigbee-herdsman-converters');
const zigbeeHerdsmanUtils = require('zigbee-herdsman-converters/lib/utils');


const exposes = zigbeeHerdsmanConverters['exposes'] || require("zigbee-herdsman-converters/lib/exposes");
const ea = exposes.access;
const e = exposes.presets;
const fz = zigbeeHerdsmanConverters.fromZigbeeConverters || zigbeeHerdsmanConverters.fromZigbee;
const tz = zigbeeHerdsmanConverters.toZigbeeConverters || zigbeeHerdsmanConverters.toZigbee;
const utils = require('zigbee-herdsman-converters/lib/utils');

const ZCL_DATATYPE_UINT16 = 0x21;
const ZCL_DATATYPE_BOOLEAN = 0x10;

const reporting = require('zigbee-herdsman-converters/lib/reporting');
const {getEndpointName} = require('zigbee-herdsman-converters/lib/utils');

function EndpointByKey(key) {
    let endpoint = 0;
    switch (key) {
        case 'relay_1': 
        case 'relay_1_duration': 
            endpoint = 1;
            break;
        case 'relay_2': 
        case 'relay_2_duration': 
            endpoint = 2;
        break;
        case 'relay_3': 
        case 'relay_3_duration': 
            endpoint = 3;
        break;
        case 'relay_4': 
        case 'relay_4_duration': 
            endpoint = 4;
        break;
        case 'beeper': 
            endpoint = 5;
        break;
        default: 
            break;
    }
    return endpoint;
}

const fz_local = {
    relay_on_off: {
        cluster: 'genOnOff',
        type: ['attributeReport', 'readResponse'],
        convert: (model, msg, publish, options, meta) => {
            const result = {};
            const endpoint = msg.endpoint.ID;
            let property = '';
            switch (endpoint) {
                case 1: 
                    property = 'relay_1';
                    break;
                case 2: 
                    property = 'relay_2';
                    break;
                case 3: 
                    property = 'relay_3';
                    break;
                case 4: 
                    property = 'relay_4';
                    break;
                case 5: 
                    property = 'beeper';
                    break;
                default: 
                    break;
            }
            const state = msg.data['onOff'] === 1 ? 'ON' : 'OFF';
            result[property] = state;
            return result;
        },
    },
    relay_config: {
        cluster: 'genOnOff',
        type: ['readResponse'],
        convert: (model, msg, publish, options, meta) => {
            const result = {};
            const endpoint = msg.endpoint.ID;
            let property = '';
            switch (endpoint) {
                case 1 : 
                    property = 'relay_1_duration';
                    break;
                case 2: 
                    property = 'relay_2_duration';
                    break;
                case 3: 
                    property = 'relay_3_duration';
                    break;
                case 4: 
                    property = 'relay_4_duration';
                    break;
                default: 
                    break;
            }
            result[property] = msg.data[0xF003];

            return result;
        },
    }

}

const tz_local = {
    relay_on_off:{
        key: ['relay_1', 'relay_2', 'relay_3', 'relay_4', 'beeper'],
        convertSet: async (entity, key, value, meta) => {
            const state = utils.isString(meta.message[key]) ? meta.message[key].toLowerCase() : null;
            utils.validateValue(state, ['toggle', 'off', 'on']);

            await meta.device.getEndpoint(EndpointByKey(key)).command('genOnOff', state, {}, utils.getOptions(meta.mapped, entity));
        },
        convertGet: async (entity, key, meta) => {
            await meta.device.getEndpoint(EndpointByKey(key)).read('genOnOff', ['onOff']);
        },
    },
    relay_config: {
        key: ['relay_1_duration', 'relay_2_duration', 'relay_3_duration', 'relay_4_duration'],
        convertSet: async (entity, key, rawValue, meta) => {
            if (key === 'beeper_on_leak') {
                const lookup = {'OFF': 0x00, 'ON': 0x01};
                const value = utils.getFromLookup(rawValue, lookup, Number(rawValue));
                await meta.device.getEndpoint(1).write('genOnOff', {0xF005: {value, type: ZCL_DATATYPE_BOOLEAN}});
                return { 
                    state: {[key]: rawValue}
                };
                }
            else {
                const value = rawValue;
                const endpoint = EndpointByKey(key);
                await meta.device.getEndpoint(endpoint).write('genOnOff', {0xF003: {value, type: ZCL_DATATYPE_UINT16}});
                return { 
                    state: {[key]: value}
                };
                }
        },
        convertGet: async (entity, key, meta) => {
            const endpoint = meta.device.getEndpoint(EndpointByKey(key));
            switch (key) {
            case 'beeper_on_leak': 
                await endpoint.read('genOnOff', [0xF005]);
            break;
            case 'relay_1_duration': 
            case 'relay_2_duration': 
            case 'relay_3_duration': 
            case 'relay_4_duration': 
                await endpoint.read('genOnOff', [0xF003]);
                break;
            default:
                break;
            }
        },
    },
}


const device = {
    zigbeeModel: ['Relay.4Ch.Timer'],
    model: 'Relay.4Ch.Timer',
    vendor: 'Bacchus',
    description: 'Fourth channel relay with guaranted shutdown',
    fromZigbee: [fz.ignore_basic_report, fz_local.relay_on_off, fz_local.relay_config],
    toZigbee: [tz_local.relay_on_off, tz_local.relay_config],

	configure: async (device, coordinatorEndpoint) => {
		const firstEndpoint    = device.getEndpoint(1);
		const secondEndpoint   = device.getEndpoint(2);
		const thirdEndpoint    = device.getEndpoint(3);
		const fourthEndpoint   = device.getEndpoint(4);
		const fifthEndpoint    = device.getEndpoint(5);

        const overrides = { min: 0, max: 3600, change: 0};

        await reporting.bind(firstEndpoint, coordinatorEndpoint, ['genOnOff']);
        await reporting.onOff(firstEndpoint, overrides);

        await reporting.bind(secondEndpoint, coordinatorEndpoint, ['genOnOff']);
        await reporting.onOff(secondEndpoint, overrides);

        await reporting.bind(thirdEndpoint, coordinatorEndpoint, ['genOnOff']);
        await reporting.onOff(thirdEndpoint, overrides);

        await reporting.bind(fourthEndpoint, coordinatorEndpoint, ['genOnOff']);
        await reporting.onOff(fourthEndpoint, overrides);

        await reporting.bind(fifthEndpoint, coordinatorEndpoint, ['genOnOff']);
        await reporting.onOff(fifthEndpoint, overrides);

        await firstEndpoint.read('genOnOff', ['onOff']);
        await firstEndpoint.read('genOnOff', [0xF003]);

        await secondEndpoint.read('genOnOff', ['onOff']);
        await secondEndpoint.read('genOnOff', [0xF003]);

        await thirdEndpoint.read('genOnOff', ['onOff']);
        await thirdEndpoint.read('genOnOff', [0xF003]);

        await fourthEndpoint.read('genOnOff', ['onOff']);
        await fourthEndpoint.read('genOnOff', [0xF003]);

        await fifthEndpoint.read('genOnOff', ['onOff']);

    },
    exposes: [
        e.binary('relay_1', ea.ALL, 'ON', 'OFF').withDescription('Enable first pump'),
        e.numeric('relay_1_duration', ea.ALL).withUnit('min').withDescription('Duration of relay 1').withValueMin(1).withValueMax(180),
        e.binary('relay_2', ea.ALL, 'ON', 'OFF').withDescription('Enable second pump'),
        e.numeric('relay_2_duration', ea.ALL).withUnit('min').withDescription('Duration of relay 2').withValueMin(1).withValueMax(180),
        e.binary('relay_3', ea.ALL, 'ON', 'OFF').withDescription('Enable third pump'),
        e.numeric('relay_3_duration', ea.ALL).withUnit('min').withDescription('Duration of relay 3').withValueMin(1).withValueMax(180),
        e.binary('relay_4', ea.ALL, 'ON', 'OFF').withDescription('Enable all pumps'),
        e.numeric('relay_4_duration', ea.ALL).withUnit('min').withDescription('Duration of relay 4').withValueMin(1).withValueMax(180),
        e.binary('beeper', ea.ALL, 'ON', 'OFF').withDescription('Beeper'),
      ],
    icon: 'data:image/jpeg;base64,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',
};

module.exports = device;
