
#include "qcom_common.h"
#include "ServerDataHandle.h"

#include "../lt_sample/ltm3210.h"
#include "MQTTPacket.h"
#include "MQTTinterface.h"
#include "hm_keyAndSmartLink.h"
#include "hm_app.h"

#include "hm_network_data_manage.h"
#include "hm_zigbeeInfohand.h"
#include "hm_halBeep.h"

#include "aes.h"

//#include "hm_reportDevToCloud.h"



extern NETWORK_USER_CONFIG network_config_call;
extern DEV_MYSELF_INFO hmIotsDevInfo;
DEV_GW_INFO hmIotsinformation;
extern BEEP_INFO beep_info;
extern TX_SEMAPHORE beep_semaphore_ptr;
extern qcom_timer_t gwNightLightTimer;
extern DEV_NUM_INFO devNumInfo;
extern GWSTATFORTIME gwStateForRealTime;
extern SUBOTARAM subOtaRam;


extern unsigned char clear_zigbeenet_flag;
extern uint16_t OTA_HTTPSERVER_PORT;




//extern DEV_MYSELF_INFO hmIotsDevInfo;





void hm_information_init(void)
{
	hmIotsinformation.protocolver=0x0002;
	hmIotsinformation.wifihardwarever=0x0001;
	hmIotsinformation.wifisoftwarever=0x0001;//0x0001
	hmIotsinformation.zigbeever=0x0001;
}
char *strupr_a(char*ch)
{
   long i,m;
   for(i=0;i<strlen(ch);i++)
   {
      if(*(ch+i)!=0)
      {
        m=*(ch+i);
        if(96<m&&m<123)
        *(ch+i)=*(ch+i)-32;
      }
      else if(*(ch+i)=0)
         break;
    }
    return ch;
}

void ctoi(unsigned char *c, unsigned char *i) {
	switch (*c) {
	case '0':
		*i |= 0;
		break;
	case '1':
		*i |= 1;
		break;
	case '2':
		*i |= 2;
		break;
	case '3':
		*i |= 3;
		break;
	case '4':
		*i |= 4;
		break;
	case '5':
		*i |= 5;
		break;
	case '6':
		*i |= 6;
		break;
	case '7':
		*i |= 7;
		break;
	case '8':
		*i |= 8;
		break;
	case '9':
		*i |= 9;
		break;
	case 'a':
	case 'A':
		*i |= 0xa;
		break;
	case 'b':
	case 'B':
		*i |= 0xb;
		break;
	case 'c':
	case 'C':
		*i |= 0xc;
		break;
	case 'd':
	case 'D':
		*i |= 0xd;
		break;
	case 'e':
	case 'E':
		*i |= 0xe;
		break;
	case 'f':
	case 'F':
		*i |= 0xf;
		break;
	default:
		break;
	}

}

void itoc(unsigned char *i, unsigned char *c) {
	switch (*i) {
	case 0:
		*c |= '0';
		break;
	case 1:
		*c |= '1';
		break;
	case 2:
		*c |= '2';
		break;
	case 3:
		*c |= '3';
		break;
	case 4:
		*c |= '4';
		break;
	case 5:
		*c |= '5';
		break;
	case 6:
		*c |= '6';
		break;
	case 7:
		*c |= '7';
		break;
	case 8:
		*c |= '8';
		break;
	case 9:
		*c |= '9';
		break;
	case 10:
		*c |= 'A';
		break;
	case 11:
		*c |= 'B';
		break;
	case 12:
		*c |= 'C';
		break;
	case 13:
		*c |= 'D';
		break;
	case 14:
		*c |= 'E';
		break;
	case 15:
		*c |= 'F';
		break;
	default:
		break;
	}
}

int combined_point_datapacket(char actiontype,char action_parm,char encrypt_parm,char datatype,unsigned char* buf,unsigned char* payload, int payloadlen)
{
	int length;
	unsigned char *ptr = buf;
	uint8_t key[40]={0};
	uint8_t *ptr_int,*ptr_out;
	uint16_t ptrint_len,ptrout_len,i,tempdata;
	uint32_t convertdata;
	//char encrypt_mode=0;

	#ifdef ENCRYPT_ENABLE
	if(encrypt_parm==PRIVATE_ENCRYPT){
		//encrypt_mode=2;
		memcpy(key,hmIotsDevInfo.aesKey,strlen(hmIotsDevInfo.aesKey));
		printf("\n PRIVATE_ENCRYPT:%s --\n",hmIotsDevInfo.aesKey);
	
	}else{
		//encrypt_mode=1;
		memcpy(key,GW_PUBPLICKEY,strlen(GW_PUBPLICKEY));
		printf("\n PULIC_ENCRYPT:%s --\n",GW_PUBPLICKEY);
	}
	#endif
	if(action_parm==SENDKEY)
	{
		memcpy(key,GW_PUBPLICKEY,strlen(GW_PUBPLICKEY));
		printf("\n PULIC_ENCRYPT:%s --\n",GW_PUBPLICKEY);
	}
	
	writeChar(&ptr, actiontype);
	writeChar(&ptr, action_parm);//ENCRYPT_TYPE
	writeChar(&ptr, ENCRYPT_TYPE);
	writeChar(&ptr, encrypt_parm);
	//ptr += MQTTPacket_encode(ptr, rem_len);
	if(datatype)
	writeChar(&ptr, datatype);

	if((encrypt_parm) && (payloadlen)){//encrypt_mode
		printf("\n encrypt_mode!=0----\n");
		ptr_int = mem_alloc(512);
		ptr_out = mem_alloc(512);

		ptrint_len=0;
		memcpy(ptr_int, payload, payloadlen);
		ptrint_len+=payloadlen;
		printf("\n pass mingwen keydata:\n");
		for(i=0;i<ptrint_len;i++)
		{
			printf("%02x ",ptr_int[i]);
		}
		ptrout_len=user_aes128_encode(ptr_int,ptrint_len,ptr_out,key);		

		writeInt(&ptr, ptrout_len);
		memcpy(ptr, ptr_out, ptrout_len);
		ptr += ptrout_len;
		mem_free(ptr_int);
		mem_free(ptr_out);		
	}else{
	if(payloadlen){
		//writeChar(&ptr, payloadlen);
		writeInt(&ptr, payloadlen);
		memcpy(ptr, payload, payloadlen);
		ptr += payloadlen;
	}
	}
	length = ptr - buf;
	return length;
}

int combined_pass_datapacket(int transport_type,int cmd_parm,unsigned char* buf,unsigned char* payload, int payloadlen)
{
	int length;
	unsigned char *ptr = buf;
	char encrypt_mode=0;
	uint8_t key[40]={0};
	uint8_t ExpressData[512];
	unsigned char *ptr_ex=ExpressData;
	uint8_t *ptr_int,*ptr_out;
	uint16_t ptrint_len,ptrout_len,i,tempdata;
	uint32_t convertdata;
	uint8_t temp_buff[3];
	
//add
	#ifdef ENCRYPT_ENABLE
		encrypt_mode=2;
		memcpy(key,hmIotsDevInfo.aesKey,strlen(hmIotsDevInfo.aesKey));
	#endif
	if(cmd_parm == GETAESKEY_CMD)
	{
		encrypt_mode=PUBLICKEY_MODE;
		memset(key,0,sizeof(key));
		memcpy(key,GW_PUBPLICKEY,strlen(GW_PUBPLICKEY));
		printf("GW_PUBPLICKEY encrypt key--\n");
	}

	writeChar(&ptr, ACTION);//action
	writeChar(&ptr, CONTROL_ACK);//ENCRYPT_TYPE
	writeChar(&ptr, ENCRYPT_TYPE);
	writeChar(&ptr, encrypt_mode);
	writeChar(&ptr, PASS);

	if(encrypt_mode)
	{
		ptr_int = mem_alloc(512);
		ptr_out = mem_alloc(512);
		ptrint_len=0;
		//tempdata=transport_type;
		big_little_exchange(transport_type,&convertdata);
		//temp_buff=(uint8_t *)&transport_type;
		memcpy(ptr_int, &convertdata, 2);ptrint_len+=2;
		tempdata=DESTINATIONID;
		big_little_exchange(DESTINATIONID,&convertdata);
		memcpy(ptr_int+ptrint_len, &convertdata, 2);ptrint_len+=2;
		tempdata=SOURCEID_TOAPP;
		big_little_exchange(SOURCEID_TOAPP,&convertdata);
		memcpy(ptr_int+ptrint_len, &convertdata, 2);ptrint_len+=2;
		tempdata=SN_COUNTS;
		big_little_exchange(SN_COUNTS,&convertdata);
		memcpy(ptr_int+ptrint_len, &convertdata, 2);ptrint_len+=2;
		tempdata=cmd_parm;
		big_little_exchange(cmd_parm,&convertdata);
		memcpy(ptr_int+ptrint_len, &convertdata, 2);ptrint_len+=2;
		tempdata=payloadlen;
		big_little_exchange(payloadlen,&convertdata);
		memcpy(ptr_int+ptrint_len, &convertdata, 2);ptrint_len+=2;	
		memcpy(ptr_int+ptrint_len, payload, payloadlen);
		ptrint_len += payloadlen;
		printf("\n pass keydata:");
		for(i=0;i<ptrint_len;i++)
		{
			printf("%02x ",ptr_int[i]);
		}
		
		//ptrint_len = 12+payloadlen;
		ptrout_len=user_aes128_encode(ptr_int,ptrint_len,ptr_out,key);

		writeInt(&ptr, ptrout_len);
		memcpy(ptr, ptr_out, ptrout_len);
		ptr += ptrout_len;
		mem_free(ptr_int);
		mem_free(ptr_out);
	}else{
	//if(GETAESKEY_CMD!=cmd_parm)
	writeInt(&ptr, payloadlen+12);//12	
	writeInt(&ptr, transport_type);//writeInt(&ptr, INFORMATIONTYPE); //REPORTINFOTYPE
	writeInt(&ptr, DESTINATIONID);
	writeInt(&ptr, SOURCEID_TOAPP);
	writeInt(&ptr, SN_COUNTS);
	writeInt(&ptr, cmd_parm);
	
	writeInt(&ptr, payloadlen);
	memcpy(ptr, payload, payloadlen);
	ptr += payloadlen;
	}

	length = ptr - buf;
	return length;
}

