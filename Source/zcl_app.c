
#include "AF.h"
#include "OSAL.h"
#include "OSAL_Clock.h"
#include "OSAL_PwrMgr.h"
#include "ZComDef.h"
#include "ZDApp.h"
#include "ZDObject.h"
#include "math.h"

#include "nwk_util.h"
#include "zcl.h"
#include "zcl_app.h"
#include "zcl_diagnostic.h"
#include "zcl_general.h"
#include "zcl_ms.h"
#include "zcl_ss.h"

#include "bdb.h"
#include "bdb_interface.h"

#include "gp_interface.h"

#include "Debug.h"

#include "OnBoard.h"

#include "commissioning.h"
#include "factory_reset.h"
/* HAL */

#include "hal_adc.h"
#include "hal_drivers.h"
#include "hal_key.h"
#include "hal_led.h"

#include "utils.h"
#include "version.h"

#include <stdint.h>
#include <math.h>
/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */
byte zclApp_TaskID;

// Структура для отправки отчета
//afAddrType_t zclApp_DstAddr;
// Номер сообщения
uint8 SeqNum = 0;

/*********************************************************************
 * GLOBAL FUNCTIONS
 */
void user_delay_ms(uint32_t period);
void user_delay_ms(uint32_t period) { MicroWait(period * 1000); }
/*********************************************************************
 * LOCAL VARIABLES
 */

afAddrType_t inderect_DstAddr = {.addrMode = (afAddrMode_t)AddrNotPresent, .endPoint = 0, .addr.shortAddr = 0};

//uint8 zclApp_ZoneID;

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void zclApp_Report(void);
static void zclApp_ReadSensors(void);
static void zclApp_BasicResetCB(void);
static void zclApp_RestoreAttributesFromNV(void);
static void zclApp_SaveAttributesToNV(void);
static void zclApp_HandleKeys(byte portAndAction, byte keyCode);
static ZStatus_t zclApp_ReadWriteAuthCB(afAddrType_t *srcAddr, zclAttrRec_t *pAttr, uint8 oper);

//      
static void zclApp_ApplyRelay(uint8 relay, bool value);
static void zclApp_StopRelay(uint16 timer);

/*********************************************************************
 * ZCL General Profile Callback table
 */
static zclGeneral_AppCallbacks_t zclApp_CmdCallbacks = {
    zclApp_BasicResetCB, // Basic Cluster Reset command
    NULL,                // Identify Trigger Effect command
    zclApp_OnOffCB,      // On/Off cluster commands
    NULL,                // On/Off cluster enhanced command Off with Effect
    NULL,                // On/Off cluster enhanced command On with Recall Global Scene
    NULL,                // On/Off cluster enhanced command On with Timed Off
    NULL,                // RSSI Location command
    NULL                 // RSSI Location Response command
};

