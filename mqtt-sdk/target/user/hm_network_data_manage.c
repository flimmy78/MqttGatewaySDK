#include "hm_network_data_manage.h"
#include "qcom_common.h"
#include "qca_json.h"
#include "hm_app.h"
#include "qca_base64.h"
#include "stdlib.h"
#include "hm_zigbeeinfohand.h"
#include "MQTTinterface.h"

#include "hm_net_data_manage.h"
#include "ServerDataHandle.h"
#include "hm_zigbee_interface.h"



uint8_t userCreate_ZTdata(uint8_t valuetype,uint8_t wr_ol,uint8_t wr_ba,uint8_t wr_sw)
{
	uint8_t rt_data=0,tempdata1,tempdata2;
	uint8_t test=0;
  switch(valuetype)
  	{
  		case ONLINE_VALUE_TYPE:
			//rt_data
			break;
		case BA_VALUE_TYPE:
			break;
		case SWITCHSTE_VALUE_TYPE:
			break;
  	}
  test=wr_sw;
  tempdata1=(test<<2);
  printf("userCreate_ZTdata tempdata1=%x -----------\r\n",tempdata1);
  test=wr_ba;
  tempdata2=(test<<1);
  rt_data=tempdata1+tempdata2+(wr_ol&0x01);
  printf("userCreate_ZTdata rt_data rt_data=%x -----------\r\n",rt_data);
	return rt_data;
}

void userCreateIasSta_actualdata(char *buff,int *bufflen,HM_DEV_RAM_LIST *pni,DEVINFOOPTION devOption)
{
	uint8_t ZT_valuedata;
	uint32_t converte_data=0,len;
	char strZBMac[18];
	uint8_t *ptr=buff;
	//json_t *state = json_create_object();
	if(devOption.modeByte == 0xffffffff){
	len=0;//bufflen=0;
	printf("pni->onLine=%x,pni->batteryAlarm=%x,pni->epList[0].devData.zoneStateApp.alarms=%x\n",pni->onLine,pni->batteryAlarm,pni->epList[0].devData.zoneStateApp.alarms);
	ZT_valuedata=userCreate_ZTdata(ONLINE_VALUE_TYPE,pni->onLine,pni->batteryAlarm,pni->epList[0].devData.zoneStateApp.alarms);
	memcpy(ptr,&ZT_valuedata,1);len+=1;
	memcpy(ptr+len,&pni->batteryRemain,1);len+=1;
	Int_biglittle_Convert(pni->epList[0].times,&converte_data);
	memcpy(ptr+len,&converte_data,4);
	len+=4;
	*bufflen=len;
	}
	else{
		if(devOption.IASState.online)
		{
			ZT_valuedata=userCreate_ZTdata(ONLINE_VALUE_TYPE,pni->onLine,pni->batteryAlarm,pni->epList[0].devData.zoneStateApp.alarms);
			memcpy(ptr,&ZT_valuedata,1);len+=1;
		}
		if(devOption.IASState.batteryPercent)
		{
			memcpy(ptr,&pni->batteryRemain,1);len+=1;
		}
		
		if(devOption.IASState.batteryAlarm)//TM
		{
			Int_biglittle_Convert(pni->epList[0].times,&converte_data);
			memcpy(ptr,&converte_data,4);
			len+=4;
		}
		*bufflen=len;
	}
}

void userCreateAceSta_actualdata(char *buff,int *bufflen,HM_DEV_RAM_LIST *pni,DEVINFOOPTION devOption)
{
	uint8_t ZT_valuedata;
	uint32_t converte_data=0,len;
	char strZBMac[18];
	uint8_t *ptr=buff;
	//json_t *state = json_create_object();
	if(devOption.modeByte == 0xffffffff){
	len=0;//bufflen=0;
	printf("pni->onLine=%x,pni->batteryAlarm=%x,pni->epList[0].devData.zoneStateApp.alarms=%x\n",pni->onLine,pni->batteryAlarm,pni->epList[0].devData.aceStateApp.station);
	ZT_valuedata=userCreate_ZTdata(ONLINE_VALUE_TYPE,pni->onLine,pni->batteryAlarm,pni->epList[0].devData.aceStateApp.station);
	memcpy(ptr,&ZT_valuedata,1);len+=1;
	memcpy(ptr+len,&pni->batteryRemain,1);len+=1;
	Int_biglittle_Convert(pni->epList[0].times,&converte_data);
	memcpy(ptr+len,&converte_data,4);
	len+=4;
	*bufflen=len;
	}
	else{
		if(devOption.IASState.online)
		{
			ZT_valuedata=userCreate_ZTdata(ONLINE_VALUE_TYPE,pni->onLine,pni->batteryAlarm,pni->epList[0].devData.aceStateApp.station);
			memcpy(ptr,&ZT_valuedata,1);len+=1;
		}
		if(devOption.IASState.batteryPercent)
		{
			memcpy(ptr,&pni->batteryRemain,1);len+=1;
		}
		
		if(devOption.IASState.batteryAlarm)//TM
		{
			Int_biglittle_Convert(pni->epList[0].times,&converte_data);
			memcpy(ptr,&converte_data,4);
			len+=4;
		}
		*bufflen=len;
	}
}


void userCreateIasSta_actual_pointdata(char *buff,int *bufflen,HM_DEV_RAM_LIST *pni,DEVINFOOPTION devOption)
{
	uint8_t ZT_valuedata;
	uint32_t converte_data=0,len=0,temp_intdata;
	char strZBMac[18];
	uint8_t *ptr=buff;
	uint8_t temp_data;

	temp_data=OL_KEY;
	memcpy(ptr,&temp_data,1);len+=1;
	//temp_data=pni->onLine;
	memcpy(ptr+len,&pni->onLine,1);len+=1;
	temp_data=BA_KEY;
	memcpy(ptr+len,&temp_data,1);len+=1;
	memcpy(ptr+len,&pni->batteryAlarm,1);len+=1;
	temp_data=OP_KEY;
	memcpy(ptr+len,&temp_data,1);len+=1;
	memcpy(ptr+len,&pni->epList[0].devData.zoneStateApp.alarms,1);len+=1;
	//pni->epList[0].devData.aceStateApp.station  
	temp_data=BP_KEY;
	memcpy(ptr+len,&temp_data,1);len+=1;
	memcpy(ptr+len,&pni->batteryRemain,1);len+=1;
	temp_data=TM_KEY;
	memcpy(ptr+len,&temp_data,1);len+=1;
	//temp_intdata=pni->epList[0].times;
	Int_biglittle_Convert(pni->epList[0].times,&converte_data);
	memcpy(ptr+len,&converte_data,4);len+=4;
	*bufflen=len;
	

}
//json_t *userCreateAceStaObject(HM_DEV_RAM_LIST *pni,DEVINFOOPTION devOption)

void userCreateAceSta_actual_pointdata(char *buff,int *bufflen,HM_DEV_RAM_LIST *pni,DEVINFOOPTION devOption)
{
	uint8_t ZT_valuedata;
	uint32_t converte_data=0,len=0,temp_intdata;
	char strZBMac[18];
	uint8_t *ptr=buff;
	uint8_t temp_data;

	temp_data=OL_KEY;
	memcpy(ptr,&temp_data,1);len+=1;
	//temp_data=pni->onLine;
	memcpy(ptr+len,&pni->onLine,1);len+=1;
	temp_data=BA_KEY;
	memcpy(ptr+len,&temp_data,1);len+=1;
	memcpy(ptr+len,&pni->batteryAlarm,1);len+=1;
	temp_data=OP_KEY;
	memcpy(ptr+len,&temp_data,1);len+=1;
	memcpy(ptr+len,&pni->epList[0].devData.aceStateApp.station,1);len+=1;
	//pni->epList[0].devData.aceStateApp.station  
	temp_data=BP_KEY;
	memcpy(ptr+len,&temp_data,1);len+=1;
	memcpy(ptr+len,&pni->batteryRemain,1);len+=1;
	temp_data=TM_KEY;
	memcpy(ptr+len,&temp_data,1);len+=1;
	//temp_intdata=pni->epList[0].times;
	Int_biglittle_Convert(pni->epList[0].times,&converte_data);
	memcpy(ptr+len,&converte_data,4);len+=4;
	*bufflen=len;
}


void userCreateSta_rgbdata(char *buff,int *bufflen,HM_DEV_RAM_LIST *pni,DEVINFOOPTION devOption)
{
	uint8_t ZT_valuedata;
	uint32_t converte_data=0,len=0;
	uint8_t *ptr=buff;
	if(devOption.modeByte == 0xffffffff){
	memcpy(ptr,&pni->onLine,1);len+=1;
	memcpy(ptr+len,&pni->epList[0].devData.colorStateApp.onOffStatus,1);len+=1;
	memcpy(ptr+len,&pni->epList[0].devData.colorStateApp.level,1);len+=1;
	big_little_exchange(pni->epList[0].devData.colorStateApp.rgb_r,&converte_data);//big_little_exchange
	memcpy(ptr+len,&converte_data,2);len+=2;
	big_little_exchange(pni->epList[0].devData.colorStateApp.rgb_g,&converte_data);//Int_biglittle_Convert
	memcpy(ptr+len,&converte_data,2);len+=2;
	big_little_exchange(pni->epList[0].devData.colorStateApp.rgb_b,&converte_data);
	memcpy(ptr+len,&converte_data,2);len+=2;
	*bufflen=len;
	}
	else{
			if(devOption.RGBState.online)
			{
				memcpy(ptr,&pni->onLine,1);len+=1;
			}

			if(devOption.RGBState.onoff)
			{
				memcpy(ptr,&pni->epList[0].devData.colorStateApp.onOffStatus,1);len+=1;
			}

			if(devOption.RGBState.level)
			{
				memcpy(ptr,&pni->epList[0].devData.colorStateApp.level,1);len+=1;
			}

			if(devOption.RGBState.colour_r)
			{
				Int_biglittle_Convert(pni->epList[0].devData.colorStateApp.rgb_r,&converte_data);
				memcpy(ptr,&converte_data,2);len+=2;
			}

			if(devOption.RGBState.colour_g)
			{
				Int_biglittle_Convert(pni->epList[0].devData.colorStateApp.rgb_g,&converte_data);
				memcpy(ptr,&converte_data,2);len+=2;
			}

			if(devOption.RGBState.colour_b)
			{
				Int_biglittle_Convert(pni->epList[0].devData.colorStateApp.rgb_b,&converte_data);
				memcpy(ptr,&converte_data,2);len+=2;
				
			}
			*bufflen=len;
		
	}
}

void userCreateSta_rgb_pointdata(char *buff,int *bufflen,HM_DEV_RAM_LIST *pni,DEVINFOOPTION devOption)
{
	uint8_t ZT_valuedata;
	uint32_t converte_data=0,len=0;
	uint8_t *ptr=buff;
	uint8_t temp_data;
	
	temp_data=OL_KEY;
	memcpy(ptr,&temp_data,1);len+=1;
	memcpy(ptr+len,&pni->onLine,1);len+=1;//OF_KEY
	temp_data=OF_KEY;
	memcpy(ptr+len,&temp_data,1);len+=1;
	memcpy(ptr+len,&pni->epList[0].devData.colorStateApp.onOffStatus,1);len+=1;
	temp_data=LE_KEY;
	memcpy(ptr+len,&temp_data,1);len+=1;
	memcpy(ptr+len,&pni->epList[0].devData.colorStateApp.level,1);len+=1;

	temp_data=CR_KEY;
	memcpy(ptr+len,&temp_data,1);len+=1;
	big_little_exchange(pni->epList[0].devData.colorStateApp.rgb_r,&converte_data);
	memcpy(ptr+len,&converte_data,2);len+=2;
	temp_data=CG_KEY;
	memcpy(ptr+len,&temp_data,1);len+=1;
	big_little_exchange(pni->epList[0].devData.colorStateApp.rgb_g,&converte_data);
	memcpy(ptr+len,&converte_data,2);len+=2;
	temp_data=CB_KEY;
	memcpy(ptr+len,&temp_data,1);len+=1;
	big_little_exchange(pni->epList[0].devData.colorStateApp.rgb_b,&converte_data);
	memcpy(ptr+len,&converte_data,2);len+=2;
	*bufflen=len;

}

void userCreateSta_AI1SWdata(char *buff,int *bufflen,HM_DEV_RAM_LIST *pni,DEVINFOOPTION devOption)
{
	uint8_t ZT_valuedata;
	uint32_t converte_data=0,len=0;
	uint8_t i,s1Index,s2Index,s3Index;
	uint8_t *ptr=buff;
	s1Index = 0xff;
	s2Index = 0xff;
	s3Index = 0xff;
	
	for(i = 0; i < pni->epNum; i ++)//ep 1 默认为第一路switch，EP 2默认为第二路switch
	{
		if(pni->epList[i].ep == 1)
		{
			s1Index = i;
		}else if(pni->epList[i].ep == 2){
			s2Index = i;
		}else if(pni->epList[i].ep == 3){
			s3Index = i;
		}
	}
	if(devOption.modeByte == 0xffffffff){
	memcpy(ptr,&pni->onLine,1);len+=1;
	memcpy(ptr+len,&pni->epList[s1Index].devData.witchStateApp.onOffStatus,1);len+=1;
	Int_biglittle_Convert(pni->epList[s1Index].times,&converte_data);
	memcpy(ptr+len,&converte_data,4);len+=4;
	*bufflen=len;
	}
	else{
		if(devOption.SWState.online)
		{
			memcpy(ptr,&pni->onLine,1);len+=1;
		}
		if(s1Index != 0xff)
		{
			if(devOption.SWState.onoff1)
			{
				memcpy(ptr,&pni->epList[s1Index].devData.witchStateApp.onOffStatus,1);len+=1;
			}

			if(devOption.SWState.time1)
			{
				Int_biglittle_Convert(pni->epList[s1Index].times,&converte_data);
				memcpy(ptr,&converte_data,4);len+=4;
				
			}
		}
		*bufflen=len;
		
	}
}

void userCreateSta_AI1SW_pointdata(char *buff,int *bufflen,HM_DEV_RAM_LIST *pni,DEVINFOOPTION devOption)
{
	uint8_t ZT_valuedata;
	uint32_t converte_data=0,len=0;
	uint8_t i,s1Index,s2Index,s3Index;
	uint8_t *ptr=buff;
	uint8_t temp_data;
	s1Index = 0xff;
	s2Index = 0xff;
	s3Index = 0xff;
	
	for(i = 0; i < pni->epNum; i ++)//ep 1 默认为第一路switch，EP 2默认为第二路switch
	{
		if(pni->epList[i].ep == 1)
		{
			s1Index = i;
		}else if(pni->epList[i].ep == 2){
			s2Index = i;
		}else if(pni->epList[i].ep == 3){
			s3Index = i;
		}
	}

	temp_data=OL_KEY;
	memcpy(ptr,&temp_data,1);len+=1;
	memcpy(ptr+len,&pni->onLine,1);len+=1;
	temp_data=OF1_KEY;
	memcpy(ptr+len,&temp_data,1);len+=1;
	memcpy(ptr+len,&pni->epList[s1Index].devData.witchStateApp.onOffStatus,1);len+=1;
	temp_data=TM_KEY;
	memcpy(ptr+len,&temp_data,1);len+=1;
	Int_biglittle_Convert(pni->epList[s1Index].times,&converte_data);
	memcpy(ptr+len,&converte_data,4);len+=4;
	*bufflen=len;
	
}

void userCreateSta_AI2SWdata(char *buff,int *bufflen,HM_DEV_RAM_LIST *pni,DEVINFOOPTION devOption)
{
	uint8_t ZT_valuedata;
	uint32_t converte_data=0,len=0;
	uint8_t i,s1Index,s2Index,s3Index;
	uint8_t *ptr=buff;
	uint8_t onoffstatus_data;
	s1Index = 0xff;
	s2Index = 0xff;
	s3Index = 0xff;
	
	for(i = 0; i < pni->epNum; i ++)//ep 1 默认为第一路switch，EP 2默认为第二路switch
	{
		if(pni->epList[i].ep == 1)
		{
			s1Index = i;
		}else if(pni->epList[i].ep == 2){
			s2Index = i;
		}else if(pni->epList[i].ep == 3){
			s3Index = i;
		}
	}
	if(devOption.modeByte == 0xffffffff){
	memcpy(ptr,&pni->onLine,1);len+=1;
	//printf("the is AI2SW  is test sw1:%d,sw2:%d\n",pni->epList[s1Index].devData.witchStateApp.onOffStatus,pni->epList[s2Index].devData.witchStateApp.onOffStatus);
	ZT_valuedata=(pni->epList[s2Index].devData.witchStateApp.onOffStatus << 1);
	onoffstatus_data=ZT_valuedata+(pni->epList[s1Index].devData.witchStateApp.onOffStatus);
	//printf("the is AI2SW  is start sw1:%d,sw2:%d\n",ZT_valuedata,onoffstatus_data);
	memcpy(ptr+len,&onoffstatus_data,1);len+=1;
	Int_biglittle_Convert(pni->epList[s1Index].times,&converte_data);
	memcpy(ptr+len,&converte_data,4);len+=4;
	Int_biglittle_Convert(pni->epList[s2Index].times,&converte_data);
	memcpy(ptr+len,&converte_data,4);len+=4;
	*bufflen=len;
	}
	else{
		if(devOption.SWState.online)
		{
			memcpy(ptr,&pni->onLine,1);len+=1;
		}
		//if(s1Index != 0xff)
		{
			if(devOption.SWState.onoff1)
			{
				ZT_valuedata=(pni->epList[s2Index].devData.witchStateApp.onOffStatus)&0x01;
				onoffstatus_data=(ZT_valuedata<<1)+(pni->epList[s1Index].devData.witchStateApp.onOffStatus)&0x01;
				memcpy(ptr,&onoffstatus_data,1);len+=1;
			}

			if(devOption.SWState.time1)
			{
				Int_biglittle_Convert(pni->epList[s1Index].times,&converte_data);
				memcpy(ptr,&converte_data,4);len+=4;
				
			}
		}
		*bufflen=len;
	}
}

void userCreateSta_AI2SW_pointdata(char *buff,int *bufflen,HM_DEV_RAM_LIST *pni,DEVINFOOPTION devOption)
{
	uint8_t ZT_valuedata;
	uint32_t converte_data=0,len=0;
	uint8_t i,s1Index,s2Index,s3Index;
	uint8_t *ptr=buff;
	uint8_t temp_data;
	s1Index = 0xff;
	s2Index = 0xff;
	s3Index = 0xff;
		
	for(i = 0; i < pni->epNum; i ++)//ep 1 默认为第一路switch，EP 2默认为第二路switch
	{
		if(pni->epList[i].ep == 1)
		{
			s1Index = i;
		}else if(pni->epList[i].ep == 2){
			s2Index = i;
		}else if(pni->epList[i].ep == 3){
			s3Index = i;
		}
	}
	temp_data=OL_KEY;
	memcpy(ptr,&temp_data,1);len+=1;
	memcpy(ptr+len,&pni->onLine,1);len+=1;
	temp_data=OF1_KEY;
	memcpy(ptr+len,&temp_data,1);len+=1;
	memcpy(ptr+len,&pni->epList[s1Index].devData.witchStateApp.onOffStatus,1);len+=1;
	temp_data=OF2_KEY;
	memcpy(ptr+len,&temp_data,1);len+=1;
	memcpy(ptr+len,&pni->epList[s2Index].devData.witchStateApp.onOffStatus,1);len+=1;
	temp_data=TM_KEY;
	memcpy(ptr+len,&temp_data,1);len+=1;
	Int_biglittle_Convert(pni->epList[s1Index].times,&converte_data);
	memcpy(ptr+len,&converte_data,4);len+=4;
	*bufflen=len;
}
void userCreateSta_AI3SWdata(char *buff,int *bufflen,HM_DEV_RAM_LIST *pni,DEVINFOOPTION devOption)
{
	uint8_t ZT_valuedata1,ZT_valuedata2;
	uint32_t converte_data=0,len=0;
	uint8_t i,s1Index,s2Index,s3Index;
	uint8_t *ptr=buff;
	uint8_t onoffstatus_data;
	s1Index = 0xff;
	s2Index = 0xff;
	s3Index = 0xff;
	
	for(i = 0; i < pni->epNum; i ++)//ep 1 默认为第一路switch，EP 2默认为第二路switch
	{
		if(pni->epList[i].ep == 1)
		{
			s1Index = i;
		}else if(pni->epList[i].ep == 2){
			s2Index = i;
		}else if(pni->epList[i].ep == 3){
			s3Index = i;
		}
	}
	if(devOption.modeByte == 0xffffffff){
	memcpy(ptr,&pni->onLine,1);len+=1;
	//printf("the is AI3SW  is start\n");
	//printf("the is AI3SW  is onoff,s1Index:%d,s2Index:%d,s3Index:%d\n",pni->epList[s1Index].devData.witchStateApp.onOffStatus,pni->epList[s2Index].devData.witchStateApp.onOffStatus,pni->epList[s3Index].devData.witchStateApp.onOffStatus);
	ZT_valuedata2=(pni->epList[s3Index].devData.witchStateApp.onOffStatus << 2);
	ZT_valuedata1=(pni->epList[s2Index].devData.witchStateApp.onOffStatus << 1);
	onoffstatus_data=(uint8_t)(ZT_valuedata2+ZT_valuedata1+pni->epList[s1Index].devData.witchStateApp.onOffStatus);
	//printf("the is AI3SW onoffstatus_data:%d,ZT_valuedata2:%d,ZT_valuedata1:%d\n",onoffstatus_data,ZT_valuedata2,ZT_valuedata1);
	memcpy(ptr+len,&onoffstatus_data,1);len+=1;
	Int_biglittle_Convert(pni->epList[s1Index].times,&converte_data);
	memcpy(ptr+len,&converte_data,4);len+=4;
	Int_biglittle_Convert(pni->epList[s2Index].times,&converte_data);
	memcpy(ptr+len,&converte_data,4);len+=4;
	Int_biglittle_Convert(pni->epList[s3Index].times,&converte_data);
	memcpy(ptr+len,&converte_data,4);len+=4;
	*bufflen=len;
	}
	else{
		if(devOption.SWState.online)
		{
			memcpy(ptr,&pni->onLine,1);len+=1;
		}
		//if(s1Index != 0xff)
		{
			if(devOption.SWState.onoff1)
			{
				ZT_valuedata2=(pni->epList[s3Index].devData.witchStateApp.onOffStatus)&0x01<<2;
				ZT_valuedata1=(pni->epList[s2Index].devData.witchStateApp.onOffStatus)&0x01<<1;
				onoffstatus_data=ZT_valuedata2+ZT_valuedata1+pni->epList[s1Index].devData.witchStateApp.onOffStatus;
				memcpy(ptr,&onoffstatus_data,1);len+=1;
			}

			if(devOption.SWState.time1)
			{
				Int_biglittle_Convert(pni->epList[s1Index].times,&converte_data);
				memcpy(ptr,&converte_data,4);len+=4;
				
			}
		}
		*bufflen=len;
	}
}

void userCreateSta_AI3SW_pointdata(char *buff,int *bufflen,HM_DEV_RAM_LIST *pni,DEVINFOOPTION devOption)
{
	uint8_t ZT_valuedata1,ZT_valuedata2;
	uint32_t converte_data=0,len=0;
	uint8_t i,s1Index,s2Index,s3Index;
	uint8_t *ptr=buff;
	uint8_t onoffstatus_data;
	uint8_t temp_data;
	s1Index = 0xff;
	s2Index = 0xff;
	s3Index = 0xff;
	for(i = 0; i < pni->epNum; i ++)//ep 1 默认为第一路switch，EP 2默认为第二路switch
	{
		if(pni->epList[i].ep == 1)
		{
			s1Index = i;
		}else if(pni->epList[i].ep == 2){
			s2Index = i;
		}else if(pni->epList[i].ep == 3){
			s3Index = i;
		}
	}
		temp_data=OL_KEY;
		memcpy(ptr,&temp_data,1);len+=1;
		memcpy(ptr+len,&pni->onLine,1);len+=1;
		temp_data=OF1_KEY;
		memcpy(ptr+len,&temp_data,1);len+=1;
		memcpy(ptr+len,&pni->epList[s1Index].devData.witchStateApp.onOffStatus,1);len+=1;
		temp_data=OF2_KEY;
		memcpy(ptr+len,&temp_data,1);len+=1;
		memcpy(ptr+len,&pni->epList[s2Index].devData.witchStateApp.onOffStatus,1);len+=1;
		temp_data=OF3_KEY;
		memcpy(ptr+len,&temp_data,1);len+=1;
		memcpy(ptr+len,&pni->epList[s3Index].devData.witchStateApp.onOffStatus,1);len+=1;
		temp_data=TM_KEY;
		memcpy(ptr+len,&temp_data,1);len+=1;
		Int_biglittle_Convert(pni->epList[s1Index].times,&converte_data);
		memcpy(ptr+len,&converte_data,4);len+=4;
		*bufflen=len;

}

