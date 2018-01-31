
#include "qcom_common.h"
#include "MQTTinterface.h"

#include "../lt_sample/ltm3210.h"


//#include "StackTrace.h"
#include "MQTTPacket.h"
//#include "MQTTPublish.h"
//#include "MQTTSubscribe.h"
//#include "MQTTUnsubscribe.h"
//#include "MQTTFormat.h"

//#include "MQTTConnect.h"
#include "ServerDataHandle.h"
#include "hm_halBeep.h"

#include "hm_Head_adaptation.h"



#define TICKS_DIFFER(cur, prev) ((cur) >= (prev)) ? ((cur)-(prev)) : ((0xFFFFFFFF-(prev))+(cur)+1)
#define qca_timems()				(time_ms()*1000/1024)

#define MQTT_SEND_CONNECT_INTERVAL 		50 
#define MQTT_SEND_SUBSCIBE_INTERVAL		50
#define MQTT_SEND_GETDEVICEID_INTERVAL	6   


static qcom_timer_t MQTTheartbeatTimer;
TX_SEMAPHORE mqtt_semaphore_ptr;
MQTTINFO MQTT_info;
MQTTPacket_connectData MQTT_connect_parm;//add
MQTT_Client MQTTclient_init;
MQTTString topicString;
MQTTString topicString_pass;


char TCPSocket_connectd=-1;
MQTT_Client* MQTTclient;//MQTT_Client* MQTTclient;
static MQTTTopic mqtt_topic = MQTT_NULL;
uint8_t mqtt_recvdata_packet[1024]={0};
uint32_t mqtt_recvdata_index;
static uint32_t mqtt_recvdata_rxHead=0,mqtt_recvdata_rxTail=0;
#define MQTTDATA_PACKET_SIZE 1024
#define MQTTDATA_PACKET_MASK (MQTTDATA_PACKET_SIZE-1)

int CurrentTime,PreviouTime;
int MqttSend_connect_TimeCounts,MqttSend_subscibe_TimeCounts,MqttSend_GetDeviceid_TimeCounts;
uint16_t pingack_count=0,ping_count=0,received_ping_ack=0;

MQTTString SubsTopic_pass= MQTTString_initializer;
MQTTString SubsTopic_point= MQTTString_initializer;
MQTTString SubsTopic_Mfunc= MQTTString_initializer;

MQTTString PublishTopic_pass= MQTTString_initializer;
MQTTString PublishTopic_point= MQTTString_initializer;
MQTTString PublishTopic_list= MQTTString_initializer;

uint16_t msgid = 1;

extern int tcp_fd;
extern NETWORK_USER_CONFIG  network_config_call;

extern uint8_t zbSysStartFlag;
//uint8_t will_topic_buff1[128]={0},will_topic_buff2[128]={0},clientIDbuff[128];//old mqtt connect parm//
uint8_t willtopicbuff[32];
uint8_t usernamebuff[20];
uint8_t passwordbuff[20];
uint8_t clientIDbuff[32];

static uint8_t pingreq_workflag=0;







int getdata(unsigned char* buf, int count)
{
	uint16_t read_counts=0;
    //return mysock.receive(buf, (size_t)count);
    if(mqtt_recvdata_rxHead==mqtt_recvdata_rxTail)
		return 0;
	//Loginfo("getdata mqtt_recvdata_rxHead:%x;mqtt_recvdata_rxTail:%x----\n",mqtt_recvdata_rxHead,mqtt_recvdata_rxTail);
	if(count==1)
	{
		*buf=mqtt_recvdata_packet[mqtt_recvdata_rxTail];
		mqtt_recvdata_rxTail = (mqtt_recvdata_rxTail+ 1) & MQTTDATA_PACKET_MASK;
		Loginfo("getdata 1----\r\n");
	}
	else
	{
		while (count--)
		{
			*buf++ = mqtt_recvdata_packet[mqtt_recvdata_rxTail];
			mqtt_recvdata_rxTail = (mqtt_recvdata_rxTail+ 1) & MQTTDATA_PACKET_MASK;
			read_counts++;
		}
		Loginfo("getdata n counts----\r\n");
		return read_counts;
	}
    return 1;
}
void mqtt_recvdata_write(char data)
{
	mqtt_recvdata_packet[mqtt_recvdata_rxHead]= data;  // load value
//	Loginfo("mqtt_recvdata_write -rxHead:%d\r\n",mqtt_recvdata_rxHead);
	mqtt_recvdata_rxHead = (mqtt_recvdata_rxHead + 1) & MQTTDATA_PACKET_MASK;
//	Loginfo("mqtt_recvdata_write -rxHead:%d\r\n",mqtt_recvdata_rxHead);
//	Loginfo("mqtt_recvdata_write ----%d,rxHead:%x,rxTail:%x\r\n",data,mqtt_recvdata_rxHead,mqtt_recvdata_rxTail);

}
void MQTT_clear_pingcount(void)
{
	MQTT_info.ping_count=0;
}