int combined_list_datapacket(char action_mode,unsigned char* buf,unsigned char* payload, int payloadlen)
{
	int length;
	unsigned char *ptr = buf;
	uint8_t key[40]={0};
	char encrypt_mode=0;
	uint8_t *ptr_int,*ptr_out;
	uint16_t ptrint_len,ptrout_len,i;
//add
	#ifdef ENCRYPT_ENABLE
		encrypt_mode=2;
	#endif

	writeChar(&ptr, ACTION);//action
	writeChar(&ptr, SENDLIST);//ENCRYPT_TYPE
	writeChar(&ptr, ENCRYPT_TYPE);
	writeChar(&ptr, encrypt_mode);
	writeChar(&ptr, LIST_KEY);

	if(encrypt_mode){
		ptr_int = mem_alloc(512);
		ptr_out = mem_alloc(512);
		
		ptrint_len=0;
		memset(key,0,sizeof(key));
		memcpy(key,hmIotsDevInfo.aesKey,strlen(hmIotsDevInfo.aesKey));
		memcpy(ptr_int+ptrint_len, payload, payloadlen);
		ptrint_len += payloadlen;
		printf("\n pass mingwen keydata:\n");
		for(i=0;i<ptrint_len;i++)
		{
			printf("%02x ",ptr_int[i]);
		}
		ptrout_len=user_aes128_encode(ptr_int,ptrint_len,ptr_out,key);
		printf("\n pass mingwen ptrout_len:%d,%x",ptrout_len,ptrout_len);
		writeInt(&ptr, ptrout_len);
		memcpy(ptr, ptr_out, ptrout_len);
		ptr += ptrout_len;
		mem_free(ptr_int);
		mem_free(ptr_out);
	}else{
	writeInt(&ptr, payloadlen);//12
	//writeInt(&ptr, payloadlen);
	memcpy(ptr, payload, payloadlen);
	ptr += payloadlen;
	}

	length = ptr - buf;
	return length;
}


void big_little_exchange(int sdata,int *edata)
{
	uint8_t temp_data=0;
	uint16_t data=0;
	temp_data=sdata&0xff;
	sdata=(sdata>>8)&0xff;
	data=temp_data<<8;
	*edata=(data)|sdata;
}
void Int_biglittle_Convert(int sdata,int *edata)
{
	int k = sdata;
	char *p = (char*)&k;
	uint32_t temp_data1=0,temp_data2=0,temp_data3=0,temp_data4=0;
	//temp_data0=(sdata<<24)&0xff000000;
	temp_data1=(unsigned int)(*p)<<24;
    temp_data2=((unsigned int)*(p+1))<<16;
    temp_data3=((unsigned int)*(p+2))<<8;
    temp_data4=((unsigned int)*(p+3));
	*edata=temp_data1 + temp_data2 +temp_data3+temp_data4;
}



//���ز������ô���//
void set_gateway_parm(char* data,int data_len)
{
	uint8_t *ptr=data;
	uint8_t data_buff[9],i;
	uint16_t enable_flag,movedata=0x0001;
	uint8_t needSaveFlashFlag = 0;
	uint8_t temp_data;
	enable_flag=(ptr[2]<<8)+ptr[3];
	Loginfo("set_gateway_parm enable_flag=%x\n",enable_flag);
	memcpy(data_buff,&ptr[4],data_len-4);
	for(i=0;i<8;i++)
	{
		if(enable_flag & movedata)
		{
			switch(i)
			{
				case 0:
					Loginfo("set_gateway_parm Alarmlevel,=%d,%x\n",i,data_buff[i]);//NULL
					needSaveFlashFlag = 1;
					break;
				case 1:
					Loginfo("set_gateway_parm Soundlevel=%d,%x\n",i,data_buff[i]);
					hmIotsDevInfo.sound_level=data_buff[i];
				 	needSaveFlashFlag = 1;
						if(beep_info.work_flag == 0) 
					 {
					 	 beep_info.value = hmIotsDevInfo.sound_level;
						 beep_info.work_type = BEEP_SOUND_REGULA;
						 beep_info.work_flag = 1;
						 tx_semaphore_put(&beep_semaphore_ptr);
					 }
				 
				 //devOption.GWStat.alarmlevel = 1;
					break;
				case 2:
					Loginfo("set_gateway_parm Betimer=%d,%x\n",i,data_buff[i]);
					hmIotsDevInfo.beep_alarm_times=data_buff[i];
					needSaveFlashFlag = 1;
						if(beep_info.work_flag == 0) 
					 {
						 beep_info.work_type = BEEP_ALARM_TIMER_CHANGE;
						 beep_info.work_flag = 1;
						 tx_semaphore_put(&beep_semaphore_ptr);
					 }
				 
				 //devOption.GWStat.betimer = 1;
					break;
				case 3:
					Loginfo("set_gateway_parm Gwlanguage=%d,%x\n",i,data_buff[i]);
					hmIotsDevInfo.app_language=data_buff[i];
					needSaveFlashFlag = 1;
					break;
				case 4:
					Loginfo("set_gateway_parm Gwlightlevel=%d,%x\n",i,data_buff[i]);
					hmIotsDevInfo.light_level=data_buff[i];
					needSaveFlashFlag = 1;
					userControlGWLedFunc(1,3);
					//devOption.GWStat.gwlightlevel = 1;
					break;
				case 5:
					Loginfo("set_gateway_parm Gwlightonoff=%d,%x\n",i,data_buff[i]);
					hmIotsDevInfo.light_onoff=data_buff[i];
					needSaveFlashFlag = 1;
					userControlGWLedFunc(hmIotsDevInfo.light_onoff,0);
					//devOption.GWStat.gwlightonoff = 1;
					break;
				case 6:
					Loginfo("set_gateway_parm Lgtimer=%d,%x\n",i,data_buff[i]);
					hmIotsDevInfo.light_on_times=data_buff[i];
					needSaveFlashFlag = 1;
					//hmIotsDevInfo.light_on_times = objs->valueint;
					qcom_timer_delete(&gwNightLightTimer);
					qcom_timer_init(&gwNightLightTimer, lightNight_timerS_handle, NULL, hmIotsDevInfo.light_on_times * 1000, ONESHOT);
					//needSaveFlashFlag = 1;
					break;
				case 7:
					Loginfo("set_gateway_parm Armtype=%d,%x\n",i,data_buff[i]);
					hmIotsDevInfo.sensor_arm_type=data_buff[i];
					//needSaveFlashFlag = 1;
					temp_data=data_buff[i];
					if(temp_data == SENSOR_DISARM){
					hmSetAllSensorDisarm();
					if(beep_info.work_flag == 0 )
					 {
						 beep_info.work_type = BEEP_DISARM;
						 beep_info.work_flag = 1;
						 tx_semaphore_put(&beep_semaphore_ptr);
					 }
					 if(hmIotsDevInfo.sensor_arm_type != SENSOR_DISARM)
					 {
						 hmIotsDevInfo.sensor_arm_type = SENSOR_DISARM;
						 needSaveFlashFlag = 1;
					 }
				}else if(temp_data == SENSOR_HOME_ARM){
					hmSetAllSensorHomeArm();
					if((beep_info.work_flag == 0) && ((beep_info.work_type != BEEP_ALARM) && (beep_info.work_type != BEE_SOUND_LOCAL_REGULA)) )
					 {
						 beep_info.work_type = BEEP_HOME_ARM;
						 beep_info.work_flag = 1;
						 tx_semaphore_put(&beep_semaphore_ptr);
					 }
					if(hmIotsDevInfo.sensor_arm_type != SENSOR_HOME_ARM)
					 {
						 hmIotsDevInfo.sensor_arm_type = SENSOR_HOME_ARM;
						 needSaveFlashFlag = 1;
					 }
				}else if(temp_data == SENSOR_ARM){
					hmSetAllSensorArm();
					if((beep_info.work_flag == 0) && ((beep_info.work_type != BEEP_ALARM) && (beep_info.work_type != BEE_SOUND_LOCAL_REGULA)))
					 {
						 beep_info.work_type = BEEP_ARM;
						 beep_info.work_flag = 1;
						 tx_semaphore_put(&beep_semaphore_ptr);
					 }
					if(hmIotsDevInfo.sensor_arm_type != SENSOR_ARM)
					 {
						 hmIotsDevInfo.sensor_arm_type = SENSOR_ARM;
						 needSaveFlashFlag = 1;
					 }
				}else{
					printf("-----the arm mode is not valid\r\n");
					//*rc = APP_CMD_RC_ERROR_PARAMETER;
					//return 1;
				}
				hmIotsDevInfo.sensor_arm_type = temp_data;
				//devOption.GWStat.armtype = 1;
					break;
			}
		}
		movedata<<=1;
	}
	if((beep_info.work_flag == 0) && ((beep_info.work_type != BEEP_ALARM) && (beep_info.work_type != BEE_SOUND_LOCAL_REGULA)))
	{
		beep_info.work_type = BEEP_LANGUAGE_SET;
		beep_info.work_flag = 1;
		tx_semaphore_put(&beep_semaphore_ptr);
	}
	 //devOption.GWStat.gwlanguage = 1;
	if(needSaveFlashFlag == 1)
	{
			userFlashWriteSysInfoFunc((uint8_t*)&hmIotsDevInfo);	
	}

}


