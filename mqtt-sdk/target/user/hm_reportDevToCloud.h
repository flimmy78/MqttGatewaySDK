#ifndef _HM_REPORTDEVTOCLOUD_H
#define _HM_REPORTDEVTOCLOUD_H
//_HM_REPORT_DEV_TO_CLOUD_H

typedef enum{
REPORT_DATA = 1,
ALARM_DATA = 2
}CLOUDTYPE;


typedef enum{
ALARM_RESUME = 0,
ALARM = 1,
TAMPER_AND_ALARM_RESUME = 2,
TAMPER_AND_ALARM = 3,
TEMP_UP_ALARM	= 4,
TEMP_LOW_ALARM = 5,
HUMI_UP_ALARM = 6,
HUMI_LOW_ALARM = 7,
TVOC_HIGH_OVER_ALARM = 8,
TVOC_LITTLE_OVER_ALARM = 9,
PM2_5_OVER_ALARM	= 10,
PM10_OVER_ALARM = 11,
HCH0_OVER_ALARM = 12,
GAS_HIGH_OVER_ALARM = 13,
GAS_LITTLE_OVER_ALARM = 14,
ELECTRI_SHORT_ALARM = 15,
ELECTRI_OPEN_ALARM = 16,
DISCONNECT_WIFI_ALARM = 17,
ELSE_ERROR_ALARM = 18,
BATTERY_LOW_ALARM = 19,
POWER_W_OVER_ALARM = 20,
CURRENT_OVER_ALARM = 21,
VOLTAGE_OVER_ALARM = 22,
DEV_TEMP_UP_ALARM = 23,
SUB_DEV_OFFLINE = 24
}ALARMTYPE;

#define DEV_DATA_FOR_RECORD_TO_CLOUD 			1
#define DEV_DATA_FOR_ALARM_TO_CLOUD				2

/*
typedef enum{
ALARM_RESUME = 0,
ALARM = 1,
TAMPER_AND_ALARM_RESUME = 2,
TAMPER_AND_ALARM = 3,
BATTERY_LOW_ALARM = 8,
TEMP_UP_ALARM	= 0x10,
TEMP_LOW_ALARM = 0x11,
HUMI_UP_ALARM = 0x12,
HUMI_LOW_ALARM = 0x13,
TVOC_HIGH_OVER_ALARM = 0x14,
TVOC_LITTLE_OVER_ALARM = 0x15,
PM2_5_OVER_ALARM	= 0x16,
PM10_OVER_ALARM = 0x17,
HCH0_OVER_ALARM = 0x18,
GAS_HIGH_OVER_ALARM = 0x19,
GAS_LITTLE_OVER_ALARM = 0x1a,
ELECTRI_SHORT_ALARM = 0x1b,
ELECTRI_OPEN_ALARM = 0x1c,
POWER_W_OVER_ALARM = 0x1d,
VOLTAGE_OVER_ALARM = 0x1e,
CURRENT_OVER_ALARM = 0x1f,
DISCONNECT_WIFI_ALARM = 0x20,
ELSE_ERROR_ALARM = 0x21,
DEV_TEMP_UP_ALARM = 0x22
}ALARMTYPE;
*/
#define ALARM_RESUME_STRING			"ALRM_RESUME_"
#define ALARM_STRING					"ALRMING_"
#define TAMPER_ALARM_STRING			"TAMPER_DEVALARM_"
#define TAMPER_ALARM_RESUME_STRING	"TAMPER_DEVRESUME_"
#define BATTERY_LOW_ALARM_STRING		"LOW_POWER_"
#define TEMP_UP_ALARM_STRING			"tempup_"
#define TEMP_LOW_ALARM_STRING			"templow_"
#define HUMI_UP_ALARM_STRING			"humup_"
#define HUMI_LOW_ALARM_STRING			"humlow_"
#define TVOC_HIGH_OVER_STRING			"TOVC_H_"
#define TVOC_LITTLE_OVER_STRING		"TOVC_S_"
#define PM2_5_OVER_STRING				"PM25_"
#define PM10_OVER_STRING				"PM10_"
#define HCHO_OVER_STRING				"HCHO_"
#define GAS_HIGH_OVER_STRING			"GAS_H_"
#define GAS_LITTLE_OVER_STRING			"GAS_S_"
#define ELECTRI_SHORT_STRING			"Fault_S_"
#define ELECTRI_OPEN_STRING				"Fault_O_"
#define POWER_W_UP_STRING				"POWER_W_UP_"
#define VOLTAGE_UP_STRING				"VOLTAGE_UP_"
#define CURRENT_UP_STRING				"CURRENT_UP_"
#define SUB_DEV_OFFLINE_STRING			"SubDevOffline_"
//���ڴ�����
#define DISCONNECT_WIFI_STRING			"Fault_N_"
#define ELSE_ERROR_STRING				"Fault_X_"