void mqtt_mesgid_increase(uint16_t* mesg_id)
{
	*mesg_id=*mesg_id+1;
	if(*mesg_id==0) *mesg_id=1;
	MQTT_clear_pingcount();
}

#if 1
void TCPsetServerStatus(unsigned char stat)
{
	TCPSocket_connectd=stat;
	if(TCPSocket_connectd==0)
		clear_mqtt_connected();
}
#endif
int hmlink_tcp_PushData(unsigned char * data, short datalen){
	//memcpy(mqtt_recvdata_packet,data,datalen);
	//mqtt_recvdata_rxHead=datalen;
	while(datalen--){
		mqtt_recvdata_write(*data++);
	}
	
}
int mqtt_pingreq_flag=0;

 void MQTTheartbeat_timerS_handle(unsigned int alarm, void *arg)
{

	if(MQTT_info.pingtoggle==1){
		if((++MQTT_info.pingack_count)%10==0)//10
		{
			if(!MQTT_info.received_ping_ack)
			{
				//MQTTclient->connetedStatus=mqtt_cs_disconnecting;
				//MQTTclient->connState=MQTT_CONNECT_SEND;
				Loginfo("mqtt_cs_disconnecting---- \r\n");
			}
			else
			{
				MQTT_info.pingack_count=0;
				Loginfo("rceive ping ack---- \r\n");
			}
			MQTT_info.pingtoggle=0;
		}
		
	}
	if((++MQTT_info.ping_count)%20==0){//50
		MQTT_clear_pingcount();
		if(MQTT_is_connected())
		{
			MQTT_info.pingreq_work_flag=1;pingreq_workflag=1;
			MQTT_info.pingtoggle=1;
			MQTT_info.received_ping_ack=0;
			//tx_semaphore_put(&mqtt_semaphore_ptr);
		//	send_buff[0]=0xc0;send_buff[1]=0x00;
		//	qcom_send(tcp_fd, (char *)send_buff, 2, 0);
			Loginfo("MQTTheartbeat timerS handle---- succed\r\n");
		}
		MQTT_clear_pingcount();
	}
	if((++MQTT_info.gettime_count)%60==0){//60
		MQTT_info.gettime_count=0;
		MQTT_info.gettime_work_flag=1;
	}
}

 void set_mqtt_connected(void)
 {
 	MQTTclient->connetedStatus = mqtt_cs_connected;
 }
 void clear_mqtt_connected(void)
 {
 	MQTTclient->connetedStatus = mqtt_cs_disconnecting;
 }
 int MQTT_is_connected(void) {
	 int ret = 0;
	 if (MQTTclient->connetedStatus != mqtt_cs_connected) {
		 ret = 0;
	 } else {
		 ret = 1;
	 }
	 return ret;
 }

 void MQTTclient_connState_init(void)
{
	MQTTclient->connState=MQTT_CONNECT_SEND;
}
 /*void Point_data_handle(char* data, int length)
 {
 	char recv_buff[1024]={0};
 	memcpy(recv_buff,data,length);
	Loginfo("Remoteserver_data_handle recv_buff=%s\n",recv_buff);
 }
 void Pass_data_handle(char* data, int length)
 {
 	char recv_buff[1024]={0};
 	memcpy(recv_buff,data,length);
	Loginfo("Remoteserver_data_handle recv_buff=%s\n",recv_buff);
 }*/



