/** 
* @file 	qca_gpio_test.h	
* @brief	
* @author	
* @version	V0.1.0
* @date		2016.3.15
* @details	
* @copyright 
* @note	Add the code below to you project to test
* extern void test_qca_http_client(void);
* test_qca_http_client();
* or, you maybe create a thread to run qca_http_client_test()
*/
#include "hm_app.h"
#include "qca_json.h"
#include "qcom_common.h"
#include "qcom_utils.h"
#include "string.h"
#include "qca_http_client.h"
#include "hm_httpEvent.h"

#include "qca_ssl_client.h"
#include "qcom_security.h"
#include "hm_net_data_manage.h"
#include "hm_zigbeeInfohand.h"

#ifdef HEIMAN_APP
//#define SERVER_IP               "47.88.191.27"
//#define SERVER_IP               "console.heiman.cn"
#else
#ifdef YUNZHIYI_APP
//#define SERVER_IP               "api2.xlink.cn"
#else
#ifdef HM_TEST
//#define SERVER_IP               "console.heiman.cn"
#endif
#endif
#endif

#define SERVER_IP 				"119.29.224.84"

//"console.heiman.cn"
#define SERVER_PORT           443

#define         SSL_STACK_SIZE      1024*4
#define 	      HTTPS_REFLASH_TIMER 	3000
TX_THREAD       ssl_thread;
A_UINT8         ssl_thread_buf[SSL_STACK_SIZE];
extern GWSTATFORTIME gwStateForRealTime;
extern DEV_MYSELF_INFO hmIotsDevInfo;
SUBOTARAM subOtaRam;
extern DEV_OTA_STATION otaStationInfo;
extern HM_GW_LIST gw_info;
extern DEV_NUM_INFO devNumInfo;
typedef enum{
HTTPS_STATE_INIT = 0,
HTTPS_STATE_CONNECTING,
HTTPS_STATE_AUTHORIZE,
HTTPS_STATE_FIRST_FLASH_TOKEN,
HTTPS_STATE_REFLASH_TOKEN,
HTTPS_STATE_SCAN_ZC_NEW_SOFTVERSION,
HTTPS_STATE_SCAN_WIFI_NEW_SOFTVERSION,
HTTPS_STATE_UPDATE_DEV_INFO_TO_CLOUD_FOR_ECHO,
HTTPS_STATE_UPDATE_GW_NAME_TO_CLOUD,
HTTPS_STATE_ERROR_CLOSE,
HTTPS_STATE_IDLE
}HTTPSTATE;

typedef enum{
TRANSFER_STATE_INIT = 0,
TRANSFER_STATE_CONNECTING,
TRANSFER_STATE_AUTHORIZE,
TRANSFER_STATE_FIRST_FLASH_TOKEN,
TRANSFER_STATE_REFLASH_TOKEN,
TRANSFER_STATE_SCAN_ZC_NEW_SOFTVERSION,
TRANSFER_STATE_SCAN_WIFI_NEW_SOFTVERSION,
TRANSFER_STATE_UPDATE_DEV_INFO_TO_CLOUD_FOR_ECHO,
TRANSFER_STATE_UPDATE_GW_NAME_TO_CLOUD,
TRANSFER_STATE_ERROR_CLOSE,
TRANSFER_STATE_IDLE
}TRANSFERSTATE;