//NODE_INFO_TYPE_TEMP_HUM_SENSOR
void userCreateSta_TEMPHUMdata(char *buff,int *bufflen,HM_DEV_RAM_LIST *pni,DEVINFOOPTION devOption)
{
	uint8_t ZT_valuedata1,ZTData;
	uint32_t converte_data=0,len=0;
	uint8_t *ptr=buff;
	uint8_t i,tempEpIndex = 0xff;
	uint8_t humiEpIndex = 0xff;
	for(i = 0; i < pni->epNum; i ++)	//默认EP 1为temperature，2为humility
	{
		if(pni->epList[i].ep == 1)
		{
			tempEpIndex = i;
			printf("the is TEMPHUMdata tempEpIndex:%d\n",tempEpIndex);
		}else if(pni->epList[i].ep == 2){
			humiEpIndex = i;
			printf("the is TEMPHUMdata humiEpIndex:%d\n",humiEpIndex);
		}
	}
	if(devOption.modeByte == 0xffffffff){
	ZT_valuedata1=(pni->batteryAlarm&0x01)<<1;
	ZTData=(pni->onLine&0x01)+ZT_valuedata1;
	memcpy(ptr,&ZTData,1);len+=1;//
	memcpy(ptr+len,&pni->batteryRemain,1);len+=1;
	Int_biglittle_Convert(pni->epList[0].times,&converte_data);
	memcpy(ptr+len,&converte_data,4);len+=4;
	if(pni->epList[humiEpIndex].devData.humiStateApp.humiMeterage!=NULL){
		printf("the is TEMPHUMdata is have humiMeterage data\n");
	big_little_exchange(pni->epList[humiEpIndex].devData.humiStateApp.humiMeterage,&converte_data);
	}
	else{
		printf("the is TEMPHUMdata is not have humiMeterage data\n");
		converte_data=0;
		}
	memcpy(ptr+len,&converte_data,2);len+=2;
	if(pni->epList[tempEpIndex].devData.tempStateApp.tempMeterage!=NULL){
		printf("the is TEMPHUMdata is have tempMeterage data\n");
	big_little_exchange(pni->epList[tempEpIndex].devData.tempStateApp.tempMeterage,&converte_data);
		}
	else{
		converte_data=0;
			printf("the is TEMPHUMdata is not have tempMeterage data\n");
		}
	memcpy(ptr+len,&converte_data,2);len+=2;
	*bufflen=len;
	}
	else{
		if(devOption.THPState.online)//zt
		{
			ZT_valuedata1=(pni->batteryAlarm&0x01)<<1;
			ZTData=(pni->onLine&0x01)+ZT_valuedata1;
			memcpy(ptr,&ZTData,1);len+=1;
		}
		if(devOption.THPState.batteryPercent)//bp
		{
			memcpy(ptr,&pni->batteryRemain,1);len+=1;
		}
		if(devOption.THPState.batteryAlarm)//tm
		{
			Int_biglittle_Convert(pni->epList[0].times,&converte_data);
			memcpy(ptr,&converte_data,4);len+=4;
		}
		if(devOption.THPState.time)//tp
		{
			big_little_exchange(pni->epList[humiEpIndex].devData.humiStateApp.humiMeterage,&converte_data);
			memcpy(ptr,&converte_data,2);len+=2;
		}
		if(devOption.THPState.temp)//hy
		{
			big_little_exchange(pni->epList[tempEpIndex].devData.tempStateApp.tempMeterage,&converte_data);
			memcpy(ptr,&converte_data,2);len+=2;
		}
		*bufflen=len;
	}

}

void userCreateIasSta_TEMPHUM_pointdata(char *buff,int *bufflen,HM_DEV_RAM_LIST *pni,DEVINFOOPTION devOption)
{
	uint8_t ZT_valuedata1,ZT_valuedata2;
	uint32_t converte_data=0,len=0;
	uint8_t *ptr=buff;
	uint8_t onoffstatus_data,i;
	uint8_t temp_data;
	uint8_t tempEpIndex = 0xff;
	uint8_t humiEpIndex = 0xff;
	for(i = 0; i < pni->epNum; i ++)	//默认EP 1为temperature，2为humility
	{
		if(pni->epList[i].ep == 1)
		{
			tempEpIndex = i;
		}else if(pni->epList[i].ep == 2){
			humiEpIndex = i;
		}
	}
	temp_data=OL_KEY;
	memcpy(ptr,&temp_data,1);len+=1;
	memcpy(ptr+len,&pni->onLine,1);len+=1;
	temp_data=BA_KEY;
	memcpy(ptr+len,&temp_data,1);len+=1;
	memcpy(ptr+len,&pni->batteryAlarm,1);len+=1;
	temp_data=BP_KEY;
	memcpy(ptr+len,&temp_data,1);len+=1;
	memcpy(ptr+len,&pni->batteryRemain,1);len+=1;
	temp_data=TM_KEY;
	memcpy(ptr+len,&temp_data,1);len+=1;
	Int_biglittle_Convert(pni->epList[0].times,&converte_data);
	memcpy(ptr+len,&converte_data,4);len+=4;

	temp_data=HY_KEY;
	memcpy(ptr+len,&temp_data,1);len+=1;
	if(pni->epList[humiEpIndex].devData.humiStateApp.humiMeterage!= NULL){
	big_little_exchange(pni->epList[humiEpIndex].devData.humiStateApp.humiMeterage,&converte_data);
	printf("---the humiMeterage is  have data\n");
	}
	else{
		converte_data=0;
		printf("----the humiMeterage is not have data\n");
		}
	memcpy(ptr+len,&converte_data,2);len+=2;	
	temp_data=TP_KEY;
	memcpy(ptr+len,&temp_data,1);len+=1;
	if(pni->epList[tempEpIndex].devData.tempStateApp.tempMeterage!= NULL){
		printf("---the tempMeterage is  have data\n");
	big_little_exchange(pni->epList[tempEpIndex].devData.tempStateApp.tempMeterage,&converte_data);
		}
	else{
		printf("---the tempMeterage is not have data\n");
		converte_data=0;
		}
	memcpy(ptr+len,&converte_data,2);len+=2;
	*bufflen=len;

}

void userCreateSta_SOCKETUSBdata(char *buff,int *bufflen,HM_DEV_RAM_LIST *pni,DEVINFOOPTION devOption)
{
	uint8_t ZT_valuedata1,ZTData;
	uint32_t converte_data=0,len=0;
	uint8_t i,relayIndex = 0xff;
	uint8_t usbIndex = 0xff;
	uint8_t *ptr=buff;
	for(i = 0; i < pni->epNum; i ++)//ep 1 默认为继电器控制，EP 2默认为usb控制
	{
		if(pni->epList[i].ep == 1)
		{
			relayIndex = i;
		}else if(pni->epList[i].ep == 2){
			usbIndex = i;
		}
	}
	if(devOption.modeByte == 0xffffffff){
	memcpy(ptr,&pni->onLine,1);len+=1;
	memcpy(ptr+len,&pni->epList[relayIndex].devData.onoffOutStateApp.onOffStatus,1);len+=1;
	memcpy(ptr+len,&pni->epList[usbIndex].devData.onoffOutStateApp.onOffStatus,1);len+=1;
	Int_biglittle_Convert(pni->epList[relayIndex].times,&converte_data);
	memcpy(ptr+len,&converte_data,4);len+=4;
	*bufflen=len;
	}
	else{
		if(devOption.UPlugState.online)
		{
			memcpy(ptr,&pni->onLine,1);len+=1;	
		}
		if(devOption.UPlugState.relayOnoff)
		{
			memcpy(ptr,&pni->epList[relayIndex].devData.onoffOutStateApp.onOffStatus,1);len+=1;
		}
		if(devOption.UPlugState.usbOnoff)
		{
			memcpy(ptr,&pni->epList[usbIndex].devData.onoffOutStateApp.onOffStatus,1);len+=1;
		}
		if(devOption.UPlugState.time_r)
		{
			Int_biglittle_Convert(pni->epList[relayIndex].times,&converte_data);
			memcpy(ptr,&converte_data,4);len+=4;
		}
		*bufflen=len;
	}	
}

void userCreateSta_SOCKETUSB_pointdata(char *buff,int *bufflen,HM_DEV_RAM_LIST *pni,DEVINFOOPTION devOption)
{
	uint8_t ZT_valuedata1,ZTData;
	uint32_t converte_data=0,len=0;
	uint8_t i,relayIndex = 0xff;
	uint8_t usbIndex = 0xff;
	uint8_t *ptr=buff;
	uint8_t temp_data;
	for(i = 0; i < pni->epNum; i ++)//ep 1 默认为继电器控制，EP 2默认为usb控制
	{
		if(pni->epList[i].ep == 1)
		{
			relayIndex = i;
		}else if(pni->epList[i].ep == 2){
			usbIndex = i;
		}
	}
	temp_data=OL_KEY;
	memcpy(ptr,&temp_data,1);len+=1;
	memcpy(ptr+len,&pni->onLine,1);len+=1;
	temp_data=RO_KEY;
	memcpy(ptr+len,&temp_data,1);len+=1;
	memcpy(ptr+len,&pni->epList[relayIndex].devData.onoffOutStateApp.onOffStatus,1);len+=1;
	temp_data=UO_KEY;
	memcpy(ptr+len,&temp_data,1);len+=1;
	memcpy(ptr+len,&pni->epList[usbIndex].devData.onoffOutStateApp.onOffStatus,1);len+=1;
	temp_data=TM_KEY;
	memcpy(ptr+len,&temp_data,1);len+=1;
	Int_biglittle_Convert(pni->epList[relayIndex].times,&converte_data);
	memcpy(ptr+len,&converte_data,4);len+=4;
	*bufflen=len;

}


void userCreateSta_SOCKETMEASUREdata(char *buff,int *bufflen,HM_DEV_RAM_LIST *pni,DEVINFOOPTION devOption)
{
	uint8_t ZT_valuedata1,ZTData;
	uint32_t converte_data=0,len=0;
	uint8_t i,relayIndex = 0xff;
	uint8_t usbIndex = 0xff;
	uint8_t *ptr=buff;
	uint32_t ETData1,ETData2;
	/*for(i = 0; i < pni->epNum; i ++)//ep 1 默认为继电器控制，EP 2默认为usb控制
	{
		if(pni->epList[i].ep == 1)
		{
			relayIndex = i;
		}else if(pni->epList[i].ep == 2){
			usbIndex = i;
		}
	}*/
	if(devOption.modeByte == 0xffffffff){
	if((pni->onLine == 0) || (pni->epList[0].devData.splugStateApp.onOffStatus == 0))	//掉线或者继电器关闭
	{
			pni->epList[0].devData.splugStateApp.InstantaneousDemand = 0;
	}
	memcpy(ptr,&pni->onLine,1);len+=1;
	memcpy(ptr+len,&pni->epList[0].devData.splugStateApp.onOffStatus,1);len+=1;
	Int_biglittle_Convert(pni->epList[0].devData.splugStateApp.InstantaneousDemand,&converte_data); //PW 4
	memcpy(ptr+len,&converte_data,4);len+=4;
	ETData1=pni->epList[0].devData.splugStateApp.CurrentSummationDelivered&0xffff;//ET L32
	Int_biglittle_Convert(ETData1,&converte_data);
	memcpy(ptr+len,&converte_data,4);len+=4;
	ETData2=(pni->epList[0].devData.splugStateApp.CurrentSummationDelivered>>32)&0xffff;//ET H32
	Int_biglittle_Convert(ETData2,&converte_data);
	memcpy(ptr+len,&converte_data,4);len+=4;
	Int_biglittle_Convert(pni->epList[0].times,&converte_data);
	memcpy(ptr+len,&converte_data,4);len+=4;
	*bufflen=len;
	}
	else{
		if(devOption.SPlugState.online)
		{
			memcpy(ptr,&pni->onLine,1);len+=1;
		}

		if(devOption.SPlugState.relayOnoff)
		{
			memcpy(ptr,&pni->epList[0].devData.splugStateApp.onOffStatus,1);len+=1;
		}

		if((pni->onLine == 0) || (pni->epList[0].devData.splugStateApp.onOffStatus == 0))	//掉线或者继电器关闭
		{
			pni->epList[0].devData.splugStateApp.InstantaneousDemand = 0;
		}

		if(devOption.SPlugState.power_w)
		{
			Int_biglittle_Convert(pni->epList[0].devData.splugStateApp.InstantaneousDemand,&converte_data); //PW 4
			memcpy(ptr,&converte_data,4);len+=4;
		}

		if(devOption.SPlugState.power_wh)
		{
			ETData1=pni->epList[0].devData.splugStateApp.CurrentSummationDelivered&0xffff;//ET L32
			Int_biglittle_Convert(ETData1,&converte_data);
			memcpy(ptr,&converte_data,4);len+=4;
			ETData2=(pni->epList[0].devData.splugStateApp.CurrentSummationDelivered>>32)&0xffff;//ET H32
			Int_biglittle_Convert(ETData2,&converte_data);
			memcpy(ptr+len,&converte_data,4);len+=4;
		}

		if(devOption.SPlugState.time)
		{
			//json_add_item_to_object(state, "TM", json_create_number(pni->epList[0].times));
		}
		*bufflen=len;
	}
}
void userCreateSta_SOCKETMEASURE_pointdata(char *buff,int *bufflen,HM_DEV_RAM_LIST *pni,DEVINFOOPTION devOption)
{
	uint8_t ZT_valuedata1,ZTData;
	uint32_t converte_data=0,len=0;
	uint8_t i,relayIndex = 0xff;
	uint8_t usbIndex = 0xff;
	uint8_t *ptr=buff;
	uint8_t temp_data;
	uint32_t ETData1,ETData2;

	temp_data=OL_KEY;
	memcpy(ptr,&temp_data,1);len+=1;
	memcpy(ptr+len,&pni->onLine,1);len+=1;
	temp_data=RO_KEY;
	memcpy(ptr+len,&temp_data,1);len+=1;
	memcpy(ptr+len,&pni->epList[0].devData.splugStateApp.onOffStatus,1);len+=1;
	if((pni->onLine == 0) || (pni->epList[0].devData.splugStateApp.onOffStatus == 0))	//掉线或者继电器关闭
	{
		pni->epList[0].devData.splugStateApp.InstantaneousDemand = 0;
	}
	temp_data=PW_KEY;
	memcpy(ptr+len,&temp_data,1);len+=1;
	Int_biglittle_Convert(pni->epList[0].devData.splugStateApp.InstantaneousDemand,&converte_data); //PW 4
	memcpy(ptr+len,&converte_data,4);len+=4;

	temp_data=ET_KEY;
	memcpy(ptr+len,&temp_data,1);len+=1;
	ETData1=pni->epList[0].devData.splugStateApp.CurrentSummationDelivered&0xffff;//ET L32
	Int_biglittle_Convert(ETData1,&converte_data);
	memcpy(ptr+len,&converte_data,4);len+=4;
	ETData2=(pni->epList[0].devData.splugStateApp.CurrentSummationDelivered>>32)&0xffff;//ET H32
	Int_biglittle_Convert(ETData2,&converte_data);
	memcpy(ptr+len,&converte_data,4);len+=4;
	temp_data=TM_KEY;
	memcpy(ptr+len,&temp_data,1);len+=1;
	Int_biglittle_Convert(pni->epList[0].times,&converte_data);
	memcpy(ptr+len,&converte_data,4);len+=4;
	*bufflen=len;
	

}
void userCreateSta_CUTRGBdata(char *buff,int *bufflen,HM_DEV_RAM_LIST *pni,DEVINFOOPTION devOption)
{
	uint8_t ZT_valuedata1,ZTData;
	uint32_t converte_data=0,len=0;
	uint8_t relayIndex = 0xff;
	uint8_t usbIndex = 0xff;
	uint8_t *ptr=buff;
	uint16_t ETData;
	uint8_t i,s1Index,s2Index,s3Index,s4Index;
	uint8_t onoffList1,onoffList2,onoffList3,onoffList4;
	uint32_t brigList1,brigList2,brigList3,brigList4;
	uint16_t colorList1,colorList2,colorList3,colorList4;
	s1Index = 0xff;
	s2Index = 0xff;
	s3Index = 0xff;
	s4Index = 0xff;
	for(i = 0; i < pni->epNum; i ++)//ep 1 默认为第一路switch，EP 2默认为第二路switch
		{
			if(pni->epList[i].ep == 1)
			{
				s1Index = i;
			}else if(pni->epList[i].ep == 2){
				s2Index = i;
			}else if(pni->epList[i].ep == 3){
				s3Index = i;
			}else if(pni->epList[i].ep == 4){
				s4Index = i;
			}
		}
	if(devOption.modeByte == 0xffffffff){
	memcpy(ptr,&pni->onLine,1);len+=1;
	//memcpy(ptr+len,&pni->epList[0].devData.splugStateApp.onOffStatus,1);len+=1;
	onoffList4=(pni->epList[s4Index].devData.cstRgbStateApp.onOffStatus&0x01)<<3;
	onoffList3=(pni->epList[s3Index].devData.cstRgbStateApp.onOffStatus&0x01)<<2;
	onoffList2=(pni->epList[s2Index].devData.cstRgbStateApp.onOffStatus&0x01)<<1;
	onoffList1=(pni->epList[s1Index].devData.cstRgbStateApp.onOffStatus&0x01)+onoffList2+onoffList3+onoffList4;
	memcpy(ptr+len,&onoffList1,1);len+=1;
	brigList1=(pni->epList[s1Index].devData.cstRgbStateApp.brightness&0xff)<<24;
	brigList2=(pni->epList[s2Index].devData.cstRgbStateApp.brightness&0xff)<<16;
	brigList3=(pni->epList[s3Index].devData.cstRgbStateApp.brightness&0xff)<<8;
	brigList4=(pni->epList[s4Index].devData.cstRgbStateApp.brightness&0xff)+brigList3+brigList2+brigList1;
	memcpy(ptr+len,&brigList4,4);len+=4;
	colorList1=(pni->epList[s1Index].devData.cstRgbStateApp.colorTemrature&0xff)<<12;
	colorList2=(pni->epList[s2Index].devData.cstRgbStateApp.colorTemrature&0xff)<<8;
	colorList3=(pni->epList[s3Index].devData.cstRgbStateApp.colorTemrature&0xff)<<4;
	colorList4=(pni->epList[s4Index].devData.cstRgbStateApp.colorTemrature&0xff)+colorList3+colorList2+colorList1;
	memcpy(ptr+len,&colorList4,2);len+=2;
	*bufflen=len;
	}
	else{
		if(devOption.CUTRGBState.online)
		{
			memcpy(ptr,&pni->onLine,1);len+=1;
		}
		if(devOption.CUTRGBState.onoffList1)
		{
			memcpy(ptr,&pni->epList[0].devData.splugStateApp.onOffStatus,1);len+=1;
		}
		if(devOption.CUTRGBState.onoffList2)
		{
			onoffList4=(pni->epList[s4Index].devData.cstRgbStateApp.onOffStatus&0x01)<<3;
			onoffList3=(pni->epList[s3Index].devData.cstRgbStateApp.onOffStatus&0x01)<<2;
			onoffList2=(pni->epList[s2Index].devData.cstRgbStateApp.onOffStatus&0x01)<<1;
			onoffList1=(pni->epList[s1Index].devData.cstRgbStateApp.onOffStatus&0x01)+onoffList2+onoffList3+onoffList4;
			memcpy(ptr,&onoffList1,1);len+=1;
		}
		if(devOption.CUTRGBState.onoffList3)
		{
			brigList1=(pni->epList[s1Index].devData.cstRgbStateApp.brightness&0xff)<<24;
			brigList2=(pni->epList[s2Index].devData.cstRgbStateApp.brightness&0xff)<<16;
			brigList3=(pni->epList[s3Index].devData.cstRgbStateApp.brightness&0xff)<<8;
			brigList4=(pni->epList[s4Index].devData.cstRgbStateApp.brightness&0xff)+brigList3+brigList2+brigList1;
			memcpy(ptr,&brigList4,4);len+=4;
		}
		*bufflen=len;
	}

}
void userCreateSta_CUTRGB_pointdata(char *buff,int *bufflen,HM_DEV_RAM_LIST *pni,DEVINFOOPTION devOption)
{
	uint8_t ZT_valuedata1,ZTData;
	uint32_t converte_data=0,len=0;
	uint8_t relayIndex = 0xff;
	uint8_t usbIndex = 0xff;
	uint8_t *ptr=buff;
	uint16_t ETData;
	uint8_t i,s1Index,s2Index,s3Index,s4Index;
	uint8_t onoffList1,onoffList2,onoffList3,onoffList4;
	uint32_t brigList1,brigList2,brigList3,brigList4;
	uint16_t colorList1,colorList2,colorList3,colorList4;
	uint8_t temp_data;
	s1Index = 0xff;
	s2Index = 0xff;
	s3Index = 0xff;
	s4Index = 0xff;
	for(i = 0; i < pni->epNum; i ++)//ep 1 默认为第一路switch，EP 2默认为第二路switch
	{
		if(pni->epList[i].ep == 1)
		{
			s1Index = i;
		}else if(pni->epList[i].ep == 2){
			s2Index = i;
		}else if(pni->epList[i].ep == 3){
			s3Index = i;
		}else if(pni->epList[i].ep == 4){
			s4Index = i;
		}
	}
	temp_data=OL_KEY;
	memcpy(ptr,&temp_data,1);len+=1;
	memcpy(ptr+len,&pni->onLine,1);len+=1;
	temp_data=OF1_KEY;
	memcpy(ptr+len,&temp_data,1);len+=1;
	memcpy(ptr+len,&pni->epList[s1Index].devData.cstRgbStateApp.onOffStatus,1);len+=1;
	temp_data=OF2_KEY;
	memcpy(ptr+len,&temp_data,1);len+=1;
	memcpy(ptr+len,&pni->epList[s2Index].devData.cstRgbStateApp.onOffStatus,1);len+=1;
	temp_data=OF3_KEY;
	memcpy(ptr+len,&temp_data,1);len+=1;
	memcpy(ptr+len,&pni->epList[s3Index].devData.cstRgbStateApp.onOffStatus,1);len+=1;
	temp_data=OF4_KEY;
	memcpy(ptr+len,&temp_data,1);len+=1;
	memcpy(ptr+len,&pni->epList[s4Index].devData.cstRgbStateApp.onOffStatus,1);len+=1;

	temp_data=LEL_KEY;
	memcpy(ptr+len,&temp_data,1);len+=1;
	brigList1=(pni->epList[s1Index].devData.cstRgbStateApp.brightness&0xff)<<24;
	brigList2=(pni->epList[s2Index].devData.cstRgbStateApp.brightness&0xff)<<16;
	brigList3=(pni->epList[s3Index].devData.cstRgbStateApp.brightness&0xff)<<8;
	brigList4=(pni->epList[s4Index].devData.cstRgbStateApp.brightness&0xff)+brigList3+brigList2+brigList1;
	memcpy(ptr+len,&brigList4,4);len+=4;
	temp_data=CWL_KEY;
	memcpy(ptr+len,&temp_data,1);len+=1;
	colorList1=(pni->epList[s1Index].devData.cstRgbStateApp.colorTemrature&0xff)<<12;
	colorList2=(pni->epList[s2Index].devData.cstRgbStateApp.colorTemrature&0xff)<<8;
	colorList3=(pni->epList[s3Index].devData.cstRgbStateApp.colorTemrature&0xff)<<4;
	colorList4=(pni->epList[s4Index].devData.cstRgbStateApp.colorTemrature&0xff)+colorList3+colorList2+colorList1;
	memcpy(ptr+len,&colorList4,2);len+=2;
	*bufflen=len;
	
}


//json_t *userCreateIasSettingObject(HM_DEV_RAM_LIST *pni,DEVINFOOPTION devOption)
void userCreateIasSetting_actualdata(char *buff,int *bufflen,HM_DEV_RAM_LIST *pni,DEVINFOOPTION devOption)
{
	uint8_t ZT_valuedata1,ZTData;
	uint32_t converte_data=0,len=0;
	uint8_t relayIndex = 0xff;
	uint8_t usbIndex = 0xff;
	uint8_t *ptr=buff;
	uint16_t lendata=0x0001;
	if(pni->devType == NODE_INFO_TYPE_VIBRATION){
		lendata=0x0003;
		big_little_exchange(lendata,&converte_data);
		memcpy(ptr,&converte_data,2);len+=2;
		memcpy(ptr+len,&pni->epList[0].devData.zoneStateApp.armMode,1);len+=1;
		memcpy(ptr+len,&pni->epList[0].devData.zoneStateApp.sensitivityLevel,1);len+=1;
	}
	else{
		big_little_exchange(lendata,&converte_data);
		memcpy(ptr,&converte_data,2);len+=2;
		memcpy(ptr+len,&pni->epList[0].devData.zoneStateApp.armMode,1);len+=1;
	}
	*bufflen=len;
}