void zclApp_Init(byte task_id) {

    zclApp_RestoreAttributesFromNV();

    zclApp_TaskID = task_id;

    bdb_RegisterSimpleDescriptor(&zclApp_FirstRelayEP);
    zclGeneral_RegisterCmdCallbacks(zclApp_FirstRelayEP.EndPoint, &zclApp_CmdCallbacks);
    zcl_registerAttrList(zclApp_FirstRelayEP.EndPoint, zclApp_AttrsFirstRelayEPCount, zclApp_AttrsFirstRelayEP);
    zcl_registerReadWriteCB(zclApp_FirstRelayEP.EndPoint, NULL, zclApp_ReadWriteAuthCB);

    bdb_RegisterSimpleDescriptor(&zclApp_SecondRelayEP);
    zclGeneral_RegisterCmdCallbacks(zclApp_SecondRelayEP.EndPoint, &zclApp_CmdCallbacks);
    zcl_registerAttrList(zclApp_SecondRelayEP.EndPoint, zclApp_AttrsSecondRelayEPCount, zclApp_AttrsSecondRelayEP);
    zcl_registerReadWriteCB(zclApp_SecondRelayEP.EndPoint, NULL, zclApp_ReadWriteAuthCB);

    bdb_RegisterSimpleDescriptor(&zclApp_ThirdRelayEP);
    zclGeneral_RegisterCmdCallbacks(zclApp_ThirdRelayEP.EndPoint, &zclApp_CmdCallbacks);
    zcl_registerAttrList(zclApp_ThirdRelayEP.EndPoint, zclApp_AttrsThirdRelayEPCount, zclApp_AttrsThirdRelayEP);
    zcl_registerReadWriteCB(zclApp_ThirdRelayEP.EndPoint, NULL, zclApp_ReadWriteAuthCB);

    bdb_RegisterSimpleDescriptor(&zclApp_FourthRelayEP);
    zclGeneral_RegisterCmdCallbacks(zclApp_FourthRelayEP.EndPoint, &zclApp_CmdCallbacks);
    zcl_registerAttrList(zclApp_FourthRelayEP.EndPoint, zclApp_AttrsFourthRelayEPCount, zclApp_AttrsFourthRelayEP);
    zcl_registerReadWriteCB(zclApp_FourthRelayEP.EndPoint, NULL, zclApp_ReadWriteAuthCB);

    bdb_RegisterSimpleDescriptor(&zclApp_BeeperEP);
    zclGeneral_RegisterCmdCallbacks(zclApp_BeeperEP.EndPoint, &zclApp_CmdCallbacks);
    zcl_registerAttrList(zclApp_BeeperEP.EndPoint, zclApp_AttrsBeeperEPCount, zclApp_AttrsBeeperEP);
    zcl_registerReadWriteCB(zclApp_BeeperEP.EndPoint, NULL, zclApp_ReadWriteAuthCB);

    zcl_registerForMsg(zclApp_TaskID);
    RegisterForKeys(zclApp_TaskID);

    LREP("Build %s \r\n", zclApp_DateCodeNT);

    osal_start_reload_timer(zclApp_TaskID, APP_REPORT_EVT, APP_REPORT_DELAY);
    osal_start_reload_timer(zclApp_TaskID, HAL_KEY_EVENT, 100);
}

static void zclApp_HandleKeys(byte portAndAction, byte keyCode) 
{
  uint8 hal_key_sw;
  uint8 relay;

  LREP("keyCode =0x%x \r\n", keyCode);

  for(int i = 0; i <= RELAYS; i++ ){
    hal_key_sw = 1 << i;
    if (keyCode & hal_key_sw){
      relay = i;
      zclApp_Relays[relay] = !zclApp_Relays[relay];
      zclApp_ApplyRelay(relay, zclApp_Relays[relay]);
    }
  }

//    zclGeneral_SendOnOff_CmdOn(zclApp_WaterLeakEP.EndPoint, &inderect_DstAddr, FALSE, bdb_getZCLFrameCounter());
    
  zclApp_Report();  
  
}