void sent_point_sceneack(char*data,int len)
{
	uint8_t buff[512],temp_data,key[64]={0};
	uint8_t *ptr=NULL,*ptr_out=NULL;
	uint16_t bufflen;

	if(data[1]==SCENE)
		data[1]=SCENE_ACK;
	memcpy(buff,data,len);
	bufflen=len;
	HM_Sent_point_data(buff,bufflen);
}
void sent_point_sceneack_new(uint8_t *data,uint32_t len,uint32_t log_id)
{
	uint8_t buff[512],temp_data,key[64]={0};
	uint8_t *ptr=NULL,*ptr_out=NULL;
	uint16_t rc=0x0001,bufflen,ptrlen=0,ptrout_len=0,aeskey_len,i;
	uint32_t convert_intdata,convertdata;
	uint8_t encrypt_mode=0;

	ptr = mem_alloc(512);
	temp_data=ACTION;
	memcpy(ptr,&temp_data,1);
	ptrlen+=1;
	temp_data=SCENE_ACK;
	memcpy(ptr+ptrlen,&temp_data,1);
	ptrlen+=1;

	#ifdef ENCRYPT_ENABLE
	{
		encrypt_mode=2;
		memcpy(key,hmIotsDevInfo.aesKey,strlen(hmIotsDevInfo.aesKey));
		printf("\n PRIVATE_ENCRYPT:%s --\n",hmIotsDevInfo.aesKey);
	
	}
	#endif
	temp_data=ENCRYPT_TYPE;
	memcpy(ptr+ptrlen,&temp_data,1);
	ptrlen+=1;
	temp_data=encrypt_mode;
	memcpy(ptr+ptrlen,&temp_data,1);
	ptrlen+=1;
	temp_data=LOGID;
	memcpy(ptr+ptrlen,&temp_data,1);
	ptrlen+=1;
	Int_biglittle_Convert(log_id,&convert_intdata);
	memcpy(ptr+ptrlen,&convert_intdata,4);
	ptrlen+=4;
	temp_data=RESULT_KEY;
	memcpy(ptr+ptrlen,&temp_data,1);
	ptrlen+=1;
	temp_data=1;
	memcpy(ptr+ptrlen,&temp_data,1);
	ptrlen+=1;
	
	if((encrypt_mode) && (len)){//encrypt_mode
		printf("\n encrypt_mode!=0----\n");
		ptr_out = mem_alloc(512);
		printf("\n pass mingwen keydata:\n");
		for(i=0;i<len;i++)
		{
			printf("%02x ",data[i]);
		}
		ptrout_len=user_aes128_encode(data,len,ptr_out,key);		

		//writeInt(&ptr, ptrout_len);
		temp_data=POINT;
		memcpy(ptr+ptrlen,&temp_data,1);
		ptrlen+=1;
		big_little_exchange(ptrout_len,&convert_intdata);
		memcpy(ptr+ptrlen,&convert_intdata,2);
		ptrlen+=2;	
		memcpy(ptr+ptrlen, ptr_out, ptrout_len);
		ptrlen += ptrout_len;
		mem_free(ptr_out);		
	}else{
	if(len){
		printf("\n sent_point_sceneack_new point---\n");
		temp_data=POINT;
		memcpy(ptr+ptrlen,&temp_data,1);
		ptrlen+=1;
		big_little_exchange(len,&convert_intdata);
		memcpy(ptr+ptrlen,&convert_intdata,2);
		ptrlen+=2;
		memcpy(ptr+ptrlen,data,len);
		ptrlen+=len;
	}
	}
	
	HM_Sent_point_data(ptr,ptrlen);
	mem_free(ptr);
}
//�����ϱ�zigbee�豸�����ɹ�//
void report_zb_intonetwork(HM_DEV_RAM_LIST *pni,unsigned char* payload, int payloadlen)
{
	uint8_t buff[512];
	uint8_t *ptr=NULL;
	uint16_t rc=0x0001,bufflen,ptrlen=0;
	uint32_t convertdata;
	uint32_t temp_deviceid;
	ptr = mem_alloc(512);
	
	big_little_exchange(rc,&convertdata);
	memcpy(ptr,&convertdata,2);
	ptrlen+=2;
	big_little_exchange(pni->index,&convertdata);
	memcpy(ptr+ptrlen,&convertdata,2);
	ptrlen+=2;
	big_little_exchange(pni->devType,&convertdata);
	memcpy(ptr+ptrlen,&convertdata,2);
	ptrlen+=2;//mac[ZIGBEE_MAC_LEN];
	memcpy(ptr+ptrlen,pni->mac,8);
	ptrlen+=8;
	memcpy(ptr+ptrlen,payload,payloadlen);
	ptrlen+=payloadlen;
	bufflen=combined_pass_datapacket(REPORTINFOTYPE,ZBINTOSS_CMD,buff,ptr,ptrlen);
	HM_Sent_pass_data(buff,bufflen);
	mem_free(ptr);

	//network_config_call.server_deviceid=0x00000001;
	temp_deviceid=0x00010001;
	sent_zigbeelist_msg(pni,ADD_ZIGBEE_MODE,network_config_call.server_deviceid,1);//server_deviceid
//ZBINTOSS_CMD
}//total����counts

//�����ϱ�zigbee�� point data //
void report_zb_pointdata(HM_DEV_RAM_LIST *pni,unsigned char* payload, int payloadlen)
{
	uint8_t buff[512],temp_data;
	uint8_t *ptr=NULL;
	uint16_t rc=0x0001,bufflen,ptrlen=0;
	uint32_t convert_intdata,convertdata;
	uint32_t temp_deviceid;
	uint8_t encrypt_mode=NO_ENCRYPT;

	#ifdef ENCRYPT_ENABLE
	encrypt_mode=PRIVATE_ENCRYPT;
	#endif
	ptr = mem_alloc(512);
	temp_data=DEVICEID_KEY;
	memcpy(ptr,&temp_data,1);
	ptrlen+=1;
	Int_biglittle_Convert(network_config_call.server_deviceid,&convert_intdata);
	memcpy(ptr+ptrlen,&convert_intdata,4);
	ptrlen+=4;

	temp_data=SUB_INDEX_KEY;
	memcpy(ptr+ptrlen,&temp_data,1);
	ptrlen+=1;
	big_little_exchange(pni->index,&convertdata);
	memcpy(ptr+ptrlen,&convertdata,2);
	ptrlen+=2;
	
	//memcpy(ptr+ptrlen,&payloadlen,2);
	//ptrlen+=2;
	memcpy(ptr+ptrlen,payload,payloadlen);
	ptrlen+=payloadlen;
	//bufflen=combined_pass_datapacket(REPORTINFOTYPE,ZBINTOSS_CMD,buff,ptr,ptrlen);
	
	bufflen=combined_point_datapacket(ACTION,REPORT,encrypt_mode,POINT,buff,ptr,ptrlen);//NO_ENCRYPT
	printf("------- report_zb_pointdata  \r\n");
	HM_Sent_point_data(buff,bufflen);
	mem_free(ptr);

}


//����zigbee���豸��SS//
void sent_zigbee_SSstate(HM_DEV_RAM_LIST *pni,unsigned char* payload, int payloadlen)
{
	uint8_t buff[512];
	uint8_t *ptr=NULL;
	uint16_t rc=0x0001,bufflen,ptrlen=0;
	uint32_t convertdata;
	
	ptr = mem_alloc(512);
	
	big_little_exchange(rc,&convertdata);
	memcpy(ptr,&convertdata,2);//
	ptrlen+=2;
	big_little_exchange(pni->index,&convertdata);
	memcpy(ptr+ptrlen,&convertdata,2);//
	ptrlen+=2;
	/*big_little_exchange(pni->devType,&convertdata);
	memcpy(ptr+ptrlen,&convertdata,2);
	ptrlen+=2;
	memcpy(ptr+ptrlen,pni->mac,8);
	ptrlen+=8;*/
	memcpy(ptr+ptrlen,payload,payloadlen);
	ptrlen+=payloadlen;
	bufflen=combined_pass_datapacket(INFORMATIONTYPE,GETONEZBSS_CMD,buff,ptr,ptrlen);
	HM_Sent_pass_data(buff,bufflen);
	mem_free(ptr);
}
//����zigbee���豸�ĵ���״ֵ̬SS//
void sent_zigbee_SingleSS(HM_DEV_RAM_LIST *pni,unsigned char* payload, int payloadlen)
{
	uint8_t buff[512];
	uint8_t *ptr=NULL;
	uint16_t rc=0x0001,bufflen,ptrlen=0;
	uint32_t convertdata;
	
	ptr = mem_alloc(512);
	big_little_exchange(rc,&convertdata);
	memcpy(ptr,&rc,2);
	ptrlen+=2;
	big_little_exchange(pni->index,&convertdata);
	memcpy(ptr+ptrlen,&pni->index,2);
	ptrlen+=2;
	/*big_little_exchange(pni->devType,&convertdata);
	memcpy(ptr+ptrlen,&convertdata,2);
	ptrlen+=2;
	memcpy(ptr+ptrlen,pni->mac,8);
	ptrlen+=8;*/
	memcpy(ptr+ptrlen,payload,payloadlen);
	ptrlen+=payloadlen;
	bufflen=combined_pass_datapacket(INFORMATIONTYPE,GETZBSINGLESS_CMD,buff,ptr,ptrlen);
	HM_Sent_pass_data(buff,bufflen);
	mem_free(ptr);

}
//����zigbee���豸��SE//
void sent_zigbee_SEsetting(HM_DEV_RAM_LIST *pni,unsigned char* payload, int payloadlen)
{
	uint8_t buff[512];
	uint8_t *ptr=NULL;
	uint16_t rc=0x0001,bufflen,ptrlen=0;
	uint32_t convertdata;
	ptr = mem_alloc(512);
	
	//big_little_exchange(rc,&convertdata);
	memcpy(ptr,&rc,2);
	ptrlen+=2;
	//big_little_exchange(pni->index,&convertdata);
	memcpy(ptr+ptrlen,&pni->index,2);
	ptrlen+=2;
	/*big_little_exchange(pni->devType,&convertdata);
	memcpy(ptr+ptrlen,&convertdata,2);
	ptrlen+=2;
	memcpy(ptr+ptrlen,pni->mac,8);
	ptrlen+=8;*/
	memcpy(ptr+ptrlen,payload,payloadlen);
	ptrlen+=payloadlen;
	bufflen=combined_pass_datapacket(INFORMATIONTYPE,GETZBSE_CMD,buff,ptr,ptrlen);
	HM_Sent_pass_data(buff,bufflen);
	mem_free(ptr);
}



//�ϱ�������list���⣬zigbee��������ӣ�ɾ�����ϴ�//
void sent_zigbeelist_msg(HM_DEV_RAM_LIST *pni,unsigned char type_mode,unsigned int server_deviceid,unsigned char total_counts)
{
	uint8_t buff[512],i=0;
	uint8_t *ptr=NULL;
	uint16_t rc=0x0001,bufflen,ptrlen=0;
	uint32_t temp_id;
	int	convert_intdata,convertdata;
	ptr = mem_alloc(512);
	memcpy(ptr,&total_counts,1);
	ptrlen+=1;
	if(total_counts==1){
	i+=1;
	memcpy(ptr+ptrlen,&i,1);
	ptrlen+=1;
	//Loginfo("\n sent_zigbeelist_msg -----------01:\r\n");
	memcpy(ptr+ptrlen,&type_mode,1);
	ptrlen+=1;
	if(server_deviceid==1){
		temp_id=network_config_call.server_deviceid;
		Int_biglittle_Convert(temp_id,&convert_intdata);
		memcpy(ptr+ptrlen,&convert_intdata,4);
		ptrlen+=4;
	}
	else{
	Int_biglittle_Convert(server_deviceid,&convert_intdata);
	memcpy(ptr+ptrlen,&convert_intdata,4);
	ptrlen+=4;
	}
	//Loginfo("\n sent_zigbeelist_msg -----------01:\r\n");
	big_little_exchange(pni->index,&convertdata);
	memcpy(ptr+ptrlen,&convertdata,2);
	ptrlen+=2;
	big_little_exchange(pni->devType,&convertdata);
	memcpy(ptr+ptrlen,&convertdata,2);
	ptrlen+=2;
	memcpy(ptr+ptrlen,pni->mac,8);
	ptrlen+=8;

	memcpy(ptr+ptrlen,&pni->onLine,1);//add
	ptrlen+=1;
	}
	//Loginfo("\n sent_zigbeelist_msg -----------03\r\n");
	bufflen=combined_list_datapacket(ADD_ZIGBEE_MODE,buff,ptr,ptrlen);
	Sent_list_data(buff,bufflen);
	mem_free(ptr);
}

//�����ϱ�zigbee�豸��������pass, //