void Sent_pass_data(char* data, int length)
{
	uint8_t topic1_buff[64]={0};
	uint8_t buf[1024];
	int buflen = sizeof(buf);
	uint32_t len,i;
	PublishTopic_pass.cstring="0";
	memset(topic1_buff,0,sizeof(topic1_buff));
	sprintf(topic1_buff,"%s/%s/%s/C",network_config_call.factory_id,network_config_call.product_id,network_config_call.mac);	
	memcpy(PublishTopic_pass.cstring,topic1_buff,strlen(topic1_buff)); 				
	len = MQTTSerialize_publish(buf, buflen, 0, ReqQos, 0, msgid, PublishTopic_pass, data,length);//payload, payloadlen
	if(len)
	{
		qcom_send(tcp_fd, (char *)buf, len, 0);
		mqtt_mesgid_increase(&msgid);
	}

	Loginfo("\nSent_pass_data hex data:\n");
	for(i=0;i<len;i++)
	{
		Loginfo("%02x ",buf[i]);
	}
	
}

void Sent_list_data(char* data,int length)//unsigned char* payload, int payloadlen
{

	uint8_t topic1_buff[64]={0};
	uint8_t buf[1024];
	int buflen = sizeof(buf);
	uint32_t len,i;
	PublishTopic_pass.cstring="0";
	memset(topic1_buff,0,sizeof(topic1_buff));
	sprintf(topic1_buff,"%s/%s/%s/A",network_config_call.factory_id,network_config_call.product_id,network_config_call.mac);	
	memcpy(PublishTopic_pass.cstring,topic1_buff,strlen(topic1_buff)); 				
	len = MQTTSerialize_publish(buf, buflen, 0, ReqQos, 0, msgid, PublishTopic_pass, data,length);//payload, payloadlen
	if(len)
	{
		qcom_send(tcp_fd, (char *)buf, len, 0);
		mqtt_mesgid_increase(&msgid);
	}

	Loginfo("\n Sent_list_data hex data:");
	for(i=0;i<len;i++)
	{
		Loginfo("%02x ",buf[i]);
	}
	Loginfo("\n");
	
}
uint8_t serverDeviceid=0;
void MQTT_serverDeviceid_confirm(void)
{
	if(serverDeviceid)
		return;
	CurrentTime = qca_timems()/1000;
	MqttSend_GetDeviceid_TimeCounts = TICKS_DIFFER(CurrentTime,PreviouTime);
	if (MqttSend_subscibe_TimeCounts > MQTT_SEND_GETDEVICEID_INTERVAL){	
		Loginfo("SEND GET serverDeviceid----%d\r\n");
		HM_GW_get_ServerDeviceId();//GW_get_ServerDeviceId();
		PreviouTime = qca_timems()/1000;
	}
	//get_megtype=MQTTPacket_read(buf, buflen, getdata);
	//if (get_megtype == PUBLISH)
	
}

void MQTT_infoparm_init(void)
{
	MQTT_info.pingreq_work_flag=0;
	MQTT_info.pingtoggle=0;
	MQTT_info.pingack_count=0;
	MQTT_info.received_ping_ack=0;
	MQTT_info.ping_count=0;
	MQTT_info.gettime_work_flag=0;
	MQTT_info.gettime_count=0;
}

void mqtt_disconnect(void)
{
 uint8_t sentdata[50]={0},sendata_len;
	sendata_len=MQTTSerialize_disconnect(sentdata, sizeof(sentdata));
	if((sendata_len) && (MQTT_is_connected())){
		qcom_send(tcp_fd, (char *)sentdata, sendata_len, 0);
	}
}