uint16 zclApp_event_loop(uint8 task_id, uint16 events) {
//    LREP("events 0x%x \r\n", events);
    if (events & SYS_EVENT_MSG) {
        afIncomingMSGPacket_t *MSGpkt;
        while ((MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive(zclApp_TaskID))) {
//            LREP("MSGpkt->hdr.event 0x%X clusterId=0x%X\r\n", MSGpkt->hdr.event, MSGpkt->clusterId);
            switch (MSGpkt->hdr.event) {
            case KEY_CHANGE:
//                LREP("KEY_CHANGE\r\n");
                zclApp_HandleKeys(((keyChange_t *)MSGpkt)->state, ((keyChange_t *)MSGpkt)->keys);
                break;

            case ZCL_INCOMING_MSG:
                if (((zclIncomingMsg_t *)MSGpkt)->attrCmd) {
                    osal_mem_free(((zclIncomingMsg_t *)MSGpkt)->attrCmd);
                }
                break;

            default:
                break;
            }

            // Release the memory
            osal_msg_deallocate((uint8 *)MSGpkt);
        }
        // return unprocessed events
        return (events ^ SYS_EVENT_MSG);
    }
    if (events & APP_REPORT_EVT) {
        LREPMaster("APP_REPORT_EVT\r\n");
        zclApp_Report();
        return (events ^ APP_REPORT_EVT);
    }

    if (events & APP_SAVE_ATTRS_EVT) {
        LREPMaster("APP_SAVE_ATTRS_EVT\r\n");
        zclApp_SaveAttributesToNV();
        return (events ^ APP_SAVE_ATTRS_EVT);
    }
    if (events & APP_READ_SENSORS_EVT) {
        LREPMaster("APP_READ_SENSORS_EVT\r\n");
        zclApp_ReadSensors();
        return (events ^ APP_READ_SENSORS_EVT);
    }
    if (events & APP_STOP_RELAY_1_EVT) {
        LREPMaster("APP_READ_SENSORS_EVT\r\n");
        zclApp_StopRelay(APP_STOP_RELAY_1_EVT);
        return (events ^ APP_STOP_RELAY_1_EVT);
    }
    if (events & APP_STOP_RELAY_2_EVT) {
        LREPMaster("APP_READ_SENSORS_EVT\r\n");
        zclApp_StopRelay(APP_STOP_RELAY_2_EVT);
        return (events ^ APP_STOP_RELAY_2_EVT);
    }
    if (events & APP_STOP_RELAY_3_EVT) {
        LREPMaster("APP_READ_SENSORS_EVT\r\n");
        zclApp_StopRelay(APP_STOP_RELAY_3_EVT);
        return (events ^ APP_STOP_RELAY_3_EVT);
    }
    if (events & APP_STOP_RELAY_4_EVT) {
        LREPMaster("APP_READ_SENSORS_EVT\r\n");
        zclApp_StopRelay(APP_STOP_RELAY_4_EVT);
        return (events ^ APP_STOP_RELAY_4_EVT);
    }

    return 0;
}

static void zclApp_Report(void) {
  osal_start_reload_timer(zclApp_TaskID, APP_READ_SENSORS_EVT, 100); 
}

static void zclApp_BasicResetCB(void) {
    LREPMaster("BasicResetCB\r\n");
    zclApp_ResetAttributesToDefaultValues();
    zclApp_SaveAttributesToNV();
}

static ZStatus_t zclApp_ReadWriteAuthCB(afAddrType_t *srcAddr, zclAttrRec_t *pAttr, uint8 oper) {
    LREPMaster("AUTH CB called\r\n");
    osal_start_timerEx(zclApp_TaskID, APP_SAVE_ATTRS_EVT, 2000);
    return ZSuccess;
}

static void zclApp_SaveAttributesToNV(void) {
    uint8 writeStatus = osal_nv_write(NW_APP_CONFIG, 0, sizeof(application_config_t), &zclApp_Config);
    LREP("Saving attributes to NV write=%d\r\n", writeStatus);
    LREP("dur1=%d\r\n", zclApp_Config.RelayDurations[0]);
    LREP("dur2=%d\r\n", zclApp_Config.RelayDurations[1]);
    LREP("dur3=%d\r\n", zclApp_Config.RelayDurations[2]);
    LREP("dur4=%d\r\n", zclApp_Config.RelayDurations[3]);
}

static void zclApp_RestoreAttributesFromNV(void) {
    uint8 status = osal_nv_item_init(NW_APP_CONFIG, sizeof(application_config_t), NULL);
    LREP("Restoring attributes from NV  status=%d \r\n", status);
    if (status == NV_ITEM_UNINIT) {
        uint8 writeStatus = osal_nv_write(NW_APP_CONFIG, 0, sizeof(application_config_t), &zclApp_Config);
        LREP("NV was empty, writing %d\r\n", writeStatus);
    }
    if (status == ZSUCCESS) {
        LREPMaster("Reading from NV\r\n");
        osal_nv_read(NW_APP_CONFIG, 0, sizeof(application_config_t), &zclApp_Config);
    }
}

