#include "AF.h"
#include "OSAL.h"
#include "ZComDef.h"
#include "ZDConfig.h"

#include "zcl.h"
#include "zcl_general.h"
#include "zcl_ha.h"
#include "zcl_ms.h"

#include "zcl_app.h"

#include "version.h"

#include "bdb_touchlink.h"
#include "bdb_touchlink_target.h"
#include "stub_aps.h"

/*********************************************************************
 * CONSTANTS
 */

#define APP_DEVICE_VERSION 2
#define APP_FLAGS 0

#define APP_HWVERSION 1
#define APP_ZCLVERSION 1

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

// Global attributes
const uint16 zclApp_clusterRevision_all = 0x0002;

// Basic Cluster
const uint8 zclApp_HWRevision = APP_HWVERSION;
const uint8 zclApp_ZCLVersion = APP_ZCLVERSION;
const uint8 zclApp_ApplicationVersion = 3;
const uint8 zclApp_StackVersion = 4;

const uint8 zclApp_ManufacturerName[] = {7, 'B', 'a', 'c', 'c', 'h', 'u', 's'};
const uint8 zclApp_ModelId[] = {15, 'R', 'e', 'l', 'a', 'y', '.', '4', 'c', 'h', '.', 'T', 'i', 'm', 'e', 'r'};

const uint8 zclApp_PowerSource = POWER_SOURCE_MAINS_1_PHASE;

#define DEFAULT_TimeLow           0
#define DEFAULT_TimeHigh          0
#define DEFAULT_Duration          30

application_config_t zclApp_Config = {
    .TimeLow           = DEFAULT_TimeLow,
    .TimeHigh          = DEFAULT_TimeHigh,
    .RelayDurations[0]  = DEFAULT_Duration,
    .RelayDurations[1]  = DEFAULT_Duration,
    .RelayDurations[2]  = DEFAULT_Duration,
    .RelayDurations[3]  = DEFAULT_Duration
};

uint32  zclApp_GenTime_TimeUTC = 0;

bool zclApp_Relays[4] = {FALSE, FALSE, FALSE, FALSE};



uint16 zclApp_IdentifyTime = 0;
uint16 zclApp_MaxDuration = 0;//LEAKDETECTOR_ALARM_MAX_DURATION;
bool    zclApp_Beeper = FALSE;

/*********************************************************************
 * ATTRIBUTE DEFINITIONS - Uses REAL cluster IDs
 */

// First Relay EP

CONST zclAttrRec_t zclApp_AttrsFirstRelayEP[] = {
    {BASIC, {ATTRID_BASIC_ZCL_VERSION, ZCL_UINT8, R, (void *)&zclApp_ZCLVersion}},
    {BASIC, {ATTRID_BASIC_APPL_VERSION, ZCL_UINT8, R, (void *)&zclApp_ApplicationVersion}},
    {BASIC, {ATTRID_BASIC_STACK_VERSION, ZCL_UINT8, R, (void *)&zclApp_StackVersion}},
    {BASIC, {ATTRID_BASIC_HW_VERSION, ZCL_UINT8, R, (void *)&zclApp_HWRevision}},
    {BASIC, {ATTRID_BASIC_MANUFACTURER_NAME, ZCL_DATATYPE_CHAR_STR, R, (void *)zclApp_ManufacturerName}},
    {BASIC, {ATTRID_BASIC_MODEL_ID, ZCL_DATATYPE_CHAR_STR, R, (void *)zclApp_ModelId}},
    {BASIC, {ATTRID_BASIC_DATE_CODE, ZCL_DATATYPE_CHAR_STR, R, (void *)zclApp_DateCode}},
    {BASIC, {ATTRID_BASIC_POWER_SOURCE, ZCL_DATATYPE_ENUM8, R, (void *)&zclApp_PowerSource}},
    {BASIC, {ATTRID_BASIC_SW_BUILD_ID, ZCL_DATATYPE_CHAR_STR, R, (void *)zclApp_DateCode}},
    {BASIC, {ATTRID_CLUSTER_REVISION, ZCL_UINT16, R, (void *)&zclApp_clusterRevision_all}},
    {ZCL_CLUSTER_ID_GEN_IDENTIFY, {ATTRID_IDENTIFY_TIME, ZCL_DATATYPE_UINT16, RW,(void *)&zclApp_IdentifyTime}},
    {GEN_ON_OFF, {ATTRID_ON_OFF, ZCL_BOOLEAN, RWR, (void *)&zclApp_Relays[0]}},
    {GEN_ON_OFF, {ATTRID_DURATION, ZCL_UINT16, RW, (void *)&zclApp_Config.RelayDurations[0]}},

};