void report_zb_alarmMsg(char mode,unsigned char* payload, int payloadlen)
{
	uint8_t buff[512];
	uint8_t temp_buff[128]={0},tempbuff_len,alarm_data,tamper_data;
	uint8_t *ptr=NULL;
	uint16_t rc=0x0001,bufflen,ptrlen=0;
	uint16_t index,tempdata,product_devType;
	uint32_t convert_intdata,convertdata;
	uint8_t encrypt_mode=NO_ENCRYPT;
	uint8_t actionvalue;

	#ifdef ENCRYPT_ENABLE
	encrypt_mode=PRIVATE_ENCRYPT;
	#endif
	memset(buff,0,sizeof(buff));
	ptr = mem_alloc(512);ptrlen=0;
	tempdata=DEVICEID_KEY;
	memcpy(ptr,&tempdata,1);ptrlen+=1;
	Int_biglittle_Convert(network_config_call.server_deviceid,&convert_intdata);
	memcpy(ptr+ptrlen,&convert_intdata,4);
	ptrlen+=4;
	//tempdata=SUB_INDEX_KEY;
	//memcpy(ptr+ptrlen,&tempdata,1);ptrlen+=1;
	//memcpy(ptr+ptrlen,&temp_buff[2],2);ptrlen+=2;

	memcpy(ptr+ptrlen,payload,payloadlen);
	ptrlen+=payloadlen;
	actionvalue=ALARM_ACT;
	bufflen=combined_point_datapacket(ACTION,actionvalue,encrypt_mode,POINT,buff,ptr,ptrlen);//NO_ENCRYPT
	HM_Sent_point_data(buff,bufflen);
	mem_free(ptr);

}

void report_zb_recordMsg(unsigned char* payload, int payloadlen)
{
	uint8_t buff[512];
	uint8_t alarm_data,tamper_data;
	uint8_t *ptr=NULL;
	uint16_t rc=0x0001,bufflen,ptrlen=0;
	uint16_t index,tempdata,product_devType;
	uint32_t convert_intdata,convertdata;
	uint8_t encrypt_mode=NO_ENCRYPT;
	uint8_t actionvalue;

	#ifdef ENCRYPT_ENABLE
	encrypt_mode=PRIVATE_ENCRYPT;
	#endif
	memset(buff,0,sizeof(buff));
	ptr = mem_alloc(512);ptrlen=0;
	tempdata=DEVICEID_KEY;
	memcpy(ptr,&tempdata,1);ptrlen+=1;
	Int_biglittle_Convert(network_config_call.server_deviceid,&convert_intdata);
	memcpy(ptr+ptrlen,&convert_intdata,4);
	ptrlen+=4;

	memcpy(ptr+ptrlen,payload,payloadlen);
	ptrlen+=payloadlen;
	actionvalue=EVENT;

	bufflen=combined_point_datapacket(ACTION,actionvalue,encrypt_mode,POINT,buff,ptr,ptrlen);//NO_ENCRYPT
	HM_Sent_point_data(buff,bufflen);
	mem_free(ptr);
}

void App_get_harmdev(void)
{
	uint16_t tempIndex;
	HM_DEV_RAM_LIST *pni_temp;
	uint8_t *ptr=NULL;
	uint8_t sent_buff[256],i;
	uint16_t sent_buff_len,ptrlen=0,rc=0x0001;
	uint32_t convertdata;	

	ptr = mem_alloc(512);
	big_little_exchange(rc,&convertdata);
	memcpy(ptr,&convertdata,2);
	ptrlen+=2;
	memcpy(ptr+ptrlen,&hmIotsDevInfo.home_arm_for_dev.indexNum,1);
	ptrlen+=1;
	if(hmIotsDevInfo.home_arm_for_dev.indexNum > 0)
	{
		for(i = 0; i < MAX_HOME_ARM_FOR_SENSOR_NUM; i ++)
		{
			tempIndex = hmIotsDevInfo.home_arm_for_dev.index[i];
			if(tempIndex != 0)
			{
				pni_temp = findInDevListByIndex(tempIndex);
				if((pni_temp != NULL) && ((pni_temp->devType == NODE_INFO_TYPE_MAGNET_DOOR) || (pni_temp->devType == NODE_INFO_TYPE_PIR_SENSOR) ||(pni_temp->devType == NODE_INFO_TYPE_VIBRATION)))
				{
					big_little_exchange(tempIndex,&convertdata);
					memcpy(ptr+ptrlen,&convertdata,2);
					ptrlen+=2;
				}else{
					hmIotsDevInfo.home_arm_for_dev.index[i] = 0;
					hmIotsDevInfo.home_arm_for_dev.indexNum --;
				}
			}
		}
	}
	sent_buff_len=combined_pass_datapacket(INFORMATIONTYPE,GETHARMDEV_CMD,sent_buff,ptr,ptrlen);
	HM_Sent_pass_data(sent_buff,sent_buff_len);
	mem_free(ptr);

}

void App_get_relatedwd(uint16_t index)
{
	uint16_t tempIndex;
	HM_DEV_RAM_LIST *pni_temp;
	uint8_t *ptr=NULL;
	uint8_t sent_buff[256],i;
	uint16_t sent_buff_len,ptrlen=0,rc=0x0001;
	uint32_t convertdata;
	uint8_t needSaveFlash=0,tempNum=0;

	ptr = mem_alloc(512);
	big_little_exchange(rc,&convertdata);
	memcpy(ptr,&convertdata,2);
	ptrlen+=2;
	big_little_exchange(index,&convertdata);
	memcpy(ptr+ptrlen,&convertdata,2);
	ptrlen+=2;
	convertdata=0;
	memcpy(ptr+ptrlen,&convertdata,1);//total num
	ptrlen+=1;
	
	pni_temp = findInDevListByIndex(index);
	if(pni_temp != NULL)
	{
		memcpy(ptr+ptrlen,&pni_temp->epList[0].devData.wdStateApp.devLinkWd.enable,1);
		ptrlen+=1;
		for(i = 0; i < MAX_DEV_LINK_WD_NUM; i ++)
		{
			tempIndex = pni_temp->epList[0].devData.wdStateApp.devLinkWd.zLinkWd[i].zIndex;
			if(tempIndex != 0)
			{
				if(findInDevListByIndex(tempIndex) != NULL)
				{
					
					big_little_exchange(tempIndex,&convertdata);
					memcpy(ptr+ptrlen,&convertdata,2);
					ptrlen+=2;
					Int_biglittle_Convert(pni_temp->epList[0].devData.wdStateApp.devLinkWd.zLinkWd[i].zAlarmType,&convertdata);
					memcpy(ptr+ptrlen,&convertdata,4);
					ptrlen+=4;
					//pni_temp->epList[0].devData.wdStateApp.devLinkWd.zLinkWd[i].zAlarmType;
					tempNum ++;
					
				}else{
					pni_temp->epList[0].devData.wdStateApp.devLinkWd.zLinkWd[i].zIndex = 0;
					pni_temp->epList[0].devData.wdStateApp.devLinkWd.zLinkWd[i].zAlarmType = 0;
					needSaveFlash = 1;
				}
			}
		}
		memcpy(ptr+4,&tempNum,1);//total num

	}
	else{
		convertdata=0;
		memcpy(ptr+ptrlen,&convertdata,2);
		ptrlen+=2;
		memcpy(ptr+ptrlen,&convertdata,2);
		ptrlen+=2;
		memcpy(ptr+ptrlen,&convertdata,1);
		ptrlen+=1;		
	}
	sent_buff_len=combined_pass_datapacket(INFORMATIONTYPE,GETRELATEDWD_CMD,sent_buff,ptr,ptrlen);
	HM_Sent_pass_data(sent_buff,sent_buff_len);
	mem_free(ptr);
	if(needSaveFlash == 1)
	{
		userUpdateDevInfoDataToFlash(pni_temp);
	}
	
}

void App_set_nightlightlinkage(char* data)
{
	uint16_t tempIndex;
	HM_DEV_RAM_LIST *pni_temp;
	uint8_t eb_data,ds_counts,temp_data;
	uint16_t tem_zbindex;
	uint8_t i,needSaveFlashFlag=0;

	eb_data=data[0];
	if(eb_data)
	{
		if(hmIotsDevInfo.gwLightForNight.enable != 1)
		{
			hmIotsDevInfo.gwLightForNight.enable = 1;
			needSaveFlashFlag = 1;
		}
	}else{
		if(hmIotsDevInfo.gwLightForNight.enable != 0)
		{
			hmIotsDevInfo.gwLightForNight.enable = 0;
			needSaveFlashFlag = 1;
		}
	}
	ds_counts=data[1];
	//for(i=0;i<ds_counts;i++)
	{
		hmIotsDevInfo.gwLightForNight.indexNum = 0;
		for(i = 0; i < ds_counts; i ++)
		{
			//tempIndex=
			tem_zbindex=(data[2+2*i]<<8)+data[3+2*i];
			pni_temp = findInDevListByIndex(tem_zbindex);
			if(pni_temp != NULL)
			{
				if((pni_temp->devType == NODE_INFO_TYPE_MAGNET_DOOR) || (pni_temp->devType == NODE_INFO_TYPE_PIR_SENSOR) || (pni_temp->devType == NODE_INFO_TYPE_VIBRATION))
				{
					hmIotsDevInfo.gwLightForNight.index[hmIotsDevInfo.gwLightForNight.indexNum] = tem_zbindex;
					hmIotsDevInfo.gwLightForNight.indexNum ++;
					printf("test hmIotsDevInfo.gwLightForNight.indexNum ++\r\n");
				}
			}
			printf("test ds_counts for---01\r\n");
		}
		needSaveFlashFlag = 1;	
	}

	if(needSaveFlashFlag == 1)
	{
		userFlashWriteSysInfoFunc((uint8_t*)&hmIotsDevInfo);	
		printf("test Write Sys Info Func----0\r\n");
	}
}