/* This cert have to be replace by user */
const unsigned char calist_cert1[] = {
   0x00, 0x00, 0x00, 0x01, 0x47, 0x65, 0x6F, 0x54, 
   0x72, 0x75, 0x73, 0x74, 0x00, 0x00, 0x00, 0x10, 
   0x30, 0x82, 0x03, 0x54, 0x30, 0x82, 0x02, 0x3C, 
   0xA0, 0x03, 0x02, 0x01, 0x02, 0x02, 0x03, 0x02, 
   0x34, 0x56, 0x30, 0x0D, 0x06, 0x09, 0x2A, 0x86, 
   0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x05, 0x05, 
   0x00, 0x30, 0x42, 0x31, 0x0B, 0x30, 0x09, 0x06, 
   0x03, 0x55, 0x04, 0x06, 0x13, 0x02, 0x55, 0x53, 
   0x31, 0x16, 0x30, 0x14, 0x06, 0x03, 0x55, 0x04, 
   0x0A, 0x13, 0x0D, 0x47, 0x65, 0x6F, 0x54, 0x72, 
   0x75, 0x73, 0x74, 0x20, 0x49, 0x6E, 0x63, 0x2E, 
   0x31, 0x1B, 0x30, 0x19, 0x06, 0x03, 0x55, 0x04, 
   0x03, 0x13, 0x12, 0x47, 0x65, 0x6F, 0x54, 0x72, 
   0x75, 0x73, 0x74, 0x20, 0x47, 0x6C, 0x6F, 0x62, 
   0x61, 0x6C, 0x20, 0x43, 0x41, 0x30, 0x1E, 0x17, 
   0x0D, 0x30, 0x32, 0x30, 0x35, 0x32, 0x31, 0x30, 
   0x34, 0x30, 0x30, 0x30, 0x30, 0x5A, 0x17, 0x0D, 
   0x32, 0x32, 0x30, 0x35, 0x32, 0x31, 0x30, 0x34, 
   0x30, 0x30, 0x30, 0x30, 0x5A, 0x30, 0x42, 0x31, 
   0x0B, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 
   0x13, 0x02, 0x55, 0x53, 0x31, 0x16, 0x30, 0x14, 
   0x06, 0x03, 0x55, 0x04, 0x0A, 0x13, 0x0D, 0x47, 
   0x65, 0x6F, 0x54, 0x72, 0x75, 0x73, 0x74, 0x20, 
   0x49, 0x6E, 0x63, 0x2E, 0x31, 0x1B, 0x30, 0x19, 
   0x06, 0x03, 0x55, 0x04, 0x03, 0x13, 0x12, 0x47, 
   0x65, 0x6F, 0x54, 0x72, 0x75, 0x73, 0x74, 0x20, 
   0x47, 0x6C, 0x6F, 0x62, 0x61, 0x6C, 0x20, 0x43, 
   0x41, 0x30, 0x82, 0x01, 0x22, 0x30, 0x0D, 0x06, 
   0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 
   0x01, 0x01, 0x05, 0x00, 0x03, 0x82, 0x01, 0x0F, 
   0x00, 0x30, 0x82, 0x01, 0x0A, 0x02, 0x82, 0x01, 
   0x01, 0x00, 0xDA, 0xCC, 0x18, 0x63, 0x30, 0xFD, 
   0xF4, 0x17, 0x23, 0x1A, 0x56, 0x7E, 0x5B, 0xDF, 
   0x3C, 0x6C, 0x38, 0xE4, 0x71, 0xB7, 0x78, 0x91, 
   0xD4, 0xBC, 0xA1, 0xD8, 0x4C, 0xF8, 0xA8, 0x43, 
   0xB6, 0x03, 0xE9, 0x4D, 0x21, 0x07, 0x08, 0x88, 
   0xDA, 0x58, 0x2F, 0x66, 0x39, 0x29, 0xBD, 0x05, 
   0x78, 0x8B, 0x9D, 0x38, 0xE8, 0x05, 0xB7, 0x6A, 
   0x7E, 0x71, 0xA4, 0xE6, 0xC4, 0x60, 0xA6, 0xB0, 
   0xEF, 0x80, 0xE4, 0x89, 0x28, 0x0F, 0x9E, 0x25, 
   0xD6, 0xED, 0x83, 0xF3, 0xAD, 0xA6, 0x91, 0xC7, 
   0x98, 0xC9, 0x42, 0x18, 0x35, 0x14, 0x9D, 0xAD, 
   0x98, 0x46, 0x92, 0x2E, 0x4F, 0xCA, 0xF1, 0x87, 
   0x43, 0xC1, 0x16, 0x95, 0x57, 0x2D, 0x50, 0xEF, 
   0x89, 0x2D, 0x80, 0x7A, 0x57, 0xAD, 0xF2, 0xEE, 
   0x5F, 0x6B, 0xD2, 0x00, 0x8D, 0xB9, 0x14, 0xF8, 
   0x14, 0x15, 0x35, 0xD9, 0xC0, 0x46, 0xA3, 0x7B, 
   0x72, 0xC8, 0x91, 0xBF, 0xC9, 0x55, 0x2B, 0xCD, 
   0xD0, 0x97, 0x3E, 0x9C, 0x26, 0x64, 0xCC, 0xDF, 
   0xCE, 0x83, 0x19, 0x71, 0xCA, 0x4E, 0xE6, 0xD4, 
   0xD5, 0x7B, 0xA9, 0x19, 0xCD, 0x55, 0xDE, 0xC8, 
   0xEC, 0xD2, 0x5E, 0x38, 0x53, 0xE5, 0x5C, 0x4F, 
   0x8C, 0x2D, 0xFE, 0x50, 0x23, 0x36, 0xFC, 0x66, 
   0xE6, 0xCB, 0x8E, 0xA4, 0x39, 0x19, 0x00, 0xB7, 
   0x95, 0x02, 0x39, 0x91, 0x0B, 0x0E, 0xFE, 0x38, 
   0x2E, 0xD1, 0x1D, 0x05, 0x9A, 0xF6, 0x4D, 0x3E, 
   0x6F, 0x0F, 0x07, 0x1D, 0xAF, 0x2C, 0x1E, 0x8F, 
   0x60, 0x39, 0xE2, 0xFA, 0x36, 0x53, 0x13, 0x39, 
   0xD4, 0x5E, 0x26, 0x2B, 0xDB, 0x3D, 0xA8, 0x14, 
   0xBD, 0x32, 0xEB, 0x18, 0x03, 0x28, 0x52, 0x04, 
   0x71, 0xE5, 0xAB, 0x33, 0x3D, 0xE1, 0x38, 0xBB, 
   0x07, 0x36, 0x84, 0x62, 0x9C, 0x79, 0xEA, 0x16, 
   0x30, 0xF4, 0x5F, 0xC0, 0x2B, 0xE8, 0x71, 0x6B, 
   0xE4, 0xF9, 0x02, 0x03, 0x01, 0x00, 0x01, 0xA3, 
   0x53, 0x30, 0x51, 0x30, 0x0F, 0x06, 0x03, 0x55, 
   0x1D, 0x13, 0x01, 0x01, 0xFF, 0x04, 0x05, 0x30, 
   0x03, 0x01, 0x01, 0xFF, 0x30, 0x1D, 0x06, 0x03, 
   0x55, 0x1D, 0x0E, 0x04, 0x16, 0x04, 0x14, 0xC0, 
   0x7A, 0x98, 0x68, 0x8D, 0x89, 0xFB, 0xAB, 0x05, 
   0x64, 0x0C, 0x11, 0x7D, 0xAA, 0x7D, 0x65, 0xB8, 
   0xCA, 0xCC, 0x4E, 0x30, 0x1F, 0x06, 0x03, 0x55, 
   0x1D, 0x23, 0x04, 0x18, 0x30, 0x16, 0x80, 0x14, 
   0xC0, 0x7A, 0x98, 0x68, 0x8D, 0x89, 0xFB, 0xAB, 
   0x05, 0x64, 0x0C, 0x11, 0x7D, 0xAA, 0x7D, 0x65, 
   0xB8, 0xCA, 0xCC, 0x4E, 0x30, 0x0D, 0x06, 0x09, 
   0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 
   0x05, 0x05, 0x00, 0x03, 0x82, 0x01, 0x01, 0x00, 
   0x35, 0xE3, 0x29, 0x6A, 0xE5, 0x2F, 0x5D, 0x54, 
   0x8E, 0x29, 0x50, 0x94, 0x9F, 0x99, 0x1A, 0x14, 
   0xE4, 0x8F, 0x78, 0x2A, 0x62, 0x94, 0xA2, 0x27, 
   0x67, 0x9E, 0xD0, 0xCF, 0x1A, 0x5E, 0x47, 0xE9, 
   0xC1, 0xB2, 0xA4, 0xCF, 0xDD, 0x41, 0x1A, 0x05, 
   0x4E, 0x9B, 0x4B, 0xEE, 0x4A, 0x6F, 0x55, 0x52, 
   0xB3, 0x24, 0xA1, 0x37, 0x0A, 0xEB, 0x64, 0x76, 
   0x2A, 0x2E, 0x2C, 0xF3, 0xFD, 0x3B, 0x75, 0x90, 
   0xBF, 0xFA, 0x71, 0xD8, 0xC7, 0x3D, 0x37, 0xD2, 
   0xB5, 0x05, 0x95, 0x62, 0xB9, 0xA6, 0xDE, 0x89, 
   0x3D, 0x36, 0x7B, 0x38, 0x77, 0x48, 0x97, 0xAC, 
   0xA6, 0x20, 0x8F, 0x2E, 0xA6, 0xC9, 0x0C, 0xC2, 
   0xB2, 0x99, 0x45, 0x00, 0xC7, 0xCE, 0x11, 0x51, 
   0x22, 0x22, 0xE0, 0xA5, 0xEA, 0xB6, 0x15, 0x48, 
   0x09, 0x64, 0xEA, 0x5E, 0x4F, 0x74, 0xF7, 0x05, 
   0x3E, 0xC7, 0x8A, 0x52, 0x0C, 0xDB, 0x15, 0xB4, 
   0xBD, 0x6D, 0x9B, 0xE5, 0xC6, 0xB1, 0x54, 0x68, 
   0xA9, 0xE3, 0x69, 0x90, 0xB6, 0x9A, 0xA5, 0x0F, 
   0xB8, 0xB9, 0x3F, 0x20, 0x7D, 0xAE, 0x4A, 0xB5, 
   0xB8, 0x9C, 0xE4, 0x1D, 0xB6, 0xAB, 0xE6, 0x94, 
   0xA5, 0xC1, 0xC7, 0x83, 0xAD, 0xDB, 0xF5, 0x27, 
   0x87, 0x0E, 0x04, 0x6C, 0xD5, 0xFF, 0xDD, 0xA0, 
   0x5D, 0xED, 0x87, 0x52, 0xB7, 0x2B, 0x15, 0x02, 
   0xAE, 0x39, 0xA6, 0x6A, 0x74, 0xE9, 0xDA, 0xC4, 
   0xE7, 0xBC, 0x4D, 0x34, 0x1E, 0xA9, 0x5C, 0x4D, 
   0x33, 0x5F, 0x92, 0x09, 0x2F, 0x88, 0x66, 0x5D, 
   0x77, 0x97, 0xC7, 0x1D, 0x76, 0x13, 0xA9, 0xD5, 
   0xE5, 0xF1, 0x16, 0x09, 0x11, 0x35, 0xD5, 0xAC, 
   0xDB, 0x24, 0x71, 0x70, 0x2C, 0x98, 0x56, 0x0B, 
   0xD9, 0x17, 0xB4, 0xD1, 0xE3, 0x51, 0x2B, 0x5E, 
   0x75, 0xE8, 0xD5, 0xD0, 0xDC, 0x4F, 0x34, 0xED, 
   0xC2, 0x05, 0x66, 0x80, 0xA1, 0xCB, 0xE6, 0x33
};
/* This cert have to be replace by user */
const unsigned char calist_cert2[] = {
 0x00, 0x00, 0x00, 0x01, 0x31, 0x39, 0x32, 0x2E, 
   0x31, 0x36, 0x38, 0x2E, 0x00, 0x00, 0x00, 0x10, 
   0x30, 0x82, 0x03, 0x0D, 0x30, 0x82, 0x01, 0xF5, 
   0xA0, 0x03, 0x02, 0x01, 0x02, 0x02, 0x09, 0x00, 
   0x96, 0xA9, 0xA9, 0x1D, 0xFC, 0x37, 0xE2, 0x4B, 
   0x30, 0x0D, 0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 
   0xF7, 0x0D, 0x01, 0x01, 0x05, 0x05, 0x00, 0x30, 
   0x81, 0x90, 0x31, 0x0B, 0x30, 0x09, 0x06, 0x03, 
   0x55, 0x04, 0x06, 0x13, 0x02, 0x43, 0x4E, 0x31, 
   0x0B, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x08, 
   0x0C, 0x02, 0x47, 0x44, 0x31, 0x0B, 0x30, 0x09, 
   0x06, 0x03, 0x55, 0x04, 0x07, 0x0C, 0x02, 0x53, 
   0x5A, 0x31, 0x12, 0x30, 0x10, 0x06, 0x03, 0x55, 
   0x04, 0x0A, 0x0C, 0x09, 0x4C, 0x4F, 0x4E, 0x47, 
   0x54, 0x48, 0x49, 0x4E, 0x4B, 0x31, 0x14, 0x30, 
   0x12, 0x06, 0x03, 0x55, 0x04, 0x0B, 0x0C, 0x0B, 
   0x44, 0x45, 0x56, 0x45, 0x4C, 0x4F, 0x50, 0x4D, 
   0x45, 0x4E, 0x54, 0x31, 0x17, 0x30, 0x15, 0x06, 
   0x03, 0x55, 0x04, 0x03, 0x0C, 0x0E, 0x31, 0x32, 
   0x30, 0x2E, 0x32, 0x34, 0x2E, 0x32, 0x31, 0x36, 
   0x2E, 0x31, 0x35, 0x33, 0x31, 0x24, 0x30, 0x22, 
   0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 
   0x01, 0x09, 0x01, 0x16, 0x15, 0x64, 0x65, 0x6E, 
   0x6E, 0x69, 0x73, 0x2E, 0x67, 0x65, 0x40, 0x6C, 
   0x6F, 0x6E, 0x67, 0x73, 0x79, 0x73, 0x2E, 0x63, 
   0x6F, 0x6D, 0x30, 0x1E, 0x17, 0x0D, 0x31, 0x36, 
   0x30, 0x34, 0x32, 0x31, 0x30, 0x32, 0x35, 0x31, 
   0x34, 0x32, 0x5A, 0x17, 0x0D, 0x31, 0x36, 0x30, 
   0x35, 0x32, 0x31, 0x30, 0x32, 0x35, 0x31, 0x34, 
   0x32, 0x5A, 0x30, 0x52, 0x31, 0x0B, 0x30, 0x09, 
   0x06, 0x03, 0x55, 0x04, 0x06, 0x13, 0x02, 0x43, 
   0x4E, 0x31, 0x0B, 0x30, 0x09, 0x06, 0x03, 0x55, 
   0x04, 0x08, 0x13, 0x02, 0x47, 0x44, 0x31, 0x0B, 
   0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x07, 0x13, 
   0x02, 0x53, 0x5A, 0x31, 0x12, 0x30, 0x10, 0x06, 
   0x03, 0x55, 0x04, 0x0B, 0x13, 0x09, 0x4C, 0x4F, 
   0x4E, 0x47, 0x54, 0x48, 0x49, 0x4E, 0x4B, 0x31, 
   0x15, 0x30, 0x13, 0x06, 0x03, 0x55, 0x04, 0x03, 
   0x13, 0x0C, 0x31, 0x39, 0x32, 0x2E, 0x31, 0x36, 
   0x38, 0x2E, 0x37, 0x2E, 0x37, 0x34, 0x30, 0x81, 
   0x9F, 0x30, 0x0D, 0x06, 0x09, 0x2A, 0x86, 0x48, 
   0x86, 0xF7, 0x0D, 0x01, 0x01, 0x01, 0x05, 0x00, 
   0x03, 0x81, 0x8D, 0x00, 0x30, 0x81, 0x89, 0x02, 
   0x81, 0x81, 0x00, 0xC7, 0x05, 0xBB, 0x0C, 0xB6, 
   0xDB, 0x9F, 0xC2, 0xFF, 0x07, 0x57, 0x43, 0xA7, 
   0x8E, 0x26, 0x68, 0x57, 0x4F, 0x64, 0x69, 0x36, 
   0xC1, 0xF5, 0x23, 0x37, 0xD5, 0x9A, 0x29, 0xF9, 
   0x99, 0x31, 0x87, 0x69, 0x13, 0x02, 0xC3, 0x5B, 
   0x64, 0x4F, 0xC7, 0x96, 0x9B, 0x23, 0x6E, 0xB7, 
   0xC8, 0x62, 0xAA, 0x0F, 0x05, 0x4A, 0x7D, 0xC5, 
   0x73, 0x4B, 0xB0, 0x40, 0x7F, 0x80, 0x24, 0xDF, 
   0xA6, 0x6D, 0xE1, 0xF9, 0x80, 0x50, 0x6B, 0x01, 
   0x1E, 0x5C, 0x42, 0x15, 0x54, 0xD0, 0xF1, 0xBA, 
   0x58, 0x09, 0xB3, 0xDF, 0xF0, 0x93, 0x43, 0x37, 
   0x14, 0x87, 0x6D, 0x73, 0xDB, 0x06, 0xDB, 0xDA, 
   0x16, 0x5F, 0x7A, 0x32, 0x61, 0xE6, 0x7B, 0x3A, 
   0x2E, 0x64, 0x57, 0x5F, 0xA0, 0x35, 0x9A, 0x34, 
   0xB8, 0x4F, 0x44, 0xE2, 0xA8, 0x8D, 0x3C, 0x86, 
   0x46, 0xD3, 0xA3, 0x1F, 0x74, 0x85, 0x06, 0xB7, 
   0x8B, 0x59, 0xFD, 0x02, 0x03, 0x01, 0x00, 0x01, 
   0xA3, 0x2B, 0x30, 0x29, 0x30, 0x09, 0x06, 0x03, 
   0x55, 0x1D, 0x13, 0x04, 0x02, 0x30, 0x00, 0x30, 
   0x0B, 0x06, 0x03, 0x55, 0x1D, 0x0F, 0x04, 0x04, 
   0x03, 0x02, 0x05, 0xE0, 0x30, 0x0F, 0x06, 0x03, 
   0x55, 0x1D, 0x11, 0x04, 0x08, 0x30, 0x06, 0x87, 
   0x04, 0xC0, 0xA8, 0x07, 0x4A, 0x30, 0x0D, 0x06, 
   0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 
   0x01, 0x05, 0x05, 0x00, 0x03, 0x82, 0x01, 0x01, 
   0x00, 0x50, 0x38, 0x22, 0xEE, 0x17, 0xDC, 0xA5, 
   0xD4, 0xF6, 0x71, 0x44, 0xB0, 0xD0, 0x20, 0x65, 
   0xC2, 0x49, 0xA0, 0xBD, 0x9F, 0x4F, 0xF0, 0xA9, 
   0xFC, 0x7C, 0xEF, 0x3E, 0x67, 0x6B, 0x5F, 0x03, 
   0xD7, 0x7E, 0x82, 0x2A, 0x5A, 0x63, 0x43, 0x2F, 
   0xD8, 0xFD, 0x0E, 0x58, 0xA6, 0xD1, 0xDE, 0x83, 
   0xD7, 0x23, 0xAE, 0x2A, 0xD4, 0x3F, 0xA4, 0x6E, 
   0x81, 0xFE, 0xFE, 0xDF, 0xCB, 0x73, 0xEE, 0x2E, 
   0x6D, 0x92, 0x44, 0xAC, 0x19, 0xAD, 0xAF, 0x72, 
   0x4E, 0x4E, 0x59, 0x5F, 0x11, 0x0B, 0x35, 0x44, 
   0x29, 0x77, 0x58, 0x53, 0x0F, 0x95, 0xA5, 0x3E, 
   0x1B, 0x3C, 0x7A, 0xAC, 0x35, 0x50, 0xE4, 0x4E, 
   0x4B, 0x53, 0x05, 0xAA, 0xEA, 0x6B, 0x8A, 0xAE, 
   0xA4, 0xD7, 0x36, 0x1E, 0x9D, 0x0F, 0x24, 0xFD, 
   0x1E, 0xC8, 0x6C, 0x72, 0x24, 0xE1, 0xCF, 0x9C, 
   0xF0, 0x46, 0x6B, 0x27, 0x97, 0xF3, 0x93, 0x0D, 
   0x48, 0x23, 0xA2, 0x6A, 0x3E, 0x59, 0xB2, 0x77, 
   0x89, 0xF6, 0x43, 0xC6, 0x4E, 0x2C, 0x6C, 0xB5, 
   0xC9, 0x15, 0x36, 0xC8, 0x33, 0x8E, 0x02, 0x06, 
   0xC8, 0x68, 0xF8, 0xF8, 0x99, 0x42, 0xCC, 0x18, 
   0x95, 0xFE, 0xBD, 0x56, 0x2D, 0x64, 0xAD, 0x62, 
   0x1A, 0x52, 0xD1, 0x97, 0x61, 0x79, 0xB9, 0x09, 
   0x6C, 0xB8, 0x2D, 0x6A, 0x85, 0xBD, 0x11, 0xC4, 
   0x87, 0x2B, 0xEC, 0x29, 0x70, 0x82, 0x32, 0xA3, 
   0x45, 0xB8, 0xC8, 0x69, 0xC8, 0x21, 0xBF, 0x40, 
   0xF9, 0x5F, 0xC5, 0xB6, 0x94, 0x0D, 0x3C, 0xE9, 
   0x2C, 0x25, 0x87, 0x46, 0xA2, 0xCD, 0x75, 0x45, 
   0x3A, 0x65, 0xE0, 0x2D, 0x5F, 0x7D, 0x7D, 0xD7, 
   0x92, 0x5B, 0xF0, 0x67, 0x06, 0x92, 0x75, 0xA9, 
   0xC0, 0xB4, 0x89, 0xD1, 0xEF, 0xEB, 0x5A, 0x75, 
   0xE1, 0x95, 0x16, 0x74, 0x47, 0x87, 0xBC, 0xDC, 
   0x43, 0xAF, 0xDD, 0x13, 0x87, 0x87, 0x60, 0x20, 
   0x80
};