uint8 CONST zclApp_AttrsFirstRelayEPCount = (sizeof(zclApp_AttrsFirstRelayEP) / sizeof(zclApp_AttrsFirstRelayEP[0]));

const cId_t zclApp_InClusterListFirstRelayEP[] = {
  ZCL_CLUSTER_ID_GEN_BASIC,
  ZCL_CLUSTER_ID_GEN_IDENTIFY,
  GEN_ON_OFF
};

const cId_t zclApp_OutClusterListFirstRelayEP[] = {
  ZCL_CLUSTER_ID_GEN_IDENTIFY,
  GEN_ON_OFF,
};

#define APP_MAX_OUTCLUSTERS_FIRST_RELAY_EP (sizeof(zclApp_OutClusterListFirstRelayEP) / sizeof(zclApp_OutClusterListFirstRelayEP[0]))
#define APP_MAX_INCLUSTERS_FIRST_RELAY_EP (sizeof(zclApp_InClusterListFirstRelayEP) / sizeof(zclApp_InClusterListFirstRelayEP[0]))

SimpleDescriptionFormat_t zclApp_FirstRelayEP = {
    FIRST_RELAY_ENDPOINT,                        //  int Endpoint;
    ZCL_HA_PROFILE_ID,                          //  uint16 AppProfId[2];
    ZCL_HA_DEVICEID_ON_OFF_SWITCH,              //  uint16 AppDeviceId[2];
    APP_DEVICE_VERSION,                         //  int   AppDevVer:4;
    APP_FLAGS,                                  //  int   AppFlags:4;
    APP_MAX_INCLUSTERS_FIRST_RELAY_EP,           //  byte  AppNumInClusters;
    (cId_t *)zclApp_InClusterListFirstRelayEP,   //  byte *pAppInClusterList;
    APP_MAX_OUTCLUSTERS_FIRST_RELAY_EP,          //  byte  AppNumInClusters;
    (cId_t *)zclApp_OutClusterListFirstRelayEP   //  byte *pAppOutClusterList;
};

// Second Relay EP

CONST zclAttrRec_t zclApp_AttrsSecondRelayEP[] = {
    {GEN_ON_OFF, {ATTRID_ON_OFF, ZCL_BOOLEAN, RWR, (void *)&zclApp_Relays[1]}},
    {GEN_ON_OFF, {ATTRID_DURATION, ZCL_UINT16, RW, (void *)&zclApp_Config.RelayDurations[1]}},
};

uint8 CONST zclApp_AttrsSecondRelayEPCount = (sizeof(zclApp_AttrsSecondRelayEP) / sizeof(zclApp_AttrsSecondRelayEP[0]));

const cId_t zclApp_InClusterListSecondRelayEP[] = {
  GEN_ON_OFF
};

const cId_t zclApp_OutClusterListSecondRelayEP[] = {
  GEN_ON_OFF,
};

#define APP_MAX_OUTCLUSTERS_SECOND_RELAY_EP (sizeof(zclApp_OutClusterListSecondRelayEP) / sizeof(zclApp_OutClusterListSecondRelayEP[0]))
#define APP_MAX_INCLUSTERS_SECOND_RELAY_EP (sizeof(zclApp_InClusterListSecondRelayEP) / sizeof(zclApp_InClusterListSecondRelayEP[0]))