void App_get_nightlightlinkage(void)
{
	uint16_t tempIndex;
	HM_DEV_RAM_LIST *pni_temp;
	uint8_t *ptr=NULL;
	uint8_t sent_buff[256],i;
	uint16_t sent_buff_len,ptrlen=0,rc=0x0001;
	uint32_t convertdata;
	uint8_t needSaveFlash=0,tempNum=0;

	ptr = mem_alloc(512);
	big_little_exchange(rc,&convertdata);
	memcpy(ptr,&convertdata,2);
	ptrlen+=2;

	memcpy(ptr+ptrlen,&hmIotsDevInfo.gwLightForNight.indexNum,1);
	ptrlen+=1;

	if(hmIotsDevInfo.gwLightForNight.indexNum > 0)
	{
		for(i = 0; i < MAX_HOME_ARM_FOR_SENSOR_NUM; i ++)
		{
			tempIndex = hmIotsDevInfo.gwLightForNight.index[i];
			if(tempIndex != 0)
			{
				pni_temp = findInDevListByIndex(tempIndex);
				if((pni_temp != NULL) && ((pni_temp->devType == NODE_INFO_TYPE_MAGNET_DOOR) || (pni_temp->devType == NODE_INFO_TYPE_PIR_SENSOR) || (pni_temp->devType == NODE_INFO_TYPE_VIBRATION)))
				{
					//json_add_item_toarray(js_ds,json_create_number(tempIndex));
					big_little_exchange(tempIndex,&convertdata);
					memcpy(ptr+ptrlen,&convertdata,2);
					ptrlen+=2;
					tempNum ++;
				}else{
					hmIotsDevInfo.gwLightForNight.index[i] = 0;
				}
			}
		}
	}
	if(hmIotsDevInfo.gwLightForNight.indexNum != tempNum)
	{
		memcpy(ptr+2,&tempNum,1);
	}
		
	sent_buff_len=combined_pass_datapacket(INFORMATIONTYPE,GETNIGHTLINKAGE_CMD,sent_buff,ptr,ptrlen);
	HM_Sent_pass_data(sent_buff,sent_buff_len);
	mem_free(ptr);
	
}

void App_set_armtimer(char* data)
{
	uint32_t convertdata;
	uint8_t needSaveFlash=0,tempNum=0,temp_data;
	uint8_t temp_armdata,ret=1;
	TIMEIN tempTimer;
	//�ó�������ʱ��
	temp_armdata = data[0];
	tempTimer.month = data[1];
	tempTimer.day = data[2];
	tempTimer.hour = data[3];
	tempTimer.min = data[4];
	tempTimer.wkflag = data[5];
	printf("test for temp_armdata:%d\n",temp_armdata);
	if(temp_armdata == SENSOR_DISARM)
	{
		printf("test for disarm\r\n");
		hmIotsDevInfo.sensor_arm_timer.disarmTimer = tempTimer;
	}else if(temp_armdata == SENSOR_ARM){
		printf("test for arm\r\n");
		hmIotsDevInfo.sensor_arm_timer.armTimer = tempTimer;
	}else if(temp_armdata == SENSOR_HOME_ARM){
		printf("test for hom arm\r\n");
		hmIotsDevInfo.sensor_arm_timer.homeArmTimer = tempTimer;
	}else{
		printf("the json arm state value was not valid\r\n");
		ret=0;
	}
	
	if(ret == 1)
	{
		needSaveFlash = 1;
		userUpdateGwArmForTimerEvent((char*)&hmIotsDevInfo);//�������س�������ʱ��
		printf("set arm timer was success\r\n");
	}else{
		printf("set arm timer was faild\r\n");
	}

	if(needSaveFlash == 1)
	{
		userFlashWriteNumFunc((uint8_t *)&devNumInfo);
	}

}

void App_get_armtimer()
{
	uint16_t tempIndex;
	HM_DEV_RAM_LIST *pni_temp;
	uint8_t *ptr=NULL;
	uint8_t sent_buff[256],i;
	uint16_t sent_buff_len,ptrlen=0,rc=0x0001;
	uint32_t convertdata;
	uint8_t needSaveFlash=0,tempNum=0;
	uint8_t temp_armdata;

	ptr = mem_alloc(512);
	big_little_exchange(rc,&convertdata);
	memcpy(ptr,&convertdata,2);
	ptrlen+=2;

	memcpy(ptr+ptrlen,&hmIotsDevInfo.sensor_arm_type,1);
	ptrlen+=1;
	temp_armdata=SENSOR_DISARM;
	memcpy(ptr+ptrlen,&temp_armdata,1);
	ptrlen+=1;
	memcpy(ptr+ptrlen,&hmIotsDevInfo.sensor_arm_timer.disarmTimer.month,1);
	ptrlen+=1;
	memcpy(ptr+ptrlen,&hmIotsDevInfo.sensor_arm_timer.disarmTimer.day,1);
	ptrlen+=1;
	memcpy(ptr+ptrlen,&hmIotsDevInfo.sensor_arm_timer.disarmTimer.hour,1);
	ptrlen+=1;
	memcpy(ptr+ptrlen,&hmIotsDevInfo.sensor_arm_timer.disarmTimer.min,1);
	ptrlen+=1;
	memcpy(ptr+ptrlen,&hmIotsDevInfo.sensor_arm_timer.disarmTimer.wkflag,1);
	ptrlen+=1;

	temp_armdata=SENSOR_ARM;
	memcpy(ptr+ptrlen,&temp_armdata,1);
	ptrlen+=1;
	memcpy(ptr+ptrlen,&hmIotsDevInfo.sensor_arm_timer.armTimer.month,1);
	ptrlen+=1;
	memcpy(ptr+ptrlen,&hmIotsDevInfo.sensor_arm_timer.armTimer.day,1);
	ptrlen+=1;
	memcpy(ptr+ptrlen,&hmIotsDevInfo.sensor_arm_timer.armTimer.hour,1);
	ptrlen+=1;
	memcpy(ptr+ptrlen,&hmIotsDevInfo.sensor_arm_timer.armTimer.min,1);
	ptrlen+=1;
	memcpy(ptr+ptrlen,&hmIotsDevInfo.sensor_arm_timer.armTimer.wkflag,1);
	ptrlen+=1;

	temp_armdata=SENSOR_HOME_ARM;
	memcpy(ptr+ptrlen,&temp_armdata,1);
	ptrlen+=1;
	memcpy(ptr+ptrlen,&hmIotsDevInfo.sensor_arm_timer.homeArmTimer.month,1);
	ptrlen+=1;
	memcpy(ptr+ptrlen,&hmIotsDevInfo.sensor_arm_timer.homeArmTimer.day,1);
	ptrlen+=1;
	memcpy(ptr+ptrlen,&hmIotsDevInfo.sensor_arm_timer.homeArmTimer.hour,1);
	ptrlen+=1;
	memcpy(ptr+ptrlen,&hmIotsDevInfo.sensor_arm_timer.homeArmTimer.min,1);
	ptrlen+=1;
	memcpy(ptr+ptrlen,&hmIotsDevInfo.sensor_arm_timer.homeArmTimer.wkflag,1);
	ptrlen+=1;


	sent_buff_len=combined_pass_datapacket(INFORMATIONTYPE,GETARMTIMER_CMD,sent_buff,ptr,ptrlen);
	HM_Sent_pass_data(sent_buff,sent_buff_len);
	mem_free(ptr);
}