extern A_UINT32 _inet_addr(A_CHAR *str);






int16_t userHttpsClientSendAndRcvResponse(int sslFd,char *sendStr,char *rcvStr)
{
	int16_t status = 0;
	int16_t ret = 0;
	do{
		if((sendStr == NULL) || (rcvStr == NULL))
		{
			printf("send https data:in put data points was null\r\n");
			break;
		}
		printf("send https:%s\r\n",sendStr);
		status = qca_ssl_write(sendStr, strlen(sendStr));
		if(status < 0)
		{
			printf("write https data was error\r\n");
			ret = -2;
			break;
		}
		if(qca_ssl_select(&sslFd) != 0)
		{
			status = qca_ssl_read(rcvStr, 1024);
			if (status <= 0)
			{
				printf("read https data was error\r\n");
				ret = -2;
				break;
			}
			printf("ssl recv:%s\n", rcvStr);
		}else{
			ret = -1;
			printf("the select was time out\r\n");
		}
	}while(0);
	return ret;
}

int16_t userWifiDevAuthorizeForHttps(int sslFd)
{
	int16_t res;
	char devInfo[200];
	char sendBuf[512];
	char rcvBuf[1024];
	char *jsStart;
	char *jsEnd;
	json_t *json = NULL;
	json_t *cjson_token,*rjson_token;
	memset(sendBuf, '\0', 512);
	memset(devInfo, '\0', 200);
	sprintf(devInfo,"{\"product_id\":\"%s\",\"mac\":\"%02X%02X%02X%02X%02X%02X\",\"authorize_code\":\"%s\"}\r\n\r\n",
        			XLINK_PRODUCT_ID,hmIotsDevInfo.wifiMac[0],hmIotsDevInfo.wifiMac[1],hmIotsDevInfo.wifiMac[2],
        			hmIotsDevInfo.wifiMac[3],hmIotsDevInfo.wifiMac[4],hmIotsDevInfo.wifiMac[5],gwStateForRealTime.xlinkAuCode);

        sprintf(sendBuf,"POST /v2/device_login HTTP/1.1\r\n%sContent-length:%d\r\n\r\n%s",HTTPS_BASIC_HEADER,strlen(devInfo),devInfo);
        res = userHttpsClientSendAndRcvResponse(sslFd,sendBuf,rcvBuf);
        do{
	        if(res < 0)
	        {
			printf("wifi dev auth was faild\r\n");
			break;
	        }else{
			if(strstr(rcvBuf,HTTPS_RCV_HEADER_OK) != NULL)
			{
				jsStart = (char*)strstr(rcvBuf,"\r\n{"); 
				if(jsStart == NULL)
				{
					printf("the Authorize data was not valid\r\n");
					res = -1;
					break;
				}else{
					jsStart = jsStart + 2;
					jsEnd = (char*)strstr(jsStart,"}\r\n0");
					if(jsEnd == NULL)
					{
						printf("the Authorize data was not valid\r\n");
						res = -1;
						break;
					}else{
						jsEnd[1] = '\0';
						json = json_parse_with_opts(jsStart,0,0);
						if(json == NULL)
						{
							printf("in http auth:this is not json data\r\n");
							res = -1;
							break;
						}
						cjson_token = json_get_object_item(json, "access_token");
						if(cjson_token == NULL)
						{
							printf("in http auth:this is not have access token\r\n");
							res = -1;
							break;
						}
						memset(gwStateForRealTime.accessToken,'\0',128);
						memcpy(gwStateForRealTime.accessToken,cjson_token->valuestring,strlen(cjson_token->valuestring));
						printf("the token:%s\r\n",gwStateForRealTime.accessToken);

						rjson_token = json_get_object_item(json, "refresh_token");
						if(rjson_token == NULL)
						{
							printf("in http auth:this is not have refresh token\r\n");
							res = -1;
							break;
						}
						memset(gwStateForRealTime.refreshToken,'\0',128);
						memcpy(gwStateForRealTime.refreshToken,rjson_token->valuestring,strlen(rjson_token->valuestring));

						printf("dev authorize ok\r\n");
					}
				}
			}else{
				res  = -1;
				printf("in auth:https response was error\r\n");
			}
	        }
        }while(0);
       if(json != NULL)
	{
		json_delete(json);
		json = NULL;
	}
        return res;
}

int16_t userWifiDevRefreshTokenForHttps(int sslFd)
{
	int16_t res;
	char *devInfo[200];
	char *sendBuf[512];
	char *rcvBuf[1024];
	char *jsStart;
	char *jsEnd;
	json_t *json = NULL;
	json_t *cjson_token,*rjson_token;
	memset(sendBuf, '\0', 512);
	memset(devInfo, '\0', 200);
	sprintf((char*)devInfo,"{\"refresh_token\":\"%s\"}\r\n\r\n",gwStateForRealTime.refreshToken);

        sprintf((char*)sendBuf,"POST /v2/device_refresh HTTP/1.1\r\n%sContent-length:%d\r\nAccess-Token:%s\r\n\r\n",HTTPS_BASIC_HEADER,strlen((char*)devInfo),gwStateForRealTime.accessToken);
        strcat_ext(sendBuf,devInfo);
        printf("fresh:len%d,data:%s\r\n",strlen((char*)sendBuf),sendBuf);
        res = userHttpsClientSendAndRcvResponse(sslFd,(char*)sendBuf,(char*)rcvBuf);
        do{
	        if(res < 0)
	        {
			printf("wifi dev auth was faild\r\n");
			break;
	        }else{
			if(strstr((char*)rcvBuf,HTTPS_RCV_HEADER_OK) != NULL)
			{
				jsStart = (char*)strstr((char*)rcvBuf,"\r\n{"); 
				if(jsStart == NULL)
				{
					printf("the Authorize data was not valid\r\n");
					res = -1;
					break;
				}else{
					jsStart = jsStart + 2;
					jsEnd = (char*)strstr(jsStart,"}\r\n0");
					if(jsEnd == NULL)
					{
						printf("the Authorize data was not valid\r\n");
						res = -1;
						break;
					}else{
						jsEnd[1] = '\0';
						json = json_parse_with_opts(jsStart,0,0);
						if(json == NULL)
						{
							printf("in http auth:this is not json data\r\n");
							res = -1;
							break;
						}
						cjson_token = json_get_object_item(json, "access_token");
						if(cjson_token == NULL)
						{
							printf("in http auth:this is not have access token\r\n");
							res = -1;
							break;
						}
						memset(gwStateForRealTime.accessToken,'\0',128);
						memcpy(gwStateForRealTime.accessToken,cjson_token->valuestring,strlen(cjson_token->valuestring) + 1);

						rjson_token = json_get_object_item(json, "refresh_token");
						if(rjson_token == NULL)
						{
							printf("in http auth:this is not have refresh token\r\n");
							res = -1;
							break;
						}
						memset(gwStateForRealTime.refreshToken,'\0',128);
						memcpy(gwStateForRealTime.refreshToken,rjson_token->valuestring,strlen(rjson_token->valuestring));
						printf("refresh token ok\r\n");
					}
				}
			}else{
				res  = -1;
				printf("in refresh:https response was error\r\n");
			}
	        }
        }while(0);
        if(json != NULL)
	{
		json_delete(json);
		json = NULL;
	}
        return res;
}