SimpleDescriptionFormat_t zclApp_SecondRelayEP = {
    SECOND_RELAY_ENDPOINT,                       //  int Endpoint;
    ZCL_HA_PROFILE_ID,                          //  uint16 AppProfId[2];
    ZCL_HA_DEVICEID_ON_OFF_SWITCH,              //  uint16 AppDeviceId[2];
    APP_DEVICE_VERSION,                         //  int   AppDevVer:4;
    APP_FLAGS,                                  //  int   AppFlags:4;
    APP_MAX_INCLUSTERS_SECOND_RELAY_EP,          //  byte  AppNumInClusters;
    (cId_t *)zclApp_InClusterListSecondRelayEP,  //  byte *pAppInClusterList;
    APP_MAX_OUTCLUSTERS_SECOND_RELAY_EP,         //  byte  AppNumInClusters;
    (cId_t *)zclApp_OutClusterListSecondRelayEP  //  byte *pAppOutClusterList;

};

// Third Relay EP

CONST zclAttrRec_t zclApp_AttrsThirdRelayEP[] = {
    {GEN_ON_OFF, {ATTRID_ON_OFF, ZCL_BOOLEAN, RWR, (void *)&zclApp_Relays[2]}},
    {GEN_ON_OFF, {ATTRID_DURATION, ZCL_UINT16, RW, (void *)&zclApp_Config.RelayDurations[2]}},
};

uint8 CONST zclApp_AttrsThirdRelayEPCount = (sizeof(zclApp_AttrsThirdRelayEP) / sizeof(zclApp_AttrsThirdRelayEP[0]));

const cId_t zclApp_InClusterListThirdRelayEP[] = {
  GEN_ON_OFF
};

const cId_t zclApp_OutClusterListThirdRelayEP[] = {
  GEN_ON_OFF,
};

#define APP_MAX_OUTCLUSTERS_THIRD_RELAY_EP (sizeof(zclApp_OutClusterListThirdRelayEP) / sizeof(zclApp_OutClusterListThirdRelayEP[0]))
#define APP_MAX_INCLUSTERS_THIRD_RELAY_EP (sizeof(zclApp_InClusterListThirdRelayEP) / sizeof(zclApp_InClusterListThirdRelayEP[0]))


SimpleDescriptionFormat_t zclApp_ThirdRelayEP = {
    THIRD_RELAY_ENDPOINT,                        //  int Endpoint;
    ZCL_HA_PROFILE_ID,                          //  uint16 AppProfId[2];
    ZCL_HA_DEVICEID_ON_OFF_SWITCH,              //  uint16 AppDeviceId[2];
    APP_DEVICE_VERSION,                         //  int   AppDevVer:4;
    APP_FLAGS,                                  //  int   AppFlags:4;
    APP_MAX_INCLUSTERS_THIRD_RELAY_EP,           //  byte  AppNumInClusters;
    (cId_t *)zclApp_InClusterListThirdRelayEP,   //  byte *pAppInClusterList;
    APP_MAX_OUTCLUSTERS_THIRD_RELAY_EP,          //  byte  AppNumInClusters;
    (cId_t *)zclApp_OutClusterListThirdRelayEP   //  byte *pAppOutClusterList;

};



// Fourth Relay EP

CONST zclAttrRec_t zclApp_AttrsFourthRelayEP[] = {
    {GEN_ON_OFF, {ATTRID_ON_OFF, ZCL_BOOLEAN, RWR, (void *)&zclApp_Relays[3]}},
    {GEN_ON_OFF, {ATTRID_DURATION, ZCL_UINT16, RW, (void *)&zclApp_Config.RelayDurations[3]}},
};

uint8 CONST zclApp_AttrsFourthRelayEPCount = (sizeof(zclApp_AttrsFourthRelayEP) / sizeof(zclApp_AttrsFourthRelayEP[0]));

const cId_t zclApp_InClusterListFourthRelayEP[] = {
  GEN_ON_OFF
};