void pass_data_process(int cmddata,char* data,int data_len)
{
	char i,payload[512]={0};
	uint32_t testdata_len,lendata;
	uint16_t subset_index,getep,tempzones;
	uint8_t homearm_set_counts,needSaveFlashFlag=0;
	uint8_t temp_data;
	uint16_t temp_zb_index;
	HM_DEV_RAM_LIST *pni_temp;
	DEVINFOOPTION devOption1;
	devOption1.modeByte = 0;
	switch(cmddata)
	{
		case GETAESKEY_CMD:
			Loginfo("\n pass_data_process GETAESKEY_CMD:\r\n");
			HM_sent_pass_askeymsg(cmddata);//respond_pass_aeskey(cmddata);
			break;
		case GETMESG_CMD:
		//qca_uart_send(buf, length);
			Loginfo("\n pass_data_process:\r\n");
			for(i = 0; i < data_len; i ++)//payloadlen mylen
			{
				Loginfo("%02x ",data[i]);
			}
			memcpy(payload,data,data_len);
			printf("\n 11pass_data_process:\r\n");
			for(i = 0; i < data_len; i ++)//payloadlen mylen
			{
				Loginfo("%02x ",payload[i]);
			}
			hm_sent_pass_mesgdata();
			break;
		case SETZONE_CMD:
			//hmIotsDevInfo.time_zones=(data[2]<<8)+data[3];//(data[3]<<8)+data[2];//
			tempzones=(data[2]<<8)+data[3];
			Loginfo("hmIotsDevInfo.time_zones set=%x\n",hmIotsDevInfo.time_zones);
			HM_pass_ack_data(cmddata,data,data_len);qcom_thread_msleep(50);
			if(hmIotsDevInfo.time_zones!=tempzones)
			{
				hmIotsDevInfo.time_zones=tempzones;
				userFlashWriteSysInfoFunc((uint8_t*)&hmIotsDevInfo);	
			}
			break;
		case SETGWPARM_CMD:
			Loginfo("pass_data_process SETGWPARM_CMD\n");
			for(i = 0; i < data_len; i ++)//payloadlen mylen
			{
				Loginfo("%02x ",data[i]);
			}
			set_gateway_parm(data,data_len);
			HM_pass_ack_data(cmddata,data,data_len);
			break;
		case GETGWPARM_CMD:
			Loginfo("pass_data_process GETGWPARM_CMD\n");
			hm_sent_pass_gatewayparm();//get_gateway_parm();
			break;
		case ZBINTONETWORK_CMD:
			if(data[2]==0x33)
			{	Loginfo("sent_GWMsg_pointdata to server\n");
				//sent_GWMsg_pointdata();
				//devNumInfo.devIndex[0] =  0;
				//memset(devNumInfo.devIndex,0,sizeof(devNumInfo.devIndex));
			 	//	userFlashInitToRam();break;
				//Loginfo("\n test-------------- userFlashInitToRam\n");
			}
			if(data[2]==0x34)
			{  //bie http://api.wnhoo.com:8555/app/HaSampleGatewaySoc_v1.4.0.ebl
				//test zigbee ota
				memset(subOtaRam.subFileUrl,0,MAX_WOFTWARE_ADDR_LEN);
				memcpy(&subOtaRam.subFileUrl,"http://47.88.192.21/firmwareFile/HaSampleGatewaySoc_v1.4.0.ebl",strlen("http://47.88.192.21/firmwareFile/HaSampleGatewaySoc_v1.4.0.ebl"));
				subOtaRam.subOtaStart = 1;			
				memcpy(subOtaRam.subFileMd5,"6ca7f74a318425dcb2444b2f702d0f9f",strlen("6ca7f74a318425dcb2444b2f702d0f9f"));
				subOtaRam.sufFileSize=141312;
			}
			Loginfo("\n pass_data_process ZBINTONETWORK_CMD\r\n");
			if((data[2]&0x0f))
			{
				Loginfo("into network\r\n");
				userFillUartCmdForPermitJoin(DEFUALT_ZIGBEE_JOINING_TIMERS);
			}else{
				Loginfo("Leave network\r\n");
				userFillUartCmdForPermitJoin(0);//Close join netowrk
			}
			HM_pass_ack_data(cmddata,data,data_len);

			if(data[2]==0x32)
			{
				//mqtt_disconnect();
			}
			//userSaveAllInfoToFlash();
			
			/*sent_point_aeskey();
			//test -----------------------
			memset(test_indata,0,sizeof(test_indata));
			memcpy(test_indata,"123456781234567812345",strlen("123456781234567812345"));
			//test_indata[0]=0xff;test_indata[1]=0xfe;test_indata[2]=0xfd;
			memset(key,0,sizeof(key));
			memcpy(key,"b7e21cb8776411e7",strlen("b7e21cb8776411e7"));
			Loginfo("\n 00 test user_aes128_encode test_indata=%s,key=%s\r\n",test_indata,key);
			memset(test_data,0,sizeof(test_data));//test_indata
			lendata=strlen("123456781234567812345");
			testdata_len=user_aes128_encode(test_indata,lendata,test_data,key);//test_indata
			Loginfo("\n 00 test user_aes128_encode hex==\r\n");
			for(i=0;i<testdata_len;i++)
			{
				Loginfo("%02x",test_data[i]);
			}
			Loginfo("\n test user_aes128_encode test_data=%s\r\n",test_data);
			memset(test_indata,0,sizeof(test_indata));
			user_aes128_decode(test_data,strlen(test_data),test_indata,key);
			Loginfo("\n 01 test user_aes128_encode hex==\r\n");
			for(i=0;i<3;i++)
			{
				Loginfo("%02x",test_indata[i]);
			}			
			//user_aes_encode(test_indata,3,test_data,key);
			Loginfo("\n 11 test user_aes128_encode test_data=%s\r\n",test_data);
			memset(test_indata,0,sizeof(test_indata));
			user_aes128_decode(test_data,strlen(test_data),test_indata,key);
			Loginfo("\n 01 test user_aes128_encode hex==\r\n");
			for(i=0;i<3;i++)
			{
				Loginfo("%02x",test_indata[i]);
			}*/
			
			break;
		case DELZBSUBSET_CMD:
			subset_index=(data[2]<<8)+data[3];//(data[3]<<8)+data[2];//
			HM_pass_ack_data(cmddata,data,data_len);
			Loginfo("\n pass_data_process DELZBSUBSET_CMD =%x\r\n",subset_index);
			pni_temp = findInDevListByIndex(subset_index);
			if(pni_temp == NULL )//not find dev from index
			{
				printf("DELZBSUBSET_CMD the control dev was not valid\r\n");
				//*rc = APP_CMD_RC_ERROR_PARAMETER;
				return ;
			}
			sent_zigbeelist_msg(pni_temp,DELETE_ZIGBEE_MODE,1,1);//delete point
			userFillUartCmdForLeaveDev(pni_temp->shortAddr);//��uart tx���������ram��flash
			
			break;
		case GETALLZBSS_CMD:
			HM_pass_ack_data(cmddata,data,data_len);
			Loginfo("\n pass_data_process all zigbee ss =%c\r\n",data[2]); //��֧��
			break;
		case GETONEZBSS_CMD://app��ȡ SS
			HM_pass_ack_data(cmddata,data,data_len);
			subset_index=(data[2]<<8)+data[3];//(data[3]<<8)+data[2];//
			Loginfo("\n pass_data_process one zigbee sub_index =%x\r\n",subset_index);
			App_get_zigbeeSS(subset_index);
			//devOption1.modeByte = ALL_DEV_STATE_ACTION;
			//pni_temp = findInDevListByIndex(subset_index);
			//userCreatNodeState_data(pni_temp,CREATE_dEV_STATE_FOR_ALARM_OID,devOption1);//SS
			break;
		case GETZBSINGLESS_CMD://get index.getep ����״ֵ̬
			HM_pass_ack_data(cmddata,data,data_len);
			subset_index=(data[2]<<8)+data[3];//(data[3]<<8)+data[2];//
			getep=data[4];
			devOption1.modeByte |=(1<<getep);
			App_get_zigbeeSingleSS(subset_index,devOption1);
			Loginfo("\n pass_data_process one zigbee sub_index=%x, getep=%x\r\n",subset_index,getep);
			break;
		case GETZBSE_CMD://3007 se 
		//App_get_zigbeeSE(uint16_t index)
			subset_index=(data[2]<<8)+data[3];//(data[3]<<8)+data[2];//
			HM_pass_ack_data(cmddata,data,data_len);
			Loginfo("\n pass_data_process get zb se cmd =%x\r\n",subset_index);
			pni_temp = findInDevListByIndex(subset_index);
			if(pni_temp == NULL )//not find dev from index
			{
				printf("set zb se cmd the control dev was not valid\r\n");
				return ;  //����
			}
			App_get_zigbeeSE(subset_index);
			
			break;
		case SETZBSE_CMD://3008  se
			subset_index=(data[2]<<8)+data[3];//(data[3]<<8)+data[2];//
			HM_pass_ack_data(cmddata,data,data_len);
			Loginfo("\n pass_data_process set zb se cmd =%x\r\n",subset_index);
			pni_temp = findInDevListByIndex(subset_index);
			if(pni_temp == NULL )//not find dev from index
			{
				printf("set zb se cmd the control dev was not valid\r\n");
				return ;  //����
			}

			memcpy(payload,data,data_len);
			printf("\n 12pass_data_process:\r\n");
			for(i = 0; i < data_len; i ++)//payloadlen mylen
			{
				Loginfo("%02x ",payload[i]);
			}
			user_SetSE_parm(pni_temp,&payload[4],data_len-4);
			//pass_ack_data(cmddata,data,data_len);
			
			break;
		case DEVICEOTA_CMD:
			Loginfo("\n pass_data_process DEVICEOTA_CMD \r\n");
			memcpy(payload,data,data_len);
			printf("\n 13 pass_data_process:\r\n");
			for(i = 0; i < data_len; i ++)//payloadlen mylen
			{
				Loginfo("%02x ",payload[i]);
			} 
			printf("\n");
			HM_pass_ack_data(cmddata,data,data_len);

			if(data[2]==WFFIRMWARE_TYPE)//if(data[2]==0x33)
			{
				if(data[3]==0x32)
				{
					gwStateForRealTime.wifiSoftUpdate.appAllowSoftWareUpdate = 1;
				}else{
					gwStateForRealTime.wifiSoftUpdate.appAllowSoftWareUpdate = 0;
				}
			}
			else if(data[2]==ZBFIRMWARE_TYPE)
			{
				if(data[3]==0x32)
				{
					gwStateForRealTime.zigbeeZcBasicInfo.appAllowSoftWareUpdate = 1;
				}else{
					gwStateForRealTime.zigbeeZcBasicInfo.appAllowSoftWareUpdate = 0;
				}
			}
			
			break;
		case SETSERVER_CMD:
			Loginfo("\n pass_data_process SETSERVER_CMD \r\n");
			printf("\n 12pass_data_process:\r\n");
			HM_pass_ack_data(cmddata,data,data_len);
			memset(payload,0,sizeof(payload));
			memcpy(payload,&data[2],data_len);
			for(i = 0; i < data_len; i ++)//payloadlen mylen
			{
				Loginfo("%02x ",payload[i]);
			}
			printf("\n");
			Loginfo("%s",payload);// hmIotsDevInfo.ipaddr
			memset(hmIotsDevInfo.ipaddr,0,sizeof(hmIotsDevInfo.ipaddr));
			memcpy(hmIotsDevInfo.ipaddr,payload,strlen(payload));
			userFlashWriteSysInfoFunc((uint8_t*)&hmIotsDevInfo);
			qcom_thread_msleep(3000);
			userCheckSaveFlashAndReset(0);
			break;
		case SETHARMDEV_CMD:
			Loginfo("\n pass_data_process SETHARMDEV_CMD \r\n");		
			HM_pass_ack_data(cmddata,data,data_len);
			memset(payload,0,sizeof(payload));
			memcpy(payload,&data[2],data_len);
			printf("\n 12pass_data_process:\r\n");
			for(i = 0; i < data_len; i ++)
			{
				Loginfo("%02x ",payload[i]);
			}
			printf("\n");
			homearm_set_counts=payload[0];
			hmIotsDevInfo.home_arm_for_dev.indexNum = 0;
			if(homearm_set_counts>0){
			for(i = 0; i < homearm_set_counts; i ++)
			{
				temp_zb_index=(payload[2*(i+1)-1]<<8)+payload[2*(i+1)];
				Loginfo("pass_data_process temp_zb_index:%02x \r\n",temp_zb_index);
				pni_temp = findInDevListByIndex(temp_zb_index);
				if(pni_temp != NULL)
				{
					if((pni_temp->devType == NODE_INFO_TYPE_MAGNET_DOOR) || (pni_temp->devType == NODE_INFO_TYPE_PIR_SENSOR) ||(pni_temp->devType == NODE_INFO_TYPE_VIBRATION))
					{
						hmIotsDevInfo.home_arm_for_dev.index[hmIotsDevInfo.home_arm_for_dev.indexNum] = temp_zb_index;
						hmIotsDevInfo.home_arm_for_dev.indexNum ++;
						needSaveFlashFlag = 1;
					}
				}
			}
			}
			
			if(needSaveFlashFlag == 1)
			{
				if(pni_temp!=NULL)
					{userUpdateDevInfoDataToFlash(pni_temp);}
				//userUpdateTheDevTimerEvent((char*)pni_temp);
			}
			userFlashWriteSysInfoFunc((uint8_t*)&hmIotsDevInfo);	
			break;
		case GETHARMDEV_CMD:
			Loginfo("\n pass_data_process GETHARMDEV_CMD \r\n");
			HM_pass_ack_data(cmddata,data,data_len);
			App_get_harmdev();
			break;
		case SETALLLIGHT_CMD:
			printf("all light control\r\n");
			HM_pass_ack_data(cmddata,data,data_len);
			temp_data=data[2];
			userFillUartCmdForOnOffByGroupsOrBroadcast(ALL_LIGHT_GROUPS,GROUPS_ADDR,temp_data);
			break;
		case SETRELATEDWD_CMD:
			printf("pass_data_process SETRELATEDWD\n");
			HM_pass_ack_data(cmddata,data,data_len);
			temp_zb_index=(data[2]<<8)+data[3];
			pni_temp = findInDevListByIndex(temp_zb_index);
			if((pni_temp == NULL) || (NODE_INFO_TYPE_WARNING_DEV != pni_temp->devType))//not find dev from index
			{
				printf("the control dev was not valid\r\n");
				break;
			}
			if(userCreatDevLink_WdFunc(&data[4],pni_temp))
			{
				printf("APP SET RELATED SUCCESS\r\n");
			}
			break;
		case GETRELATEDWD_CMD:
			printf("pass_data_process GETRELATEDWD\n");
			HM_pass_ack_data(cmddata,data,data_len);
			temp_zb_index=(data[2]<<8)+data[3];
			App_get_relatedwd(temp_zb_index);
			break;
		case SETNIGHTLINKAGE_CMD:
			printf("pass_data_process SETNIGHTLINKAGE\n");
			HM_pass_ack_data(cmddata,data,data_len);
			App_set_nightlightlinkage(&data[2]);
			break;
		case GETNIGHTLINKAGE_CMD:
			printf("pass_data_process GETNIGHTLINKAGE\n");
			HM_pass_ack_data(cmddata,data,data_len);
			App_get_nightlightlinkage();
			break;
		case SETARMTIMER_CMD:
			printf("pass_data_process SETARMTIMER\n");
			HM_pass_ack_data(cmddata,data,data_len);
			App_set_armtimer(&data[2]);
			break;
		case GETARMTIMER_CMD:
			printf("pass_data_process GETARMTIMER\n");
			HM_pass_ack_data(cmddata,data,data_len);
			App_get_armtimer();
			break;
		default:
		break;

	}
	

}