int16_t userCheckZCUpgradeForHttps(int sslFd)
{
	int16_t res;
	char devInfo[200];
	char sendBuf[1024];
	char rcvBuf[1024];
	char *jsStart;
	char *jsEnd;
	json_t *json = NULL;
	json_t *json_child;
	uint8_t tempVersion;
	if(gwStateForRealTime.zigbeeZcBasicInfo.zcSoftVersion == 0)
	{
		tempVersion = 1;
	}else{
		tempVersion = ((gwStateForRealTime.zigbeeZcBasicInfo.zcSoftVersion >> 4) & 0x0f) * 10 + (gwStateForRealTime.zigbeeZcBasicInfo.zcSoftVersion & 0x0f) - DEV_SOFT_VERSION_BASIC_VALUE;
	}
	memset(sendBuf, '\0', 1024);
	memset(devInfo, '\0', 200);
	sprintf(devInfo,"{\"product_id\":\"%s\",\"device_id\":\"%d\",\"type\":\"%d\",\"current_version\":\"%d\",\"identify\":\"%d\",}\r\n\r\n",
					XLINK_PRODUCT_ID,gwStateForRealTime.xlinkDevId,DEV_SOFT_FIRMWARE_TYPE_SUBDEV,tempVersion,DEV_SOFT_FIRMWARE_INDENTIFY_ZC);

        sprintf(sendBuf,"POST /v2/upgrade/firmware/check/%d HTTP/1.1\r\n%sContent-length:%d\r\nAccess-Token:%s\r\n\r\n",
        gwStateForRealTime.xlinkDevId,HTTPS_BASIC_HEADER,strlen(devInfo),gwStateForRealTime.accessToken);
        strcat_ext(sendBuf,devInfo);
        printf("fresh:len%d,data:%s\r\n",strlen(sendBuf),sendBuf);
        res = userHttpsClientSendAndRcvResponse(sslFd,sendBuf,rcvBuf);
        do{
	        if(res < 0)
	        {
			printf("wifi dev auth was faild\r\n");
			break;
	        }else{
			if(strstr(rcvBuf,HTTPS_RCV_HEADER_OK) != NULL)
			{
				jsStart = (char*)strstr(rcvBuf,"\r\n{"); 
				if(jsStart == NULL)
				{
					printf("the Authorize data was not valid for json start\r\n");
					res = -1;
					break;
				}else{
					jsStart = jsStart + 2;
					jsEnd = (char*)strstr(jsStart,"}\r\n0");
					if(jsEnd == NULL)
					{
						printf("the Authorize data was not validfor json end\r\n");
						res = -1;
						break;
					}else{
						jsEnd[1] = '\0';
						json = json_parse_with_opts(jsStart,0,0);
						if(json == NULL)
						{
							printf("in http auth:this is not json data\r\n");
							res = -1;
							break;
						}
						json_child = json->child;
						while(json_child != NULL)
						{
							if(strcmp_ext(json_child->string, "id") == 0)
							{
								printf("the upgrad task id:%s\r\n",json_child->valuestring);
							}else if(strcmp_ext(json_child->string, "product_id") == 0){
								printf("the upgrad product_id id:%s\r\n",json_child->valuestring);
							}else if(strcmp_ext(json_child->string, "name") == 0){
								printf("the upgrad name:%s\r\n",json_child->valuestring);
							}else if(strcmp_ext(json_child->string, "description") == 0){
								printf("the upgrad description:%s\r\n",json_child->valuestring);
							}else if(strcmp_ext(json_child->string, "from_version") == 0){
								printf("the upgrad from_version:%d\r\n",json_child->valueint);
							}else if(strcmp_ext(json_child->string, "target_version_url") == 0){
								memcpy(subOtaRam.subFileUrl,json_child->valuestring,strlen(json_child->valuestring));
								printf("the upgrad target_version_url:%s\r\n",json_child->valuestring);
							}else if(strcmp_ext(json_child->string, "target_version_md5") == 0){
								memcpy(subOtaRam.subFileMd5,json_child->valuestring,(strlen(json_child->valuestring) > 32? 32:strlen(json_child->valuestring)));
								printf("the upgrad target_version_md5:%s\r\n",json_child->valuestring);
							}else if(strcmp_ext(json_child->string, "target_version_size") == 0){
								subOtaRam.sufFileSize = json_child->valueint;
								printf("the upgrad target_version_size:%d\r\n",json_child->valueint);
							}else if(strcmp_ext(json_child->string, "type") == 0){
								printf("the upgrad type:%d\r\n",json_child->valueint);
							}else if(strcmp_ext(json_child->string, "identify") == 0){
								subOtaRam.subFileType = json_child->valueint;
								printf("the upgrad type:%d\r\n",json_child->valueint);
							}else if(strcmp_ext(json_child->string, "target_version") == 0){
								subOtaRam.subFileVersion = (((json_child->valueint + DEV_SOFT_VERSION_BASIC_VALUE) /10) << 4) + (json_child->valueint % 10);//�����װ汾�ż���basic value�͵���ʵ�ʰ汾��
								printf("the upgrad target_version:%d,contern to yzy version:%d\r\n",json_child->valueint,subOtaRam.subFileVersion);
							}
							json_child = json_child->next;
						}
						printf("check upgrade ok\r\n");
					}
				}
			}else{
				res  = -1;
				printf("in refresh:https response was error\r\n");
			}
	        }
        }while(0);
       if(json != NULL)
	{
		json_delete(json);
		json = NULL;
	}
        return res;
}


int16_t userCheckWifiUpgradeForHttps(int sslFd)
{
	int16_t res;
	char devInfo[200];
	char sendBuf[1024];
	char rcvBuf[1024];
	char *jsStart;
	char *jsEnd;
	json_t *json = NULL;
	json_t *json_child;
	uint8_t tempVersion;

	tempVersion = XLINK_SOFT_VERSION;
	memset(sendBuf, '\0', 1024);
	memset(devInfo, '\0', 200);
	sprintf(devInfo,"{\"product_id\":\"%s\",\"device_id\":\"%d\",\"type\":\"%d\",\"current_version\":\"%d\",\"identify\":\"%d\",}\r\n\r\n",
					XLINK_PRODUCT_ID,gwStateForRealTime.xlinkDevId,1,tempVersion,0);

        sprintf(sendBuf,"POST /v2/upgrade/firmware/check/%d HTTP/1.1\r\n%sContent-length:%d\r\nAccess-Token:%s\r\n\r\n",
        gwStateForRealTime.xlinkDevId,HTTPS_BASIC_HEADER,strlen(devInfo),gwStateForRealTime.accessToken);
        strcat_ext(sendBuf,devInfo);
        printf("fresh:len%d,data:%s\r\n",strlen(sendBuf),sendBuf);
        res = userHttpsClientSendAndRcvResponse(sslFd,sendBuf,rcvBuf);
        do{
	        if(res < 0)
	        {
			printf("wifi dev auth was faild\r\n");
			break;
	        }else{
			if(strstr(rcvBuf,HTTPS_RCV_HEADER_OK) != NULL)
			{
				jsStart = (char*)strstr(rcvBuf,"\r\n{"); 
				if(jsStart == NULL)
				{
					printf("the Authorize data was not valid for json start\r\n");
					res = -1;
					break;
				}else{
					jsStart = jsStart + 2;
					jsEnd = (char*)strstr(jsStart,"}\r\n0");
					if(jsEnd == NULL)
					{
						printf("the Authorize data was not validfor json end\r\n");
						res = -1;
						break;
					}else{
						jsEnd[1] = '\0';
						json = json_parse_with_opts(jsStart,0,0);
						if(json == NULL)
						{
							printf("in http auth:this is not json data\r\n");
							res = -1;
							break;
						}
						json_child = json->child;
						while(json_child != NULL)
						{
							if(strcmp_ext(json_child->string, "id") == 0)
							{
								printf("the upgrad task id:%s\r\n",json_child->valuestring);
							}else if(strcmp_ext(json_child->string, "product_id") == 0){
								printf("the upgrad product_id id:%s\r\n",json_child->valuestring);
							}else if(strcmp_ext(json_child->string, "name") == 0){
								printf("the upgrad name:%s\r\n",json_child->valuestring);
							}else if(strcmp_ext(json_child->string, "description") == 0){
								printf("the upgrad description:%s\r\n",json_child->valuestring);
							}else if(strcmp_ext(json_child->string, "from_version") == 0){
								printf("the upgrad from_version:%d\r\n",json_child->valueint);
							}else if(strcmp_ext(json_child->string, "target_version_url") == 0){
								memcpy(gwStateForRealTime.wifiSoftUpdate.softUrl,json_child->valuestring,strlen(json_child->valuestring));
								printf("the upgrad target_version_url:%s\r\n",json_child->valuestring);
							}else if(strcmp_ext(json_child->string, "target_version_md5") == 0){
								printf("the upgrad target_version_md5:%s\r\n",json_child->valuestring);
							}else if(strcmp_ext(json_child->string, "target_version_size") == 0){
								printf("the upgrad target_version_size:%d\r\n",json_child->valueint);
							}else if(strcmp_ext(json_child->string, "type") == 0){
								printf("the upgrad type:%d\r\n",json_child->valueint);
							}else if(strcmp_ext(json_child->string, "identify") == 0){
								printf("the upgrad type:%d\r\n",json_child->valueint);
							}else if(strcmp_ext(json_child->string, "target_version") == 0){
								if(json_child->string > tempVersion)
								{
									gwStateForRealTime.wifiSoftUpdate.haveNewestSoftWare = 1;
								}else{
									gwStateForRealTime.wifiSoftUpdate.appAllowSoftWareUpdate = 0;
									uint8_t parm[4];
									parm[0] = OTA_MODE_WIFI;
									parm[1] = HM_IOTS_SMALL_GW_TYPE & 0xff;
									parm[2] = (HM_IOTS_SMALL_GW_TYPE >> 8) & 0xff;
									parm[3] = 0;
									hmSetIotsEvent(HFZ_EVENT_SOURCE_OTA, 0, (void *)&parm,4);
								}
								printf("the upgrad target_version:%d\r\n",json_child->valueint);
							}
							json_child = json_child->next;
						}
						printf("check upgrade ok\r\n");
					}
				}
			}else{
				res  = -1;
				printf("in refresh:https response was error\r\n");
			}
	        }
        }while(0);
        if(json != NULL)
	{
		json_delete(json);
		json = NULL;
	}
        return res;
}