void MQTT_main(void)
{
	int rc = 0;
    char buf[1024];
    int buflen = sizeof(buf);

    int req_qos = 1;//0
    char* payload = "mypayload";
    int payloadlen = strlen(payload);
    int len = 0;
	int get_megtype;
  	unsigned char mqtt_flag=0;
	uint8_t recv_publish_buffer[1024];
	uint8_t topic_buff[128]={0},topic1_buff[64]={0},topic2_buff[64]={0};
	uint8_t tempbuff[64]={0},temp_data,i;
	
	int connack_rc,session_data,ret_connack;	

	hm_information_init();
	qcom_thread_msleep(500);
	MQTTclient=&MQTTclient_init;
	MQTTclient->connState=MQTT_CONNECT_SEND;
	MQTTclient->connetedStatus=mqtt_cs_null;

//	topicString = (MQTTString)MQTTString_initializer;
//	topicString_pass = (MQTTString)MQTTString_initializer;

	//MQTT_connectparm_init();  old
	

	//MQTT_SubscibeTopic_init();
	
	/* subscribe */
	/*
	topicString.cstring = "01";
	topicString_pass.cstring="02";
	sprintf(topic_buff,"%s/%s/%s/D",network_config_call.factory_id,network_config_call.product_id,network_config_call.mac);
	memcpy(topicString.cstring,topic_buff,strlen(topic_buff)+1);
	Loginfo("topic_buff-1:%s\r\n",topicString.cstring); 
	sprintf(topic1_buff,"%s/%s/%s/E",network_config_call.factory_id,network_config_call.product_id,network_config_call.mac);	
	memcpy(topicString_pass.cstring,topic1_buff,strlen(topic1_buff)+1);
	sprintf(topic2_buff,"%s/%s/%s/F",network_config_call.factory_id,network_config_call.product_id,network_config_call.mac);	
	*/
	
	qcom_timer_init(&MQTTheartbeatTimer, MQTTheartbeat_timerS_handle, NULL, 1000, PERIODIC);
	qcom_timer_start(&MQTTheartbeatTimer);

	MQTT_infoparm_init();
    while(1)
	{
		qcom_thread_msleep(50);
		//if(tx_semaphore_get(&mqtt_semaphore_ptr,TX_NO_WAIT) == 0)
		{
			if((MQTT_info.pingreq_work_flag==1) && (pingreq_workflag==1))
			{
				mqtt_pingreq_flag=0;
			 	MQTT_info.pingreq_work_flag=0;
				memset(buf,0,sizeof(buf));
			  	len=MQTTSerialize_pingreq(buf, buflen);
			  	if(len)
				{
					if(MQTT_is_connected())
					qcom_send(tcp_fd, (char *)buf, len, 0);
					Loginfo("mqtt pingreq------\n");
			  	}
				MQTT_info.pingreq_work_flag=0;pingreq_workflag=0;
			}
			 if(MQTT_info.gettime_work_flag==1)
			{
				MQTT_info.gettime_work_flag=0;
				if(MQTT_is_connected())
				{
					HM_get_systemtime();//get_systemtime();
				}
			}
		}

		#if 1
 	switch(MQTTclient->connState)
 	{
 		case CONNSTATE_NULL:
			break;
 		case MQTT_CONNECT_SEND:
			if(TCPSocket_connectd!=1)
				break;
			if(mqtt_initconnect_func()==1)
			{
				Loginfo("mqtt init connect succeed\r\n");
				MQTTclient->connState=MQTT_SUBSCIBE_SEND;
				MQTTclient->connetedStatus=mqtt_cs_connected;
				set_mqttserver_connected();
				userControlGWLedFunc(temp_data,0);//light
				zbSysStartFlag = 0;
			}
			break;
			/*
			MQTT_connect_parm.will.message.cstring = "3!";
			if(TCPSocket_connectd!=1)
				break;//return;
			memset(buf,0,sizeof(buf));
			len = MQTTSerialize_connect(buf, buflen, &MQTT_connect_parm);
			Loginfo("MQTT_CONNECT_SEND----0,%d,%d\r\n",buflen,len);
			if(len)
			{
				Loginfo("Sent MQTT_CONNECT_SEND hex data:");
				for(i=0;i<len;i++)
				{
					Loginfo("%02x ",buf[i]);
				}
				//send data to tcpserver 
				qcom_send(tcp_fd, (char *)buf, len, 0);//add
				MQTTclient->connState=MQTT_CONNACK;
			}
			PreviouTime = qca_timems()/1000;
			break;
		case MQTT_CONNACK:
			CurrentTime = qca_timems()/1000;
			MqttSend_connect_TimeCounts = TICKS_DIFFER(CurrentTime,PreviouTime);		
					
			 if (MQTTPacket_read(buf, buflen, getdata) == CONNACK)	// wait for CONNACK 
			 {	zbSysStartFlag = 0;//debugon
			 	
				Loginfo("buf, buflen--%x,%x,%x,%x\r\n",buf[0],buf[1],buf[2],buf[3]);
				Loginfo("mqtt connect CONNACK ?--\r\n");
				ret_connack=MQTTDeserialize_connack(&session_data,&connack_rc, buf, buflen);
				Loginfo("ret_connack=%x,connack_rc=%x --\r\n",ret_connack,connack_rc);
        		if(ret_connack != 1)//if (ret_connack != 1 || connack_rc != 0)
				{
			 		Loginfo("mqtt connect error\r\n");				
        		}else{
					MQTTclient->connState=MQTT_SUBSCIBE_SEND;
					mqtt_topic=MQTT_TOPIC_FIRST;// add 
					Loginfo("mqtt connect succeed\r\n");
					MQTTclient->connetedStatus=mqtt_cs_connected;
					msgid=1;//add
					set_mqttserver_connected();
					temp_data=0;
					userControlGWLedFunc(temp_data,0);//light
        		}
			 }
			 else{
			 	Loginfo("mqtt connect fail\r\n");
				if (MqttSend_connect_TimeCounts > MQTT_SEND_CONNECT_INTERVAL){	// timeout return again connect //
					Loginfo("Current  Time----%d\r\n",CurrentTime);
					Loginfo("Current  Time----%d\r\n",CurrentTime);
					MQTTclient->connState=MQTT_CONNECT_SEND;	
				}
			 }
			break;*/
		case MQTT_SUBSCIBE_SEND:
				if(mqtt_topicsubscibe_func()==1)
				{
					Loginfo("mqtt topic subscibe success\r\n");
					MQTTclient->connState=MQTT_DATA_HANDLE;
					HM_gw_report_online();
					HM_get_systemtime();//get_systemtime();
					HM_GW_get_ServerDeviceId();//GW_get_ServerDeviceId();
					user_start_dealthread();
					break;
				}
				

			/*if(mqtt_topic!=MQTT_NULL){
				switch((int) mqtt_topic)
				{
					case MQTT_TOPIC_FIRST:
						//mqtt_topic = MOSQ_SEND_TWICE;
						//memcpy(topicString.cstring,topic_buff,strlen(topic_buff)+1);
						SubsTopic_point.cstring="0";
						memcpy(SubsTopic_point.cstring,topic_buff,strlen(topic_buff)+1);
						len = MQTTSerialize_subscribe(buf, buflen, 0, msgid, 1, &SubsTopic_point, &req_qos);
						
						break;
					case MQTT_TOPIC_SECOND:
						//memcpy(topicString_pass.cstring,topic1_buff,strlen(topic1_buff)+1);
						//SubsTopic_pass
						SubsTopic_pass.cstring="0";
						memcpy(SubsTopic_pass.cstring,topic1_buff,strlen(topic1_buff)+1);
						len = MQTTSerialize_subscribe(buf, buflen, 0, msgid, 1, &SubsTopic_pass, &req_qos);
						break;
					case MQTT_TOPIC_THIRD:
						SubsTopic_Mfunc.cstring="0";
						memcpy(SubsTopic_Mfunc.cstring,topic2_buff,strlen(topic2_buff)+1);
						len = MQTTSerialize_subscribe(buf, buflen, 0, msgid, 1, &SubsTopic_Mfunc, &req_qos);
						break;
				}
			}
			//len = MQTTSerialize_subscribe(buf, buflen, 0, msgid, 1, &topicString, &req_qos);
			if(len)
			{
				qcom_send(tcp_fd, (char *)buf, len, 0);//add
				MQTTclient->connState=MQTT_SUBSACK;
				Loginfo(" 1 MQTT_TOPIC_FIRST msgid=%x,---\r\n",msgid);
				mqtt_mesgid_increase(&msgid);
				Loginfo("2 MQTT_TOPIC_FIRST msgid=%x,---\r\n",msgid);
			}
			PreviouTime = qca_timems()/1000;			
			break;
		case MQTT_SUBSACK:
			CurrentTime = qca_timems()/1000;
			MqttSend_subscibe_TimeCounts = TICKS_DIFFER(CurrentTime,PreviouTime);
		 	if (MQTTPacket_read(buf, buflen, getdata) == SUBACK)    // wait for suback //
    		{
    			int submsgid,subcount,granted_qos;

		        rc = MQTTDeserialize_suback(&submsgid, 1, &subcount, &granted_qos, buf, buflen);
		        if (granted_qos != 1)
		        {
		            Loginfo("error granted qos != 1, %d\n", granted_qos);
		           
		        }else{
					//Loginfo("mqtt subscibe succeed\r\n");
						switch((int) mqtt_topic)
					{
						case MQTT_TOPIC_FIRST:
							MQTTclient->connState=MQTT_SUBSCIBE_SEND;
							mqtt_topic = MQTT_TOPIC_SECOND;
							Loginfo("\n mqtt subscibe 1 succeed\r\n");
							break;
						case MQTT_TOPIC_SECOND:
							
							MQTTclient->connState=MQTT_SUBSCIBE_SEND;
							mqtt_topic = MQTT_TOPIC_THIRD;
							Loginfo("\n mqtt subscibe 2 succeed\r\n");
							break;
						case MQTT_TOPIC_THIRD:
							MQTTclient->connState=MQTT_DATA_HANDLE;
							mqtt_topic = MQTT_TOPIC_END;
							GW_report_online();
							
							get_systemtime();//add
							Loginfo("\n mqtt subscibe 3 succeed\r\n");
							PreviouTime = 0;
							GW_get_ServerDeviceId();
							user_start_dealthread();//add
							break;
					}
					//MQTTclient->connState=MQTT_DATA_HANDLE;
		        }
			
		 	}else{
				Loginfo("mqtt subscibe fail\r\n");
				if (MqttSend_subscibe_TimeCounts > MQTT_SEND_SUBSCIBE_INTERVAL){	// timeout return again subscibe //
					Loginfo("Current  Time----%d\r\n",CurrentTime);
					MQTTclient->connState=MQTT_SUBSCIBE_SEND;
				}
		 	}		
			break;*/
			
		case MQTT_DATA_HANDLE:

			MQTT_serverDeviceid_confirm();
			mqtt_loophandle_func();
			/*get_megtype=MQTTPacket_read(buf, buflen, getdata);
			 if (get_megtype == PUBLISH)
        	{
	            int dup,qos,retained,rv_msgid=0;//,msgid=0
	            int payloadlen_in;
	            char* payload_in;
	            int rc;
	            MQTTString receivedTopic;

	            rc = MQTTDeserialize_publish(&dup, &qos, &retained, &rv_msgid, &receivedTopic,
	                    &payload_in, &payloadlen_in, buf, buflen);
				*(payload_in+payloadlen_in)=0;

				memset(recv_publish_buffer,0,sizeof(recv_publish_buffer));
				memcpy(recv_publish_buffer,payload_in,payloadlen_in);
				Loginfo("receive message,topic:%s,payload:%s,len:%d,msgid:%x\n", receivedTopic.lenstring.data, recv_publish_buffer,payloadlen_in,rv_msgid);//recv_publish_buffer payload_in
       			Remoteserver_data_handle(receivedTopic.lenstring.data[receivedTopic.lenstring.len-1],recv_publish_buffer, payloadlen_in);
				Loginfo("receivedTopic.lenstring.len=%d\r\n",receivedTopic.lenstring.len);

				len=MQTTSerialize_puback(buf, buflen,rv_msgid);
				if(len)
				{
					qcom_send(tcp_fd, (char *)buf, len, 0);				
				}

				
				qcom_thread_msleep(10);
			//	GW_getDeviceId();

				//Loginfo("receive message,topic:%s,payload:%s,len:%d,msgid:%x\n", receivedTopic.lenstring.data, recv_publish_buffer,payloadlen_in,rv_msgid);//recv_publish_buffer payload_in
       		}
			 else if(get_megtype == PINGRESP)
			 {
				MQTT_info.received_ping_ack=1;
				Loginfo("mqtt recv heartbeat pingresp\r\n");
				//MQTTclient->connState=MQTT_CONNECT_SEND;
			 }
			 else if(get_megtype == DISCONNECT)
			 {
			 	Loginfo("mqtt server socket close\r\n");
			 }*/

			break;
			default:
			break;
		}
		//hm_get_user_times();
		#endif
		//break;
	}
 }
	