const cId_t zclApp_OutClusterListFourthRelayEP[] = {
  GEN_ON_OFF,
};

#define APP_MAX_OUTCLUSTERS_FOURTH_RELAY_EP (sizeof(zclApp_OutClusterListFourthRelayEP) / sizeof(zclApp_OutClusterListFourthRelayEP[0]))
#define APP_MAX_INCLUSTERS_FOURTH_RELAY_EP (sizeof(zclApp_InClusterListFourthRelayEP) / sizeof(zclApp_InClusterListFourthRelayEP[0]))


SimpleDescriptionFormat_t zclApp_FourthRelayEP = {
    FOURTH_RELAY_ENDPOINT,                         //  int Endpoint;
    ZCL_HA_PROFILE_ID,                          //  uint16 AppProfId[2];
    ZCL_HA_DEVICEID_ON_OFF_SWITCH,              //  uint16 AppDeviceId[2];
    APP_DEVICE_VERSION,                         //  int   AppDevVer:4;
    APP_FLAGS,                                  //  int   AppFlags:4;
    APP_MAX_INCLUSTERS_FOURTH_RELAY_EP,            //  byte  AppNumInClusters;
    (cId_t *)zclApp_InClusterListFourthRelayEP,    //  byte *pAppInClusterList;
    APP_MAX_OUTCLUSTERS_FOURTH_RELAY_EP,           //  byte  AppNumInClusters;
    (cId_t *)zclApp_OutClusterListFourthRelayEP    //  byte *pAppOutClusterList;

};


// Beeper EP

CONST zclAttrRec_t zclApp_AttrsBeeperEP[] = {
    {GEN_ON_OFF, {ATTRID_ON_OFF, ZCL_BOOLEAN, RWR, (void *)&zclApp_Beeper}},
};

uint8 CONST zclApp_AttrsBeeperEPCount = (sizeof(zclApp_AttrsBeeperEP) / sizeof(zclApp_AttrsBeeperEP[0]));

const cId_t zclApp_InClusterListBeeperEP[] = {
  GEN_ON_OFF, 
};

const cId_t zclApp_OutClusterListBeeperEP[] = {
  GEN_ON_OFF,
};

#define APP_MAX_OUTCLUSTERS_BEEPER_EP (sizeof(zclApp_OutClusterListBeeperEP) / sizeof(zclApp_OutClusterListBeeperEP[0]))
#define APP_MAX_INCLUSTERS_BEEPER_EP (sizeof(zclApp_InClusterListBeeperEP) / sizeof(zclApp_InClusterListBeeperEP[0]))


SimpleDescriptionFormat_t zclApp_BeeperEP = {
    BEEPER_ENDPOINT,                            //  int Endpoint;
    ZCL_HA_PROFILE_ID,                          //  uint16 AppProfId[2];
    ZCL_HA_DEVICEID_ON_OFF_SWITCH,              //  uint16 AppDeviceId[2];
    APP_DEVICE_VERSION,                         //  int   AppDevVer:4;
    APP_FLAGS,                                  //  int   AppFlags:4;
    APP_MAX_INCLUSTERS_BEEPER_EP,               //  byte  AppNumInClusters;
    (cId_t *)zclApp_InClusterListBeeperEP,      //  byte *pAppInClusterList;
    APP_MAX_OUTCLUSTERS_BEEPER_EP,              //  byte  AppNumInClusters;
    (cId_t *)zclApp_OutClusterListBeeperEP      //  byte *pAppOutClusterList;

};

void zclApp_ResetAttributesToDefaultValues(void) {
    zclApp_Config.TimeLow           = DEFAULT_TimeLow;
    zclApp_Config.TimeHigh          = DEFAULT_TimeHigh;
    zclApp_Config.RelayDurations[0]  = DEFAULT_Duration;
    zclApp_Config.RelayDurations[1]  = DEFAULT_Duration;
    zclApp_Config.RelayDurations[2]  = DEFAULT_Duration;
    zclApp_Config.RelayDurations[3]  = DEFAULT_Duration;
}