int16_t userUpdateGwNameToCloudeForHttps(int sslFd)
{
	int16_t res;
	char devInfo[200];
	char sendBuf[1024];
	char rcvBuf[1024];
	uint8_t tempVersion;

	memset(sendBuf, '\0', 1024);
	memset(devInfo, '\0', 200);
	sprintf(devInfo,"{\"name\":\"%s\"}\r\n\r\n",hmIotsDevInfo.gwName);

        sprintf(sendBuf,"PUT /v2/product/%s/device/%d HTTP/1.1\r\n%sContent-length:%d\r\nAccess-Token:%s",
        XLINK_PRODUCT_ID,gwStateForRealTime.xlinkDevId,HTTPS_BASIC_HEADER,strlen(devInfo),gwStateForRealTime.accessToken);
        strcat_ext(sendBuf,"\r\n\r\n");//���ڸ�sdk��sprintfֻ�ܲ�������С��256�ֽڵ��ַ��������Խ������ŵ��˵ڶ����ַ�����
        strcat_ext(sendBuf,devInfo);
        printf("fresh:len%d,data:%s\r\n",strlen(sendBuf),sendBuf);
        res = userHttpsClientSendAndRcvResponse(sslFd,sendBuf,rcvBuf);
        do{
	        if(res < 0)
	        {
			printf("wifi dev auth was faild\r\n");
			break;
	        }else{
			if(strstr(rcvBuf,HTTPS_RCV_HEADER_OK) != NULL)
			{
				printf("update the dev name to cloude success\r\n");
			}else{
				res  = -1;
				printf("in refresh:https response was error\r\n");
			}
	        }
        }while(0);
        return res;
}

