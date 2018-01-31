#ifndef _SERVERDATAHANDLE_H_
#define _SERVERDATAHANDLE_H_

#include "stdint.h"
#include "hm_app.h"

//#define ENCRYPT_ENABLE
#define NOTKEY_MODE			0
#define PUBLICKEY_MODE		1
#define PRIVATEKEY_MODE		2

#define FAIL_RESULT			0
#define SUCCESSS_RESULT		1

typedef struct{
uint8_t head;
uint16_t wifisoftwarever;
uint16_t wifihardwarever;
uint16_t protocolver;
uint16_t zigbeever;
}DEV_GW_INFO;

typedef struct HMLINK_UPGRADE_FUN {
	unsigned int fileSize;
	char *checkStr;
	unsigned int checkStrLength;
	char *urlstr;
	unsigned short urlLength;
	unsigned char mCheckFlag;
	unsigned char mHardVersion;
	unsigned short mSoftVersion;
    unsigned short mCurrentVersion;
	unsigned char isWifi;
} HMLINK_UPGRADE;

#define GW_PUBPLICKEY 						"bd17df6d548211e7"
#define DEFUALT_ZIGBEE_JOINING_TIMERS		250

#define DEL_ZIGBEE_MODE		1
#define ADD_ZIGBEE_MODE		2
#define DELETE_ZIGBEE_MODE	3
//delete

#define	CONTROL				1
#define CONTROL_ACK			2
#define REPORT				3
#define EVENT				4
#define ALARM_ACT			5
#define FAULT				6
#define COERCION_OTA		7
#define COERCION_OTA_ACK	8
#define OTA_KEY				9
#define OTA_ACK				10
#define SENDKEY				11
#define SENDLIST			12
#define GETDEVICEID			13
#define GETDEVICEID_ACK		14
#define GETTIME				15
#define GETTIME_ACK			16
#define SCENE				17
#define SCENE_ACK			18
#define LINKAGE				19
#define LINKAGE_ACK			20
#define ONLINE_KEY			22
#define DISCONNET_ACK		23

#define ACTION				51
#define PASS				52
#define POINT				53
#define RESULT_KEY			54
#define ENCRYPT_TYPE		55
#define LIST_KEY			56
#define TIMESTAMP			57
#define LOGID				58

#define ALARM_KEY			101
#define BP_KEY				102
#define MESSAGE_KEY			103
#define CHECK_KEY			104
#define OL_KEY				105
#define SECRET_KEY			106
#define DEVICEID_KEY		107
#define MAC_KEY				108
#define ALARMTIME_KEY		109
#define SUB_INDEX_KEY		110
#define BA_KEY				111
#define OP_KEY				112
#define AS_KEY				113
#define OF_KEY				114
#define HY_KEY				115
#define TP_KEY				116
#define HU_KEY				117
#define HL_KEY				118
#define TU_KEY				119
#define TL_KEY				120
#define LE_KEY				121
#define CR_KEY				122
#define CG_KEY				123
#define CB_KEY				124
#define RO_KEY				125
#define UO_KEY				126
#define PW_KEY				127
#define ET_KEY				128
#define TM_KEY				129
#define OF1_KEY				130
#define OF2_KEY				131
#define OF3_KEY				132
#define TV_KEY				133
#define P2_KEY				134
#define CC_KEY				135
#define AQ_KEY				136
#define P1_KEY				137
#define LEL_KEY				138
#define CWL_KEY				139
#define AL_KEY				140
#define SL_KEY				141
#define BT_KEY				142
#define LG_KEY				143
#define LL_KEY				144
#define LO_KEY				145
#define LT_KEY				146
#define AT_KEY				147
#define RD_KEY				148
#define HO_KEY				149
#define BL_KEY				150
#define URL_KEY				151
#define SIZE_KEY			152
#define SOFT_VERSION		153
#define MD5_KEY				154
#define OTA_ID_KEY			155
#define FIRMWARE_TYPE		156
#define OF4_KEY				157
#define REMOVE_KEY			158
#define TEMPBA_KEY			159
//#define HUMPBA_KEY			160
#define COUNT_KEY			160
#define LEL1_KEY			161
#define LEL2_KEY			162
#define LEL3_KEY			163
#define LEL4_KEY			164
#define CWL1_KEY			165
#define CWL2_KEY			166
#define CWL3_KEY			167
#define CWL4_KEY			168
#define HUMPBA_KEY			169
#define WIFISOFT_VERSION	170
#define ZBSOFT_VERSION		171

#define WFFIRMWARE_TYPE		1
#define ZBFIRMWARE_TYPE		2//ZBЭ����

//#define TIMESTAMP			57


#define NO_ENCRYPT			0
#define PULIC_ENCRYPT		1
#define PRIVATE_ENCRYPT		2

#define REPORTINFOTYPE			0x4000
#define INFORMATIONTYPE			0x3000
#define DESTINATIONID			0xfff1//��app
#define SOURCEID_TOAPP			0x3022
#define SN_COUNTS				0x0001

#define GETAESKEY_CMD			0x0001
#define GETMESG_CMD				0x3000
#define SETZONE_CMD				0x3002
#define SETGWPARM_CMD			0x3001  
#define ZBINTONETWORK_CMD		0x3009
#define DELZBSUBSET_CMD			0x300b
#define GETALLZBSS_CMD			0x3004
#define GETONEZBSS_CMD			0x3005
#define GETZBSINGLESS_CMD		0x3006
#define GETZBSE_CMD				0x3007
#define SETZBSE_CMD				0x3008
#define DEVICEOTA_CMD			0x300f
#define ZBINTOSS_CMD			0x300a

#define GETGWPARM_CMD			0x3010
#define SETSERVER_CMD			0x3021

#define SETHARMDEV_CMD			0x3022
#define GETHARMDEV_CMD			0x3023
#define SETALLLIGHT_CMD			0x3024
#define SETRELATEDWD_CMD		0x3025
#define GETRELATEDWD_CMD		0x3026
#define SETARMTIMER_CMD			0x3027
#define GETARMTIMER_CMD			0x3028
#define SETNIGHTLINKAGE_CMD		0x3029
#define GETNIGHTLINKAGE_CMD		0x302a

//DestinationID   SourceID  sn

char *strupr_a(char*ch);
void Int_biglittle_Convert(int sdata,int *edata);
void hm_information_init(void);


void Pass_data_handle(char* data, int length);
void report_zb_intonetwork(HM_DEV_RAM_LIST *pni,unsigned char* payload, int payloadle);
void report_zb_pointdata(HM_DEV_RAM_LIST *pni,unsigned char* payload, int payloadlen);
void sent_zigbee_SSstate(HM_DEV_RAM_LIST *pni,unsigned char* payload, int payloadlen);
void sent_zigbee_SingleSS(HM_DEV_RAM_LIST *pni,unsigned char* payload, int payloadlen);
void sent_zigbee_SEsetting(HM_DEV_RAM_LIST *pni,unsigned char* payload, int payloadlen);
void sent_zigbeelist_msg(HM_DEV_RAM_LIST *pni,unsigned char type_mode,unsigned int server_deviceid,unsigned char total_counts);
void report_zb_alarmMsg(char mode,unsigned char* payload, int payloadlen);
void report_zb_recordMsg(unsigned char* payload, int payloadlen);
void pass_data_process(int cmddata,char* data,int data_len);
void respond_otaack_pointdata(char result);
void respond_otaack_passdata(char* data,char result);





#endif