void respond_otaack_pointdata(char result)
{
	uint8_t buff[512];
	uint8_t temp_data;
	uint8_t *ptr=NULL;
	uint16_t rc=0x0001,bufflen,ptrlen=0;
	uint16_t index,tempdata,product_devType;
	uint32_t convert_intdata,convertdata;
	uint8_t encrypt_mode=NO_ENCRYPT;
	uint8_t actionvalue;
	
	#ifdef ENCRYPT_ENABLE
	encrypt_mode=PRIVATE_ENCRYPT;
	#endif
	memset(buff,0,sizeof(buff));
	ptr = mem_alloc(512);ptrlen=0;
	temp_data=DEVICEID_KEY;
	memcpy(ptr,&temp_data,1);ptrlen+=1;
	Int_biglittle_Convert(network_config_call.server_deviceid,&convert_intdata);
	memcpy(ptr+ptrlen,&convert_intdata,4);
	ptrlen+=4;

	temp_data=OTA_ID_KEY;
	memcpy(ptr+ptrlen,&temp_data,1);
	ptrlen+=1;
	memcpy(ptr+ptrlen,&gwStateForRealTime.ota_id,1);
	ptrlen+=1;

	temp_data=RESULT_KEY;
	memcpy(ptr+ptrlen,&temp_data,1);
	ptrlen+=1;
	memcpy(ptr+ptrlen,&result,1);
	ptrlen+=1;
	
	bufflen=combined_point_datapacket(ACTION,OTA_ACK,encrypt_mode,POINT,buff,ptr,ptrlen);
	HM_Sent_point_data(buff,bufflen);
	mem_free(ptr);
}

void respond_otaack_passdata(char* data,char result)
{
	uint8_t *ptr=NULL;
	uint8_t parm[5],i;
	uint16_t enable_flag,movedata=0x0001,rc=0x0000,enable_data=0x00ff;
	uint8_t needSaveFlashFlag = 0;
	uint8_t sent_buff[256],sent_buff_len,ptrlen;
	uint8_t alarmlevel=0x40;
	uint32_t convertdata;

	if(result==SUCCESSS_RESULT)
		rc=0x0001;
	else if(result==FAIL_RESULT)
		rc=0x0005;
	memset(parm,0,sizeof(parm));
	memcpy(parm,data,4);
	parm[1]=0x32;
	ptr = mem_alloc(512);
	big_little_exchange(rc,&convertdata);
	memcpy(ptr,&convertdata,2);
	ptrlen+=2;
	//parm[0] = OTA_MODE_ZC;
	//parm[1] = HM_IOTS_SMALL_GW_TYPE & 0xff;
	//parm[2] = (HM_IOTS_SMALL_GW_TYPE >> 8) & 0xff;
	//parm[3] = 0;
	memcpy(ptr+ptrlen,&parm[0],2);
	ptrlen+=2;
	memcpy(ptr+ptrlen,&parm[3],1);
	ptrlen+=1;//status
	sent_buff_len=combined_pass_datapacket(INFORMATIONTYPE,DEVICEOTA_CMD,sent_buff,ptr,ptrlen);
	HM_Sent_pass_data(sent_buff,sent_buff_len);
	mem_free(ptr);
}
void respond_aeskey_fail(void)
{
	uint8_t buff[512];
	uint8_t *ptr=NULL;
	uint16_t rc=0x0004,bufflen,ptrlen=0;
	uint32_t convertdata;
	uint32_t key_buff[2]={0x12345665,0},keybuff1=0x12341234;
	uint8_t  keybuff[16]={0x12,0x34,0x56,0x65},tempdata;//hmIotsDevInfo.aesKey
	ptr = mem_alloc(512);
	big_little_exchange(rc,&convertdata);
	memcpy(ptr,&convertdata,2);
	ptrlen+=2;
	//memcpy(ptr+ptrlen,key_buff[0],strlen(key_buff[0]));//keybuff
//	memcpy(ptr+ptrlen,keybuff,strlen(keybuff));
//	ptrlen+=strlen(keybuff);//hmIotsDevInfo.aesKey

	//memcpy(ptr+ptrlen,hmIotsDevInfo.aesKey,strlen(hmIotsDevInfo.aesKey));
	//ptrlen+=strlen(hmIotsDevInfo.aesKey);
	tempdata=1;
	memcpy(ptr+ptrlen,&tempdata,1);
	ptrlen+=1;
	//printf("--------hmIotsDevInfo.aesKey=%s \n",hmIotsDevInfo.aesKey);
	bufflen=combined_pass_datapacket(INFORMATIONTYPE,GETAESKEY_CMD,buff,ptr,ptrlen);
	mem_free(ptr);
	HM_Sent_pass_data(buff,bufflen);
}


void pointdata_decode(char* data,int length)
{
	uint32_t r_index=0,temp_length;
	uint32_t temp_data;
	uint8_t temp_buff[128]={0},cmdvalue,temp_cdata;
	uint8_t temp_FileUrl[128]={0},data_FileUrl[128]={0},temp_port[10]={0},i,j,temp_FileUrlLen,temp_portLen;
	uint16_t len;
	printf("pointdata_decode ENTER ---\n");
	temp_length=length;
	do{
		cmdvalue=data[r_index];
		printf("switch(cmdvalue):%02x ---\n",cmdvalue);
		switch(cmdvalue)
		{
			case URL_KEY:// nbyte
				printf("URL_KEY 1-16 0001---\n");
				r_index++;
				len=(data[r_index]<<8)+data[r_index+1];
				r_index+=2;
				if(gwStateForRealTime.ota_type==WFFIRMWARE_TYPE){
				//memset(gwStateForRealTime.wifiSoftUpdate.softUrl,0,MAX_WOFTWARE_ADDR_LEN);
				//memcpy(&gwStateForRealTime.wifiSoftUpdate.softUrl[0],"119.29.224.84/firmware/9a181de56d53420a84764badb2f8059b.bin",strlen("119.29.224.84/firmware/9a181de56d53420a84764badb2f8059b.bin"));
				//http://47.88.192.21/firmwareFile/9a181de56d53420a84764badb2f8059b.bin

				memset(temp_FileUrl,0,sizeof(temp_FileUrl));
				memcpy(temp_FileUrl,&data[r_index],len);
				if(strstr(temp_FileUrl,":"!=NULL))
				{
						temp_FileUrlLen=strlen(temp_FileUrl);
						for(i=0;i<temp_FileUrlLen;i++)
						{
						 	if(temp_FileUrl[i]==':')
								break;
						}
						temp_portLen=0;
						for(j=i;j<temp_FileUrlLen;j++)
						{
							if(temp_FileUrl[j]=='/')
								break;
							temp_portLen++;
						}
						memcpy(temp_port,temp_FileUrl[i],temp_portLen);
						OTA_HTTPSERVER_PORT=atoi(temp_port);
						memcpy(data_FileUrl,temp_FileUrl,i+1);
						memcpy(data_FileUrl+strlen(data_FileUrl),temp_FileUrl[j],strlen(temp_FileUrl)-j);
						printf("the new zigbee soft new data_FileUrl addr:%s,newlen:%d\r\n",data_FileUrl,strlen(data_FileUrl));
						memset(gwStateForRealTime.wifiSoftUpdate.softUrl[0],0,MAX_WOFTWARE_ADDR_LEN);
						memcpy(&gwStateForRealTime.wifiSoftUpdate.softUrl[0],data_FileUrl,strlen(data_FileUrl));
				}

				gwStateForRealTime.wifiSoftUpdate.haveNewestSoftWare = 1;
				printf("the new wifi soft addr:%s\r\n",gwStateForRealTime.wifiSoftUpdate.softUrl);
				r_index+=len;
				gwStateForRealTime.wifiSoftUpdate.appAllowSoftWareUpdate = 1;//del
				}
				else if(gwStateForRealTime.ota_type==ZBFIRMWARE_TYPE){
					gwStateForRealTime.zigbeeZcBasicInfo.appAllowSoftWareUpdate = 1;//subOtaRam.subFileUrl					
					memset(temp_FileUrl,0,sizeof(temp_FileUrl));
					memcpy(temp_FileUrl,&data[r_index],len);
					//printf("the new zigbee soft old addr:%s,old len:%d\r\n",temp_FileUrl,len);
					//temp_cdata=0x32;
					//memcpy(&temp_FileUrl[len],&temp_cdata,1);
					//printf("the new zigbee soft new addr:%s,newlen:%d\r\n",temp_FileUrl,len);					
					//memset(subOtaRam.subFileUrl,0,MAX_WOFTWARE_ADDR_LEN);
					//memcpy(&subOtaRam.subFileUrl,"http://47.88.192.21/firmwareFile/HaSampleGatewaySoc_v1.4.0.ebl",strlen("http://47.88.192.21/firmwareFile/HaSampleGatewaySoc_v1.4.0.ebl"));

					if(strstr(temp_FileUrl,":"!=NULL))
					{
						temp_FileUrlLen=strlen(temp_FileUrl);
						for(i=0;i<temp_FileUrlLen;i++)
						{
						 	if(temp_FileUrl[i]==':')
								break;
						}
						temp_portLen=0;
						for(j=i;j<temp_FileUrlLen;j++)
						{
							if(temp_FileUrl[j]=='/')
								break;
							temp_portLen++;
						}
						memcpy(temp_port,temp_FileUrl[i],temp_portLen);
						OTA_HTTPSERVER_PORT=atoi(temp_port);
						memcpy(data_FileUrl,temp_FileUrl,i+1);
						memcpy(data_FileUrl+strlen(data_FileUrl),temp_FileUrl[j],strlen(temp_FileUrl)-j);
						printf("the new zigbee soft new data_FileUrl addr:%s,newlen:%d\r\n",data_FileUrl,strlen(data_FileUrl));
						memset(subOtaRam.subFileUrl,0,MAX_WOFTWARE_ADDR_LEN);
						memcpy(subOtaRam.subFileUrl,data_FileUrl,strlen(data_FileUrl));
					}
					
					
					//api.wnhoo.com:8555/app/ltm3210_lt_sample_ota.bin
					//memcpy(&subOtaRam.subFileUrl,"http://api.wnhoo.com/app/HaSampleGatewaySoc_v1.4.0.ebl",strlen("http://api.wnhoo.com/app/HaSampleGatewaySoc_v1.4.0.ebl"));

					//http://120.24.225.120/app/zi.ebl http://io.heimantech.com/firmware/zi.ebl http://47.88.192.21//firmware/zi.ebl
					//119.29.224.84/firmware/zi.ebl
					//memcpy(&subOtaRam.subFileUrl,"http://47.88.192.21/firmware/zi.ebl",strlen("http://47.88.192.21/firmware/zi.ebl"));
					
					//memcpy(&subOtaRam.subFileUrl,"http://hmqq.free.ngrok.cc/firmwareFile/zi.ebl",strlen("http://hmqq.free.ngrok.cc/firmwareFile/zi.ebl"));
					//http://47.88.192.21/firmwareFile/zi.ebl
					//memcpy(&subOtaRam.subFileUrl,temp_FileUrl,strlen(temp_FileUrl));
					
					//memcpy(&subOtaRam.subFileUrl,&data[r_index],len);
					//add---
					//temp_cdata=0x32;
					//memcpy(&subOtaRam.subFileUrl[len],&temp_cdata,1);
					//subOtaRam.subFileUrl[len]=0x32;
					//subOtaRam.subFileUrl[len+1]=0x32;
					//subOtaRam.subFileUrl[len+2]=0;
					//
					printf("the new zigbee soft addr:%s,len:%d\r\n",subOtaRam.subFileUrl,len);
					r_index+=len;
				}
				else{
					printf("the info ota type error:%d\n",gwStateForRealTime.ota_type);
					
					}
				break;
			case SIZE_KEY://int
				r_index++;
				temp_data=(data[r_index]<<24)+(data[r_index+1]<<16)+(data[r_index+2]<<8)+data[r_index+3];
				r_index+=4;
				printf("pointdata_decode SIZE_KEY:%x\r\n",temp_data);
				if(gwStateForRealTime.ota_type==ZBFIRMWARE_TYPE)
				{subOtaRam.sufFileSize=temp_data;
				if(subOtaRam.sufFileSize < MAX_SUB_FIRMWARE_SIZE && subOtaRam.sufFileSize!=0)
				{
					subOtaRam.subOtaStart = 1;
					printf("the info ota subOtaRam.subOtaStart \n");
				}
				}
				break;
			case MD5_KEY://nbyte
				r_index++;
				len=(data[r_index]<<8)+data[r_index+1];
				r_index+=2;
				memset(temp_buff,0,sizeof(temp_buff));
				memcpy(temp_buff,&data[r_index],len);
				r_index+=len;
				if(gwStateForRealTime.ota_type==ZBFIRMWARE_TYPE)
					{
						//subOtaRam.subFileMd5=
						memcpy(subOtaRam.subFileMd5,temp_buff,len);
					}
					
				printf("pointdata_decode MD5_KEY:%s\r\n",temp_buff);
				break;
			case SOFT_VERSION://short
				r_index++;
				temp_data=(data[r_index]<<8)+data[r_index+1];
				r_index+=2;
				printf("pointdata_decode SOFT_VERSION:%x\r\n",temp_data);
				temp_cdata=temp_data;
				if(gwStateForRealTime.ota_type==ZBFIRMWARE_TYPE)
				subOtaRam.subFileVersion=0x24;//temp_cdata;
				break;
			case OTA_ID_KEY://int
				r_index++;
				temp_data=(data[r_index]<<24)+(data[r_index+1]<<16)+(data[r_index+2]<<8)+data[r_index+3];
				gwStateForRealTime.ota_id=temp_data;
				r_index+=4;
				printf("pointdata_decode OTA_ID_KEY:%x\r\n",temp_data);
				break;
			case FIRMWARE_TYPE:// 1byte
				r_index++;
				temp_data=data[r_index];
				gwStateForRealTime.ota_type=temp_data;
				r_index+=1;
				printf("pointdata_decode FIRMWARE_TYPE:%x\r\n",temp_data);
				break;
			case SUB_INDEX_KEY:// short
				r_index++;
				temp_data=(data[r_index]<<8)+data[r_index+1];
				r_index+=2;
				printf("pointdata_decode FIRMWARE_TYPE:%x\r\n",temp_data);
				break;
			default:
				r_index++;
			break;
		}
		
		temp_length=length-r_index;
		if(temp_length==0)
			break;
		}while(1);//temp_length
		if(temp_length==0)  
			printf("pointdata_decode success\n");
}
void read_pointdata(char* indata,int length,char* outdata)
{
	while(length--)
	{
		*outdata++=*indata++;
	}
}