json_t* userCheckNeedUpEchoDevInfoToBuff(void)
{
	json_t *json = json_create_object();
	json_t *json_child,*json_g,*json_d,*json_subd,*json_c,*json_avs;
	HM_GW_LIST *pgw_info=&gw_info;
	uint8_t tmEp1 = 0xff,tmEp2 = 0xff,tmEp3 = 0xff,tmEp4 = 0xff;
	uint8_t j;
	HM_DEV_RAM_LIST *pni= (HM_DEV_RAM_LIST *)pgw_info->pnode_list;
	char wifiMacString[15],subDevMacString[20],temp[30];
	char *tmName,*tmName1,tmName2[48];
	memset(wifiMacString,'\0',15);

	sprintf(wifiMacString,"%02X%02X%02X%02X%02X%02X",hmIotsDevInfo.wifiMac[0],hmIotsDevInfo.wifiMac[1],hmIotsDevInfo.wifiMac[2],
													hmIotsDevInfo.wifiMac[3],hmIotsDevInfo.wifiMac[4],hmIotsDevInfo.wifiMac[5]);

	

	json_add_item_to_object(json, "g", json_g = json_create_object());
	json_add_item_to_object(json, "d", json_d = json_create_object());
	
	json_add_item_to_object(json_g,"v",json_create_string("2"));
	json_add_item_to_object(json_g,"n",json_create_string(hmIotsDevInfo.gwName));
	json_add_item_to_object(json_g,"m",json_create_string(wifiMacString));
	json_add_item_to_object(json_g,"c",json_c = json_create_object());
	json_add_item_to_object(json_c,"avs",json_avs = json_create_object());
	json_add_item_to_object(json_avs,"v",json_create_number(1));
	json_add_item_to_object(json_avs,"p",json_create_string("json"));
	json_add_item_to_object(json_avs,"d",json_create_string("mac"));
	if (pni != NULL)
	{

		HM_DEV_RAM_LIST *pni_tmp = pni;
		do{
			switch(pni_tmp->devType)
			{
				case NODE_INFO_TYPE_LIGHT_RGB:
					printf("echo update for RGB\r\n");
					memset(subDevMacString,'\0',20);
					sprintf(subDevMacString,"%02X%02X%02X%02X%02X%02X%02X%02X",pni_tmp->mac[0],pni_tmp->mac[1],pni_tmp->mac[2],
											pni_tmp->mac[3],pni_tmp->mac[4],pni_tmp->mac[5],pni_tmp->mac[6],pni_tmp->mac[7]);
					json_add_item_to_object(json_d,subDevMacString,json_subd = json_create_object());
					json_add_item_to_object(json_subd,"n",json_create_string(pni_tmp->name));
					json_add_item_to_object(json_subd,"m",json_create_string(subDevMacString));
					json_add_item_to_object(json_subd,"c",json_c = json_create_object());
					json_add_item_to_object(json_c,"avs",json_avs = json_create_array());
					json_add_item_toarray(json_avs,json_create_number(0));
					json_add_item_toarray(json_avs,json_create_number(1));
					json_add_item_toarray(json_avs,json_create_number(0x10));
					json_add_item_toarray(json_avs,json_create_number(0x11));
					json_add_item_toarray(json_avs,json_create_number(0x12));
					json_add_item_to_object(json_subd,"v",json_create_string("1.0"));
					json_add_item_to_object(json_subd,"ex",json_child= json_create_object());
					json_add_item_to_object(json_child,"type", json_create_number(1));
					
					break;
				case NODE_INFO_TYPE_AI1SW:
				case NODE_INFO_TYPE_AI2SW:
				case NODE_INFO_TYPE_AI3SW:
					printf("echo update for switch\r\n");
					for(j = 0; j < pni->epNum; j ++)
					{
						if(pni->epList[j].ep == 1)
						{
							tmEp1 = j;
						}else if(pni->epList[j].ep == 2){
							tmEp2 = j;
						}else if(pni->epList[j].ep == 3){
							tmEp3 = j;
						}
					}

					if(tmEp1 != 0xff)
					{
						memset(tmName2,'\0',48);
						memcpy(tmName2,pni_tmp->name,strlen((char*)pni_tmp->name));
						printf("the ep num:%d\r\n",pni->epNum);
						if(pni->epNum > 1)
						{
							tmName1 = strstr_ext(tmName2,"&^$");
							if(tmName1 == NULL)
							{
								printf("the name was not valid\r\n");
								break;
							}
							tmName1[0] = '\0';
							
						}
						tmName = tmName2;
						memset(subDevMacString,'\0',20);
						sprintf(subDevMacString,"%02X%02X%02X%02X%02X%02X%02X%02X%02X",pni_tmp->mac[0],pni_tmp->mac[1],pni_tmp->mac[2],
												pni_tmp->mac[3],pni_tmp->mac[4],pni_tmp->mac[5],pni_tmp->mac[6],pni_tmp->mac[7],1);
						json_add_item_to_object(json_d,subDevMacString,json_subd = json_create_object());
						json_add_item_to_object(json_subd,"n",json_create_string(tmName));
						json_add_item_to_object(json_subd,"m",json_create_string(subDevMacString));
						json_add_item_to_object(json_subd,"c",json_c = json_create_object());
						json_add_item_to_object(json_c,"avs",json_avs = json_create_array());
						json_add_item_toarray(json_avs,json_create_number(0));
						json_add_item_toarray(json_avs,json_create_number(1));
						json_add_item_to_object(json_subd,"v",json_create_string("1.0"));
						json_add_item_to_object(json_subd,"ex",json_child= json_create_object());
						json_add_item_to_object(json_child,"type", json_create_number(1));
					}

					if(tmEp2 != 0xff)
					{
						if(tmName1 == NULL)
						{
							printf("invalid dev name\r\n");
							break;
						}
						tmName = &tmName1[3];
						if(pni->epNum > 2)
						{
							tmName1 = strstr_ext(tmName,"&^$");
							if(tmName1 == NULL)
							{
								printf("the name was not valid\r\n");
								break;
							}
							tmName1[0] = '\0';
							
						}
						memset(subDevMacString,'\0',20);
						sprintf(subDevMacString,"%02X%02X%02X%02X%02X%02X%02X%02X%02X",pni_tmp->mac[0],pni_tmp->mac[1],pni_tmp->mac[2],
												pni_tmp->mac[3],pni_tmp->mac[4],pni_tmp->mac[5],pni_tmp->mac[6],pni_tmp->mac[7],2);
						json_add_item_to_object(json_d,subDevMacString,json_subd = json_create_object());
						json_add_item_to_object(json_subd,"n",json_create_string(tmName));
						json_add_item_to_object(json_subd,"m",json_create_string(subDevMacString));
						json_add_item_to_object(json_subd,"c",json_c = json_create_object());
						json_add_item_to_object(json_c,"avs",json_avs = json_create_array());
						json_add_item_toarray(json_avs,json_create_number(0));
						json_add_item_toarray(json_avs,json_create_number(1));
						json_add_item_to_object(json_subd,"v",json_create_string("1.0"));
						json_add_item_to_object(json_subd,"ex",json_child= json_create_object());
						json_add_item_to_object(json_child,"type", json_create_number(1));
					}

					if(tmEp3 != 0xff)
					{
						if(tmName1 == NULL)
						{
							printf("invalid dev name\r\n");
							break;
						}
						tmName = &tmName1[3];

						memset(subDevMacString,'\0',20);
						sprintf(subDevMacString,"%02X%02X%02X%02X%02X%02X%02X%02X%02X",pni_tmp->mac[0],pni_tmp->mac[1],pni_tmp->mac[2],
												pni_tmp->mac[3],pni_tmp->mac[4],pni_tmp->mac[5],pni_tmp->mac[6],pni_tmp->mac[7],3);
						json_add_item_to_object(json_d,subDevMacString,json_subd = json_create_object());
						json_add_item_to_object(json_subd,"n",json_create_string(tmName));
						json_add_item_to_object(json_subd,"m",json_create_string(subDevMacString));
						json_add_item_to_object(json_subd,"c",json_c = json_create_object());
						json_add_item_to_object(json_c,"avs",json_avs = json_create_array());
						json_add_item_toarray(json_avs,json_create_number(0));
						json_add_item_toarray(json_avs,json_create_number(1));
						json_add_item_to_object(json_subd,"v",json_create_string("1.0"));
						json_add_item_to_object(json_subd,"ex",json_child= json_create_object());
						json_add_item_to_object(json_child,"type", json_create_number(1));
					}
					
					break;
				case NODE_INFO_TYPE_RELAY_CONTROL:
					printf("echo update for relay\r\n");
					break;
				case NODE_INFO_TYPE_AI_SOCKET_USB:
					printf("echo update for usb plug\r\n");
					for(j = 0; j < pni->epNum; j ++)
					{
						if(pni->epList[j].ep == 1)
						{
							tmEp1 = j;
						}else if(pni->epList[j].ep == 2){
							tmEp2 = j;
						}
					}

					if(tmEp1 != 0xff)
					{
						memset(tmName2,'\0',48);
						memcpy(tmName2,pni_tmp->name,strlen((char*)pni_tmp->name));
						if(pni->epNum > 1)
						{
							tmName1 = strstr_ext(tmName2,"&^$");
							if(tmName1 == NULL)
							{
								printf("the name was not valid\r\n");
								break;
							}
							tmName1[0] = '\0';
							
						}
						tmName = tmName2;
						memset(subDevMacString,'\0',20);
						sprintf(subDevMacString,"%02X%02X%02X%02X%02X%02X%02X%02X%02X",pni_tmp->mac[0],pni_tmp->mac[1],pni_tmp->mac[2],
												pni_tmp->mac[3],pni_tmp->mac[4],pni_tmp->mac[5],pni_tmp->mac[6],pni_tmp->mac[7],1);
						json_add_item_to_object(json_d,subDevMacString,json_subd = json_create_object());
						json_add_item_to_object(json_subd,"n",json_create_string(tmName));
						json_add_item_to_object(json_subd,"m",json_create_string(subDevMacString));
						json_add_item_to_object(json_subd,"c",json_c = json_create_object());
						json_add_item_to_object(json_c,"avs",json_avs = json_create_array());
						json_add_item_toarray(json_avs,json_create_number(0));
						json_add_item_toarray(json_avs,json_create_number(1));
						json_add_item_to_object(json_subd,"v",json_create_string("1.0"));
						json_add_item_to_object(json_subd,"ex",json_child= json_create_object());
						json_add_item_to_object(json_child,"type", json_create_number(1));
					}

					if(tmEp2 != 0xff)
					{
						if(tmName1 == NULL)
						{
							printf("invalid dev name\r\n");
							break;
						}
						tmName = &tmName1[3];
				
						memset(subDevMacString,'\0',20);
						sprintf(subDevMacString,"%02X%02X%02X%02X%02X%02X%02X%02X%02X",pni_tmp->mac[0],pni_tmp->mac[1],pni_tmp->mac[2],
												pni_tmp->mac[3],pni_tmp->mac[4],pni_tmp->mac[5],pni_tmp->mac[6],pni_tmp->mac[7],2);
						json_add_item_to_object(json_d,subDevMacString,json_subd = json_create_object());
						json_add_item_to_object(json_subd,"n",json_create_string(tmName));
						json_add_item_to_object(json_subd,"m",json_create_string(subDevMacString));
						json_add_item_to_object(json_subd,"c",json_c = json_create_object());
						json_add_item_to_object(json_c,"avs",json_avs = json_create_array());
						json_add_item_toarray(json_avs,json_create_number(0));
						json_add_item_toarray(json_avs,json_create_number(1));
						json_add_item_to_object(json_subd,"v",json_create_string("1.0"));
						json_add_item_to_object(json_subd,"ex",json_child= json_create_object());
						json_add_item_to_object(json_child,"type", json_create_number(1));
					}

					break;
				case NODE_INFO_TYPE_AI_SOCKET_MEASURE:
					printf("echo update for smart plug\r\n");
					memset(subDevMacString,'\0',20);
					sprintf(subDevMacString,"%02X%02X%02X%02X%02X%02X%02X%02X",pni_tmp->mac[0],pni_tmp->mac[1],pni_tmp->mac[2],
											pni_tmp->mac[3],pni_tmp->mac[4],pni_tmp->mac[5],pni_tmp->mac[6],pni_tmp->mac[7]);
					json_add_item_to_object(json_d,subDevMacString,json_subd = json_create_object());
					json_add_item_to_object(json_subd,"n",json_create_string(pni_tmp->name));
					json_add_item_to_object(json_subd,"m",json_create_string(subDevMacString));
					json_add_item_to_object(json_subd,"c",json_c = json_create_object());
					json_add_item_to_object(json_c,"avs",json_avs = json_create_array());
					json_add_item_toarray(json_avs,json_create_number(0));
					json_add_item_toarray(json_avs,json_create_number(1));
					json_add_item_to_object(json_subd,"v",json_create_string("1.0"));
					json_add_item_to_object(json_subd,"ex",json_child= json_create_object());
					json_add_item_to_object(json_child,"type", json_create_number(1));
					break;
				case NODE_INFO_TYPE_WARNING_DEV: //reserve
					printf("echo output for wd\r\n");
					//memset(subDevMacString,'\0',20);
					//sprintf(subDevMacString,"%02X%02X%02X%02X%02X%02X%02X%02X",pni_tmp->mac[0],pni_tmp->mac[1],pni_tmp->mac[2],
					//						pni_tmp->mac[3],pni_tmp->mac[4],pni_tmp->mac[5],pni_tmp->mac[6],pni_tmp->mac[7]);
					//json_add_item_to_object(json_d,subDevMacString,json_subd = json_create_object());
					//json_add_item_to_object(json_subd,"n",json_create_string(pni_tmp->name));
					//json_add_item_to_object(json_subd,"m",json_create_string(subDevMacString));
					//json_add_item_to_object(json_subd,"c",json_c = json_create_object());
					//json_add_item_to_object(json_c,"avs",json_avs = json_create_array());
					//json_add_item_toarray(json_avs,json_create_number(0));
					//json_add_item_toarray(json_avs,json_create_number(1));
					//json_add_item_to_object(json_subd,"v",json_create_string("1.0"));
					//json_add_item_to_object(json_subd,"ex",json_child= json_create_object());
					//json_add_item_to_object(json_child,"type", json_create_number(1));

					break;
				case NODE_INFO_TYPE_CUSTOM_RGB_AREA:
					printf("echo update for cut rgb\r\n");
					for(j = 0; j < pni->epNum; j ++)
					{
						if(pni->epList[j].ep == 1)
						{
							tmEp1 = j;
						}else if(pni->epList[j].ep == 2){
							tmEp2 = j;
						}else if(pni->epList[j].ep == 3){
							tmEp3 = j;
						}else if(pni->epList[j].ep == 4){
							tmEp4 = j;
						}
					}

					if(tmEp1 != 0xff)
					{
						memset(tmName2,'\0',48);
						memcpy(tmName2,pni_tmp->name,strlen((char*)pni_tmp->name));
						if(pni->epNum > 1)
						{
							tmName1 = strstr_ext(tmName2,"&^$");
							if(tmName1 == NULL)
							{
								printf("the name was not valid\r\n");
								break;
							}
							tmName1[0] = '\0';
							
						}
						tmName = tmName2;
						memset(subDevMacString,'\0',20);
						sprintf(subDevMacString,"%02X%02X%02X%02X%02X%02X%02X%02X%02X",pni_tmp->mac[0],pni_tmp->mac[1],pni_tmp->mac[2],
												pni_tmp->mac[3],pni_tmp->mac[4],pni_tmp->mac[5],pni_tmp->mac[6],pni_tmp->mac[7],1);
						json_add_item_to_object(json_d,subDevMacString,json_subd = json_create_object());
						json_add_item_to_object(json_subd,"n",json_create_string(tmName));
						json_add_item_to_object(json_subd,"m",json_create_string(subDevMacString));
						json_add_item_to_object(json_subd,"c",json_c = json_create_object());
						json_add_item_to_object(json_c,"avs",json_avs = json_create_array());
						json_add_item_toarray(json_avs,json_create_number(0));
						json_add_item_toarray(json_avs,json_create_number(1));
						json_add_item_toarray(json_avs,json_create_number(0x10));
						json_add_item_toarray(json_avs,json_create_number(0x11));
						json_add_item_toarray(json_avs,json_create_number(0x12));
						json_add_item_to_object(json_subd,"v",json_create_string("1.0"));
						json_add_item_to_object(json_subd,"ex",json_child= json_create_object());
						json_add_item_to_object(json_child,"type", json_create_number(1));
					}

					if(tmEp2 != 0xff)
					{
						if(tmName1 == NULL)
						{
							printf("invalid dev name\r\n");
							break;
						}
						tmName = tmName1[3];
						if(pni->epNum > 2)
						{
							tmName1 = strstr_ext(tmName,"&^$");
							if(tmName1 == NULL)
							{
								printf("the name was not valid\r\n");
								break;
							}
							tmName1[0] = '\0';
							
						}
						memset(subDevMacString,'\0',20);
						sprintf(subDevMacString,"%02X%02X%02X%02X%02X%02X%02X%02X%02X",pni_tmp->mac[0],pni_tmp->mac[1],pni_tmp->mac[2],
												pni_tmp->mac[3],pni_tmp->mac[4],pni_tmp->mac[5],pni_tmp->mac[6],pni_tmp->mac[7],2);
						json_add_item_to_object(json_d,subDevMacString,json_subd = json_create_object());
						json_add_item_to_object(json_subd,"n",json_create_string(tmName));
						json_add_item_to_object(json_subd,"m",json_create_string(subDevMacString));
						json_add_item_to_object(json_subd,"c",json_c = json_create_object());
						json_add_item_to_object(json_c,"avs",json_avs = json_create_array());
						json_add_item_toarray(json_avs,json_create_number(0));
						json_add_item_toarray(json_avs,json_create_number(1));
						json_add_item_toarray(json_avs,json_create_number(0x10));
						json_add_item_toarray(json_avs,json_create_number(0x11));
						json_add_item_toarray(json_avs,json_create_number(0x12));
						json_add_item_to_object(json_subd,"v",json_create_string("1.0"));
						json_add_item_to_object(json_subd,"ex",json_child= json_create_object());
						json_add_item_to_object(json_child,"type", json_create_number(1));
					}

					if(tmEp3 != 0xff)
					{
						if(tmName1 == NULL)
						{
							printf("invalid dev name\r\n");
							break;
						}
						tmName = tmName1[3];
						if(pni->epNum > 3)
						{
							tmName1 = strstr_ext(tmName,"&^$");
							if(tmName1 == NULL)
							{
								printf("the name was not valid\r\n");
								break;
							}
							tmName1[0] = '\0';
							
						}

						memset(subDevMacString,'\0',20);
						sprintf(subDevMacString,"%02X%02X%02X%02X%02X%02X%02X%02X%02X",pni_tmp->mac[0],pni_tmp->mac[1],pni_tmp->mac[2],
												pni_tmp->mac[3],pni_tmp->mac[4],pni_tmp->mac[5],pni_tmp->mac[6],pni_tmp->mac[7],3);
						json_add_item_to_object(json_d,subDevMacString,json_subd = json_create_object());
						json_add_item_to_object(json_subd,"n",json_create_string(tmName));
						json_add_item_to_object(json_subd,"m",json_create_string(subDevMacString));
						json_add_item_to_object(json_subd,"c",json_c = json_create_object());
						json_add_item_to_object(json_c,"avs",json_avs = json_create_array());
						json_add_item_toarray(json_avs,json_create_number(0));
						json_add_item_toarray(json_avs,json_create_number(1));
						json_add_item_toarray(json_avs,json_create_number(0x10));
						json_add_item_toarray(json_avs,json_create_number(0x11));
						json_add_item_toarray(json_avs,json_create_number(0x12));
						json_add_item_to_object(json_subd,"v",json_create_string("1.0"));
						json_add_item_to_object(json_subd,"ex",json_child= json_create_object());
						json_add_item_to_object(json_child,"type", json_create_number(1));
					}

					if(tmEp4 != 0xff)
					{
						if(tmName1 == NULL)
						{
							printf("invalid dev name\r\n");
							break;
						}
						tmName = tmName1[3];

						memset(subDevMacString,'\0',20);
						sprintf(subDevMacString,"%02X%02X%02X%02X%02X%02X%02X%02X%02X",pni_tmp->mac[0],pni_tmp->mac[1],pni_tmp->mac[2],
												pni_tmp->mac[3],pni_tmp->mac[4],pni_tmp->mac[5],pni_tmp->mac[6],pni_tmp->mac[7],4);
						json_add_item_to_object(json_d,subDevMacString,json_subd = json_create_object());
						json_add_item_to_object(json_subd,"n",json_create_string(tmName));
						json_add_item_to_object(json_subd,"m",json_create_string(subDevMacString));
						json_add_item_to_object(json_subd,"c",json_c = json_create_object());
						json_add_item_to_object(json_c,"avs",json_avs = json_create_array());
						json_add_item_toarray(json_avs,json_create_number(0));
						json_add_item_toarray(json_avs,json_create_number(1));
						json_add_item_toarray(json_avs,json_create_number(0x10));
						json_add_item_toarray(json_avs,json_create_number(0x11));
						json_add_item_toarray(json_avs,json_create_number(0x12));
						json_add_item_to_object(json_subd,"v",json_create_string("1.0"));
						json_add_item_to_object(json_subd,"ex",json_child= json_create_object());
						json_add_item_to_object(json_child,"type", json_create_number(1));
					}
					
					break;
				default:
					printf("the echo output was unknow dev\r\n");
					break;
			}
			pni_tmp=(HM_DEV_RAM_LIST *)pni_tmp->pnext_dev_info;
		}while (pni_tmp != pni);
	}

	for(j = 0; j < MAX_SCENE_NUM; j ++)
	{
		if(devNumInfo.sceneIndex[j] != INVALID_SCENE_LINKAGE_INDEX)
		{
			//if(pgw_info->scene_list[j].valid == 1)
			{
				memset(subDevMacString,'\0',20);
				sprintf(subDevMacString,"%s%02X",wifiMacString,pgw_info->scene_list[j].sceneId);
				json_add_item_to_object(json_d,subDevMacString,json_subd = json_create_object());
				json_add_item_to_object(json_subd,"n",json_create_string(pgw_info->scene_list[j].sceneName));
				json_add_item_to_object(json_subd,"m",json_create_string(subDevMacString));
				json_add_item_to_object(json_subd,"c",json_c = json_create_object());
				json_add_item_to_object(json_c,"avs",json_avs = json_create_array());
				json_add_item_toarray(json_avs,json_create_number(0));
				json_add_item_to_object(json_subd,"v",json_create_string("1.0"));
				json_add_item_to_object(json_subd,"ex",json_child= json_create_object());
				json_add_item_to_object(json_child,"type", json_create_number(1));
			}
		}
	}

	return json;
}