void userCreateWDSettingData(char *buff,int *bufflen,HM_DEV_RAM_LIST *pni,DEVINFOOPTION devOption)
{
	uint8_t ZT_valuedata1,ZTData;
	uint32_t converte_data=0,len=0;
	uint8_t relayIndex = 0xff;
	uint8_t usbIndex = 0xff;
	uint8_t *ptr=buff;
	uint16_t lendata=0x0003;
	
	big_little_exchange(lendata,&converte_data);
	memcpy(ptr,&converte_data,2);len+=2;
	memcpy(ptr+len,&pni->epList[0].devData.wdStateApp.action,1);len+=1;
	big_little_exchange(pni->epList[0].devData.wdStateApp.duration,&converte_data);
	memcpy(ptr+len,&converte_data,2);len+=2;
	*bufflen=len;
}
//
void userCreateRGBSettingData(char *buff,int *bufflen,HM_DEV_RAM_LIST *pni,DEVINFOOPTION devOption)
{
	uint8_t ZT_valuedata1,ZTData;
	uint32_t converte_data=0,len=0;
	uint8_t relayIndex = 0xff;
	uint8_t usbIndex = 0xff;
	uint8_t *ptr=buff;
	uint16_t lendata=0x00ff;
	//if(devOption.modeByte == 0xffffffff)
		{
	big_little_exchange(lendata,&converte_data);
	memcpy(ptr,&converte_data,2);len+=2;
	memcpy(ptr+len,&pni->epList[0].devData.colorStateApp.onOffStatus,1);len+=1;
	memcpy(ptr+len,&pni->epList[0].devData.colorStateApp.level,1);len+=1;
	big_little_exchange(pni->epList[0].devData.colorStateApp.rgb_r,&converte_data);
	memcpy(ptr,&converte_data,2);len+=2;
	big_little_exchange(pni->epList[0].devData.colorStateApp.rgb_g,&converte_data);
	memcpy(ptr,&converte_data,2);len+=2;
	big_little_exchange(pni->epList[0].devData.colorStateApp.rgb_b,&converte_data);
	memcpy(ptr,&converte_data,2);len+=2;
	memcpy(ptr+len,&pni->epList[0].devData.colorStateApp.timer.s_month,1);len+=1;
	memcpy(ptr+len,&pni->epList[0].devData.colorStateApp.timer.s_day,1);len+=1;
	memcpy(ptr+len,&pni->epList[0].devData.colorStateApp.timer.s_hour,1);len+=1;
	memcpy(ptr+len,&pni->epList[0].devData.colorStateApp.timer.s_min,1);len+=1;
	memcpy(ptr+len,&pni->epList[0].devData.colorStateApp.timer.e_month,1);len+=1;
	memcpy(ptr+len,&pni->epList[0].devData.colorStateApp.timer.e_day,1);len+=1;
	memcpy(ptr+len,&pni->epList[0].devData.colorStateApp.timer.e_hour,1);len+=1;
	memcpy(ptr+len,&pni->epList[0].devData.colorStateApp.timer.e_min,1);len+=1;
	memcpy(ptr+len,&pni->epList[0].devData.colorStateApp.timer.wk_flag,1);len+=1;
	*bufflen=len;
	}

}
void userCreateAISWSettingData(char *buff,int *bufflen,HM_DEV_RAM_LIST *pni,DEVINFOOPTION devOption)
{
	uint8_t ZT_valuedata1,ZTData;
	uint32_t converte_data=0,len=0;
	uint8_t relayIndex = 0xff;
	uint8_t usbIndex = 0xff;
	uint8_t *ptr=buff;
	uint16_t lendata=0x000f;
	uint8_t i,s1Index = 0xff,s2Index = 0xff,s3Index = 0xff;
	uint8_t onOffStatus1,onOffStatus2,onOffStatus3,onOffStatus;
	s1Index = 0xff;
	s2Index = 0xff;
	s3Index = 0xff;
	for(i = 0; i < pni->epNum; i ++)//ep 1 默认为第一路switch，EP 2默认为第二路switch
	{
		if(pni->epList[i].ep == 1)
		{
			s1Index = i;
		}else if(pni->epList[i].ep == 2){
			s2Index = i;
		}else if(pni->epList[i].ep == 3){
			s3Index = i;
		}
	}
	if(s1Index != 0xff)
	{
		big_little_exchange(lendata,&converte_data);
		memcpy(ptr,&converte_data,2);len+=2;
		onOffStatus1=pni->epList[s1Index].devData.witchStateApp.onOffStatus&0x01;
		memcpy(ptr+2,&pni->epList[s1Index].devData.witchStateApp.onOffStatus,1);len+=1;
		memcpy(ptr+len,&pni->epList[s1Index].devData.witchStateApp.timer.s_month,1);len+=1;
		memcpy(ptr+len,&pni->epList[s1Index].devData.witchStateApp.timer.s_day,1);len+=1;
		memcpy(ptr+len,&pni->epList[s1Index].devData.witchStateApp.timer.s_hour,1);len+=1;
		memcpy(ptr+len,&pni->epList[s1Index].devData.witchStateApp.timer.s_min,1);len+=1;
		memcpy(ptr+len,&pni->epList[s1Index].devData.witchStateApp.timer.e_month,1);len+=1;
		memcpy(ptr+len,&pni->epList[s1Index].devData.witchStateApp.timer.e_day,1);len+=1;
		memcpy(ptr+len,&pni->epList[s1Index].devData.witchStateApp.timer.e_hour,1);len+=1;
		memcpy(ptr+len,&pni->epList[s1Index].devData.witchStateApp.timer.e_min,1);len+=1;
		memcpy(ptr+len,&pni->epList[s1Index].devData.witchStateApp.timer.wk_flag,1);len+=1;
	}
	if(s2Index != 0xff)
	{
		lendata=0x007f;
		big_little_exchange(lendata,&converte_data);
		memcpy(ptr,&converte_data,2);
		onOffStatus2=pni->epList[s2Index].devData.witchStateApp.onOffStatus&0x01;
		onOffStatus=(onOffStatus2<<1)+onOffStatus1;
		memcpy(ptr+2,&onOffStatus,1);
		memcpy(ptr+len,&pni->epList[s2Index].devData.witchStateApp.timer.s_month,1);len+=1;
		memcpy(ptr+len,&pni->epList[s2Index].devData.witchStateApp.timer.s_day,1);len+=1;
		memcpy(ptr+len,&pni->epList[s2Index].devData.witchStateApp.timer.s_hour,1);len+=1;
		memcpy(ptr+len,&pni->epList[s2Index].devData.witchStateApp.timer.s_min,1);len+=1;
		memcpy(ptr+len,&pni->epList[s2Index].devData.witchStateApp.timer.e_month,1);len+=1;
		memcpy(ptr+len,&pni->epList[s2Index].devData.witchStateApp.timer.e_day,1);len+=1;
		memcpy(ptr+len,&pni->epList[s2Index].devData.witchStateApp.timer.e_hour,1);len+=1;
		memcpy(ptr+len,&pni->epList[s2Index].devData.witchStateApp.timer.e_min,1);len+=1;
		memcpy(ptr+len,&pni->epList[s2Index].devData.witchStateApp.timer.wk_flag,1);len+=1;
	
	}
	if(s3Index != 0xff)
	{
		lendata=0x03ff;
		big_little_exchange(lendata,&converte_data);
		memcpy(ptr,&converte_data,2);
		onOffStatus3=pni->epList[s3Index].devData.witchStateApp.onOffStatus&0x01;
		onOffStatus=(onOffStatus3<<2)+(onOffStatus2<<1)+onOffStatus1;
		memcpy(ptr+2,&onOffStatus,1);
		memcpy(ptr+len,&pni->epList[s3Index].devData.witchStateApp.timer.s_month,1);len+=1;
		memcpy(ptr+len,&pni->epList[s3Index].devData.witchStateApp.timer.s_day,1);len+=1;
		memcpy(ptr+len,&pni->epList[s3Index].devData.witchStateApp.timer.s_hour,1);len+=1;
		memcpy(ptr+len,&pni->epList[s3Index].devData.witchStateApp.timer.s_min,1);len+=1;
		memcpy(ptr+len,&pni->epList[s3Index].devData.witchStateApp.timer.e_month,1);len+=1;
		memcpy(ptr+len,&pni->epList[s3Index].devData.witchStateApp.timer.e_day,1);len+=1;
		memcpy(ptr+len,&pni->epList[s3Index].devData.witchStateApp.timer.e_hour,1);len+=1;
		memcpy(ptr+len,&pni->epList[s3Index].devData.witchStateApp.timer.e_min,1);len+=1;
		memcpy(ptr+len,&pni->epList[s3Index].devData.witchStateApp.timer.wk_flag,1);len+=1;
	}
	*bufflen=len;
}
void userCreatePlugSettingData(char *buff,int *bufflen,HM_DEV_RAM_LIST *pni,DEVINFOOPTION devOption)
{
	uint8_t ZT_valuedata1,ZTData;
	uint32_t converte_data=0,len=0;
	uint8_t relayIndex = 0xff;
	uint8_t usbIndex = 0xff,i;
	uint8_t *ptr=buff;
	uint16_t lendata=0x00ff;

	for(i = 0; i < pni->epNum; i ++)//ep 1 默认为继电器控制，EP 2默认为usb控制
	{
		if(pni->epList[i].ep == 1)
		{
			relayIndex = i;
		}else if(pni->epList[i].ep == 2){
			usbIndex = i;
		}
	}
	big_little_exchange(lendata,&converte_data);
	memcpy(ptr,&converte_data,2);len+=2;
	memcpy(ptr+len,&pni->epList[relayIndex].devData.onoffOutStateApp.onOffStatus,1);len+=1;
	memcpy(ptr+len,&pni->epList[relayIndex].devData.onoffOutStateApp.timer.s_month,1);len+=1;
	memcpy(ptr+len,pni->epList[relayIndex].devData.onoffOutStateApp.timer.s_day,1);len+=1;
	memcpy(ptr+len,pni->epList[relayIndex].devData.onoffOutStateApp.timer.s_hour,1);len+=1;
	memcpy(ptr+len,pni->epList[relayIndex].devData.onoffOutStateApp.timer.s_min,1);len+=1;
	memcpy(ptr+len,&pni->epList[relayIndex].devData.onoffOutStateApp.timer.e_month,1);len+=1;
	memcpy(ptr+len,pni->epList[relayIndex].devData.onoffOutStateApp.timer.e_day,1);len+=1;
	memcpy(ptr+len,pni->epList[relayIndex].devData.onoffOutStateApp.timer.e_hour,1);len+=1;
	memcpy(ptr+len,pni->epList[relayIndex].devData.onoffOutStateApp.timer.e_min,1);len+=1;
	memcpy(ptr+len,pni->epList[relayIndex].devData.onoffOutStateApp.timer.wk_flag,1);len+=1;

	
	memcpy(ptr+len,&pni->epList[usbIndex].devData.onoffOutStateApp.onOffStatus,1);len+=1;
	memcpy(ptr+len,&pni->epList[usbIndex].devData.onoffOutStateApp.timer.s_month,1);len+=1;
	memcpy(ptr+len,pni->epList[usbIndex].devData.onoffOutStateApp.timer.s_day,1);len+=1;
	memcpy(ptr+len,pni->epList[usbIndex].devData.onoffOutStateApp.timer.s_hour,1);len+=1;
	memcpy(ptr+len,pni->epList[usbIndex].devData.onoffOutStateApp.timer.s_min,1);len+=1;
	memcpy(ptr+len,&pni->epList[usbIndex].devData.onoffOutStateApp.timer.e_month,1);len+=1;
	memcpy(ptr+len,pni->epList[usbIndex].devData.onoffOutStateApp.timer.e_day,1);len+=1;
	memcpy(ptr+len,pni->epList[usbIndex].devData.onoffOutStateApp.timer.e_hour,1);len+=1;
	memcpy(ptr+len,pni->epList[usbIndex].devData.onoffOutStateApp.timer.e_min,1);len+=1;
	memcpy(ptr+len,pni->epList[usbIndex].devData.onoffOutStateApp.timer.wk_flag,1);len+=1;
	*bufflen=len;

}
void userCreateSPlugSettingData(char *buff,int *bufflen,HM_DEV_RAM_LIST *pni,DEVINFOOPTION devOption)
{
	uint8_t ZT_valuedata1,ZTData;
	uint32_t converte_data=0,len=0;
	uint8_t relayIndex = 0xff;
	uint8_t usbIndex = 0xff;
	uint8_t *ptr=buff;
	uint16_t lendata=0x000f;

	big_little_exchange(lendata,&converte_data);
	memcpy(ptr,&converte_data,2);len+=2;
	memcpy(ptr+len,&pni->epList[0].devData.splugStateApp.onOffStatus,1);len+=1;
	memcpy(ptr+len,&pni->epList[0].devData.splugStateApp.timer.s_month,1);len+=1;
	memcpy(ptr+len,&pni->epList[0].devData.splugStateApp.timer.s_day,1);len+=1;
	memcpy(ptr+len,&pni->epList[0].devData.splugStateApp.timer.s_hour,1);len+=1;
	memcpy(ptr+len,&pni->epList[0].devData.splugStateApp.timer.s_min,1);len+=1;
	memcpy(ptr+len,&pni->epList[0].devData.splugStateApp.timer.e_month,1);len+=1;
	memcpy(ptr+len,&pni->epList[0].devData.splugStateApp.timer.e_day,1);len+=1;
	memcpy(ptr+len,&pni->epList[0].devData.splugStateApp.timer.e_hour,1);len+=1;
	memcpy(ptr+len,&pni->epList[0].devData.splugStateApp.timer.e_min,1);len+=1;
	memcpy(ptr+len,&pni->epList[0].devData.splugStateApp.timer.wk_flag,1);len+=1;
	*bufflen=len;
}
void userCreateCutRgbSettingData(char *buff,int *bufflen,HM_DEV_RAM_LIST *pni,DEVINFOOPTION devOption)
{
	uint8_t ZT_valuedata1,ZTData;
	uint32_t converte_data=0,len=0;
	uint8_t relayIndex = 0xff;
	uint8_t usbIndex = 0xff;
	uint8_t *ptr=buff;
	uint16_t lendata=0x7fff;
	uint8_t i,s1Index,s2Index,s3Index,s4Index;
	uint8_t OFL1,OFL2,OFL3,OFL4,OFL;
	uint32_t LEL1,LEL2,LEL3,LEL4,LEL;
	uint16_t CWL1,CWL2,CWL3,CWL4,CWL;
	s1Index = 0xff;
	s2Index = 0xff;
	s3Index = 0xff;
	s4Index = 0xff;
	for(i = 0; i < pni->epNum; i ++)//ep 1 默认为第一路switch，EP 2默认为第二路switch
	{
		if(pni->epList[i].ep == 1)
		{
			s1Index = i;
		}else if(pni->epList[i].ep == 2){
			s2Index = i;
		}else if(pni->epList[i].ep == 3){
			s3Index = i;
		}else if(pni->epList[i].ep == 4){
			s4Index = i;
		}
	}

	big_little_exchange(lendata,&converte_data);
	memcpy(ptr,&converte_data,2);len+=2;
	OFL4=(pni->epList[s4Index].devData.cstRgbStateApp.onOffStatus&0x01)<<3;
	OFL3=(pni->epList[s3Index].devData.cstRgbStateApp.onOffStatus&0x01)<<2;
	OFL2=(pni->epList[s2Index].devData.cstRgbStateApp.onOffStatus&0x01)<<1;
	OFL1=(pni->epList[s2Index].devData.cstRgbStateApp.onOffStatus&0x01)+OFL2+OFL3+OFL4;
	memcpy(ptr,&OFL1,1);len+=1;
	LEL1=(pni->epList[s1Index].devData.cstRgbStateApp.brightness&0xff)<<24;
	LEL2=(pni->epList[s2Index].devData.cstRgbStateApp.brightness&0xff)<<16;
	LEL3=(pni->epList[s3Index].devData.cstRgbStateApp.brightness&0xff)<<8;
	LEL4=(pni->epList[s4Index].devData.cstRgbStateApp.brightness&0xff)+LEL2+LEL3+LEL4;
	memcpy(ptr,&LEL4,4);len+=4;

	CWL1=(pni->epList[s1Index].devData.cstRgbStateApp.colorTemrature&0xff)<<16;
	CWL2=(pni->epList[s2Index].devData.cstRgbStateApp.colorTemrature&0xff)<<12;
	CWL3=(pni->epList[s3Index].devData.cstRgbStateApp.colorTemrature&0xff)<<4;
	CWL4=(pni->epList[s4Index].devData.cstRgbStateApp.colorTemrature&0xff)+CWL2+CWL3+CWL4;
	memcpy(ptr,&CWL4,2);len+=2;

	memcpy(ptr,&pni->epList[s1Index].devData.cstRgbStateApp.timer.s_month,1);len+=1;
	memcpy(ptr,&pni->epList[s1Index].devData.cstRgbStateApp.timer.s_day,1);len+=1;
	memcpy(ptr,&pni->epList[s1Index].devData.cstRgbStateApp.timer.s_hour,1);len+=1;
	memcpy(ptr,&pni->epList[s1Index].devData.cstRgbStateApp.timer.s_min,1);len+=1;
	memcpy(ptr,&pni->epList[s1Index].devData.cstRgbStateApp.timer.e_month,1);len+=1;
	memcpy(ptr,&pni->epList[s1Index].devData.cstRgbStateApp.timer.e_day,1);len+=1;
	memcpy(ptr,&pni->epList[s1Index].devData.cstRgbStateApp.timer.e_hour,1);len+=1;
	memcpy(ptr,&pni->epList[s1Index].devData.cstRgbStateApp.timer.e_min,1);len+=1;
	memcpy(ptr,&pni->epList[s1Index].devData.cstRgbStateApp.timer.wk_flag,1);len+=1;

	memcpy(ptr,&pni->epList[s2Index].devData.cstRgbStateApp.timer.s_month,1);len+=1;
	memcpy(ptr,&pni->epList[s2Index].devData.cstRgbStateApp.timer.s_day,1);len+=1;
	memcpy(ptr,&pni->epList[s2Index].devData.cstRgbStateApp.timer.s_hour,1);len+=1;
	memcpy(ptr,&pni->epList[s2Index].devData.cstRgbStateApp.timer.s_min,1);len+=1;
	memcpy(ptr,&pni->epList[s2Index].devData.cstRgbStateApp.timer.e_month,1);len+=1;
	memcpy(ptr,&pni->epList[s2Index].devData.cstRgbStateApp.timer.e_day,1);len+=1;
	memcpy(ptr,&pni->epList[s2Index].devData.cstRgbStateApp.timer.e_hour,1);len+=1;
	memcpy(ptr,&pni->epList[s2Index].devData.cstRgbStateApp.timer.e_min,1);len+=1;
	memcpy(ptr,&pni->epList[s2Index].devData.cstRgbStateApp.timer.wk_flag,1);len+=1;

	memcpy(ptr,&pni->epList[s3Index].devData.cstRgbStateApp.timer.s_month,1);len+=1;
	memcpy(ptr,&pni->epList[s3Index].devData.cstRgbStateApp.timer.s_day,1);len+=1;
	memcpy(ptr,&pni->epList[s3Index].devData.cstRgbStateApp.timer.s_hour,1);len+=1;
	memcpy(ptr,&pni->epList[s3Index].devData.cstRgbStateApp.timer.s_min,1);len+=1;
	memcpy(ptr,&pni->epList[s3Index].devData.cstRgbStateApp.timer.e_month,1);len+=1;
	memcpy(ptr,&pni->epList[s3Index].devData.cstRgbStateApp.timer.e_day,1);len+=1;
	memcpy(ptr,&pni->epList[s3Index].devData.cstRgbStateApp.timer.e_hour,1);len+=1;
	memcpy(ptr,&pni->epList[s3Index].devData.cstRgbStateApp.timer.e_min,1);len+=1;
	memcpy(ptr,&pni->epList[s3Index].devData.cstRgbStateApp.timer.wk_flag,1);len+=1;

	memcpy(ptr,&pni->epList[s4Index].devData.cstRgbStateApp.timer.s_month,1);len+=1;
	memcpy(ptr,&pni->epList[s4Index].devData.cstRgbStateApp.timer.s_day,1);len+=1;
	memcpy(ptr,&pni->epList[s4Index].devData.cstRgbStateApp.timer.s_hour,1);len+=1;
	memcpy(ptr,&pni->epList[s4Index].devData.cstRgbStateApp.timer.s_min,1);len+=1;
	memcpy(ptr,&pni->epList[s4Index].devData.cstRgbStateApp.timer.e_month,1);len+=1;
	memcpy(ptr,&pni->epList[s4Index].devData.cstRgbStateApp.timer.e_day,1);len+=1;
	memcpy(ptr,&pni->epList[s4Index].devData.cstRgbStateApp.timer.e_hour,1);len+=1;
	memcpy(ptr,&pni->epList[s4Index].devData.cstRgbStateApp.timer.e_min,1);len+=1;
	memcpy(ptr,&pni->epList[s4Index].devData.cstRgbStateApp.timer.wk_flag,1);len+=1;
	*bufflen=len;
}
void userCreateTHPSettingData(char *buff,int *bufflen,HM_DEV_RAM_LIST *pni,DEVINFOOPTION devOption)
{
	uint8_t THP_enable,TP_enable,HP_enable;
	uint32_t converte_data=0,len=0;
	uint8_t i,tempEpIndex = 0xff;
	uint8_t humiEpIndex = 0xff;
	uint8_t *ptr=buff;
	uint16_t lendata=0x001f;
	
	for(i = 0; i < pni->epNum; i ++)	//默认EP 1为temperature，2为humility
	{
		if(pni->epList[i].ep == 1)
		{
			tempEpIndex = i;
		}else if(pni->epList[i].ep == 2){
			humiEpIndex = i;
		}
	}
	big_little_exchange(lendata,&converte_data);
	memcpy(ptr,&converte_data,2);len+=2;
	if(humiEpIndex != 0xff)
	{
		big_little_exchange(pni->epList[humiEpIndex].devData.humiStateApp.upHumi,&converte_data);
		memcpy(ptr+len,&converte_data,2);len+=2;
		big_little_exchange(pni->epList[humiEpIndex].devData.humiStateApp.lowHumi,&converte_data);
		memcpy(ptr+len,&converte_data,2);len+=2;
		HP_enable=pni->epList[humiEpIndex].devData.humiStateApp.checkEnable;
	}
	if(tempEpIndex != 0xff)
	{
		big_little_exchange(pni->epList[tempEpIndex].devData.tempStateApp.upTemp,&converte_data);
		memcpy(ptr+len,&converte_data,2);len+=2;
		big_little_exchange(pni->epList[tempEpIndex].devData.tempStateApp.lowTemp,&converte_data);
		memcpy(ptr+len,&converte_data,2);len+=2;
		TP_enable=pni->epList[tempEpIndex].devData.tempStateApp.checkEnable;
	}
	THP_enable=(TP_enable<<4)+HP_enable;
	memcpy(ptr+len,&THP_enable,1);len+=1;
	*bufflen=len;

}