void Point_data_handle_old(char* data, int length)
{
   char recv_buff[1024]={0};
   uint8_t execute_typedata;
   uint32_t systemtime_data,serverdeviceid;
   uint8_t key[40];
   uint16_t i,recvbuff_len,point_len;
   char encrypt_mode,read_data,pointvalue_index;
   int read_intdata,ReadHdata,ReadLdata,DataLength,PointLength,LogidData;
   char *curdata=data;
   uint8_t sceneId;

   encrypt_mode=data[3];
   if(encrypt_mode==NO_ENCRYPT){
   		memcpy(recv_buff,data,length);
		Loginfo("Remoteserver Point data recv_buff:\n");
	   for(i=0;i<length;i++)
	   	{
	   	 Loginfo("%02x ",recv_buff[i]);
	   	}
	    Loginfo("\n");
   	}else if(encrypt_mode==PULIC_ENCRYPT)
   	{
   		memcpy(key,GW_PUBPLICKEY,strlen(GW_PUBPLICKEY)); 	
   	}else if(encrypt_mode==PRIVATE_ENCRYPT)
   	{
   		memcpy(key,hmIotsDevInfo.aesKey,strlen(hmIotsDevInfo.aesKey)); 
   	}
	if(encrypt_mode!=NO_ENCRYPT)
	{//4
		pointvalue_index=0;
		for(i-=0;i<10;i++)
		{
			//read_pointdata(&data[0],1,&read_data);//
			read_data=readChar(&curdata);
			pointvalue_index++;
			switch(read_data)
			{
				case ACTION:
					//read_pointdata(&data[0],1,&execute_typedata);
					execute_typedata=readChar(&curdata);
					pointvalue_index++;
					Loginfo("ACTION:%d\n",execute_typedata);
					break;
				case ENCRYPT_TYPE:
					//read_pointdata(&data[0],1,&encrypt_mode);
					encrypt_mode=readChar(&curdata);
					pointvalue_index++;
					Loginfo("ENCRYPT_TYPE:%d\n",encrypt_mode);
					break;
				case LOGID:
					//read_pointdata(&data);
					ReadHdata=readInt(&curdata);
					ReadLdata=readInt(&curdata);
					read_intdata=(ReadHdata<<16)+ReadLdata;
					LogidData=read_intdata;
					pointvalue_index+=4;
					Loginfo("LOGID:%d,hex:%x\n",read_intdata,read_intdata);
					break;
				case POINT:
					DataLength=readInt(&curdata);
					pointvalue_index+=2;
					Loginfo("POINT:%d,hex:%x----%x\n",DataLength,DataLength,pointvalue_index);
					break;
				default:
				break;
			}
			if(read_data==POINT)
			{
				Loginfo("test read_data==POINT,pointvalue_index:%d \n",pointvalue_index);
				break;
			}
			Loginfo("test loop read read_pointdata i:%d\n",i);
		}
		//recvbuff_len=user_aes128_decode(&data[7],length-7,&recv_buff[7],key);
		//memcpy(recv_buff,data,7);
		//recvbuff_len+=7;
		recvbuff_len=user_aes128_decode(&data[pointvalue_index],length-pointvalue_index,&recv_buff[pointvalue_index],key);
		if(recvbuff_len == 0)
		{
			Loginfo("point decode error----\n");
			return;
		}
		//change data len
		PointLength=recvbuff_len;
		//data[pointvalue_index-2]=(recvbuff_len>>8)&0xff;
		//data[pointvalue_index-1]=recvbuff_len&0xff;
		memcpy(recv_buff,data,pointvalue_index);
		recv_buff[pointvalue_index-2]=(recvbuff_len>>8)&0xff;
		recv_buff[pointvalue_index-1]=recvbuff_len&0xff;
		recvbuff_len+=pointvalue_index;
		Loginfo("Remoteserver Point data recvbuff_len:%d;recv_buff:\n",recvbuff_len);
	   for(i=0;i<recvbuff_len;i++)
	   	{
	   	 Loginfo("%02x ",recv_buff[i]);
	   	}
	    Loginfo("\n");
	}
   
   execute_typedata=recv_buff[1];
   switch(execute_typedata)
   {
   		
   		case CONTROL:
			break;
		
		case GETDEVICEID_ACK:
			if(recv_buff[4]==POINT)
			{
				if(recv_buff[6]!=5)//len
					Loginfo("get serverdeviceid len error\n");
				if(recv_buff[7]==DEVICEID_KEY)
				//memcpy(&serverdeviceid,&recv_buff[8],4);
				serverdeviceid=(recv_buff[8]<<24)+(recv_buff[9]<<16)+(recv_buff[10]<<8)+recv_buff[11];
				Loginfo("-------------get 11 serverdeviceid:%x,decimal:%d\n",serverdeviceid,serverdeviceid);
				network_config_call.server_deviceid=serverdeviceid;
				HM_sent_point_aeskeymsg();//sent_point_aeskey();
				hm_sent_point_mesgdata();//sent_GWMsg_pointdata();

				Loginfo("-------------the app clear_zigbeenet_flag:%d\n",clear_zigbeenet_flag);
				if(clear_zigbeenet_flag){
				HM_sent_zigbeelist_clearmsg(DEL_ZIGBEE_MODE,1);;//sent_zigbeelist_clearmsg(DEL_ZIGBEE_MODE,1);
				Loginfo("------------clearzigbee net msg\n");
				clear_zigbeenet_flag=0;
				}

				//add test
				//test_save_hmIotsDevInfo();
			}
			break;
		case COERCION_OTA:
			point_len=(recv_buff[5]<<8)+recv_buff[6];
			//pointdata_decode(&recv_buff[7],point_len);  ///---test qudiao
			break;
		case SCENE:
			//sent_point_sceneack(data,length);
			sent_point_sceneack_new(&recv_buff[pointvalue_index],recvbuff_len-PointLength,LogidData);
			sceneId = user_addscene_func(&recv_buff[pointvalue_index],INVALID_SCENE_LINKAGE_INDEX);
			//userAddSceneFunc(&recv_buff[pointvalue_index],INVALID_SCENE_LINKAGE_INDEX);
			//sent_point_sceneack(&recv_buff,recvbuff_len);//sent_point_sceneack(&recv_buff[pointvalue_index],PointLength);
			break;
			default:
			break;
   }
}
