int16_t userUpgradeEchoDevForHttps(int sslFd)
{
	int16_t res;
	char *devInfo = NULL;
	char *sendBuf;
	char rcvBuf[1024];
	char *jsStart;
	char *jsEnd;
	char temp[30];
	json_t *json;


	json = userCheckNeedUpEchoDevInfoToBuff();

	if(json == NULL)
	{
		printf("the echo update dev info was null\r\n");
		return 0;
	}

	devInfo = json_PrintUnformatted(json);

	if(devInfo == NULL)
	{
		printf("creat devinfo was null in update echo dev\r\n");
		if(json != NULL)
		{
			json_delete(json);
			json = NULL;
		}
		return 0;
	}

	 sendBuf = mem_alloc(strlen(devInfo) + 512);
	 if(sendBuf == NULL)
	 {
	 	printf("mem alloc was error\r\n");
		if(json != NULL)
		{
			json_delete(json);
			json = NULL;
		}
		if(devInfo != NULL)
		{
			mem_free(devInfo);
			devInfo = NULL;
		}
		return 0;
	 }

        sprintf(sendBuf,"POST /v2/product/%s/device/%d/subdevices HTTP/1.1\r\n%sAccess-Token:%s\r\n",
        XLINK_PRODUCT_ID,gwStateForRealTime.xlinkDevId,HTTPS_BASIC_HEADER,gwStateForRealTime.accessToken);

        // sprintf(sendBuf,"POST /v2/product/%s/device/%d/subdevices HTTP/1.1\r\n%sAccess-Token:%s\r\n\r\n",
       // XLINK_PRODUCT_ID,gwStateForRealTime.xlinkDevId,HTTPS_BASIC_HEADER,gwStateForRealTime.accessToken);
        memset(temp,'\0',30);
        sprintf(temp,"Content-length:%d\r\n\r\n",strlen(devInfo));
	 strcat_ext(sendBuf,temp);
        strcat_ext(sendBuf,devInfo);
        printf("fresh:len%d,data:%s\r\n",strlen(sendBuf),sendBuf);
        res = userHttpsClientSendAndRcvResponse(sslFd,sendBuf,rcvBuf);
        do{
	        if(res < 0)
	        {
			printf("wifi dev auth was faild\r\n");
			break;
	        }else{
			if(strstr(rcvBuf,HTTPS_RCV_HEADER_OK) != NULL)
			{
				printf("update dev to echo was success\r\n");
			}else{
				res  = -1;
				printf("in refresh:https response was error\r\n");
			}
	        }
        }while(0);
        if(json != NULL)
	{
		json_delete(json);
		json = NULL;
	}
	if(devInfo != NULL)
	{
		mem_free(devInfo);
		devInfo = NULL;
	}
	if(sendBuf != NULL)
	{
		mem_free(sendBuf);
		sendBuf = NULL;		
	}
       return res;
}