void userCreatNodeState_data(HM_DEV_RAM_LIST *pni,uint8_t stateMode,DEVINFOOPTION devOption)
{
	char sent_buff[512];
	int sent_buff_len;
	ZB_DOOR_PARAM 		cur_zbdoor_type;
	ZB_WATER_PARAM 		cur_zbwater_type;
	ZB_PIR_PARAM 		cur_zbpir_type;
	ZB_SMOKE_PARAM 		cur_zbsmoke_type;
	ZB_GAS_PARAM 		cur_gas_type;
	ZB_CO_PARAM 		cur_co_type;
	ZB_SOS_PARAM 		cur_sos_type;
	ZB_REMOTE_PARAM 	cur_remote_type;
	ZB_HUMITURE_PARAM 	cur_humiture_type;
	ZB_RGB_PARAM 		cur_rgb_type;
	ZB_UPLUG_PARAM 		cur_uplug_type;
	ZB_SPLUG_PARAM 		cur_splug_type;
	ZB_Slight1_PARAM 	cur_slight1_type;
	ZB_Slight2_PARAM 	cur_slight2_type;
	ZB_Slight3_PARAM 	cur_slight3_type;
	ZB_SAlARM_PARAM 	cur_salarm_type;
	ZB_FOURLAMP_PARAM 	cur_fourlamp_type;
	ZB_VIBRATION_PARAM 	cur_vibration_type;
	

	//memset(oid,'\0',25);
	//jsMain = json_create_object();
	//if(jsMain != NULL)
	printf("pni->devType=%x-----------\r\n",pni->devType);
	if(pni->devType!=NULL)
		{
			printf("userCreatNodeState_data if(pni->devType!=NULL)-----------\r\n");
		}
	else{
			printf(" if(pni->devType =NULL)-----------\r\n");
		}
	//return;
	memset(sent_buff,0,sizeof(sent_buff));
		switch(pni->devType)
		{
			case NODE_INFO_TYPE_LIGHT_RGB:
				if((CREATE_DEV_STATE_FOR_NEW_NODE_JOIN_OID == stateMode) || (CREATE_DEV_STATE_FOR_UPDATE_STATE_OID == stateMode))
				{
					printf("hm net work JOIN NODE_INFO_TYPE_LIGHT_RGB \r\n");
					if(CREATE_DEV_STATE_FOR_NEW_NODE_JOIN_OID == stateMode){
					userCreateSta_rgbdata(sent_buff,&sent_buff_len,pni,devOption);
					report_zb_intonetwork(pni,sent_buff,sent_buff_len);}

					memset(sent_buff,0,sizeof(sent_buff));
					userCreateSta_rgb_pointdata(sent_buff,&sent_buff_len,pni,devOption);
					report_zb_pointdata(pni,sent_buff,sent_buff_len);
					if(CREATE_DEV_STATE_FOR_UPDATE_STATE_OID == stateMode){
					memset(sent_buff,0,sizeof(sent_buff));
					userCreateSta_rgbdata(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SSstate(pni,sent_buff,sent_buff_len);
					}
				}
				else if(CREATE_dEV_STATE_FOR_ALARM_OID == stateMode){
					printf("hm net work SS NODE_INFO_TYPE_LIGHT_RGB \r\n");
					hm_get_rgblight_state(pni,cur_rgb_type);
					userCreateSta_rgbdata(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SSstate(pni,sent_buff,sent_buff_len);

					memset(sent_buff,0,sizeof(sent_buff));
					userCreateSta_rgb_pointdata(sent_buff,&sent_buff_len,pni,devOption);
					report_zb_pointdata(pni,sent_buff,sent_buff_len);	
				}
				else if(CREATE_DEV_STATE_FOR_GET_NODE_SETTING_OID == stateMode){
					printf("hm net work SE RGB \r\n");
					userCreateRGBSettingData(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SEsetting(pni,sent_buff,sent_buff_len);
				}
				else if(stateMode == CREATE_DEV_STATE_FOR_GET_NODE_STATE_OID){
					printf("hm net work single RGB \r\n");
					userCreateSta_rgbdata(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SingleSS(pni,sent_buff,sent_buff_len);
				}
				break;
			case NODE_INFO_TYPE_AI1SW:
				if((CREATE_DEV_STATE_FOR_NEW_NODE_JOIN_OID == stateMode) || (CREATE_DEV_STATE_FOR_UPDATE_STATE_OID == stateMode))
				{
					printf("hm net work  NODE_INFO_TYPE_AI1SW \r\n");
					if(CREATE_DEV_STATE_FOR_NEW_NODE_JOIN_OID == stateMode){
					userCreateSta_AI1SWdata(sent_buff,&sent_buff_len,pni,devOption);
					report_zb_intonetwork(pni,sent_buff,sent_buff_len);}

					memset(sent_buff,0,sizeof(sent_buff));
					userCreateSta_AI1SW_pointdata(sent_buff,&sent_buff_len,pni,devOption);
					report_zb_pointdata(pni,sent_buff,sent_buff_len);
					if(CREATE_DEV_STATE_FOR_UPDATE_STATE_OID == stateMode){
					memset(sent_buff,0,sizeof(sent_buff));
					userCreateSta_AI1SWdata(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SSstate(pni,sent_buff,sent_buff_len);
					}
				}
				else if(CREATE_dEV_STATE_FOR_ALARM_OID == stateMode){
					printf("hm net work SS NODE_INFO_TYPE_AI1SW \r\n");
					hm_get_ai1sw_state(pni,cur_slight1_type);
					userCreateSta_AI1SWdata(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SSstate(pni,sent_buff,sent_buff_len);	

					memset(sent_buff,0,sizeof(sent_buff));
					userCreateSta_AI1SW_pointdata(sent_buff,&sent_buff_len,pni,devOption);
					report_zb_pointdata(pni,sent_buff,sent_buff_len);
				}
				else if(CREATE_DEV_STATE_FOR_GET_NODE_SETTING_OID == stateMode){
					printf("hm net work SE AI1SW \r\n");
					userCreateAISWSettingData(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SEsetting(pni,sent_buff,sent_buff_len);
				
				}
				else if(stateMode == CREATE_DEV_STATE_FOR_GET_NODE_STATE_OID){
					printf("hm net work single AI1SW \r\n");
					userCreateSta_AI1SWdata(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SingleSS(pni,sent_buff,sent_buff_len);
				}
				break;
			case NODE_INFO_TYPE_AI2SW:
				if((CREATE_DEV_STATE_FOR_NEW_NODE_JOIN_OID == stateMode) || (CREATE_DEV_STATE_FOR_UPDATE_STATE_OID == stateMode))
				{
					printf("hm net work  join NODE_INFO_TYPE_AI2SW \r\n");
					if(CREATE_DEV_STATE_FOR_NEW_NODE_JOIN_OID == stateMode){
					userCreateSta_AI2SWdata(sent_buff,&sent_buff_len,pni,devOption);
					report_zb_intonetwork(pni,sent_buff,sent_buff_len);}

					memset(sent_buff,0,sizeof(sent_buff));
					userCreateSta_AI2SW_pointdata(sent_buff,&sent_buff_len,pni,devOption);
					report_zb_pointdata(pni,sent_buff,sent_buff_len);
					if(CREATE_DEV_STATE_FOR_UPDATE_STATE_OID == stateMode){
					memset(sent_buff,0,sizeof(sent_buff));
					userCreateSta_AI2SWdata(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SSstate(pni,sent_buff,sent_buff_len);
					}
				}
				else if(CREATE_dEV_STATE_FOR_ALARM_OID == stateMode){
					printf("hm net work SS NODE_INFO_TYPE_AI2SW \r\n");
					hm_get_ai2sw_state(pni,cur_slight2_type);
					userCreateSta_AI2SWdata(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SSstate(pni,sent_buff,sent_buff_len);

					memset(sent_buff,0,sizeof(sent_buff));
					userCreateSta_AI2SW_pointdata(sent_buff,&sent_buff_len,pni,devOption);
					report_zb_pointdata(pni,sent_buff,sent_buff_len);
				}
				else if(CREATE_DEV_STATE_FOR_GET_NODE_SETTING_OID == stateMode){
					printf("hm net work SE AI2SW \r\n");
					userCreateAISWSettingData(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SEsetting(pni,sent_buff,sent_buff_len);
				}
				else if(stateMode == CREATE_DEV_STATE_FOR_GET_NODE_STATE_OID){
					printf("hm net work single AI2SW \r\n");
					userCreateSta_AI2SWdata(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SingleSS(pni,sent_buff,sent_buff_len);
				}
				break;
			case NODE_INFO_TYPE_AI3SW:
				if((CREATE_DEV_STATE_FOR_NEW_NODE_JOIN_OID == stateMode) || (CREATE_DEV_STATE_FOR_UPDATE_STATE_OID == stateMode))
				{
					
					if(CREATE_DEV_STATE_FOR_NEW_NODE_JOIN_OID == stateMode){
						printf("hm net work  JOIN NODE_INFO_TYPE_AI3SW \r\n");
					userCreateSta_AI3SWdata(sent_buff,&sent_buff_len,pni,devOption);
					report_zb_intonetwork(pni,sent_buff,sent_buff_len);}

					if(CREATE_DEV_STATE_FOR_UPDATE_STATE_OID == stateMode){
						printf("hm net work  JOIN CREATE_DEV_STATE_FOR_UPDATE_STATE_OID \r\n");
					memset(sent_buff,0,sizeof(sent_buff));
					userCreateSta_AI3SWdata(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SSstate(pni,sent_buff,sent_buff_len);
					}

					memset(sent_buff,0,sizeof(sent_buff));
					userCreateSta_AI3SW_pointdata(sent_buff,&sent_buff_len,pni,devOption);
					report_zb_pointdata(pni,sent_buff,sent_buff_len);
				}
				else if(CREATE_dEV_STATE_FOR_ALARM_OID == stateMode){
					printf("hm net work SS NODE_INFO_TYPE_AI3SW \r\n");
					hm_get_ai3sw_state(pni,cur_slight3_type);
					userCreateSta_AI3SWdata(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SSstate(pni,sent_buff,sent_buff_len);

					memset(sent_buff,0,sizeof(sent_buff));
					userCreateSta_AI3SW_pointdata(sent_buff,&sent_buff_len,pni,devOption);
					report_zb_pointdata(pni,sent_buff,sent_buff_len);
				}
				else if(CREATE_DEV_STATE_FOR_GET_NODE_SETTING_OID == stateMode){
					printf("hm net work SE AI3SW \r\n");
					userCreateAISWSettingData(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SEsetting(pni,sent_buff,sent_buff_len);
				}
				else if(stateMode == CREATE_DEV_STATE_FOR_GET_NODE_STATE_OID){
					printf("hm net work single AI3SW \r\n");
					userCreateSta_AI3SWdata(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SingleSS(pni,sent_buff,sent_buff_len);
				}
				break;
			case NODE_INFO_TYPE_ZB_IR_TRANSPONDER:
				break;
			case NODE_INFO_TYPE_RELAY_CONTROL:
				break;
			case NODE_INFO_TYPE_MAGNET_DOOR:
				if((CREATE_DEV_STATE_FOR_NEW_NODE_JOIN_OID == stateMode)||(CREATE_DEV_STATE_FOR_UPDATE_STATE_OID == stateMode))
				{
					printf("hm net work  join NODE_INFO_TYPE_MAGNET_DOOR -----------\r\n");
					if(CREATE_DEV_STATE_FOR_NEW_NODE_JOIN_OID == stateMode){
					userCreateIasSta_actualdata(sent_buff,&sent_buff_len,pni,devOption);
					report_zb_intonetwork(pni,sent_buff,sent_buff_len);}

					memset(sent_buff,0,sizeof(sent_buff));
					userCreateIasSta_actual_pointdata(sent_buff,&sent_buff_len,pni,devOption);
					report_zb_pointdata(pni,sent_buff,sent_buff_len);
				}
				else if(CREATE_dEV_STATE_FOR_ALARM_OID == stateMode){
					printf("hm net work SS NODE_INFO_TYPE_MAGNET_DOOR \r\n");
					hm_get_doorsensor_state(pni,cur_zbdoor_type);
				
					userCreateIasSta_actualdata(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SSstate(pni,sent_buff,sent_buff_len);

					memset(sent_buff,0,sizeof(sent_buff));
					userCreateIasSta_actual_pointdata(sent_buff,&sent_buff_len,pni,devOption);
					report_zb_pointdata(pni,sent_buff,sent_buff_len);
				}
				else if(CREATE_DEV_STATE_FOR_GET_NODE_SETTING_OID == stateMode){
					printf("hm net work SE DOOR \r\n");
					userCreateIasSetting_actualdata(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SEsetting(pni,sent_buff,sent_buff_len);
				}
				else if(stateMode == CREATE_DEV_STATE_FOR_GET_NODE_STATE_OID){
					printf("hm net work single DOOR \r\n");
					userCreateIasSta_actualdata(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SingleSS(pni,sent_buff,sent_buff_len);
				}
				break;
			case NODE_INFO_TYPE_WATER_SENSOR:
				if((CREATE_DEV_STATE_FOR_NEW_NODE_JOIN_OID == stateMode)||(CREATE_DEV_STATE_FOR_UPDATE_STATE_OID == stateMode))
				{
					printf("hm net work join NODE_INFO_TYPE_WATER_SENSOR -----------\r\n");
					if(CREATE_DEV_STATE_FOR_NEW_NODE_JOIN_OID == stateMode){
					userCreateIasSta_actualdata(sent_buff,&sent_buff_len,pni,devOption);
					report_zb_intonetwork(pni,sent_buff,sent_buff_len);}

					memset(sent_buff,0,sizeof(sent_buff));
					userCreateIasSta_actual_pointdata(sent_buff,&sent_buff_len,pni,devOption);
					report_zb_pointdata(pni,sent_buff,sent_buff_len);
				}
				else if(CREATE_dEV_STATE_FOR_ALARM_OID == stateMode){
					printf("hm net work SS NODE_INFO_TYPE_WATER_SENSOR \r\n");
					hm_get_watersensor_state(pni,cur_zbwater_type);
					
					userCreateIasSta_actualdata(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SSstate(pni,sent_buff,sent_buff_len);

					//memset(sent_buff,0,sizeof(sent_buff));
					//userCreateIasSta_actual_pointdata(sent_buff,&sent_buff_len,pni,devOption);
					//report_zb_pointdata(pni,sent_buff,sent_buff_len);
				}
				else if(CREATE_DEV_STATE_FOR_GET_NODE_SETTING_OID == stateMode){
					printf("hm net work SE WATER SENSOR \r\n");
					userCreateIasSetting_actualdata(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SEsetting(pni,sent_buff,sent_buff_len);
				}
				else if(stateMode == CREATE_DEV_STATE_FOR_GET_NODE_STATE_OID){
					printf("hm net work single WATER \r\n");
					userCreateIasSta_actualdata(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SingleSS(pni,sent_buff,sent_buff_len);
				}
				break;
			case NODE_INFO_TYPE_PIR_SENSOR:
				if((CREATE_DEV_STATE_FOR_NEW_NODE_JOIN_OID == stateMode)||(CREATE_DEV_STATE_FOR_UPDATE_STATE_OID == stateMode))
				{
					printf("hm net work join NODE_INFO_TYPE_PIR_SENSOR -----------\r\n");
					if(CREATE_DEV_STATE_FOR_NEW_NODE_JOIN_OID == stateMode){
					userCreateIasSta_actualdata(sent_buff,&sent_buff_len,pni,devOption);
					report_zb_intonetwork(pni,sent_buff,sent_buff_len);}

					memset(sent_buff,0,sizeof(sent_buff));
					userCreateIasSta_actual_pointdata(sent_buff,&sent_buff_len,pni,devOption);
					report_zb_pointdata(pni,sent_buff,sent_buff_len);
					}
				else if(CREATE_dEV_STATE_FOR_ALARM_OID == stateMode){
					printf("hm net work SS NODE_INFO_TYPE_PIR_SENSOR \r\n");
					hm_get_pirsensor_state(pni,cur_zbpir_type);
				
					userCreateIasSta_actualdata(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SSstate(pni,sent_buff,sent_buff_len);

					memset(sent_buff,0,sizeof(sent_buff));
					userCreateIasSta_actual_pointdata(sent_buff,&sent_buff_len,pni,devOption);
					report_zb_pointdata(pni,sent_buff,sent_buff_len);
				}
				else if(CREATE_DEV_STATE_FOR_GET_NODE_SETTING_OID == stateMode){
					printf("hm net work SE PIR SENSOR \r\n");
					userCreateIasSetting_actualdata(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SEsetting(pni,sent_buff,sent_buff_len);
				}
				else if(stateMode == CREATE_DEV_STATE_FOR_GET_NODE_STATE_OID){
					printf("hm net work single PIR \r\n");
					userCreateIasSta_actualdata(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SingleSS(pni,sent_buff,sent_buff_len);
				}
				break;
			case NODE_INFO_TYPE_SMOKE_SENSOR:
				if((CREATE_DEV_STATE_FOR_NEW_NODE_JOIN_OID == stateMode)||(CREATE_DEV_STATE_FOR_UPDATE_STATE_OID == stateMode))
				{
					printf("hm net work join NODE_INFO_TYPE_SMOKE_SENSOR -----------\r\n");
					if(CREATE_DEV_STATE_FOR_NEW_NODE_JOIN_OID == stateMode){
					userCreateIasSta_actualdata(sent_buff,&sent_buff_len,pni,devOption);
					report_zb_intonetwork(pni,sent_buff,sent_buff_len);}

					memset(sent_buff,0,sizeof(sent_buff));
					userCreateIasSta_actual_pointdata(sent_buff,&sent_buff_len,pni,devOption);
					report_zb_pointdata(pni,sent_buff,sent_buff_len);
					}
				else if(CREATE_dEV_STATE_FOR_ALARM_OID == stateMode){
					printf("hm net work SS NODE_INFO_TYPE_SMOKE_SENSOR \r\n");
					hm_get_smokesensor_state(pni,cur_zbsmoke_type);
					userCreateIasSta_actualdata(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SSstate(pni,sent_buff,sent_buff_len);

					//memset(sent_buff,0,sizeof(sent_buff));
					//userCreateIasSta_actual_pointdata(sent_buff,&sent_buff_len,pni,devOption);
					//report_zb_pointdata(pni,sent_buff,sent_buff_len);
				}
				else if(CREATE_DEV_STATE_FOR_GET_NODE_SETTING_OID == stateMode){
					printf("hm net work SE SMOKE SENSOR \r\n");
					userCreateIasSetting_actualdata(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SEsetting(pni,sent_buff,sent_buff_len);
				}
				else if(stateMode == CREATE_DEV_STATE_FOR_GET_NODE_STATE_OID){
					printf("hm net work single SMOKE \r\n");
					userCreateIasSta_actualdata(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SingleSS(pni,sent_buff,sent_buff_len);
				}
				break;
			case NODE_INFO_TYPE_GAS_SENSOR:
				if((CREATE_DEV_STATE_FOR_NEW_NODE_JOIN_OID == stateMode)||(CREATE_DEV_STATE_FOR_UPDATE_STATE_OID == stateMode))
				{
				//气感
					printf("hm net work join NODE_INFO_TYPE_GAS_SENSOR -----------\r\n");
					if(CREATE_DEV_STATE_FOR_NEW_NODE_JOIN_OID == stateMode){
					userCreateIasSta_actualdata(sent_buff,&sent_buff_len,pni,devOption);
					report_zb_intonetwork(pni,sent_buff,sent_buff_len);}

					memset(sent_buff,0,sizeof(sent_buff));
					userCreateIasSta_actual_pointdata(sent_buff,&sent_buff_len,pni,devOption);
					report_zb_pointdata(pni,sent_buff,sent_buff_len);
				}
				else if(CREATE_dEV_STATE_FOR_ALARM_OID == stateMode){
					printf("hm net work SS NODE_INFO_TYPE_GAS_SENSOR \r\n");
					hm_get_gassensor_state(pni,cur_gas_type);
				
					userCreateIasSta_actualdata(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SSstate(pni,sent_buff,sent_buff_len);

					//memset(sent_buff,0,sizeof(sent_buff));
					//userCreateIasSta_actual_pointdata(sent_buff,&sent_buff_len,pni,devOption);
					//report_zb_pointdata(pni,sent_buff,sent_buff_len);
				}
				else if(CREATE_DEV_STATE_FOR_GET_NODE_SETTING_OID == stateMode){
					printf("hm net work SE GAS SENSOR \r\n");
					userCreateIasSetting_actualdata(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SEsetting(pni,sent_buff,sent_buff_len);
				}
				else if(stateMode == CREATE_DEV_STATE_FOR_GET_NODE_STATE_OID){
					printf("hm net work single GAS \r\n");
					userCreateIasSta_actualdata(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SingleSS(pni,sent_buff,sent_buff_len);
				}
				break;
	
			case NODE_INFO_TYPE_CO_SENSOR:
				if((CREATE_DEV_STATE_FOR_NEW_NODE_JOIN_OID == stateMode)||(CREATE_DEV_STATE_FOR_UPDATE_STATE_OID == stateMode))
				{
					printf("hm net work join NODE_INFO_TYPE_CO_SENSOR -----------\r\n");
					if(CREATE_DEV_STATE_FOR_NEW_NODE_JOIN_OID == stateMode){
					userCreateIasSta_actualdata(sent_buff,&sent_buff_len,pni,devOption);
					report_zb_intonetwork(pni,sent_buff,sent_buff_len);}

					memset(sent_buff,0,sizeof(sent_buff));
					userCreateIasSta_actual_pointdata(sent_buff,&sent_buff_len,pni,devOption);
					report_zb_pointdata(pni,sent_buff,sent_buff_len);
					}
				else if(CREATE_dEV_STATE_FOR_ALARM_OID == stateMode){
					printf("hm net work SS NODE_INFO_TYPE_CO_SENSOR \r\n");
					hm_get_cosensor_state(pni,cur_co_type);
				
					userCreateIasSta_actualdata(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SSstate(pni,sent_buff,sent_buff_len);

					//memset(sent_buff,0,sizeof(sent_buff));
					//userCreateIasSta_actual_pointdata(sent_buff,&sent_buff_len,pni,devOption);
					//report_zb_pointdata(pni,sent_buff,sent_buff_len);
				}
				else if(CREATE_DEV_STATE_FOR_GET_NODE_SETTING_OID == stateMode){
					printf("hm net work SE CO SENSOR \r\n");
					userCreateIasSetting_actualdata(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SEsetting(pni,sent_buff,sent_buff_len);
				}
				else if(stateMode == CREATE_DEV_STATE_FOR_GET_NODE_STATE_OID){
					printf("hm net work single CO \r\n");
					userCreateIasSta_actualdata(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SingleSS(pni,sent_buff,sent_buff_len);
				}
				break;
			case NODE_INFO_TYPE_VIBRATION:
				if((CREATE_DEV_STATE_FOR_NEW_NODE_JOIN_OID == stateMode)||(CREATE_DEV_STATE_FOR_UPDATE_STATE_OID == stateMode))
				{
					printf("hm net work join NODE_INFO_TYPE_VIBRATION -----------\r\n");
					if(CREATE_DEV_STATE_FOR_NEW_NODE_JOIN_OID == stateMode){
					userCreateIasSta_actualdata(sent_buff,&sent_buff_len,pni,devOption);
					report_zb_intonetwork(pni,sent_buff,sent_buff_len);}

					memset(sent_buff,0,sizeof(sent_buff));
					userCreateIasSta_actual_pointdata(sent_buff,&sent_buff_len,pni,devOption);
					report_zb_pointdata(pni,sent_buff,sent_buff_len);
					}
				else if(CREATE_dEV_STATE_FOR_ALARM_OID == stateMode){
					printf("hm net work SS NODE_INFO_TYPE_VIBRATION \r\n");
					hm_get_vibrationsensor_state(pni,cur_vibration_type);
				
					userCreateIasSta_actualdata(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SSstate(pni,sent_buff,sent_buff_len);

					memset(sent_buff,0,sizeof(sent_buff));
					userCreateIasSta_actual_pointdata(sent_buff,&sent_buff_len,pni,devOption);
					report_zb_pointdata(pni,sent_buff,sent_buff_len);
				}
				else if(CREATE_DEV_STATE_FOR_GET_NODE_SETTING_OID == stateMode){
					printf("hm net work SE VIBRATIONSENSOR \r\n");
					userCreateIasSetting_actualdata(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SEsetting(pni,sent_buff,sent_buff_len);
				}
				else if(stateMode == CREATE_DEV_STATE_FOR_GET_NODE_STATE_OID){
					printf("hm net work single VIBRATION \r\n");
					userCreateIasSta_actualdata(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SingleSS(pni,sent_buff,sent_buff_len);
				}
				break;
			case NODE_INFO_TYPE_SOS:
				if((CREATE_DEV_STATE_FOR_NEW_NODE_JOIN_OID == stateMode)||(CREATE_DEV_STATE_FOR_UPDATE_STATE_OID == stateMode))
				{
					printf("hm net work join NODE_INFO_TYPE_SOS -----------\r\n");
					if(CREATE_DEV_STATE_FOR_NEW_NODE_JOIN_OID == stateMode){
					userCreateIasSta_actualdata(sent_buff,&sent_buff_len,pni,devOption);
					report_zb_intonetwork(pni,sent_buff,sent_buff_len);}

					memset(sent_buff,0,sizeof(sent_buff));
					userCreateIasSta_actual_pointdata(sent_buff,&sent_buff_len,pni,devOption);
					report_zb_pointdata(pni,sent_buff,sent_buff_len);
					}
				else if(CREATE_dEV_STATE_FOR_ALARM_OID == stateMode){
					printf("hm net work SS NODE_INFO_TYPE_SOS \r\n");
					hm_get_sossensor_state(pni,cur_sos_type);
					userCreateIasSta_actualdata(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SSstate(pni,sent_buff,sent_buff_len);

					//memset(sent_buff,0,sizeof(sent_buff));
					//userCreateIasSta_actual_pointdata(sent_buff,&sent_buff_len,pni,devOption);
					//report_zb_pointdata(pni,sent_buff,sent_buff_len);
				}
				else if(CREATE_DEV_STATE_FOR_GET_NODE_SETTING_OID == stateMode){
					printf("hm net work SE SOS \r\n");
					userCreateIasSetting_actualdata(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SEsetting(pni,sent_buff,sent_buff_len);
				}
				else if(stateMode == CREATE_DEV_STATE_FOR_GET_NODE_STATE_OID){
					printf("hm net work single SOS \r\n");
					userCreateIasSta_actualdata(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SingleSS(pni,sent_buff,sent_buff_len);
				}
				break;
			case NODE_INFO_TYPE_TEMP_HUM_SENSOR:
				if((CREATE_DEV_STATE_FOR_NEW_NODE_JOIN_OID == stateMode)||(CREATE_DEV_STATE_FOR_UPDATE_STATE_OID == stateMode))
				{
					printf("hm net work join TEMP_HUM -----------\r\n");
					if(CREATE_DEV_STATE_FOR_NEW_NODE_JOIN_OID == stateMode){
					userCreateSta_TEMPHUMdata(sent_buff,&sent_buff_len,pni,devOption);
					report_zb_intonetwork(pni,sent_buff,sent_buff_len);
					}

					memset(sent_buff,0,sizeof(sent_buff));
					userCreateIasSta_TEMPHUM_pointdata(sent_buff,&sent_buff_len,pni,devOption);
					report_zb_pointdata(pni,sent_buff,sent_buff_len);
				}
				else if(CREATE_dEV_STATE_FOR_ALARM_OID == stateMode){
					printf("hm net work SS TEMP_HUM \r\n");
					hm_get_humituresensor_state(pni,cur_humiture_type);
					userCreateSta_TEMPHUMdata(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SSstate(pni,sent_buff,sent_buff_len);

					memset(sent_buff,0,sizeof(sent_buff));
					userCreateIasSta_TEMPHUM_pointdata(sent_buff,&sent_buff_len,pni,devOption);
					report_zb_pointdata(pni,sent_buff,sent_buff_len);
				}
				else if(CREATE_DEV_STATE_FOR_GET_NODE_SETTING_OID == stateMode){
					printf("hm net work SE TEMP_HUM \r\n");
					userCreateTHPSettingData(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SEsetting(pni,sent_buff,sent_buff_len);//----------------------------
				}
				else if(stateMode == CREATE_DEV_STATE_FOR_GET_NODE_STATE_OID)
				{
					printf("hm net work single TEMP_HUM \r\n");
					userCreateSta_TEMPHUMdata(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SingleSS(pni,sent_buff,sent_buff_len);
				}
				break;
			case NODE_INFO_TYPE_WARNING_DEV:
				if((CREATE_DEV_STATE_FOR_NEW_NODE_JOIN_OID == stateMode)||(CREATE_DEV_STATE_FOR_UPDATE_STATE_OID == stateMode))
				{
					printf("hm net work join NODE_INFO_TYPE_WARNING_DEV -----------\r\n");
					if(CREATE_DEV_STATE_FOR_NEW_NODE_JOIN_OID == stateMode){
					userCreateIasSta_actualdata(sent_buff,&sent_buff_len,pni,devOption);
					report_zb_intonetwork(pni,sent_buff,sent_buff_len);}

					memset(sent_buff,0,sizeof(sent_buff));
					userCreateIasSta_actual_pointdata(sent_buff,&sent_buff_len,pni,devOption);
					report_zb_pointdata(pni,sent_buff,sent_buff_len);
					}
				else if(CREATE_dEV_STATE_FOR_ALARM_OID == stateMode){
					printf("hm net work SS NODE_INFO_TYPE_WARNING_DEV \r\n");
					hm_get_sensoralarm_state(pni,cur_salarm_type);
					userCreateIasSta_actualdata(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SSstate(pni,sent_buff,sent_buff_len);

					memset(sent_buff,0,sizeof(sent_buff));
					userCreateIasSta_actual_pointdata(sent_buff,&sent_buff_len,pni,devOption);
					report_zb_pointdata(pni,sent_buff,sent_buff_len);
				}
				else if(CREATE_DEV_STATE_FOR_GET_NODE_SETTING_OID == stateMode){
					printf("hm net work SE WARNING \r\n");
					userCreateWDSettingData(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SEsetting(pni,sent_buff,sent_buff_len);
				}
				else if(stateMode == CREATE_DEV_STATE_FOR_GET_NODE_STATE_OID){
					printf("hm net work single WARNING \r\n");
					userCreateIasSta_actualdata(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SingleSS(pni,sent_buff,sent_buff_len);
				}
				break;
			case NODE_INFO_TYPE_ILLUMINATION:
				break;
			case NODE_INFO_TYPE_AIR_QUALITY:
				break;
			case NODE_INFO_TYPE_THERMOSTAT:
				break;

			case NODE_INFO_TYPE_REMOTE_CONTROL://--------------
				if((CREATE_DEV_STATE_FOR_NEW_NODE_JOIN_OID == stateMode)||(CREATE_DEV_STATE_FOR_UPDATE_STATE_OID == stateMode))
				{
					printf("hm net work join NODE_INFO_TYPE_REMOTE_CONTROL -----------\r\n");
					if(CREATE_DEV_STATE_FOR_NEW_NODE_JOIN_OID == stateMode){
					//userCreateIasSta_actualdata(sent_buff,&sent_buff_len,pni,devOption);
					userCreateAceSta_actualdata(sent_buff,&sent_buff_len,pni,devOption);
					report_zb_intonetwork(pni,sent_buff,sent_buff_len);}

					
					memset(sent_buff,0,sizeof(sent_buff));
					//userCreateIasSta_actual_pointdata(sent_buff,&sent_buff_len,pni,devOption);
					userCreateAceSta_actual_pointdata(sent_buff,&sent_buff_len,pni,devOption);
					report_zb_pointdata(pni,sent_buff,sent_buff_len);
					}
				else if(CREATE_dEV_STATE_FOR_ALARM_OID == stateMode){
					printf("hm net work SS NODE_INFO_TYPE_REMOTE_CONTROL \r\n");
					hm_get_remotecontrol_state(pni,cur_remote_type);
				
					userCreateAceSta_actualdata(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SSstate(pni,sent_buff,sent_buff_len);

					memset(sent_buff,0,sizeof(sent_buff));
					userCreateAceSta_actual_pointdata(sent_buff,&sent_buff_len,pni,devOption);
					report_zb_pointdata(pni,sent_buff,sent_buff_len);
				}
				else if(CREATE_DEV_STATE_FOR_GET_NODE_SETTING_OID == stateMode){
					printf("hm net work SE REMOTE_CONTROL  no you setting\r\n");
					//userCreateIasSetting_actualdata(sent_buff,&sent_buff_len,pni,devOption);
					//sent_zigbee_SEsetting(pni,sent_buff,sent_buff_len);
				}
				else if(stateMode == CREATE_DEV_STATE_FOR_GET_NODE_STATE_OID){
					printf("hm net work single REMOTE_CONTROL \r\n");
					//userCreateIasSta_actualdata(sent_buff,&sent_buff_len,pni,devOption);
					userCreateAceSta_actualdata(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SingleSS(pni,sent_buff,sent_buff_len);
				}
				break;
			case NODE_INFO_TYPE_AI_SOCKET_USB:
				if((CREATE_DEV_STATE_FOR_NEW_NODE_JOIN_OID == stateMode)||(CREATE_DEV_STATE_FOR_UPDATE_STATE_OID == stateMode))
				{
					printf("hm net work  NODE_INFO_TYPE_AI_SOCKET_USB -----------\r\n");
					if(CREATE_DEV_STATE_FOR_NEW_NODE_JOIN_OID == stateMode){
					userCreateSta_SOCKETUSBdata(sent_buff,&sent_buff_len,pni,devOption);
					report_zb_intonetwork(pni,sent_buff,sent_buff_len);}

					memset(sent_buff,0,sizeof(sent_buff));
					userCreateSta_SOCKETUSB_pointdata(sent_buff,&sent_buff_len,pni,devOption);
					report_zb_pointdata(pni,sent_buff,sent_buff_len);
					if(CREATE_DEV_STATE_FOR_UPDATE_STATE_OID == stateMode){
					memset(sent_buff,0,sizeof(sent_buff));	
					userCreateSta_SOCKETUSBdata(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SSstate(pni,sent_buff,sent_buff_len);	
					}
				}
				else if(CREATE_dEV_STATE_FOR_ALARM_OID == stateMode){
					printf("hm net work SS NODE_INFO_TYPE_AI_SOCKET_USB \r\n");
					hm_get_usbplug_state(pni,cur_uplug_type);
					userCreateSta_SOCKETUSBdata(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SSstate(pni,sent_buff,sent_buff_len);

					memset(sent_buff,0,sizeof(sent_buff));
					userCreateSta_SOCKETUSB_pointdata(sent_buff,&sent_buff_len,pni,devOption);
					report_zb_pointdata(pni,sent_buff,sent_buff_len);
				}
				else if(CREATE_DEV_STATE_FOR_GET_NODE_SETTING_OID == stateMode){
					printf("hm net work SE SOCKET_USB \r\n");
					userCreatePlugSettingData(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SEsetting(pni,sent_buff,sent_buff_len);
				}
				else if(stateMode == CREATE_DEV_STATE_FOR_GET_NODE_STATE_OID){
					printf("hm net work single SOCKET_USB \r\n");
					userCreateSta_SOCKETUSBdata(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SingleSS(pni,sent_buff,sent_buff_len);
				}
				break;
			case NODE_INFO_TYPE_AI_SOCKET_MEASURE:
				if((CREATE_DEV_STATE_FOR_NEW_NODE_JOIN_OID == stateMode)||(CREATE_DEV_STATE_FOR_UPDATE_STATE_OID == stateMode))
				{
					printf("hm net work join SOCKET_MEASURE -----------\r\n");
					if(CREATE_DEV_STATE_FOR_NEW_NODE_JOIN_OID == stateMode){
					userCreateSta_SOCKETMEASUREdata(sent_buff,&sent_buff_len,pni,devOption);
					report_zb_intonetwork(pni,sent_buff,sent_buff_len);}

					memset(sent_buff,0,sizeof(sent_buff));
					userCreateSta_SOCKETMEASURE_pointdata(sent_buff,&sent_buff_len,pni,devOption);
					report_zb_pointdata(pni,sent_buff,sent_buff_len);
					if(CREATE_DEV_STATE_FOR_UPDATE_STATE_OID == stateMode){
					memset(sent_buff,0,sizeof(sent_buff));	
					userCreateSta_SOCKETMEASUREdata(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SSstate(pni,sent_buff,sent_buff_len);
					}
				}
				else if(CREATE_dEV_STATE_FOR_ALARM_OID == stateMode){
					printf("hm net work SS SOCKET_MEASURE \r\n");
					hm_get_measureplug_state(pni,cur_splug_type);
					userCreateSta_SOCKETMEASUREdata(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SSstate(pni,sent_buff,sent_buff_len);

					memset(sent_buff,0,sizeof(sent_buff));
					userCreateSta_SOCKETMEASURE_pointdata(sent_buff,&sent_buff_len,pni,devOption);
					report_zb_pointdata(pni,sent_buff,sent_buff_len);
				}
				else if(CREATE_DEV_STATE_FOR_GET_NODE_SETTING_OID == stateMode){
					printf("hm net work SE SOCKET_MEASURE \r\n");
					userCreateSPlugSettingData(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SEsetting(pni,sent_buff,sent_buff_len);
				}
				else if(stateMode == CREATE_DEV_STATE_FOR_GET_NODE_STATE_OID){
					printf("hm net work single SOCKET_USB \r\n");
					userCreateSta_SOCKETMEASUREdata(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SingleSS(pni,sent_buff,sent_buff_len);
				}
				break;
			case NODE_INFO_TYPE_CUSTOM_RGB_AREA:
				if((CREATE_DEV_STATE_FOR_NEW_NODE_JOIN_OID == stateMode)||(CREATE_DEV_STATE_FOR_UPDATE_STATE_OID == stateMode))
				{
					printf("hm net work join CUSTOM_RGB -----------\r\n");
					if(CREATE_DEV_STATE_FOR_NEW_NODE_JOIN_OID == stateMode){
					userCreateSta_CUTRGBdata(sent_buff,&sent_buff_len,pni,devOption);
					report_zb_intonetwork(pni,sent_buff,sent_buff_len);}

					memset(sent_buff,0,sizeof(sent_buff));
					userCreateSta_CUTRGB_pointdata(sent_buff,&sent_buff_len,pni,devOption);
					report_zb_pointdata(pni,sent_buff,sent_buff_len);
				}
				else if(CREATE_dEV_STATE_FOR_ALARM_OID == stateMode){
					printf("hm net work SS CUSTOM_RGB \r\n");
					hm_get_fouredrivelamp_state(pni,cur_fourlamp_type);
					userCreateSta_CUTRGBdata(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SSstate(pni,sent_buff,sent_buff_len);

					memset(sent_buff,0,sizeof(sent_buff));
					userCreateSta_CUTRGB_pointdata(sent_buff,&sent_buff_len,pni,devOption);
					report_zb_pointdata(pni,sent_buff,sent_buff_len);
				}
				else if(CREATE_DEV_STATE_FOR_GET_NODE_SETTING_OID == stateMode){
					printf("hm net work SE CUSTOM_RGB \r\n");
					userCreateCutRgbSettingData(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SEsetting(pni,sent_buff,sent_buff_len);
				}
				else if(stateMode == CREATE_DEV_STATE_FOR_GET_NODE_STATE_OID){
					printf("hm net work single CUSTOM_RGB \r\n");
					userCreateSta_CUTRGBdata(sent_buff,&sent_buff_len,pni,devOption);
					sent_zigbee_SingleSS(pni,sent_buff,sent_buff_len);
				}
				break;
			default:
				break;
		}
}
// SE
void user_SetSE_parm(HM_DEV_RAM_LIST *pni,unsigned char *payload,unsigned int payload_len)
{
	char recv_buff[256];
	int recv_buff_len;
	uint8_t needSaveFlashFlag = 0,value_enable,value_Henable,value_Tenable;
	uint16_t i,enable_flag,movedata=0x0001,move_index;
	uint8_t *ptr=payload;
	uint16_t epIndex1= 0xff, epIndex2 = 0xff, epIndex3 = 0xff,epIndex4 = 0xff;
	float tempF;
	uint16_t temp_data,devType_value;
	int16_t vFlag;
	uint8_t colorChange = 0,DataOF1,DataOF2,DataOF3,DataONOFF1,DataONOFF2,DataONOFF3,DataONOFF4;
	uint8_t DataLEL1,DataLEL2,DataLEL3,DataLEL4;
	uint16_t DataCWL1,DataCWL2,DataCWL3,DataCWL4;
	uint8_t wdAction = 0xff;
	DEVINFOOPTION devOption;
	ZB_RGB_PARAM cur_rgb_type;
	ZB_UPLUG_PARAM cur_uplug_type;
	ZB_SPLUG_PARAM cur_splug_type;
	ZB_Slight1_PARAM cur_slight1_type;
	ZB_Slight2_PARAM cur_slight2_type;
	ZB_Slight3_PARAM cur_slight3_type;
	ZB_SAlARM_PARAM cur_salarm_type;
	ZB_FOURLAMP_PARAM cur_fourlamp_type;

	memcpy(recv_buff,payload,payload_len);
	printf("\n user_SetSE_parm --payload:\r\n");
		for(i = 0; i < payload_len; i ++)//payloadlen mylen
		{
				Loginfo("%02x ",recv_buff[i]);
		}
	switch(pni->devType)
	{
			case NODE_INFO_TYPE_LIGHT_RGB:
								
				if(payload_len<18)
				{
					Loginfo("Length check fail\n");
					return;
				}
				enable_flag=(ptr[0]<<8)+ptr[1];
				for(i=0;i<8;i++)
				{
					if(enable_flag & movedata)
					{
						switch(i)
						{
							case 0:
								Loginfo("set LIGHT_RGB OF\n");
								temp_data=ptr[2];
								//pni->epList[0].devData.colorStateApp.onOffStatus = temp_data;
								//userFillUartCmdForOnOff(pni->shortAddr,pni->epList[0].ep,temp_data);
								cur_rgb_type.OFState_se=temp_data;
								devOption.RGBSet.onoff = 1;
								break;
							case 1:
								Loginfo("set LIGHT_RGB LE\n");
								temp_data=ptr[3];
								if(temp_data>100)
								{
									Loginfo("set LE data error\n");
									break;
								}
								//pni->epList[0].devData.colorStateApp.level= temp_data;
								//userFillUartCmdForLevel(pni->shortAddr,pni->epList[0].ep,MOVE_TO_LEVEL_WTONOFF,
								//					pni->epList[0].devData.colorStateApp.level,0);
								cur_rgb_type.Brightness_se=temp_data;
								devOption.RGBSet.level = 1;
								break;
							case 2:
								Loginfo("set LIGHT_RGB CR\n");
								temp_data=(ptr[4]<<8)+ptr[5];//(ptr[5]<<8)+ptr[4];
								//colorChange = 1;
								//pni->epList[0].devData.colorStateApp.rgb_r = temp_data;
								cur_rgb_type.Red_se= temp_data;
								devOption.RGBSet.colour_r = 1;
								break;
							case 3:
								Loginfo("set LIGHT_RGB CG\n");
								temp_data=(ptr[6]<<8)+ptr[7];//(ptr[7]<<8)+ptr[6];
								//colorChange = 1;
								//pni->epList[0].devData.colorStateApp.rgb_g = temp_data;
								cur_rgb_type.Gree_se= temp_data;
								devOption.RGBSet.colour_g = 1;

								break;
							case 4:
								Loginfo("set LIGHT_RGB CB\n");
								temp_data=(ptr[8]<<8)+ptr[9];
								//colorChange = 1;
								//pni->epList[0].devData.colorStateApp.rgb_b = temp_data;
								cur_rgb_type.Blue_se= temp_data;
								devOption.RGBSet.colour_b = 1;
								
								break;
							case 5:
								//pni->epList[0].devData.colorStateApp.timer.s_month = ptr[10];
								//pni->epList[0].devData.colorStateApp.timer.s_day = ptr[11];
								//pni->epList[0].devData.colorStateApp.timer.s_hour = ptr[12];
								//pni->epList[0].devData.colorStateApp.timer.s_min = ptr[13];
								needSaveFlashFlag = 1;
								cur_rgb_type.Timer_se.s_month = ptr[10];
								cur_rgb_type.Timer_se.s_day = ptr[11];
								cur_rgb_type.Timer_se.s_hour = ptr[12];
								cur_rgb_type.Timer_se.s_min = ptr[13];
								devOption.RGBSet.ts = 1;
								break;
							case 6:
								//pni->epList[0].devData.colorStateApp.timer.e_month = ptr[14];
								//pni->epList[0].devData.colorStateApp.timer.e_day = ptr[15];
								//pni->epList[0].devData.colorStateApp.timer.e_hour = ptr[16];
								//pni->epList[0].devData.colorStateApp.timer.e_min = ptr[17];
								needSaveFlashFlag = 1;
								cur_rgb_type.Timer_se.e_month = ptr[14];
								cur_rgb_type.Timer_se.e_day = ptr[15];
								cur_rgb_type.Timer_se.e_hour = ptr[16];
								cur_rgb_type.Timer_se.e_min = ptr[17];
								devOption.RGBSet.te = 1;
								break;
							case 7:
								temp_data=ptr[18];
								//if(pni->epList[0].devData.colorStateApp.timer.wk_flag != temp_data)
								{
									//pni->epList[0].devData.colorStateApp.timer.wk_flag = temp_data;
									//needSaveFlashFlag = 1;
								}
								needSaveFlashFlag = 1;
								cur_rgb_type.Timer_se.wk_flag=temp_data;
								devOption.RGBSet.wflag = 1;
								break;						
							
							default:
								break;
								
						}
					}
					movedata<<=1;
				}
				/*if(colorChange == 1)
		{
			printf("control color r:%d,g:%d,b:%d\r\n",pni->epList[0].devData.colorStateApp.rgb_r,pni->epList[0].devData.colorStateApp.rgb_g,pni->epList[0].devData.colorStateApp.rgb_b);
			if((pni->epList[0].devData.colorStateApp.onOffStatus == 1) && (pni->onLine == 1))
			{	
				
				userFillUartCmdForColorControl(pni->shortAddr,pni->epList[0].ep,pni->epList[0].devData.colorStateApp.rgb_r,
											pni->epList[0].devData.colorStateApp.rgb_g,pni->epList[0].devData.colorStateApp.rgb_b);
			}else{
				printf("the rgb was not control color status\r\n");
				return;
			}
		}*/
		hm_set_rgblight_separam(pni,cur_rgb_type,devOption);
		if(needSaveFlashFlag == 1)
		{
			userUpdateDevInfoDataToFlash(pni);
			userUpdateTheDevTimerEvent((char*)pni);
		}
		devOption.modeByte = ALL_DEV_STATE_ACTION;
		userCreatNodeState_data(pni,CREATE_dEV_STATE_FOR_ALARM_OID,devOption);
		//if(devOption.modeByte != 0)
		{
			//writeUserControlDevStateToCloud(USER_CONRTOL_DEV_TYPE_ZB,intOid,devOption,app_id,TEID);
		}
				break;
			case NODE_INFO_TYPE_AI1SW:
			case NODE_INFO_TYPE_AI2SW:
			case NODE_INFO_TYPE_AI3SW:
				if(payload_len<11)
				{
					Loginfo("Length check fail\n");
					return;
				}
				enable_flag=(ptr[0]<<8)+ptr[1];
				for(i = 0; i < pni->epNum; i ++)
				{
					if(pni->epList[i].ep == 1)
					{
						epIndex1 = i;
					}else if(pni->epList[i].ep == 2){
						epIndex2 = i;
					}else if(pni->epList[i].ep == 3){
						epIndex3 = i;
					}
				}
				if(pni->devType == NODE_INFO_TYPE_AI1SW){
					if((epIndex1 == 0xff) || (payload_len != 12))
					{
						printf("the dev info was error\r\n");
						return;
					}
					move_index=4;
				}
				else if(pni->devType == NODE_INFO_TYPE_AI2SW)
				{
					if((epIndex1 == 0xff) || (epIndex2 == 0xff) || (payload_len != 21))
					{
						printf("the dev info was error\r\n");
						return;
					}
					move_index=7;
				}else if(pni->devType == NODE_INFO_TYPE_AI3SW){
					if((epIndex1 == 0xff) || (epIndex2 == 0xff) || (epIndex3 == 0xff) || (payload_len != 30))
					{
						printf("the dev info was error\r\n");
						return;
					}
					move_index=10;
				} 
				for(i=0;i<move_index;i++)//10
				{
					if(enable_flag & movedata)
					{
						switch(i)
						{
							case 0://OF1 OF2 OF3
								//DataOF1,DataOF2,DataOF3;
								DataOF1=ptr[2]&0x01;
								DataOF2=(ptr[2]&0x02)>>1;
								DataOF3=(ptr[2]&0x04)>>2;
								//pni->epList[epIndex1].devData.witchStateApp.onOffStatus = DataOF1;
								//userFillUartCmdForOnOff(pni->shortAddr,pni->epList[epIndex1].ep,DataOF1);
								if(pni->devType == NODE_INFO_TYPE_AI1SW){
								cur_slight1_type.OFState=DataOF1;}
								else if(pni->devType == NODE_INFO_TYPE_AI2SW){
									cur_slight2_type.OF1State=DataOF1;
									cur_slight2_type.OF2State=DataOF2;
									devOption.SWSet.onoff2 = 1;
								}else if(pni->devType == NODE_INFO_TYPE_AI3SW){
									cur_slight3_type.OF1State=DataOF1;
									cur_slight3_type.OF2State=DataOF2;
									cur_slight3_type.OF2State=DataOF3;
									devOption.SWSet.onoff3 = 1;
								}
								devOption.SWSet.onoff1 = 1;
								if(pni->devType == NODE_INFO_TYPE_AI2SW || pni->devType == NODE_INFO_TYPE_AI3SW)
								{
									//pni->epList[epIndex2].devData.witchStateApp.onOffStatus = DataOF2;
									//userFillUartCmdForOnOff(pni->shortAddr,pni->epList[epIndex2].ep,DataOF2);
								}
								if(pni->devType == NODE_INFO_TYPE_AI3SW)
								{
									//pni->epList[epIndex3].devData.witchStateApp.onOffStatus = DataOF3;
									//userFillUartCmdForOnOff(pni->shortAddr,pni->epList[epIndex3].ep,DataOF3);
								}

								break;
							case 1://TS1
								
								if(pni->devType == NODE_INFO_TYPE_AI1SW){
								cur_slight1_type.Timer_se.s_month=ptr[3];
								cur_slight1_type.Timer_se.s_day=ptr[4];
								cur_slight1_type.Timer_se.s_hour=ptr[5];
								cur_slight1_type.Timer_se.s_min=ptr[6];
								}else if(pni->devType == NODE_INFO_TYPE_AI2SW){
								cur_slight2_type.Timer1_se.s_month=ptr[3];
								cur_slight2_type.Timer1_se.s_day=ptr[4];
								cur_slight2_type.Timer1_se.s_hour=ptr[5];
								cur_slight2_type.Timer1_se.s_min=ptr[6];
								}else if(pni->devType == NODE_INFO_TYPE_AI3SW){
								cur_slight3_type.Timer1_se.s_month=ptr[3];
								cur_slight3_type.Timer1_se.s_day=ptr[4];
								cur_slight3_type.Timer1_se.s_hour=ptr[5];
								cur_slight3_type.Timer1_se.s_min=ptr[6];
								}
								needSaveFlashFlag = 1;
								devOption.SWSet.ts1 = 1;
								break;
							case 2://TE1
							
								if(pni->devType == NODE_INFO_TYPE_AI1SW){
								cur_slight1_type.Timer_se.e_month=ptr[7];
								cur_slight1_type.Timer_se.e_day=ptr[8];
								cur_slight1_type.Timer_se.e_hour=ptr[9];
								cur_slight1_type.Timer_se.e_min=ptr[10];
								}else if(pni->devType == NODE_INFO_TYPE_AI2SW){
								cur_slight2_type.Timer1_se.e_month=ptr[7];
								cur_slight2_type.Timer1_se.e_day=ptr[8];
								cur_slight2_type.Timer1_se.e_hour=ptr[9];
								cur_slight2_type.Timer1_se.e_min=ptr[10];
								}else if(pni->devType == NODE_INFO_TYPE_AI3SW){
								cur_slight3_type.Timer1_se.e_month=ptr[7];
								cur_slight3_type.Timer1_se.e_day=ptr[8];
								cur_slight3_type.Timer1_se.e_hour=ptr[9];
								cur_slight3_type.Timer1_se.e_min=ptr[10];
								}
								needSaveFlashFlag = 1;
								devOption.SWSet.te1 = 1;
								break;
							case 3://WF1
								
								if(pni->devType == NODE_INFO_TYPE_AI1SW){
								cur_slight1_type.Timer_se.wk_flag=ptr[11];
								
								}else if(pni->devType == NODE_INFO_TYPE_AI2SW){
								cur_slight2_type.Timer1_se.wk_flag=ptr[11];
								
								}else if(pni->devType == NODE_INFO_TYPE_AI3SW){
								cur_slight3_type.Timer1_se.wk_flag=ptr[11];
								
								}
								needSaveFlashFlag = 1;
								devOption.SWSet.wflag1 = 1;
								break;
							case 4://TS2
								
								if(pni->devType == NODE_INFO_TYPE_AI2SW){
								cur_slight2_type.Timer2_se.s_month=ptr[12];
								cur_slight2_type.Timer2_se.s_day=ptr[13];
								cur_slight2_type.Timer2_se.s_hour=ptr[14];
								cur_slight2_type.Timer2_se.s_min=ptr[15];
								}else if(pni->devType == NODE_INFO_TYPE_AI3SW){
								cur_slight3_type.Timer2_se.s_month=ptr[12];
								cur_slight3_type.Timer2_se.s_day=ptr[13];
								cur_slight3_type.Timer2_se.s_hour=ptr[14];
								cur_slight3_type.Timer2_se.s_min=ptr[15];
								}
								needSaveFlashFlag = 1;
								devOption.SWSet.ts2 = 1;
								break;
							case 5://TE2
								
								if(pni->devType == NODE_INFO_TYPE_AI2SW){
								cur_slight2_type.Timer2_se.e_month=ptr[16];
								cur_slight2_type.Timer2_se.e_day=ptr[17];
								cur_slight2_type.Timer2_se.e_hour=ptr[18];
								cur_slight2_type.Timer2_se.e_min=ptr[19];
								}else if(pni->devType == NODE_INFO_TYPE_AI3SW){
								cur_slight3_type.Timer2_se.e_month=ptr[16];
								cur_slight3_type.Timer2_se.e_day=ptr[17];
								cur_slight3_type.Timer2_se.e_hour=ptr[18];
								cur_slight3_type.Timer2_se.e_min=ptr[19];
								}
								needSaveFlashFlag = 1;
								devOption.SWSet.te2 = 1;
								break;
							case 6://WF2
								
								if(pni->devType == NODE_INFO_TYPE_AI2SW){
								cur_slight2_type.Timer2_se.wk_flag=ptr[20];
								
								}else if(pni->devType == NODE_INFO_TYPE_AI3SW){
								cur_slight3_type.Timer2_se.wk_flag=ptr[20];
								}
								needSaveFlashFlag = 1;
								devOption.SWSet.wflag2 = 1;
								break;
							case 7://TS3
								cur_slight3_type.Timer3_se.s_month=ptr[21];
								cur_slight3_type.Timer3_se.s_day=ptr[22];
								cur_slight3_type.Timer3_se.s_hour=ptr[23];
								cur_slight3_type.Timer3_se.s_min=ptr[24];
								needSaveFlashFlag = 1;
								devOption.SWSet.ts3 = 1;
								break;
							case 8://TE3
								
								cur_slight3_type.Timer3_se.e_month=ptr[25];
								cur_slight3_type.Timer3_se.e_day=ptr[26];
								cur_slight3_type.Timer3_se.e_hour=ptr[27];
								cur_slight3_type.Timer3_se.e_min=ptr[28];
								needSaveFlashFlag = 1;
								devOption.SWSet.te3 = 1;
								break;
							case 9://WF3
								//if(pni->epList[epIndex3].devData.witchStateApp.timer.wk_flag != ptr[29])
								{
									//pni->epList[epIndex3].devData.witchStateApp.timer.wk_flag = ptr[29];
									//needSaveFlashFlag = 1;
								}
								needSaveFlashFlag = 1;
								cur_slight3_type.Timer3_se.wk_flag=ptr[29];
								devOption.SWSet.wflag3 = 1;
								break;
								default:
								break;
								
						}
					}
					movedata<<=1;
				}
				if(pni->devType == NODE_INFO_TYPE_AI1SW){
					hm_set_ai1sw_separam(pni,cur_slight1_type,devOption);
				}else if(pni->devType == NODE_INFO_TYPE_AI2SW){
					hm_set_ai2sw_separam(pni,cur_slight2_type,devOption);		
				}else if(pni->devType == NODE_INFO_TYPE_AI3SW){
					hm_set_ai3sw_separam(pni,cur_slight3_type,devOption);				
				}
				if(needSaveFlashFlag == 1)
				{
					//userUpdateDevInfoDataToFlash(pni);
					//userUpdateTheDevTimerEvent((char*)pni);
				}
				
				//if(devOption.modeByte != 0)
				{
					//writeUserControlDevStateToCloud(USER_CONRTOL_DEV_TYPE_ZB,intOid,devOption,app_id,TEID);
				}
				devOption.modeByte = ALL_DEV_STATE_ACTION;
				userCreatNodeState_data(pni,CREATE_dEV_STATE_FOR_ALARM_OID,devOption);
				break;
			
			case NODE_INFO_TYPE_ZB_IR_TRANSPONDER:
				break;
			case NODE_INFO_TYPE_RELAY_CONTROL://relay
			
				break;
			case NODE_INFO_TYPE_MAGNET_DOOR:
			case NODE_INFO_TYPE_PIR_SENSOR:	
				//have se as
				printf("\n user_SetSE_parm --payload:\r\n");
				
				for(i = 0; i < payload_len; i ++)//payloadlen mylen
				{
					Loginfo("%02x ",recv_buff[i]);
				}
				if(payload_len<3){
					Loginfo("Length check fail\n");
					return;
					}
				printf("\n user_SetSE_parm payloadlen>=3:\r\n");
				
				pni->epList[0].devData.zoneStateApp.armMode = recv_buff[2]-0x30;
				needSaveFlashFlag = 1;
				//devOption.IASSet.armMode = 1;
				if(needSaveFlashFlag == 1)
				{
					userUpdateDevInfoDataToFlash(pni);
				}
				//if(devOption.modeByte != 0)
				{
					//writeUserControlDevStateToCloud(USER_CONRTOL_DEV_TYPE_ZB,intOid,devOption,app_id,TEID);
				}
				if(recv_buff[2]-0x30)
				{
				//as ==1
				}
				else{
					//as ==0
				}
				
				break;
			case NODE_INFO_TYPE_WATER_SENSOR:
			case NODE_INFO_TYPE_SMOKE_SENSOR:
			case NODE_INFO_TYPE_GAS_SENSOR:
			case NODE_INFO_TYPE_CO_SENSOR:
			case NODE_INFO_TYPE_SOS:
			case NODE_INFO_TYPE_REMOTE_CONTROL:
				//NO SE			
				break;
			case NODE_INFO_TYPE_VIBRATION:
	
				break;
			case NODE_INFO_TYPE_TEMP_HUM_SENSOR:
				// have
				if(payload_len<10)
					return;
				enable_flag=(ptr[0]<<8)+ptr[1];
				for(i = 0; i < pni->epNum; i ++)
				{
					if(pni->epList[i].ep == 1)
					{
						epIndex1 = i;
					}else if(pni->epList[i].ep == 2){
						epIndex2 = i;
					}
				}
				if((epIndex1 == 0xff) || (epIndex2 == 0xff))
				{
					printf("the dev info was error\r\n");
					//*rc = APP_CMD_RC_ERROR_PARAMETER;
					return;
				}
				
				for(i=0;i<8;i++)
				{
					if(enable_flag & movedata)
					{
						switch(i)
						{
							case 0:
								Loginfo("set TEMP_HUM_SENSOR HU\n");
								temp_data=(ptr[3]<<8)+ptr[2];
								if(pni->epList[epIndex2].devData.humiStateApp.upHumi != (uint16_t)(temp_data))//*100
								{
									pni->epList[epIndex2].devData.humiStateApp.upHumi = (uint16_t)(temp_data);
									needSaveFlashFlag = 1;
								}
								//devOption.THPSet.humiUp = 1;

								break;
							case 1:
								Loginfo("set TEMP_HUM_SENSOR HL\n");
								temp_data=(ptr[5]<<8)+ptr[4];
								if(pni->epList[epIndex2].devData.humiStateApp.lowHumi != (uint16_t)(temp_data))
								{
									pni->epList[epIndex2].devData.humiStateApp.lowHumi = (uint16_t)(temp_data);
									needSaveFlashFlag = 1;
								}
								//devOption.THPSet.humiLow = 1;
								break;
							case 2:
								Loginfo("set TEMP_HUM_SENSOR TU\n");
								temp_data=(ptr[7]<<8)+ptr[6];
								if((temp_data&0x80) ==0x80)
								{
									vFlag = -1;	
									temp_data = temp_data & 0x7f;
								}else {
									vFlag = 1;
									temp_data = temp_data & 0x7f;
								}

								if(pni->epList[epIndex1].devData.tempStateApp.upTemp != (int)(vFlag * temp_data))
								{
									pni->epList[epIndex1].devData.tempStateApp.upTemp = (int)(vFlag * temp_data);//100
									needSaveFlashFlag = 1;
								}
								//devOption.THPSet.tempUp = 1;
								break;
							case 3:
								Loginfo("set TEMP_HUM_SENSOR TL\n");
								temp_data=(ptr[9]<<8)+ptr[8];
								if((temp_data&0x80) ==0x80)
								{
									vFlag = -1;	
									temp_data = temp_data & 0x7f;
								}else {
									vFlag = 1;
									temp_data = temp_data & 0x7f;
								}
								if(pni->epList[epIndex1].devData.tempStateApp.lowTemp != (int)(vFlag * temp_data))
								{
									pni->epList[epIndex1].devData.tempStateApp.lowTemp = (int)(vFlag * temp_data);
									needSaveFlashFlag = 1;
								}
								//devOption.THPSet.tempLow = 1;

								break;
							case 4:
								Loginfo("set TEMP_HUM_SENSOR ZA\n");
								value_Henable=(ptr[10] & 0x0f);
								value_Tenable=((ptr[10]>>4) & 0x0f);
								if(pni->epList[epIndex2].devData.humiStateApp.checkEnable != value_Henable)
								{
									pni->epList[epIndex2].devData.humiStateApp.checkEnable = value_Henable;
									needSaveFlashFlag = 1;
								}
								//devOption.THPSet.h_checkEn = 1;
								if(pni->epList[epIndex1].devData.tempStateApp.checkEnable != value_Tenable)
								{
									pni->epList[epIndex1].devData.tempStateApp.checkEnable = value_Tenable;
									needSaveFlashFlag = 1;
								}
								//devOption.THPSet.t_checkEn = 1;
								
								break;
							
							default:
								break;
								
						}
					}
					movedata<<=1;
				}
				if(needSaveFlashFlag == 1)
				{
					userUpdateDevInfoDataToFlash(pni);
				}
				devOption.modeByte = ALL_DEV_STATE_ACTION;
				userCreatNodeState_data(pni,CREATE_dEV_STATE_FOR_ALARM_OID,devOption);
				break;
			case NODE_INFO_TYPE_WARNING_DEV:
				//have
				if(payload_len<5){
					Loginfo("Length check fail\n");
					return;
				}
				enable_flag=(ptr[0]<<8)+ptr[1];
				for(i=0;i<8;i++)
				{
					if(enable_flag & movedata)
					{
						switch(i)
						{
							case 0://RO
								//wdAction=ptr[2];
								cur_salarm_type.OP_se=ptr[2];
								break;
							case 1:
								temp_data=(ptr[3]<<8)+ptr[4];
								//printf("wd set duration value:%d\r\n",temp_data);
								//if(pni->epList[0].devData.wdStateApp.duration != temp_data)
								{
									//pni->epList[0].devData.wdStateApp.duration = temp_data;
									//needSaveFlashFlag = 1;
								}
								cur_salarm_type.AlarmTime_se=temp_data;
								needSaveFlashFlag = 1;
								devOption.WDSet.duration = 1;
								break;
							default:
								break;
						}
					}
					movedata<<=1;
				}
				hm_set_sensoralarm_separam(pni,cur_salarm_type);
					
				
				break;
			case NODE_INFO_TYPE_ILLUMINATION:
				break;
			case NODE_INFO_TYPE_AIR_QUALITY:
				break;
			case NODE_INFO_TYPE_THERMOSTAT:

				break;
			case NODE_INFO_TYPE_AI_SOCKET_USB:
				//普通插座
				enable_flag=(ptr[0]<<8)+ptr[1];
				if(payload_len<21){
					Loginfo("Length check fail\n");
					return;
				}
				for(i = 0; i < pni->epNum; i ++)
				{
					if(pni->epList[i].ep == 1)
					{
						epIndex1 = i;
					}else if(pni->epList[i].ep == 2){
						epIndex2 = i;
					}
				}
			
				if((epIndex1 == 0xff) || (epIndex2 == 0xff))
				{
					printf("the dev info was error\r\n");
					//*rc = APP_CMD_RC_ERROR_PARAMETER;
					return;
				}
				for(i=0;i<8;i++)
				{
					if(enable_flag & movedata)
					{
						switch(i)
						{
							case 0://RO
								temp_data=ptr[2];
								//pni->epList[epIndex1].devData.onoffOutStateApp.onOffStatus = temp_data;
								//userFillUartCmdForOnOff(pni->shortAddr,pni->epList[epIndex1].ep,temp_data);
								cur_uplug_type.Realy_se=temp_data;
								devOption.UPlugSet.relayOnoff = 1;
							
								break;
							case 1://TSr
								//pni->epList[epIndex1].devData.onoffOutStateApp.timer.s_month = ptr[3];
								//pni->epList[epIndex1].devData.onoffOutStateApp.timer.s_day = ptr[4];
								//pni->epList[epIndex1].devData.onoffOutStateApp.timer.s_hour = ptr[5];
								//pni->epList[epIndex1].devData.onoffOutStateApp.timer.s_min = ptr[6];
								cur_uplug_type.RTimer_se.s_month=ptr[3];
								cur_uplug_type.RTimer_se.s_day=ptr[4];
								cur_uplug_type.RTimer_se.s_hour=ptr[5];
								cur_uplug_type.RTimer_se.s_min=ptr[6];
								needSaveFlashFlag = 1;
								devOption.UPlugSet.tsr = 1;
								break;
							case 2://TEr
								//pni->epList[epIndex1].devData.onoffOutStateApp.timer.e_month = ptr[7];
								//pni->epList[epIndex1].devData.onoffOutStateApp.timer.e_day = ptr[8];
								//pni->epList[epIndex1].devData.onoffOutStateApp.timer.e_hour = ptr[9];
								//pni->epList[epIndex1].devData.onoffOutStateApp.timer.e_min = ptr[10];
								needSaveFlashFlag = 1;
								cur_uplug_type.RTimer_se.e_month=ptr[7];
								cur_uplug_type.RTimer_se.e_day=ptr[8];
								cur_uplug_type.RTimer_se.e_hour=ptr[9];
								cur_uplug_type.RTimer_se.e_min=ptr[10];
								devOption.UPlugSet.ter = 1;
								break;
							case 3://WFr
								temp_data=ptr[11];
								//if(pni->epList[epIndex1].devData.onoffOutStateApp.timer.wk_flag != temp_data)
								//{
									//pni->epList[epIndex1].devData.onoffOutStateApp.timer.wk_flag = temp_data;
									//needSaveFlashFlag = 1;
								//}
								cur_uplug_type.RTimer_se.wk_flag=temp_data;
								devOption.UPlugSet.wflag_r = 1;
								needSaveFlashFlag = 1;
								break;
							case 4://UO
								temp_data=ptr[12];
								//pni->epList[epIndex2].devData.onoffOutStateApp.onOffStatus = temp_data;
								//userFillUartCmdForOnOff(pni->shortAddr,pni->epList[epIndex2].ep,temp_data);
								cur_uplug_type.UsbState=temp_data;
								devOption.UPlugSet.usbOnoff = 1;
								break;
							case 5://TSu
								//pni->epList[epIndex2].devData.onoffOutStateApp.timer.s_month = ptr[13];
								//pni->epList[epIndex2].devData.onoffOutStateApp.timer.s_day = ptr[14];
								//pni->epList[epIndex2].devData.onoffOutStateApp.timer.s_hour = ptr[15];
								//pni->epList[epIndex2].devData.onoffOutStateApp.timer.s_min = ptr[16];
								needSaveFlashFlag = 1;
								cur_uplug_type.UTimer_se.s_month=ptr[13];
								cur_uplug_type.UTimer_se.s_day=ptr[14];
								cur_uplug_type.UTimer_se.s_hour=ptr[15];
								cur_uplug_type.UTimer_se.s_min=ptr[16];
								devOption.UPlugSet.tsu = 1;
								break;
							case 6://TEu
								//pni->epList[epIndex2].devData.onoffOutStateApp.timer.e_month = ptr[17];
								//pni->epList[epIndex2].devData.onoffOutStateApp.timer.e_day = ptr[18];
								//pni->epList[epIndex2].devData.onoffOutStateApp.timer.e_hour = ptr[19];
								//pni->epList[epIndex2].devData.onoffOutStateApp.timer.e_min = ptr[20];
								needSaveFlashFlag = 1;
								cur_uplug_type.UTimer_se.e_month=ptr[17];
								cur_uplug_type.UTimer_se.e_day=ptr[18];
								cur_uplug_type.UTimer_se.e_hour=ptr[19];
								cur_uplug_type.UTimer_se.e_min=ptr[20];
								devOption.UPlugSet.teu = 1;
								break;
							case 7://WFu
								temp_data=ptr[21];
								//if(pni->epList[epIndex2].devData.onoffOutStateApp.timer.wk_flag != temp_data)
								{
									//pni->epList[epIndex2].devData.onoffOutStateApp.timer.wk_flag = temp_data;
									//needSaveFlashFlag = 1;
								}
								needSaveFlashFlag = 1;
								cur_uplug_type.UTimer_se.wk_flag=temp_data;
								devOption.UPlugSet.wflag_u = 1;
								break;
								default:
								break;
								
						}
					}
					movedata<<=1;
				}
				hm_set_usbplug_separam(pni,cur_uplug_type,devOption);
				if(needSaveFlashFlag == 1)
				{
					//userUpdateDevInfoDataToFlash(pni);
					//userUpdateTheDevTimerEvent((char*)pni);
				}
				//if(devOption.modeByte != 0)
				{
					//writeUserControlDevStateToCloud(USER_CONRTOL_DEV_TYPE_ZB,intOid,devOption,app_id,TEID);
					devOption.modeByte = ALL_DEV_STATE_ACTION;
					userCreatNodeState_data(pni,CREATE_dEV_STATE_FOR_ALARM_OID,devOption);
				}
				break;
			case NODE_INFO_TYPE_AI_SOCKET_MEASURE:
				//计量插座
				if(payload_len<11){
					Loginfo("Length check fail\n");
					return;
				}
				enable_flag=(ptr[0]<<8)+ptr[1];
				for(i=0;i<8;i++)
				{
					if(enable_flag & movedata)
					{
						switch(i)
						{
							case 0://RO
								//pni->epList[0].devData.splugStateApp.onOffStatus = ptr[2];
								//userFillUartCmdForOnOff(pni->shortAddr,pni->epList[0].ep,ptr[2]);
								printf("----zb----splugStateApp.onOffStatus=%d\n",ptr[2]);
								cur_splug_type.Realy_se=ptr[2];
								devOption.SPlugSet.relayOnoff = 1;
								break;
							case 1://TS
								//pni->epList[0].devData.splugStateApp.timer.s_month = ptr[3];
								//pni->epList[0].devData.splugStateApp.timer.s_day = ptr[4];
								//pni->epList[0].devData.splugStateApp.timer.s_hour = ptr[5];
								//pni->epList[0].devData.splugStateApp.timer.s_min = ptr[6];
								needSaveFlashFlag = 1;
								cur_splug_type.Timer_se.s_month = ptr[3];
								cur_splug_type.Timer_se.s_day = ptr[4];
								cur_splug_type.Timer_se.s_hour = ptr[5];
								cur_splug_type.Timer_se.s_min = ptr[6];
								devOption.SPlugSet.ts = 1;
								break;
							case 2://TE
								//pni->epList[0].devData.splugStateApp.timer.e_month = ptr[7];
								//pni->epList[0].devData.splugStateApp.timer.e_day = ptr[8];
								//pni->epList[0].devData.splugStateApp.timer.e_hour = ptr[9];
								//pni->epList[0].devData.splugStateApp.timer.e_min = ptr[10];
								cur_splug_type.Timer_se.e_month = ptr[7];
								cur_splug_type.Timer_se.e_day = ptr[8];
								cur_splug_type.Timer_se.e_hour = ptr[9];
								cur_splug_type.Timer_se.e_min = ptr[10];
								needSaveFlashFlag = 1;
								devOption.SPlugSet.te = 1;
								break;
							case 3://WF
								//if(pni->epList[0].devData.splugStateApp.timer.wk_flag != ptr[11])
								//{
									//pni->epList[0].devData.splugStateApp.timer.wk_flag = ptr[11];
									//needSaveFlashFlag = 1;
								//}
								cur_splug_type.Timer_se.wk_flag= ptr[11];
								needSaveFlashFlag = 1;
								devOption.SPlugSet.wflag = 1;
								break;
							default:
								break;
								
						}
					}
					movedata<<=1;
				}
				//cur_splug_type
				hm_set_measureplug_separam(pni,cur_splug_type,devOption);
				if(needSaveFlashFlag == 1)
				{
					printf("the dev data was change,start save to flash\r\n");
					//userUpdateDevInfoDataToFlash(pni);
					//userUpdateTheDevTimerEvent((char*)pni);
				}
				
				//if(devOption.modeByte != 0)  //控制的记录上报
				{
					//writeUserControlDevStateToCloud(USER_CONRTOL_DEV_TYPE_ZB,intOid,devOption,app_id,TEID);
					devOption.modeByte = ALL_DEV_STATE_ACTION;
					userCreatNodeState_data(pni,CREATE_dEV_STATE_FOR_ALARM_OID,devOption);//CREATE_dEV_STATE_FOR_ALARM_OID  CREATE_DEV_STATE_FOR_NEW_NODE_JOIN_OID
				}	
				break;
			case NODE_INFO_TYPE_CUSTOM_RGB_AREA:
				if(payload_len<11){
					Loginfo("Length check fail\n");
					return;
				}
				enable_flag=(ptr[0]<<8)+ptr[1];
				for(i = 0; i < pni->epNum; i ++)
				{
					if(pni->epList[i].ep == 1)
					{
						epIndex1 = i;
					}else if(pni->epList[i].ep == 2){
						epIndex2 = i;
					}else if(pni->epList[i].ep == 3){
						epIndex3 = i;
					}else if(pni->epList[i].ep == 4){
						epIndex4 = i;
					}
				}
		
				for(i=0;i<14;i++)
				{
					if(enable_flag & movedata)
					{
						switch(i)
						{
							case 0://OFL
								//DataONOFF1,DataONOFF2,DataONOFF3,DataONOFF4;
								
								DataONOFF1=ptr[2]&0x01;
								DataONOFF2=(ptr[2]&0x02)>>1;
								DataONOFF3=(ptr[2]&0x04)>>2;
								DataONOFF4=(ptr[2]&0x08)>>3;
								cur_fourlamp_type.OF1_se=DataONOFF1;
								devOption.CUTRGBSet.onoffList1 = 1;
			
								cur_fourlamp_type.OF2_se=DataONOFF2;
								devOption.CUTRGBSet.onoffList2 = 1;

								cur_fourlamp_type.OF3_se=DataONOFF3;
								devOption.CUTRGBSet.onoffList3 = 1;

								cur_fourlamp_type.OF4_se=DataONOFF4;
								devOption.CUTRGBSet.onoffList4 = 1;
								
								
								break;
							case 1://LEL
								//DataLEL1,DataLEL2,DataLEL3,DataLEL4;
								DataLEL1=ptr[3];
								DataLEL2=ptr[4];
								DataLEL3=ptr[5];
								DataLEL4=ptr[6];
								cur_fourlamp_type.Brightness1_se=DataLEL1;
								devOption.CUTRGBSet.brightnessList1 = 1;

								cur_fourlamp_type.Brightness2_se=DataLEL2;
								devOption.CUTRGBSet.brightnessList2 = 1;

								cur_fourlamp_type.Brightness3_se=DataLEL3;
								devOption.CUTRGBSet.brightnessList3 = 1;

								cur_fourlamp_type.Brightness4_se=DataLEL4;
								devOption.CUTRGBSet.brightnessList4 = 1;
								
								
								break;
							case 2://CWL
								//DataCWL1,DataCWL2,DataCWL3,DataCWL4;
								DataCWL1=ptr[7]&0x0f;
								DataCWL2=(ptr[7]>>4&0x0f);
								DataCWL3=ptr[8]&0x0f;
								DataCWL4=(ptr[8]>>4&0x0f);

								cur_fourlamp_type.CWL1_se=DataCWL1;
								devOption.CUTRGBSet.colorTempList1 = 1;
								cur_fourlamp_type.CWL2_se=DataCWL2;
								devOption.CUTRGBSet.colorTempList2 = 1;
								cur_fourlamp_type.CWL3_se=DataCWL3;
								devOption.CUTRGBSet.colorTempList3 = 1;
								cur_fourlamp_type.CWL4_se=DataCWL4;
								devOption.CUTRGBSet.colorTempList4 = 1;
								
								
								break;
							case 3://TS1
								cur_fourlamp_type.Timer1_se.s_month= ptr[9];
								cur_fourlamp_type.Timer1_se.s_day= ptr[10];
								cur_fourlamp_type.Timer1_se.s_hour= ptr[11];
								cur_fourlamp_type.Timer1_se.s_min= ptr[12];
							
								needSaveFlashFlag = 1;
								devOption.CUTRGBSet.ts1 = 1;
								break;
							case 4://TE1
								cur_fourlamp_type.Timer1_se.e_month= ptr[13];
								cur_fourlamp_type.Timer1_se.e_day= ptr[14];
								cur_fourlamp_type.Timer1_se.e_hour= ptr[15];
								cur_fourlamp_type.Timer1_se.e_min= ptr[16];
								
								needSaveFlashFlag = 1;
								devOption.CUTRGBSet.te1 = 1;
								break;
							case 5://WF1
								cur_fourlamp_type.Timer1_se.wk_flag= ptr[17];
								
									needSaveFlashFlag = 1;
								
								devOption.CUTRGBSet.wflag1 = 1;
								break;
							case 6://TS2
								
								cur_fourlamp_type.Timer2_se.s_month= ptr[18];
								cur_fourlamp_type.Timer2_se.s_day= ptr[19];
								cur_fourlamp_type.Timer2_se.s_hour= ptr[20];
								cur_fourlamp_type.Timer2_se.s_min= ptr[21];
								needSaveFlashFlag = 1;
								devOption.CUTRGBSet.ts2 = 1;
								break;
							case 7://TE2
								
								cur_fourlamp_type.Timer2_se.e_month= ptr[22];
								cur_fourlamp_type.Timer2_se.e_day= ptr[23];
								cur_fourlamp_type.Timer2_se.e_hour= ptr[24];
								cur_fourlamp_type.Timer2_se.e_min= ptr[25];
								needSaveFlashFlag = 1;
								devOption.CUTRGBSet.te2 = 1;
								break;
							case 8://WF2
								
								cur_fourlamp_type.Timer2_se.wk_flag= ptr[26];
								needSaveFlashFlag = 1;
								devOption.CUTRGBSet.wflag2 = 1;
								break;
							case 9://TS3
								
								cur_fourlamp_type.Timer3_se.s_month= ptr[27];
								cur_fourlamp_type.Timer3_se.s_day= ptr[28];
								cur_fourlamp_type.Timer3_se.s_hour= ptr[29];
								cur_fourlamp_type.Timer3_se.s_min= ptr[30];
								needSaveFlashFlag = 1;
								devOption.CUTRGBSet.ts3 = 1;
								break;
							case 10://TE3
								
								cur_fourlamp_type.Timer3_se.e_month= ptr[31];
								cur_fourlamp_type.Timer3_se.e_day= ptr[32];
								cur_fourlamp_type.Timer3_se.e_hour= ptr[33];
								cur_fourlamp_type.Timer3_se.e_min= ptr[34];
								needSaveFlashFlag = 1;
								devOption.CUTRGBSet.te3 = 1;
								break;
							case 11://WF3
								
								cur_fourlamp_type.Timer3_se.wk_flag= ptr[35];
								needSaveFlashFlag = 1;
								devOption.CUTRGBSet.wflag3 = 1;
								break;
							case 12://TS4
								
								cur_fourlamp_type.Timer4_se.s_month= ptr[36];
								cur_fourlamp_type.Timer4_se.s_day= ptr[37];
								cur_fourlamp_type.Timer4_se.s_hour= ptr[38];
								cur_fourlamp_type.Timer4_se.s_min= ptr[39];
								needSaveFlashFlag = 1;
								devOption.CUTRGBSet.ts4 = 1;
								break;
							case 13://TE4
								
								cur_fourlamp_type.Timer4_se.e_month= ptr[40];
								cur_fourlamp_type.Timer4_se.e_day= ptr[41];
								cur_fourlamp_type.Timer4_se.e_hour= ptr[42];
								cur_fourlamp_type.Timer4_se.e_min= ptr[43];
								needSaveFlashFlag = 1;
								devOption.CUTRGBSet.te4 = 1;
								break;
							case 14://WF4
								
								cur_fourlamp_type.Timer4_se.wk_flag= ptr[44];
								needSaveFlashFlag = 1;
								devOption.CUTRGBSet.wflag3 = 1;
								break; 
							default:
								break;
								
						}
					}
					movedata<<=1;
				}
				
				hm_set_fouredrivelamp_separam(pni,cur_fourlamp_type,devOption);
				if(needSaveFlashFlag == 1)
				{
					//userUpdateDevInfoDataToFlash(pni);
					//userUpdateTheDevTimerEvent((char*)pni);
				}	
				//if(devOption.modeByte != 0)
				{
					//writeUserControlDevStateToCloud(USER_CONRTOL_DEV_TYPE_ZB,intOid,devOption,app_id,TEID);
					devOption.modeByte = ALL_DEV_STATE_ACTION;
					userCreatNodeState_data(pni,CREATE_dEV_STATE_FOR_ALARM_OID,devOption);
				}
				
				break;
			default:
				break;
		}
		

}

void user_SetSE_parm_old(HM_DEV_RAM_LIST *pni,unsigned char *payload,unsigned int payload_len)
{
	char recv_buff[256];
	int recv_buff_len;
	uint8_t needSaveFlashFlag = 0,value_enable,value_Henable,value_Tenable;
	uint16_t i,enable_flag,movedata=0x0001,move_index;
	uint8_t *ptr=payload;
	uint16_t epIndex1= 0xff, epIndex2 = 0xff, epIndex3 = 0xff,epIndex4 = 0xff;
	float tempF;
	uint16_t temp_data,devType_value;
	int16_t vFlag;
	uint8_t colorChange = 0,DataOF1,DataOF2,DataOF3,DataONOFF1,DataONOFF2,DataONOFF3,DataONOFF4;
	uint8_t DataLEL1,DataLEL2,DataLEL3,DataLEL4;
	uint16_t DataCWL1,DataCWL2,DataCWL3,DataCWL4;
	uint8_t wdAction = 0xff;
	DEVINFOOPTION devOption;

	memcpy(recv_buff,payload,payload_len);
	printf("\n user_SetSE_parm --payload:\r\n");
				for(i = 0; i < payload_len; i ++)//payloadlen mylen
				{
					Loginfo("%02x ",recv_buff[i]);
				}
	switch(pni->devType)
	{
			case NODE_INFO_TYPE_LIGHT_RGB:
				//pni_temp->epList[0].devData.colorStateApp.onOffStatus = objs->valueint;
				//userFillUartCmdForOnOff(pni_temp->shortAddr,pni_temp->epList[0].ep,objs->valueint);
				//devOption.RGBSet.onoff = 1;				
				if(payload_len<18)
				{
					Loginfo("Length check fail\n");
					return;
				}
				enable_flag=(ptr[0]<<8)+ptr[1];
				for(i=0;i<8;i++)
				{
					if(enable_flag & movedata)
					{
						switch(i)
						{
							case 0:
								Loginfo("set LIGHT_RGB OF\n");
								temp_data=ptr[2];
								pni->epList[0].devData.colorStateApp.onOffStatus = temp_data;
								userFillUartCmdForOnOff(pni->shortAddr,pni->epList[0].ep,temp_data);
								//devOption.RGBSet.onoff = 1;

								break;
							case 1:
								Loginfo("set LIGHT_RGB LE\n");
								temp_data=ptr[3];
								if(temp_data>100)
								{
									Loginfo("set LE data error\n");
									break;
								}
								pni->epList[0].devData.colorStateApp.level= temp_data;
								userFillUartCmdForLevel(pni->shortAddr,pni->epList[0].ep,MOVE_TO_LEVEL_WTONOFF,
													pni->epList[0].devData.colorStateApp.level,0);
								//devOption.RGBSet.level = 1;
								break;
							case 2:
								Loginfo("set LIGHT_RGB CR\n");
								temp_data=(ptr[4]<<8)+ptr[5];//(ptr[5]<<8)+ptr[4];
								colorChange = 1;
								pni->epList[0].devData.colorStateApp.rgb_r = temp_data;
								//devOption.RGBSet.colour_r = 1;
								break;
							case 3:
								Loginfo("set LIGHT_RGB CG\n");
								temp_data=(ptr[6]<<8)+ptr[7];//(ptr[7]<<8)+ptr[6];
								colorChange = 1;
								pni->epList[0].devData.colorStateApp.rgb_g = temp_data;
								//devOption.RGBSet.colour_g = 1;

								break;
							case 4:
								Loginfo("set LIGHT_RGB CB\n");
								temp_data=(ptr[8]<<8)+ptr[9];
								colorChange = 1;
								pni->epList[0].devData.colorStateApp.rgb_b = temp_data;
								//devOption.RGBSet.colour_b = 1;
								
								break;
							case 5:
								pni->epList[0].devData.colorStateApp.timer.s_month = ptr[10];
								pni->epList[0].devData.colorStateApp.timer.s_day = ptr[11];
								pni->epList[0].devData.colorStateApp.timer.s_hour = ptr[12];
								pni->epList[0].devData.colorStateApp.timer.s_min = ptr[13];
								needSaveFlashFlag = 1;
								//devOption.RGBSet.ts = 1;
								break;
							case 6:
								pni->epList[0].devData.colorStateApp.timer.e_month = ptr[14];
								pni->epList[0].devData.colorStateApp.timer.e_day = ptr[15];
								pni->epList[0].devData.colorStateApp.timer.e_hour = ptr[16];
								pni->epList[0].devData.colorStateApp.timer.e_min = ptr[17];
								needSaveFlashFlag = 1;
								//devOption.RGBSet.te = 1;
								break;
							case 7:
								temp_data=ptr[18];
								if(pni->epList[0].devData.colorStateApp.timer.wk_flag != temp_data)
								{
									pni->epList[0].devData.colorStateApp.timer.wk_flag = temp_data;
									needSaveFlashFlag = 1;
								}
								//devOption.RGBSet.wflag = 1;
								break;
							
							
							default:
								break;
								
						}
					}
					movedata<<=1;
				}
				if(colorChange == 1)
		{
			printf("control color r:%d,g:%d,b:%d\r\n",pni->epList[0].devData.colorStateApp.rgb_r,pni->epList[0].devData.colorStateApp.rgb_g,pni->epList[0].devData.colorStateApp.rgb_b);
			if((pni->epList[0].devData.colorStateApp.onOffStatus == 1) && (pni->onLine == 1))
			{	
				
				userFillUartCmdForColorControl(pni->shortAddr,pni->epList[0].ep,pni->epList[0].devData.colorStateApp.rgb_r,
											pni->epList[0].devData.colorStateApp.rgb_g,pni->epList[0].devData.colorStateApp.rgb_b);
			}else{
				printf("the rgb was not control color status\r\n");
				//*rc = APP_CMD_RC_ERROR_PARAMETER;
				return;
			}
		}
		if(needSaveFlashFlag == 1)
		{
			userUpdateDevInfoDataToFlash(pni);
			userUpdateTheDevTimerEvent((char*)pni);
		}
		devOption.modeByte = ALL_DEV_STATE_ACTION;
		userCreatNodeState_data(pni,CREATE_dEV_STATE_FOR_ALARM_OID,devOption);
		//if(devOption.modeByte != 0)
		{
			//writeUserControlDevStateToCloud(USER_CONRTOL_DEV_TYPE_ZB,intOid,devOption,app_id,TEID);
		}
				break;
			case NODE_INFO_TYPE_AI1SW:
			case NODE_INFO_TYPE_AI2SW:
			case NODE_INFO_TYPE_AI3SW:
				if(payload_len<11)
				{
					Loginfo("Length check fail\n");
					return;
				}
				enable_flag=(ptr[0]<<8)+ptr[1];
				for(i = 0; i < pni->epNum; i ++)
				{
					if(pni->epList[i].ep == 1)
					{
						epIndex1 = i;
					}else if(pni->epList[i].ep == 2){
						epIndex2 = i;
					}else if(pni->epList[i].ep == 3){
						epIndex3 = i;
					}
				}
				if(pni->devType == NODE_INFO_TYPE_AI1SW){
					if((epIndex1 == 0xff) || (payload_len != 12))
					{
						printf("the dev info was error\r\n");
						return;
					}
					move_index=4;
				}
				else if(pni->devType == NODE_INFO_TYPE_AI2SW)
				{
					if((epIndex1 == 0xff) || (epIndex2 == 0xff) || (payload_len != 21))
					{
						printf("the dev info was error\r\n");
						return;
					}
					move_index=7;
				}else if(pni->devType == NODE_INFO_TYPE_AI3SW){
					if((epIndex1 == 0xff) || (epIndex2 == 0xff) || (epIndex3 == 0xff) || (payload_len != 30))
					{
						printf("the dev info was error\r\n");
						return;
					}
					move_index=10;
				} 
				for(i=0;i<move_index;i++)//10
				{
					if(enable_flag & movedata)
					{
						switch(i)
						{
							case 0://OF1 OF2 OF3
								//DataOF1,DataOF2,DataOF3;
								DataOF1=ptr[2]&0x01;
								DataOF2=(ptr[2]&0x02)>>1;
								DataOF3=(ptr[2]&0x04)>>2;
								pni->epList[epIndex1].devData.witchStateApp.onOffStatus = DataOF1;
								userFillUartCmdForOnOff(pni->shortAddr,pni->epList[epIndex1].ep,DataOF1);
								//devOption.SWSet.onoff1 = 1;
								if(pni->devType == NODE_INFO_TYPE_AI2SW || pni->devType == NODE_INFO_TYPE_AI3SW)
								{
									pni->epList[epIndex2].devData.witchStateApp.onOffStatus = DataOF2;
									userFillUartCmdForOnOff(pni->shortAddr,pni->epList[epIndex2].ep,DataOF2);
									//devOption.SWSet.onoff2 = 1;
								}
								if(pni->devType == NODE_INFO_TYPE_AI3SW)
								{
									pni->epList[epIndex3].devData.witchStateApp.onOffStatus = DataOF3;
									userFillUartCmdForOnOff(pni->shortAddr,pni->epList[epIndex3].ep,DataOF3);
									//devOption.SWSet.onoff3 = 1;
								}

								break;
							case 1://TS1
								pni->epList[epIndex1].devData.witchStateApp.timer.s_month = ptr[3];
								pni->epList[epIndex1].devData.witchStateApp.timer.s_day = ptr[4];
								pni->epList[epIndex1].devData.witchStateApp.timer.s_hour = ptr[5];
								pni->epList[epIndex1].devData.witchStateApp.timer.s_min = ptr[6];
								needSaveFlashFlag = 1;
								//devOption.SWSet.ts1 = 1;
								break;
							case 2://TE1
								pni->epList[epIndex1].devData.witchStateApp.timer.e_month = ptr[7];
								pni->epList[epIndex1].devData.witchStateApp.timer.e_day = ptr[8];
								pni->epList[epIndex1].devData.witchStateApp.timer.e_hour = ptr[9];
								pni->epList[epIndex1].devData.witchStateApp.timer.e_min = ptr[10];
								needSaveFlashFlag = 1;
								//devOption.SWSet.te1 = 1;
								break;
							case 3://WF1
								if(pni->epList[epIndex1].devData.witchStateApp.timer.wk_flag != ptr[11])
								{
									pni->epList[epIndex1].devData.witchStateApp.timer.wk_flag = ptr[11];
									needSaveFlashFlag = 1;
								}
								//devOption.SWSet.wflag1 = 1;
								break;
							case 4://TS2
								pni->epList[epIndex2].devData.witchStateApp.timer.s_month = ptr[12];
								pni->epList[epIndex2].devData.witchStateApp.timer.s_day = ptr[13];
								pni->epList[epIndex2].devData.witchStateApp.timer.s_hour = ptr[14];
								pni->epList[epIndex2].devData.witchStateApp.timer.s_min = ptr[15];
								needSaveFlashFlag = 1;
								//devOption.SWSet.ts2 = 1;
								break;
							case 5://TE2
								pni->epList[epIndex2].devData.witchStateApp.timer.e_month = ptr[16];
								pni->epList[epIndex2].devData.witchStateApp.timer.e_day = ptr[17];
								pni->epList[epIndex2].devData.witchStateApp.timer.e_hour = ptr[18];
								pni->epList[epIndex2].devData.witchStateApp.timer.e_min = ptr[19];
								needSaveFlashFlag = 1;
								//devOption.SWSet.te2 = 1;
								break;
							case 6://WF2
								if(pni->epList[epIndex2].devData.witchStateApp.timer.wk_flag != ptr[20])
								{
									pni->epList[epIndex2].devData.witchStateApp.timer.wk_flag = ptr[20];
									needSaveFlashFlag = 1;
								}
								//devOption.SWSet.wflag2 = 1;
								break;
							case 7://TS3
								pni->epList[epIndex3].devData.witchStateApp.timer.s_month = ptr[21];
								pni->epList[epIndex3].devData.witchStateApp.timer.s_day = ptr[22];
								pni->epList[epIndex3].devData.witchStateApp.timer.s_hour = ptr[23];
								pni->epList[epIndex3].devData.witchStateApp.timer.s_min = ptr[24];
								needSaveFlashFlag = 1;
								//devOption.SWSet.ts3 = 1;
								break;
							case 8://TE3
								pni->epList[epIndex3].devData.witchStateApp.timer.e_month = ptr[25];
								pni->epList[epIndex3].devData.witchStateApp.timer.e_day = ptr[26];
								pni->epList[epIndex3].devData.witchStateApp.timer.e_hour = ptr[27];
								pni->epList[epIndex3].devData.witchStateApp.timer.e_min = ptr[28];
								needSaveFlashFlag = 1;
								//devOption.SWSet.te3 = 1;
								break;
							case 9://WF3
								if(pni->epList[epIndex3].devData.witchStateApp.timer.wk_flag != ptr[29])
								{
									pni->epList[epIndex3].devData.witchStateApp.timer.wk_flag = ptr[29];
									needSaveFlashFlag = 1;
								}
								//devOption.SWSet.wflag3 = 1;
								break;
								default:
								break;
								
						}
					}
					movedata<<=1;
				}
				if(needSaveFlashFlag == 1)
				{
					userUpdateDevInfoDataToFlash(pni);
					userUpdateTheDevTimerEvent((char*)pni);
				}
				
				//if(devOption.modeByte != 0)
				{
					//writeUserControlDevStateToCloud(USER_CONRTOL_DEV_TYPE_ZB,intOid,devOption,app_id,TEID);
				}
				devOption.modeByte = ALL_DEV_STATE_ACTION;
				userCreatNodeState_data(pni,CREATE_dEV_STATE_FOR_ALARM_OID,devOption);
				break;
			
			case NODE_INFO_TYPE_ZB_IR_TRANSPONDER:
				break;
			case NODE_INFO_TYPE_RELAY_CONTROL://relay
			
				break;
			case NODE_INFO_TYPE_MAGNET_DOOR:
			case NODE_INFO_TYPE_PIR_SENSOR:	
				//have se as
				printf("\n user_SetSE_parm --payload:\r\n");
				
				for(i = 0; i < payload_len; i ++)//payloadlen mylen
				{
					Loginfo("%02x ",recv_buff[i]);
				}
				if(payload_len<3){
					Loginfo("Length check fail\n");
					return;
					}
				printf("\n user_SetSE_parm payloadlen>=3:\r\n");
				
				pni->epList[0].devData.zoneStateApp.armMode = recv_buff[2]-0x30;
				needSaveFlashFlag = 1;
				//devOption.IASSet.armMode = 1;
				if(needSaveFlashFlag == 1)
				{
					userUpdateDevInfoDataToFlash(pni);
				}
				//if(devOption.modeByte != 0)
				{
					//writeUserControlDevStateToCloud(USER_CONRTOL_DEV_TYPE_ZB,intOid,devOption,app_id,TEID);
				}
				if(recv_buff[2]-0x30)
				{
				//as ==1
				}
				else{
					//as ==0
				}
				
				break;
			case NODE_INFO_TYPE_WATER_SENSOR:
			case NODE_INFO_TYPE_SMOKE_SENSOR:
			case NODE_INFO_TYPE_GAS_SENSOR:
			case NODE_INFO_TYPE_CO_SENSOR:
			case NODE_INFO_TYPE_SOS:
			case NODE_INFO_TYPE_REMOTE_CONTROL:
				//NO SE			
				break;
			case NODE_INFO_TYPE_VIBRATION:
	
				break;
			case NODE_INFO_TYPE_TEMP_HUM_SENSOR:
				// have
				if(payload_len<10)
					return;
				enable_flag=(ptr[0]<<8)+ptr[1];
				for(i = 0; i < pni->epNum; i ++)
				{
					if(pni->epList[i].ep == 1)
					{
						epIndex1 = i;
					}else if(pni->epList[i].ep == 2){
						epIndex2 = i;
					}
				}
				if((epIndex1 == 0xff) || (epIndex2 == 0xff))
				{
					printf("the dev info was error\r\n");
					//*rc = APP_CMD_RC_ERROR_PARAMETER;
					return;
				}
				
				for(i=0;i<8;i++)
				{
					if(enable_flag & movedata)
					{
						switch(i)
						{
							case 0:
								Loginfo("set TEMP_HUM_SENSOR HU\n");
								temp_data=(ptr[3]<<8)+ptr[2];
								if(pni->epList[epIndex2].devData.humiStateApp.upHumi != (uint16_t)(temp_data))//*100
								{
									pni->epList[epIndex2].devData.humiStateApp.upHumi = (uint16_t)(temp_data);
									needSaveFlashFlag = 1;
								}
								//devOption.THPSet.humiUp = 1;

								break;
							case 1:
								Loginfo("set TEMP_HUM_SENSOR HL\n");
								temp_data=(ptr[5]<<8)+ptr[4];
								if(pni->epList[epIndex2].devData.humiStateApp.lowHumi != (uint16_t)(temp_data))
								{
									pni->epList[epIndex2].devData.humiStateApp.lowHumi = (uint16_t)(temp_data);
									needSaveFlashFlag = 1;
								}
								//devOption.THPSet.humiLow = 1;
								break;
							case 2:
								Loginfo("set TEMP_HUM_SENSOR TU\n");
								temp_data=(ptr[7]<<8)+ptr[6];
								if((temp_data&0x80) ==0x80)
								{
									vFlag = -1;	
									temp_data = temp_data & 0x7f;
								}else {
									vFlag = 1;
									temp_data = temp_data & 0x7f;
								}

								if(pni->epList[epIndex1].devData.tempStateApp.upTemp != (int)(vFlag * temp_data))
								{
									pni->epList[epIndex1].devData.tempStateApp.upTemp = (int)(vFlag * temp_data);//100
									needSaveFlashFlag = 1;
								}
								//devOption.THPSet.tempUp = 1;
								break;
							case 3:
								Loginfo("set TEMP_HUM_SENSOR TL\n");
								temp_data=(ptr[9]<<8)+ptr[8];
								if((temp_data&0x80) ==0x80)
								{
									vFlag = -1;	
									temp_data = temp_data & 0x7f;
								}else {
									vFlag = 1;
									temp_data = temp_data & 0x7f;
								}
								if(pni->epList[epIndex1].devData.tempStateApp.lowTemp != (int)(vFlag * temp_data))
								{
									pni->epList[epIndex1].devData.tempStateApp.lowTemp = (int)(vFlag * temp_data);
									needSaveFlashFlag = 1;
								}
								//devOption.THPSet.tempLow = 1;

								break;
							case 4:
								Loginfo("set TEMP_HUM_SENSOR ZA\n");
								value_Henable=(ptr[10] & 0x0f);
								value_Tenable=((ptr[10]>>4) & 0x0f);
								if(pni->epList[epIndex2].devData.humiStateApp.checkEnable != value_Henable)
								{
									pni->epList[epIndex2].devData.humiStateApp.checkEnable = value_Henable;
									needSaveFlashFlag = 1;
								}
								//devOption.THPSet.h_checkEn = 1;
								if(pni->epList[epIndex1].devData.tempStateApp.checkEnable != value_Tenable)
								{
									pni->epList[epIndex1].devData.tempStateApp.checkEnable = value_Tenable;
									needSaveFlashFlag = 1;
								}
								//devOption.THPSet.t_checkEn = 1;
								
								break;
							
							default:
								break;
								
						}
					}
					movedata<<=1;
				}
				if(needSaveFlashFlag == 1)
				{
					userUpdateDevInfoDataToFlash(pni);
				}
				devOption.modeByte = ALL_DEV_STATE_ACTION;
				userCreatNodeState_data(pni,CREATE_dEV_STATE_FOR_ALARM_OID,devOption);
				break;
			case NODE_INFO_TYPE_WARNING_DEV:
				//have
				if(payload_len<5){
					Loginfo("Length check fail\n");
					return;
				}
				enable_flag=(ptr[0]<<8)+ptr[1];
				for(i=0;i<8;i++)
				{
					if(enable_flag & movedata)
					{
						switch(i)
						{
							case 0://RO
								wdAction=ptr[2];
								
								break;
							case 1:
								temp_data=(ptr[3]<<8)+ptr[4];
								printf("wd set duration value:%d\r\n",temp_data);
								if(pni->epList[0].devData.wdStateApp.duration != temp_data)
								{
									pni->epList[0].devData.wdStateApp.duration = temp_data;
									needSaveFlashFlag = 1;
								}
								//devOption.WDSet.duration = 1;
								break;
							default:
								break;
						}
					}
					movedata<<=1;
				}
				if(wdAction!=0xff){
					if(pni->onLine == 1)
					{
						userAppControlWDFunc(pni->shortAddr,pni->epList[0].ep,wdAction,pni->epList[0].devData.wdStateApp.duration);
					}else{
						printf("the wd was off lin\r\n");
						//*rc= APP_CMD_RC_ERROR_PARAMETER;
						return;
					}
				}
				if(needSaveFlashFlag == 1)
				{
					userUpdateDevInfoDataToFlash(pni);
				}
				
				//if(devOption.modeByte != 0)
				{
					//writeUserControlDevStateToCloud(USER_CONRTOL_DEV_TYPE_ZB,intOid,devOption,app_id,TEID);
				}
				break;
			case NODE_INFO_TYPE_ILLUMINATION:
				break;
			case NODE_INFO_TYPE_AIR_QUALITY:
				break;
			case NODE_INFO_TYPE_THERMOSTAT:

				break;
			case NODE_INFO_TYPE_AI_SOCKET_USB:
				//普通插座
				enable_flag=(ptr[0]<<8)+ptr[1];
				if(payload_len<21){
					Loginfo("Length check fail\n");
					return;
				}
				for(i = 0; i < pni->epNum; i ++)
				{
					if(pni->epList[i].ep == 1)
					{
						epIndex1 = i;
					}else if(pni->epList[i].ep == 2){
						epIndex2 = i;
					}
				}
			
				if((epIndex1 == 0xff) || (epIndex2 == 0xff))
				{
					printf("the dev info was error\r\n");
					//*rc = APP_CMD_RC_ERROR_PARAMETER;
					return;
				}
				for(i=0;i<8;i++)
				{
					if(enable_flag & movedata)
					{
						switch(i)
						{
							case 0://RO
								temp_data=ptr[2];
								pni->epList[epIndex1].devData.onoffOutStateApp.onOffStatus = temp_data;
								userFillUartCmdForOnOff(pni->shortAddr,pni->epList[epIndex1].ep,temp_data);
								//devOption.UPlugSet.relayOnoff = 1;
								break;
							case 1://TSr
								pni->epList[epIndex1].devData.onoffOutStateApp.timer.s_month = ptr[3];
								pni->epList[epIndex1].devData.onoffOutStateApp.timer.s_day = ptr[4];
								pni->epList[epIndex1].devData.onoffOutStateApp.timer.s_hour = ptr[5];
								pni->epList[epIndex1].devData.onoffOutStateApp.timer.s_min = ptr[6];
								needSaveFlashFlag = 1;
								//devOption.UPlugSet.tsr = 1;
								break;
							case 2://TEr
								pni->epList[epIndex1].devData.onoffOutStateApp.timer.e_month = ptr[7];
								pni->epList[epIndex1].devData.onoffOutStateApp.timer.e_day = ptr[8];
								pni->epList[epIndex1].devData.onoffOutStateApp.timer.e_hour = ptr[9];
								pni->epList[epIndex1].devData.onoffOutStateApp.timer.e_min = ptr[10];
								needSaveFlashFlag = 1;
								//devOption.UPlugSet.ter = 1;
								break;
							case 3://WFr
								temp_data=ptr[11];
								if(pni->epList[epIndex1].devData.onoffOutStateApp.timer.wk_flag != temp_data)
								{
									pni->epList[epIndex1].devData.onoffOutStateApp.timer.wk_flag = temp_data;
									needSaveFlashFlag = 1;
								}
								//devOption.UPlugSet.wflag_r = 1;

								break;
							case 4://UO
								temp_data=ptr[12];
								pni->epList[epIndex2].devData.onoffOutStateApp.onOffStatus = temp_data;
								userFillUartCmdForOnOff(pni->shortAddr,pni->epList[epIndex2].ep,temp_data);
								//devOption.UPlugSet.usbOnoff = 1;
								break;
							case 5://TSu
								pni->epList[epIndex2].devData.onoffOutStateApp.timer.s_month = ptr[13];
								pni->epList[epIndex2].devData.onoffOutStateApp.timer.s_day = ptr[14];
								pni->epList[epIndex2].devData.onoffOutStateApp.timer.s_hour = ptr[15];
								pni->epList[epIndex2].devData.onoffOutStateApp.timer.s_min = ptr[16];
								needSaveFlashFlag = 1;
								//devOption.UPlugSet.tsu = 1;
								break;
							case 6://TEu
								pni->epList[epIndex2].devData.onoffOutStateApp.timer.e_month = ptr[17];
								pni->epList[epIndex2].devData.onoffOutStateApp.timer.e_day = ptr[18];
								pni->epList[epIndex2].devData.onoffOutStateApp.timer.e_hour = ptr[19];
								pni->epList[epIndex2].devData.onoffOutStateApp.timer.e_min = ptr[20];
								needSaveFlashFlag = 1;
								//devOption.UPlugSet.teu = 1;
								break;
							case 7://WFu
								temp_data=ptr[21];
								if(pni->epList[epIndex2].devData.onoffOutStateApp.timer.wk_flag != temp_data)
								{
									pni->epList[epIndex2].devData.onoffOutStateApp.timer.wk_flag = temp_data;
									needSaveFlashFlag = 1;
								}
								//devOption.UPlugSet.wflag_u = 1;
								break;
								default:
								break;
								
						}
					}
					movedata<<=1;
				}
				if(needSaveFlashFlag == 1)
				{
					userUpdateDevInfoDataToFlash(pni);
					userUpdateTheDevTimerEvent((char*)pni);
				}
				//if(devOption.modeByte != 0)
				{
					//writeUserControlDevStateToCloud(USER_CONRTOL_DEV_TYPE_ZB,intOid,devOption,app_id,TEID);
					devOption.modeByte = ALL_DEV_STATE_ACTION;
					userCreatNodeState_data(pni,CREATE_dEV_STATE_FOR_ALARM_OID,devOption);
				}
				break;
			case NODE_INFO_TYPE_AI_SOCKET_MEASURE:
				//计量插座
				if(payload_len<11){
					Loginfo("Length check fail\n");
					return;
				}
				enable_flag=(ptr[0]<<8)+ptr[1];
				for(i=0;i<8;i++)
				{
					if(enable_flag & movedata)
					{
						switch(i)
						{
							case 0://RO
								pni->epList[0].devData.splugStateApp.onOffStatus = ptr[2];
								userFillUartCmdForOnOff(pni->shortAddr,pni->epList[0].ep,ptr[2]);
								printf("----zb----splugStateApp.onOffStatus=%d\n",ptr[2]);
								//devOption.SPlugSet.relayOnoff = 1;
								break;
							case 1://TS
								pni->epList[0].devData.splugStateApp.timer.s_month = ptr[3];
								pni->epList[0].devData.splugStateApp.timer.s_day = ptr[4];
								pni->epList[0].devData.splugStateApp.timer.s_hour = ptr[5];
								pni->epList[0].devData.splugStateApp.timer.s_min = ptr[6];
								needSaveFlashFlag = 1;
								//devOption.SPlugSet.ts = 1;
								break;
							case 2://TE
								pni->epList[0].devData.splugStateApp.timer.e_month = ptr[7];
								pni->epList[0].devData.splugStateApp.timer.e_day = ptr[8];
								pni->epList[0].devData.splugStateApp.timer.e_hour = ptr[9];
								pni->epList[0].devData.splugStateApp.timer.e_min = ptr[10];
								needSaveFlashFlag = 1;
								//devOption.SPlugSet.te = 1;
								break;
							case 3://WF
								if(pni->epList[0].devData.splugStateApp.timer.wk_flag != ptr[11])
								{
									pni->epList[0].devData.splugStateApp.timer.wk_flag = ptr[11];
									needSaveFlashFlag = 1;
								}
								//devOption.SPlugSet.wflag = 1;
								break;
							default:
								break;
								
						}
					}
					movedata<<=1;
				}
				if(needSaveFlashFlag == 1)
				{
					printf("the dev data was change,start save to flash\r\n");
					userUpdateDevInfoDataToFlash(pni);
					userUpdateTheDevTimerEvent((char*)pni);
				}
				
				//if(devOption.modeByte != 0)  //控制的记录上报
				{
					//writeUserControlDevStateToCloud(USER_CONRTOL_DEV_TYPE_ZB,intOid,devOption,app_id,TEID);
					devOption.modeByte = ALL_DEV_STATE_ACTION;
					userCreatNodeState_data(pni,CREATE_dEV_STATE_FOR_ALARM_OID,devOption);//CREATE_dEV_STATE_FOR_ALARM_OID  CREATE_DEV_STATE_FOR_NEW_NODE_JOIN_OID
				}	
				break;
			case NODE_INFO_TYPE_CUSTOM_RGB_AREA:
				if(payload_len<11){
					Loginfo("Length check fail\n");
					return;
				}
				enable_flag=(ptr[0]<<8)+ptr[1];
				for(i = 0; i < pni->epNum; i ++)
				{
					if(pni->epList[i].ep == 1)
					{
						epIndex1 = i;
					}else if(pni->epList[i].ep == 2){
						epIndex2 = i;
					}else if(pni->epList[i].ep == 3){
						epIndex3 = i;
					}else if(pni->epList[i].ep == 4){
						epIndex4 = i;
					}
				}
		
				for(i=0;i<14;i++)
				{
					if(enable_flag & movedata)
					{
						switch(i)
						{
							case 0://OFL
								//DataONOFF1,DataONOFF2,DataONOFF3,DataONOFF4;
								
								DataONOFF1=ptr[2]&0x01;
								DataONOFF2=(ptr[2]&0x02)>>1;
								DataONOFF3=(ptr[2]&0x04)>>2;
								DataONOFF4=(ptr[2]&0x08)>>3;
								if(pni->epList[epIndex1].devData.cstRgbStateApp.onOffStatus != DataONOFF1)
								{
									pni->epList[epIndex1].devData.cstRgbStateApp.onOffStatus = DataONOFF1;
									userFillUartCmdForOnOff(pni->shortAddr,pni->epList[epIndex1].ep,DataONOFF1);
									//devOption.CUTRGBSet.onoffList1 = 1;
								}else if(pni->epList[epIndex2].devData.cstRgbStateApp.onOffStatus != DataONOFF2){
									pni->epList[epIndex2].devData.cstRgbStateApp.onOffStatus = DataONOFF2;
									userFillUartCmdForOnOff(pni->shortAddr,pni->epList[epIndex2].ep,DataONOFF2);
									//devOption.CUTRGBSet.onoffList2 = 1;
								}else if(pni->epList[epIndex3].devData.cstRgbStateApp.onOffStatus != DataONOFF3){
									pni->epList[epIndex3].devData.cstRgbStateApp.onOffStatus = DataONOFF3;
									userFillUartCmdForOnOff(pni->shortAddr,pni->epList[epIndex3].ep,DataONOFF3);
									//devOption.CUTRGBSet.onoffList3 = 1;
								}else if(pni->epList[epIndex4].devData.cstRgbStateApp.onOffStatus != DataONOFF4){
									pni->epList[epIndex4].devData.cstRgbStateApp.onOffStatus = DataONOFF4;
									userFillUartCmdForOnOff(pni->shortAddr,pni->epList[epIndex4].ep,DataONOFF4);
									//devOption.CUTRGBSet.onoffList4 = 1;
								}
								break;
							case 1://LEL
								//DataLEL1,DataLEL2,DataLEL3,DataLEL4;
								DataLEL1=ptr[3];
								DataLEL2=ptr[4];
								DataLEL3=ptr[5];
								DataLEL4=ptr[6];
								if(pni->epList[epIndex1].devData.cstRgbStateApp.brightness != DataLEL1){
									pni->epList[epIndex1].devData.cstRgbStateApp.brightness = DataLEL1;
									userFillUartCmdForBrightness(pni->shortAddr,pni->epList[epIndex1].ep,DataLEL1);
									//devOption.CUTRGBSet.brightnessList1 = 1;
								}else if(pni->epList[epIndex2].devData.cstRgbStateApp.brightness != DataLEL2){
									pni->epList[epIndex2].devData.cstRgbStateApp.brightness = DataLEL2;
									userFillUartCmdForBrightness(pni->shortAddr,pni->epList[epIndex2].ep,DataLEL2);
									//devOption.CUTRGBSet.brightnessList2 = 1;
								}else if(pni->epList[epIndex3].devData.cstRgbStateApp.brightness != DataLEL3){
									pni->epList[epIndex3].devData.cstRgbStateApp.brightness = DataLEL3;
									userFillUartCmdForBrightness(pni->shortAddr,pni->epList[epIndex3].ep,DataLEL3);
									//devOption.CUTRGBSet.brightnessList3 = 1;
									
								}else if(pni->epList[epIndex4].devData.cstRgbStateApp.brightness != DataLEL4){
									pni->epList[epIndex4].devData.cstRgbStateApp.brightness = DataLEL4;
									userFillUartCmdForBrightness(pni->shortAddr,pni->epList[epIndex4].ep,DataLEL4);
									//devOption.CUTRGBSet.brightnessList4 = 1;
								}
								break;
							case 2://CWL
								//DataCWL1,DataCWL2,DataCWL3,DataCWL4;
								DataCWL1=ptr[7]&0x0f;
								DataCWL2=(ptr[7]>>4&0x0f);
								DataCWL3=ptr[8]&0x0f;
								DataCWL4=(ptr[8]>>4&0x0f);
								if(pni->epList[epIndex1].devData.cstRgbStateApp.colorTemrature != DataCWL1){
									pni->epList[epIndex1].devData.cstRgbStateApp.colorTemrature = DataCWL1;
									userFillUartCmdForColorTemperature(pni->shortAddr,pni->epList[epIndex1].ep,DataCWL1);
									//devOption.CUTRGBSet.colorTempList1 = 1;
								
								}else if(pni->epList[epIndex2].devData.cstRgbStateApp.colorTemrature != DataCWL2){
									pni->epList[epIndex2].devData.cstRgbStateApp.colorTemrature = DataCWL2;
									userFillUartCmdForColorTemperature(pni->shortAddr,pni->epList[epIndex2].ep,DataCWL2);
									//devOption.CUTRGBSet.colorTempList2 = 1;
								
								}else if(pni->epList[epIndex3].devData.cstRgbStateApp.colorTemrature != DataCWL3){
									
									pni->epList[epIndex3].devData.cstRgbStateApp.colorTemrature = DataCWL3;
									userFillUartCmdForColorTemperature(pni->shortAddr,pni->epList[epIndex3].ep,DataCWL3);
									//devOption.CUTRGBSet.colorTempList3 = 1;
									
								}else if(pni->epList[epIndex4].devData.cstRgbStateApp.colorTemrature != DataCWL4){
									pni->epList[epIndex4].devData.cstRgbStateApp.colorTemrature = DataCWL4;
									userFillUartCmdForColorTemperature(pni->shortAddr,pni->epList[epIndex4].ep,DataCWL4);
										//devOption.CUTRGBSet.colorTempList4 = 1;
								}
								break;
							case 3://TS1
								pni->epList[epIndex1].devData.cstRgbStateApp.timer.s_month = ptr[9];
								pni->epList[epIndex1].devData.cstRgbStateApp.timer.s_day = ptr[10];
								pni->epList[epIndex1].devData.cstRgbStateApp.timer.s_hour = ptr[11];
								pni->epList[epIndex1].devData.cstRgbStateApp.timer.s_min = ptr[12];
								needSaveFlashFlag = 1;
								//devOption.CUTRGBSet.ts1 = 1;
								break;
							case 4://TE1
								pni->epList[epIndex1].devData.cstRgbStateApp.timer.e_month = ptr[13];
								pni->epList[epIndex1].devData.cstRgbStateApp.timer.e_day = ptr[14];
								pni->epList[epIndex1].devData.cstRgbStateApp.timer.e_hour = ptr[15];
								pni->epList[epIndex1].devData.cstRgbStateApp.timer.e_min = ptr[16];
								needSaveFlashFlag = 1;
								//devOption.CUTRGBSet.te1 = 1;
								break;
							case 5://WF1
								if(pni->epList[epIndex1].devData.cstRgbStateApp.timer.wk_flag != ptr[17])
								{
									pni->epList[epIndex1].devData.cstRgbStateApp.timer.wk_flag = ptr[17];
									needSaveFlashFlag = 1;
								}
								//devOption.CUTRGBSet.wflag1 = 1;
								break;
							case 6://TS2
								pni->epList[epIndex2].devData.cstRgbStateApp.timer.s_month = ptr[18];
								pni->epList[epIndex2].devData.cstRgbStateApp.timer.s_day = ptr[19];
								pni->epList[epIndex2].devData.cstRgbStateApp.timer.s_hour = ptr[20];
								pni->epList[epIndex2].devData.cstRgbStateApp.timer.s_min = ptr[21];
								needSaveFlashFlag = 1;
								//devOption.CUTRGBSet.ts2 = 1;
								break;
							case 7://TE2
								pni->epList[epIndex2].devData.cstRgbStateApp.timer.e_month = ptr[22];
								pni->epList[epIndex2].devData.cstRgbStateApp.timer.e_day = ptr[23];
								pni->epList[epIndex2].devData.cstRgbStateApp.timer.e_hour = ptr[24];
								pni->epList[epIndex2].devData.cstRgbStateApp.timer.e_min = ptr[25];
								needSaveFlashFlag = 1;
								//devOption.CUTRGBSet.te2 = 1;
								break;
							case 8://WF2
								if(pni->epList[epIndex2].devData.cstRgbStateApp.timer.wk_flag != ptr[26])
								{
									pni->epList[epIndex2].devData.cstRgbStateApp.timer.wk_flag = ptr[26];
									needSaveFlashFlag = 1;
								}
								//devOption.CUTRGBSet.wflag2 = 1;
								break;
							case 9://TS3
								pni->epList[epIndex3].devData.cstRgbStateApp.timer.s_month = ptr[27];
								pni->epList[epIndex3].devData.cstRgbStateApp.timer.s_day = ptr[28];
								pni->epList[epIndex3].devData.cstRgbStateApp.timer.s_hour = ptr[29];
								pni->epList[epIndex3].devData.cstRgbStateApp.timer.s_min = ptr[30];
								needSaveFlashFlag = 1;
								//devOption.CUTRGBSet.ts3 = 1;
								break;
							case 10://TE3
								pni->epList[epIndex3].devData.cstRgbStateApp.timer.e_month = ptr[31];
								pni->epList[epIndex3].devData.cstRgbStateApp.timer.e_day = ptr[32];
								pni->epList[epIndex3].devData.cstRgbStateApp.timer.e_hour = ptr[33];
								pni->epList[epIndex3].devData.cstRgbStateApp.timer.e_min = ptr[34];
								needSaveFlashFlag = 1;
								//devOption.CUTRGBSet.te3 = 1;
								break;
							case 11://WF3
								if(pni->epList[epIndex3].devData.cstRgbStateApp.timer.wk_flag != ptr[35])
								{
									pni->epList[epIndex3].devData.cstRgbStateApp.timer.wk_flag = ptr[35];
									needSaveFlashFlag = 1;
								}
								//devOption.CUTRGBSet.wflag3 = 1;
								break;
							case 12://TS4
								pni->epList[epIndex4].devData.cstRgbStateApp.timer.s_month = ptr[36];
								pni->epList[epIndex4].devData.cstRgbStateApp.timer.s_day = ptr[37];
								pni->epList[epIndex4].devData.cstRgbStateApp.timer.s_hour = ptr[38];
								pni->epList[epIndex4].devData.cstRgbStateApp.timer.s_min = ptr[39];
								needSaveFlashFlag = 1;
								//devOption.CUTRGBSet.ts4 = 1;
								break;
							case 13://TE4
								pni->epList[epIndex4].devData.cstRgbStateApp.timer.e_month = ptr[40];
								pni->epList[epIndex4].devData.cstRgbStateApp.timer.e_day = ptr[41];
								pni->epList[epIndex4].devData.cstRgbStateApp.timer.e_hour = ptr[42];
								pni->epList[epIndex4].devData.cstRgbStateApp.timer.e_min = ptr[43];
								needSaveFlashFlag = 1;
								//devOption.CUTRGBSet.te4 = 1;
								break;
							case 14://WF4
								if(pni->epList[epIndex4].devData.cstRgbStateApp.timer.wk_flag != ptr[44])
								{
									pni->epList[epIndex4].devData.cstRgbStateApp.timer.wk_flag = ptr[44];
									needSaveFlashFlag = 1;
								}
								//devOption.CUTRGBSet.wflag3 = 1;
								break; 
							default:
								break;
								
						}
					}
					movedata<<=1;
				}
				if(needSaveFlashFlag == 1)
				{
					userUpdateDevInfoDataToFlash(pni);
					userUpdateTheDevTimerEvent((char*)pni);
				}	
				//if(devOption.modeByte != 0)
				{
					//writeUserControlDevStateToCloud(USER_CONRTOL_DEV_TYPE_ZB,intOid,devOption,app_id,TEID);
					devOption.modeByte = ALL_DEV_STATE_ACTION;
					userCreatNodeState_data(pni,CREATE_dEV_STATE_FOR_ALARM_OID,devOption);
				}
				
				break;
			default:
				break;
		}
		

}

void App_get_zigbeeSS(uint16_t index)
{
	DEVINFOOPTION devOption1;
	HM_DEV_RAM_LIST *pni_temp;
	devOption1.modeByte = ALL_DEV_STATE_ACTION;
	pni_temp = findInDevListByIndex(index);
	if(pni_temp == NULL )//not find dev from index
	{
		printf("set zb se cmd the control dev was not valid\r\n");
		return ;  //调试
	}
	userCreatNodeState_data(pni_temp,CREATE_dEV_STATE_FOR_ALARM_OID,devOption1);//SS
}

void App_get_zigbeeSE(uint16_t index)
{
	DEVINFOOPTION devOption1;
	HM_DEV_RAM_LIST *pni_temp;
	devOption1.modeByte = ALL_DEV_STATE_ACTION;
	pni_temp = findInDevListByIndex(index);
	if(pni_temp == NULL )
	{
		printf("set zb se cmd the control dev was not valid\r\n");
		return ;  //调试
	}
	userCreatNodeState_data(pni_temp,CREATE_DEV_STATE_FOR_GET_NODE_SETTING_OID,devOption1);//SE
} 

void App_get_zigbeeSingleSS(uint16_t index,DEVINFOOPTION devOption)
{

	HM_DEV_RAM_LIST *pni_temp;
	pni_temp = findInDevListByIndex(index);
	if(pni_temp == NULL )
	{
		printf("set zb se cmd the control dev was not valid\r\n");
		return ;  //调试
	}
	userCreatNodeState_data(pni_temp,CREATE_DEV_STATE_FOR_GET_NODE_STATE_OID,devOption);//CREATE_DEV_STATE_FOR_GET_NODE_STATE_OID

}

