#define MAX_REPORT_CLOUD_BUF_NUM		5
#define MAX_REPROT_CLOUD_BUF_LEN		256
typedef struct{
uint8_t validFlag;
uint16_t dataLen;
char data[MAX_REPROT_CLOUD_BUF_LEN];
}REPORTTOCLOUD;


#define MAX_ALARM_CLOUD_BUF_NUM		5
#define MAX_ALARM_CLOUD_BUF_LEN		256
typedef struct{
uint8_t validFlag;
uint16_t iosLen;
char iosBuf[MAX_ALARM_CLOUD_BUF_LEN];
uint16_t androidLen;
char androidBuf[MAX_ALARM_CLOUD_BUF_LEN];
char alarmMode;
}ALARMTOCLOUD;





typedef enum{
USER_CONRTOL_DEV_TYPE_ZB,
USER_CONTROL_DEV_TYPE_GW
}USER_CONTROL_TYPE;


typedef enum 
{
	DP_BYTE = 0x00,
	DP_INT16 = 0x10,
	DP_UINT16 = 0x20,
	DP_INT32 = 0x30,
	DP_UINT32 = 0x40,
	DP_INT64 = 0x50,
	DP_UINT64 = 0x60,
	DP_FLOAT = 0x70,
	DP_DOUBLE = 0x80,
	DP_STRING = 0x90,
}DATAPOINTTYPE;



json_t* userReadArmStateAndTimerFunc(void);
json_t* userReadHomeArmForDevList(void);

json_t *userCreateCutRgbSettingObject(HM_DEV_RAM_LIST *pni,DEVINFOOPTION devOption);
json_t *userCreateCutRgbStaObject(HM_DEV_RAM_LIST *pni,DEVINFOOPTION devOption);

void writeGWStateRecordToCloud(char *SID,char *TEID,DEVINFOOPTION devOption);
json_t *userCreatWifiDevInfoJsObject(DEV_MYSELF_INFO *tempGwInfo);
json_t *userCreatGWStaObject(DEV_MYSELF_INFO *tempGwInfo,DEVINFOOPTION devOption);
json_t *userCreateWDSettingObject(HM_DEV_RAM_LIST *pni,DEVINFOOPTION devOption);
json_t *userCreateWDStaObject(HM_DEV_RAM_LIST *pni,DEVINFOOPTION devOption);


json_t *userCreateAiSwitchSettingObject(HM_DEV_RAM_LIST *pni,DEVINFOOPTION devOption);
json_t *userCreateAiSwitchStaObject(HM_DEV_RAM_LIST *pni,DEVINFOOPTION devOption);



json_t *userCreateSPlugSettingObject(HM_DEV_RAM_LIST *pni,DEVINFOOPTION devOption);

json_t *userCreateSPlugStaObject(HM_DEV_RAM_LIST *pni,DEVINFOOPTION devOption);


json_t *userCreatePlugSettingObject(HM_DEV_RAM_LIST *pni,DEVINFOOPTION devOption);
json_t *userCreatePlugStaObject(HM_DEV_RAM_LIST *pni,DEVINFOOPTION devOption);
json_t *userCreateRgbSettingObject(HM_DEV_RAM_LIST *pni,DEVINFOOPTION devOption);
json_t *userCreateRgbStaObject(HM_DEV_RAM_LIST *pni,DEVINFOOPTION devOption);
json_t *userCreateTHPSettingObject(HM_DEV_RAM_LIST *pni,DEVINFOOPTION devOption);
json_t *userCreateTHPStaObject(HM_DEV_RAM_LIST *pni,DEVINFOOPTION devOption);

json_t *userCreateIasSettingObject(HM_DEV_RAM_LIST *pni,DEVINFOOPTION devOption);
json_t *userCreateIasStaObject(HM_DEV_RAM_LIST *pni,DEVINFOOPTION devOption);
json_t *userCreateAceStaObject(HM_DEV_RAM_LIST *pni,DEVINFOOPTION devOption);
void writeNodeStateRecordToCloud(HM_DEV_RAM_LIST *pni,DEVINFOOPTION devOption);
void writeUserControlDevStateToCloud(uint8_t devMode,uint16_t index,DEVINFOOPTION devOption,char* sid,char*teid);
void writeUserControlSceneStateToCloud(uint8_t sceneId,uint8_t exeSta,char * sid,char *teid);


void userAlarmMsgData_write(unsigned char* buff, int* bufflen,HM_DEV_RAM_LIST *pni,char alarm_data);
void userUpdateMsgDataToCloud(void);

void writeNodeStateAlarnToCloud(HM_DEV_RAM_LIST *pni,uint8_t alarnState);


#endif