void qca_ssl_client_func(void)
{
	int fd,server_ip;
	int res;
	printf("ssl start\r\n");
	int state = 0;
	uint16_t reFlashNum;
	uint8_t connectNum;
	uint8_t echoUpdateNum = 0;
	uint8_t httpState = HTTPS_STATE_INIT;
	static uint8_t cpLastHttpState = HTTPS_STATE_INIT;
	gwStateForRealTime.echoNeedUpdateFlag = 1;

	while(1)
	{
		switch(httpState)
		{
			case HTTPS_STATE_INIT:
				if((gwStateForRealTime.xlinkDevId != 0) && (gwStateForRealTime.httpsConnect == 0))
				{
					qca_ssl_client_start((A_UINT8 *)calist_cert1, sizeof(calist_cert1));
					httpState = HTTPS_STATE_CONNECTING;
					printf("start https event\r\n");
				}
				break;
			case HTTPS_STATE_CONNECTING:
				fd = qca_ssl_socket();

				qcom_dnsc_get_host_by_name(SERVER_IP, &server_ip);
				qcom_tcp_conn_timeout(5);//��ֹconnect ����
				res = qca_ssl_connect(&fd,server_ip, SERVER_PORT);
				//res = qca_ssl_connect(&fd, _inet_addr(SERVER_IP), SERVER_PORT);

				printf("res:%d, A_ERROR:%d, A_OK:%d\n", res, A_ERROR, A_OK);
				if (res != A_ERROR)
				{
					printf("break:%d\n", res);
					httpState = HTTPS_STATE_AUTHORIZE;
					gwStateForRealTime.httpsConnect = 1;
				}else{
					printf("https connect error\r\n");
					connectNum ++;
					if(connectNum > 10)
					{
						connectNum = 0;
						httpState = HTTPS_STATE_ERROR_CLOSE;
					}else{
						qca_ssl_close(&fd);//��ֹ�����Ӵ������˺�error�е��ظ�close
					}
				}
				break;
			case HTTPS_STATE_AUTHORIZE:
				if(userWifiDevAuthorizeForHttps(fd) < 0)
				{
					httpState = HTTPS_STATE_ERROR_CLOSE;
				}else{
					httpState = HTTPS_STATE_FIRST_FLASH_TOKEN;
				}	
				break;
			case HTTPS_STATE_FIRST_FLASH_TOKEN:
				if(0)//(userWifiDevRefreshTokenForHttps(fd) < 0)
				{
					httpState = HTTPS_STATE_ERROR_CLOSE;
				}else{
					if(otaStationInfo.needDownloadImageFromCloud == 1)//�����ж��Ƿ���Ҫ���ƶ�����zc�̼�
					{//ZC�̼���Чʱ�Ż����β������Է��̼��𻵺��޷���������
						httpState = HTTPS_STATE_SCAN_ZC_NEW_SOFTVERSION;
						otaStationInfo.needDownloadImageFromCloud = 0;
					}else{
						httpState = HTTPS_STATE_REFLASH_TOKEN;
						reFlashNum = HTTPS_REFLASH_TIMER;
					}
				}
				break;
			case HTTPS_STATE_REFLASH_TOKEN:
				reFlashNum ++;
				if(reFlashNum >= 60)//HTTPS_REFLASH_TIMER
				{
					 reFlashNum = 0;
					if(userWifiDevRefreshTokenForHttps(fd) < 0)
					{
						httpState = HTTPS_STATE_ERROR_CLOSE;
						break;
					}else{
						if(cpLastHttpState != HTTPS_STATE_INIT)
						{
							httpState = cpLastHttpState;
							cpLastHttpState = HTTPS_STATE_INIT;
						}else{
							httpState = HTTPS_STATE_IDLE;
						}
					}
				}
				break;
			case HTTPS_STATE_SCAN_ZC_NEW_SOFTVERSION:
				res = userCheckZCUpgradeForHttps(fd);
				if(res >= 0)
				{
					//��ȡ������sub firmware��Ϣ�󣬱ȶ��Ƿ���Ҫ���ظù̼�
					//�̼�������wifiota C�ļ���
					userCheckOtaSubFirmware();
				}
				if(res == -2){
					httpState = HTTPS_STATE_ERROR_CLOSE;
					cpLastHttpState = HTTPS_STATE_SCAN_ZC_NEW_SOFTVERSION;
				}else{
					httpState = HTTPS_STATE_IDLE;
				}
				break;
			case HTTPS_STATE_SCAN_WIFI_NEW_SOFTVERSION:
				res = userCheckWifiUpgradeForHttps(fd);
				if(res == -2){
					httpState = HTTPS_STATE_ERROR_CLOSE;
					cpLastHttpState = HTTPS_STATE_SCAN_WIFI_NEW_SOFTVERSION;
				}else{
					httpState = HTTPS_STATE_IDLE;
				}
				break;
			case HTTPS_STATE_UPDATE_DEV_INFO_TO_CLOUD_FOR_ECHO:
				res = userUpgradeEchoDevForHttps(fd);
				if(res <0)
				{
					echoUpdateNum ++;
					if(echoUpdateNum >= 3)//�����ϴ�ʧ�ܺ��˳��ϴ�
					{
						echoUpdateNum = 0;
						httpState = HTTPS_STATE_IDLE;
					}else{
						httpState = HTTPS_STATE_ERROR_CLOSE;
						cpLastHttpState = HTTPS_STATE_UPDATE_DEV_INFO_TO_CLOUD_FOR_ECHO;
					}
				}else{
					echoUpdateNum = 0;
					httpState = HTTPS_STATE_IDLE;
				}
				break;
			/*case HTTPS_STATE_UPDATE_GW_NAME_TO_CLOUD://�ϵ��һ����������ִ��һ�Σ���������ִ��
				printf("update gw name to cloud\r\n");//�����豸�˵Ľӿڣ����ô˽ӿ�һֱ��ʾaccess-token����
				res = userUpdateGwNameToCloudeForHttps(fd);
				if(res < 0){
					httpState = HTTPS_STATE_ERROR_CLOSE;
					cpLastHttpState = HTTPS_STATE_UPDATE_GW_NAME_TO_CLOUD;
				}else{
					httpState = HTTPS_STATE_IDLE;
				}
				break;*/
			case HTTPS_STATE_ERROR_CLOSE:
				printf("close https ,https communication error\r\n");
				gwStateForRealTime.httpsConnect = 0;
				httpState = HTTPS_STATE_INIT;
				qca_ssl_close(&fd);
				qca_ssl_client_stop();
				break;
			case HTTPS_STATE_IDLE:
				printf("https idel\r\n");
				if(gwStateForRealTime.httpsConnect != 0)
				{
					printf("close https ,https event complet,idel\r\n");
					gwStateForRealTime.httpsConnect = 0;
					qca_ssl_close(&fd);
					qca_ssl_client_stop();
				}
				break;
			default:
				printf("close https ,https communication default\r\n");
				gwStateForRealTime.httpsConnect = 0;
				httpState = HTTPS_STATE_INIT;
				qca_ssl_close(&fd);
				qca_ssl_client_stop();
				break;
		}
		if(HTTPS_STATE_IDLE == httpState)
		{
			if(gwStateForRealTime.zigbeeZcBasicInfo.appAllowSoftWareUpdate == 1)
			{
				httpState = HTTPS_STATE_SCAN_ZC_NEW_SOFTVERSION;
				gwStateForRealTime.zigbeeZcBasicInfo.appAllowSoftWareUpdate = 0;
			}else if((gwStateForRealTime.wifiSoftUpdate.appAllowSoftWareUpdate == 1) && (gwStateForRealTime.wifiSoftUpdate.haveNewestSoftWare == 0)){
				httpState = HTTPS_STATE_SCAN_WIFI_NEW_SOFTVERSION;
				//gwStateForRealTime.wifiSoftUpdate.appAllowSoftWareUpdate = 0;
			}else if(gwStateForRealTime.echoNeedUpdateFlag == 1){
				httpState = HTTPS_STATE_UPDATE_DEV_INFO_TO_CLOUD_FOR_ECHO;
				gwStateForRealTime.echoNeedUpdateFlag = 0;
			}
		}
		qcom_thread_msleep(2000);
	}
	
	qca_task_exit(); 
}




HTTPSSENDBUF httpsSendBuf[MAX_HTTPS_SEND_BUF_NUM];



void userFillHttpsBufForDev(uint8_t subDevType,uint8_t *data,uint8_t cmd)
{
	uint8_t i,bufIndex = 0xff;
	uint16_t dataLen = 0;
	if((cmd == HTTPS_ADD_DEV_CMD) || (cmd == HTTPS_UPDATE_DEV_CMD))
	{
		if(data == NULL)
		{
			printf("in fill https buf:the data was null\r\n");
			return;
		}
		dataLen = strlen(data);
		if(dataLen > MAX_HTTPS_SEND_BUF_LEN)
		{
			printf("in fill https buf:the data was too long\r\n");
			return;
		}
	}else if(cmd != HTTPS_DELETE_DEV_CMD){
		printf("in fill https buf:unknow https cmd\r\n");
		return;
	}
	for(i = 0; i < MAX_HTTPS_SEND_BUF_NUM; i ++)
	{
		if(httpsSendBuf[i].validFlag == 0)
		{
			bufIndex = i;
			break;
		}
	}
	if(bufIndex != 0xff)
	{
		httpsSendBuf[bufIndex].subDevType = subDevType;
		httpsSendBuf[bufIndex].sysDevType = HM_IOTS_SMALL_GW_TYPE;
		httpsSendBuf[bufIndex].cmdMode = cmd;
		if(dataLen != 0)
		{
			memcpy(httpsSendBuf[bufIndex].data,data,dataLen);
		}
		httpsSendBuf[bufIndex].validFlag = 1;
	}else{
		printf("in fill https buf:not have free https buf\r\n");
		return;
	}
}

/**
* @fn void get_cb(unsigned char *buf, unsigned int len)
* @brief callback
* @param 
* @return 
*/
void get_cb(unsigned char *buf, unsigned int len)
{
    unsigned int i;

	printf("<Test>get cb.\n");
    for(i = 0; i < len; i++){
        printf("%c", buf[i]);
    }
}

/**
* @fn void post_cb(unsigned char **send_buf, unsigned int *buf_len)
* @brief post callback
* @param **send_buf send buf
* @param *buf_len buf length
* @return none
*/
void post_cb(unsigned char **send_buf, unsigned int *buf_len)
{

}

/**
* @fn void test_qca_http_client(void)
* @brief http client test
* @param 
* @return 
*/
void qca_http_client_test(void)
{
   int ret = 0;
    http_client_header_t header = {
        "application/json",
        KEEP_ALIVE,
    };

    ret = http_client_startup((unsigned char *)"https://console.heiman.cn/v2/device_active", 443);
    if(ret == 0)
	{
        printf("Http client start success\r\n");

      /*  ret = http_client_get("/get", NULL, get_cb);
        if(ret == 0)
		{
            printf("\r\nHttp client GET success\r\n");
        }
		else
		{
            printf("Http client GET failed\r\n");
        }


        ret = http_client_get("/get", &header, get_cb);
        if(ret == 0)
		{
            printf("\r\nHttp client GET success\r\n");
        }
		else
		{
            printf("Http client GET failed\r\n");
        }*/

        ret = http_client_post("/post", strlen("{\"mac\":\"ACCF23B3C38A\",\"mcu_mod\":\"1\",\"mcu_version\":\"1\",\"firmware_mod\":\"1\",\"firmware_version\":\"1\",\"active_code\":\"123\"}"), &header, post_cb, get_cb);
        if(ret == 0)
		{
            printf("\r\nHttp client POST success\r\n");
        }else
		{
            printf("Http client POST failed ret:%d\r\n",ret);
        }

        http_client_stop();
    }
	else
	{
        printf("Http client start failed\r\n");
    }
 }



void user_https_task_start(unsigned int arg)
{
	printf("start http task");
	//qca_ssl_client_func();
}




//�б仯ʱ�������б��ϴ�//
void hm_client_event_task(void)
{
	static uint8_t  transferEventState = TRANSFER_STATE_INIT;
	//gwStateForRealTime.echoNeedUpdateFlag = 1;
	while(1)
	{
		switch(transferEventState)
		{
			case TRANSFER_STATE_INIT:
				if(MQTT_is_connected())
				transferEventState = TRANSFER_STATE_IDLE;
				break;
			case TRANSFER_STATE_UPDATE_DEV_INFO_TO_CLOUD_FOR_ECHO:
				break;
			default:
				break;
		}
		qcom_thread_msleep(500);
		if(TRANSFER_STATE_IDLE == transferEventState)
		{
			if(gwStateForRealTime.zigbeeZcBasicInfo.appAllowSoftWareUpdate == 1)
			{
				transferEventState = TRANSFER_STATE_SCAN_ZC_NEW_SOFTVERSION;
				gwStateForRealTime.zigbeeZcBasicInfo.appAllowSoftWareUpdate = 0;
			}else if((gwStateForRealTime.wifiSoftUpdate.appAllowSoftWareUpdate == 1) && (gwStateForRealTime.wifiSoftUpdate.haveNewestSoftWare == 0)){
				transferEventState = TRANSFER_STATE_SCAN_WIFI_NEW_SOFTVERSION;
				//gwStateForRealTime.wifiSoftUpdate.appAllowSoftWareUpdate = 0;
			}else if(gwStateForRealTime.echoNeedUpdateFlag == 1){
				transferEventState = TRANSFER_STATE_UPDATE_DEV_INFO_TO_CLOUD_FOR_ECHO;
				gwStateForRealTime.echoNeedUpdateFlag = 0;
			}
		}
	}
	qca_task_exit(); 
}