// Обработчик команд кластера OnOff
static void zclApp_OnOffCB(uint8 cmd)
{
  afIncomingMSGPacket_t *pPtr = zcl_getRawAFMsg();

  uint8 relay = pPtr->endPoint - 1;

  if (pPtr->endPoint <= RELAYS) {
    switch (cmd) {
      case COMMAND_ON: {
        zclApp_Relays[relay] = TRUE;
        break;
      }
      case COMMAND_OFF: {
        zclApp_Relays[relay] = FALSE;
        break;
      }
      case COMMAND_TOGGLE: {
        zclApp_Relays[relay] = !zclApp_Relays[relay];
        break;
      }
    }
    zclApp_ApplyRelay(relay, zclApp_Relays[relay]);
  }

  //HalLedSet(HAL_LED_5, zclApp_Beeper ? HAL_LED_MODE_ON : HAL_LED_MODE_OFF);

  if (pPtr->endPoint == 5) {
    switch (cmd) {
      case COMMAND_ON: {
        zclApp_Beeper = TRUE;
        break;
      }
      case COMMAND_OFF: {
        zclApp_Beeper = FALSE;
        break;
      }
      case COMMAND_TOGGLE: {
        zclApp_Beeper = !zclApp_Beeper;
        break;
      }
    }
    
    HalLedSet(HAL_LED_5, zclApp_Beeper ? HAL_LED_MODE_ON : HAL_LED_MODE_OFF);
  }
  
}

static void zclApp_ApplyRelay(uint8 relay, bool value)
{
  HalLedSet(1 << relay, value ? HAL_LED_MODE_ON : HAL_LED_MODE_OFF);
  if (value & (zclApp_Config.RelayDurations[relay] > 0))
    osal_start_timerEx(zclApp_TaskID, 1 << (2 + relay), (uint32)(zclApp_Config.RelayDurations[relay] * 1000));

  zclApp_Report();  
}

static void zclApp_StopRelay(uint16 timer)
{
  uint8 relay =  (uint8)(log10(timer >> 2)/log10(2));
  zclApp_Relays[relay] = FALSE;
  zclApp_ApplyRelay(relay, zclApp_Relays[relay]);
  
  osal_stop_timerEx(zclApp_TaskID, timer);
  osal_clear_event(zclApp_TaskID, timer);

  zclApp_Report();  
}


static void zclApp_ReadSensors(void) 
{
  static uint8 currentSensorsReadingPhase = 0;

  LREP("currentSensorsReadingPhase %d\r\n", currentSensorsReadingPhase);
    // FYI: split reading sensors into phases, so single call wouldn't block processor
    // for extensive ammount of time
  switch (currentSensorsReadingPhase++) {
  case 0:
    bdb_RepChangedAttrValue(FIRST_RELAY_ENDPOINT, GEN_ON_OFF, ATTRID_ON_OFF);
    break;
  case 1:
    bdb_RepChangedAttrValue(SECOND_RELAY_ENDPOINT, GEN_ON_OFF, ATTRID_ON_OFF);
    break;
  case 2:
    bdb_RepChangedAttrValue(THIRD_RELAY_ENDPOINT, GEN_ON_OFF, ATTRID_ON_OFF);
    break;
  case 3:
    bdb_RepChangedAttrValue(FOURTH_RELAY_ENDPOINT, GEN_ON_OFF, ATTRID_ON_OFF);
    break;
  case 4:
    bdb_RepChangedAttrValue(BEEPER_ENDPOINT, GEN_ON_OFF, ATTRID_ON_OFF);
    break;
  default:
    osal_stop_timerEx(zclApp_TaskID, APP_READ_SENSORS_EVT);
    osal_clear_event(zclApp_TaskID, APP_READ_SENSORS_EVT);
    currentSensorsReadingPhase = 0;
    break;
  }
}

/****************************************************************************
****************************************************************************/
