#ifndef _HM_NETWORK_DATA_MANAGE_H
#define _HM_NETWORK_DATA_MANAGE_H

#include "hm_app.h"

#define ONLINE_VALUE_TYPE			1
#define BA_VALUE_TYPE				2
#define SWITCHSTE_VALUE_TYPE		3

void userCreatNodeState_data(HM_DEV_RAM_LIST *pni,uint8_t stateMode,DEVINFOOPTION devOption);

void user_SetSE_parm(HM_DEV_RAM_LIST *pni,unsigned char *payload,unsigned int payload_len);
void App_get_zigbeeSS(uint16_t index);






#endif

