#include "qcom_common.h"
#include "qca_json.h"
#include "hm_app.h"
#include "hm_zigbeeInfohand.h"
#include "qca_calendar.h"
#include "hm_timerEvent.h"
#include "hm_halBeep.h"
#include "hm_reportDevToCloud.h"
#include "hm_net_data_manage.h"


extern HM_GW_LIST gw_info;
extern DEV_NUM_INFO devNumInfo;
extern DEV_MYSELF_INFO hmIotsDevInfo;
extern GWSTATFORTIME gwStateForRealTime;
uint8_t sysTimerEventChangFlag = 0;
SYSTIMER sysTimerBuf[MAX_SYS_TIMER_EVENT_NUM];
extern BEEP_INFO beep_info;
extern BEEP_INFO beep_info;
extern TX_SEMAPHORE beep_semaphore_ptr;
extern NETSENDINFO netSendInfo;
extern NETRCVINFO   netRcvInfo;
extern WIFICMDWATIRSP wifiCmdWaitRspBuf[SPECIALCMDNUM];


static uint8_t t_index;
static uint32_t t_times;
static uint8_t t_action = TIMER_EVENT_ACTION_NONE;

extern int wifi_connect_flag;


/**************************************************************
当前时间转换为格林威治时间，在定时器中
gwStateForRealTime.sysRealTimeFors时间保存的是格林威治时间，所有
定时器的时间也需要将当前时间转换为格林威治时间。
**************************************************************/
uint32_t userMakeGlwzTimeForSecond(HMTM *times)
{
	uint32_t tempTime;
	int32_t zoneTime;
	zoneTime = (hmIotsDevInfo.time_zones / 100) * 3600 + (hmIotsDevInfo.time_zones % 100) * 60;
	tempTime = hm_mktime(times) - zoneTime;
	return tempTime;
}

/****************************************************************
timer : 定时周期，和有效数据负载，用于存储设备动作的参数
output :根据不同的设备，output为一个不同设备输出的联合体
****************************************************************/
void userFillTimerEvent(T_TIMER timer,OUTPUTINFO output )
{
	uint8_t i;
	HMTM inTime;
	uint32_t tempTimes;
	if(gwStateForRealTime.sysRealTimerValid == 0)//系统时间无效
	{
		printf("in fill timer:the sys real time was invalid\r\n");
		return;
	}
	if(!(timer.twf & 0x80))//the timer was disable
	{
		printf("the timer was disable\r\n");
		return;
	}
	
	if((timer.thour == 0xff) || (timer.tmin == 0xff))//无效的定时器
	{
		printf("this is a invalid timer\r\n");
		return;
	}
	inTime.tm_year = gwStateForRealTime.sysRealTime.year;
	uint8_t tempIndex = 0xff;
	for(i = 0; i < MAX_SYS_TIMER_EVENT_NUM; i ++)
	{
		if(sysTimerBuf[i].timerValid == 0)
		{
			tempIndex = i;
			break;
		}
	}
	if(tempIndex != 0xff)
	{
		printf("fill the timer index:%d\r\n",tempIndex);
		sysTimerBuf[tempIndex].output = output;
		sysTimerBuf[tempIndex].timerValid = 1;
		sysTimerBuf[tempIndex].t_timer = timer;
		if(sysTimerBuf[tempIndex].t_timer.twf & 0x7f)//周期定时
		{
			if(is_today(gwStateForRealTime.sysRealTime.week, sysTimerBuf[tempIndex].t_timer.twf) == 0)
			{
				printf("the wk timer was not today\r\n");
				return;
			}
			inTime.tm_mon = gwStateForRealTime.sysRealTime.month;
			inTime.tm_mday = gwStateForRealTime.sysRealTime.day;
			inTime.tm_hour = sysTimerBuf[tempIndex].t_timer.thour;
			inTime.tm_min = sysTimerBuf[tempIndex].t_timer.tmin;
			inTime.tm_sec = sysTimerBuf[tempIndex].t_timer.tsec;
		}else{
			inTime.tm_mon = sysTimerBuf[tempIndex].t_timer.tmonth;
			inTime.tm_mday = sysTimerBuf[tempIndex].t_timer.tday;
			inTime.tm_hour = sysTimerBuf[tempIndex].t_timer.thour;
			inTime.tm_min = sysTimerBuf[tempIndex].t_timer.tmin;
			inTime.tm_sec = sysTimerBuf[tempIndex].t_timer.tsec;
		}
		tempTimes = userMakeGlwzTimeForSecond(&inTime);
		printf("test 11111111111111\r\n");
		if(tempTimes > gwStateForRealTime.sysRealTimeFors)//定时器未执行，处于有效状态
		{
			printf("test 2222222222222\r\n");
			if(t_action == TIMER_EVENT_ACTION_TIMERS)//处于定时状态
			{
				if(tempTimes < t_times)//该定时器比当前等待的定时器要快，先执行
				{
					t_index = tempIndex;
					t_times = tempTimes;
				}
			}
		}
	
	}else{
		printf("in fill timer event:the timer event was full\r\n");
	}
	printf("test 66666666\r\n");
}

/****************************************************************
timer : 定时周期，和有效数据负载，用于存储设备动作的参数
output :根据不同的设备，output为一个不同设备输出的联合体
****************************************************************/
void userUpdateTimerEvent(T_TIMER timer,OUTPUTINFO output)
{
	uint8_t i;
	HMTM inTime;
	uint32_t tempTimes;
	if(gwStateForRealTime.sysRealTimerValid == 0)//系统时间无效
	{
		printf("in update timer:the sys real time was invalid\r\n");
		return;
	}
	if(!(timer.twf & 0x80))//the timer was disable
	{
		printf("the timer was disable\r\n");
		return;
	}
	
	if((timer.thour == 0xff) || (timer.tmin == 0xff))//无效的定时器
	{
		printf("this is a invalid timer\r\n");
		return;
	}
	inTime.tm_year = gwStateForRealTime.sysRealTime.year;
	uint8_t tempIndex = 0xff;
	for(i = 0; i < MAX_SYS_TIMER_EVENT_NUM; i ++)
	{
		if(sysTimerBuf[i].timerValid == 0)
		{
			if(tempIndex == 0xff)
			{
				tempIndex = i;
			}
		}else{
			
			if(memcmp((char*)&sysTimerBuf[i].output,(char*)&output,sizeof(OUTPUTINFO)) == 0)
			{
				printf("timer update,the timer index:%d\r\n",i);
				tempIndex = i;
				break;
			}
		}
	}
	if(tempIndex != 0xff)
	{
		printf("fill the timer index:%d\r\n",tempIndex);
		sysTimerBuf[tempIndex].output = output;
		sysTimerBuf[tempIndex].timerValid = 1;
		sysTimerBuf[tempIndex].t_timer = timer;
		if(sysTimerBuf[tempIndex].t_timer.twf & 0x7f)//周期定时
		{
			if(is_today(gwStateForRealTime.sysRealTime.week, sysTimerBuf[tempIndex].t_timer.twf) == 0)
			{
				printf("the wk timer was not today\r\n");
				return;
			}
			inTime.tm_mon = gwStateForRealTime.sysRealTime.month;
			inTime.tm_mday = gwStateForRealTime.sysRealTime.day;
			inTime.tm_hour = sysTimerBuf[tempIndex].t_timer.thour;
			inTime.tm_min = sysTimerBuf[tempIndex].t_timer.tmin;
			inTime.tm_sec = sysTimerBuf[tempIndex].t_timer.tsec;
		}else{
			inTime.tm_mon = sysTimerBuf[tempIndex].t_timer.tmonth;
			inTime.tm_mday = sysTimerBuf[tempIndex].t_timer.tday;
			inTime.tm_hour = sysTimerBuf[tempIndex].t_timer.thour;
			inTime.tm_min = sysTimerBuf[tempIndex].t_timer.tmin;
			inTime.tm_sec = sysTimerBuf[tempIndex].t_timer.tsec;
		}
		tempTimes = userMakeGlwzTimeForSecond(&inTime);
		printf("test update 11111111111111\r\n");
		if(tempIndex == t_index)
		{
			printf("need find the nearest timer\r\n");
			t_action = TIMER_EVENT_ACTION_NONE;
		}else{
			if(tempTimes > gwStateForRealTime.sysRealTimeFors)//定时器未执行，处于有效状态
			{
				printf("test update 2222222222222\r\n");
				if(t_action == TIMER_EVENT_ACTION_TIMERS)//处于定时状态
				{

					if(tempTimes < t_times)//该定时器比当前等待的定时器要快，先执行
					{
						t_index = tempIndex;
						t_times = tempTimes;
						printf("the new nearest timer inidex:%d,time:%d\r\n",t_index,t_times);
					}
				}
			}
		}
	
	}else{
		printf("in update fill timer event:the timer event was full\r\n");
	}
	printf("test update 66666666\r\n");
}


void userInitAllTimerEvent(void)
{
	HM_GW_LIST *pgw_info=&gw_info;
	HM_DEV_RAM_LIST *pni= (HM_DEV_RAM_LIST *)pgw_info->pnode_list;
	HM_DEV_RAM_LIST *pni_tmp;
	HM_DEV_FLASH_INFO tempFlashData;
	uint16_t tempDataLen;
	uint8_t i;

	printf("start init the timer\r\n");

	if(pni != NULL)
	{
		pni_tmp = pni;
		do{
			userInitTheDevTimerEvent((char*)pni_tmp);
			pni_tmp=(HM_DEV_RAM_LIST *)pni_tmp->pnext_dev_info;
		}while (pni_tmp != pni);
	}


	tempDataLen = sizeof(LINKAGEINFO);
	for(i = 0; i < MAX_LINKAGE_NUM;i ++)
	{
		if(devNumInfo.linkageIndex[i] != INVALID_SCENE_LINKAGE_INDEX)
		{
			userInitTheLinkageTimerEvent((char*) &pgw_info->linkage_list[i]);
		}
	}

	userInitGwArmForTimerEvent((char*)&devNumInfo);
}


/***************************************************************
确认该定时器是否被触发(定时时间到)
***************************************************************/
static int8_t userCheckTimerEventHasTrigger(uint8_t index)
{
	HMTM inTime;
	uint32_t tempTimes;
	inTime.tm_year = gwStateForRealTime.sysRealTime.year;
	if(index >= MAX_SYS_TIMER_EVENT_NUM)
	{
		printf("the timer index was invalid:%d\r\n",index);
		return TIMER_CHECK_STATE_NOT_VALID_EVENT;//定时器不存在
	}
	if(sysTimerBuf[index].timerValid == 1)
	{
		if(sysTimerBuf[index].t_timer.twf & 0x80)//周期定时
		{
			if(is_today(gwStateForRealTime.sysRealTime.week, sysTimerBuf[index].t_timer.twf) == 0)
			{
				printf("the wk timer was not today\r\n");
				return TIMER_CHECK_STATE_NOT_TRIGGER;
			}
			inTime.tm_mon = gwStateForRealTime.sysRealTime.month;
			inTime.tm_mday = gwStateForRealTime.sysRealTime.day;
			inTime.tm_hour = sysTimerBuf[index].t_timer.thour;
			inTime.tm_min = sysTimerBuf[index].t_timer.tmin;
			inTime.tm_sec = sysTimerBuf[index].t_timer.tsec;
		}else{
			inTime.tm_mon = sysTimerBuf[index].t_timer.tmonth;
			inTime.tm_mday = sysTimerBuf[index].t_timer.tday;
			inTime.tm_hour = sysTimerBuf[index].t_timer.thour;
			inTime.tm_min = sysTimerBuf[index].t_timer.tmin;
			inTime.tm_sec = sysTimerBuf[index].t_timer.tsec;
		}
		tempTimes = userMakeGlwzTimeForSecond(&inTime);
		if(tempTimes > gwStateForRealTime.sysRealTimeFors)//时间还未到
		{
			return TIMER_CHECK_STATE_NOT_TRIGGER;//时间还未到
		}else{
			if((gwStateForRealTime.sysRealTimeFors - tempTimes) < 20)//时间过了，但在10s内
			{
				if(sysTimerBuf[index].t_timer.dealFlag == 0)//如果定时器还未处理过
				{
					
					return TIMER_CHECK_STATE_TRIGGER_NOW;//时间到了，返回成功
				}
			}else{//时间过了，但超过了20，清理该定时器
				if(sysTimerBuf[index].t_timer.dealFlag == 1)
				{
					sysTimerBuf[index].t_timer.dealFlag = 0;
				}
				if(!(sysTimerBuf[index].t_timer.twf & 0x80))
				{
					sysTimerBuf[index].timerValid = 0;
					memset(&sysTimerBuf[index],0,sizeof(SYSTIMER));
				}
				return TIMER_CHECK_STATE_OUT_MODEDE;//定时时间超时
			}
		}

	}else{
		printf("the timer index was invalid\r\n");
		return TIMER_CHECK_STATE_NOT_VALID_EVENT;//定时器不存在
	}
}

/*******************************************************************
定时器执行输出函数，timeIndex为定时器索引
********************************************************************/
static void userDealTimerAction(uint8_t timeIndex)
{
	HM_DEV_RAM_LIST *pni;
	uint8_t j;
	HM_GW_LIST *pgw_info=&gw_info;
	uint8_t needSaveFlashFlag = 0;
	uint8_t tempIndex;
	DEVINFOOPTION devOption;
	devOption.modeByte = 0;
	uint8_t tmEp1 = 0xff,tmEp2 = 0xff,tmEp3 = 0xff,tmEp4 = 0xff;
	if(timeIndex >= MAX_SYS_TIMER_EVENT_NUM)
	{
		printf("in deal timer:the index was invalid\r\n");
		return;
	}
	if(sysTimerBuf[timeIndex].timerValid == 1)
	{
		switch(sysTimerBuf[timeIndex].output.type)
		{
			case SCENE_LIST_GW_PAYLOAD:
				printf("gw timer control\r\n");
	
				if(sysTimerBuf[timeIndex].output.devAction.GWStat.armtype == 1)
				{
					if(sysTimerBuf[timeIndex].output.devData.gwOutput.armType == SENSOR_DISARM){
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
							 devOption.GWStat.armtype = 1;
						 }
					}else if(sysTimerBuf[timeIndex].output.devData.gwOutput.armType == SENSOR_HOME_ARM){
						hmSetAllSensorHomeArm();
						if((beep_info.work_flag == 0) && ((beep_info.work_type != BEEP_ALARM) && (beep_info.work_type != BEE_SOUND_LOCAL_REGULA)))
						 {
							 beep_info.work_type = BEEP_HOME_ARM;
							 beep_info.work_flag = 1;
							 tx_semaphore_put(&beep_semaphore_ptr);
						 }
						if(hmIotsDevInfo.sensor_arm_type != SENSOR_HOME_ARM)
						 {
							 hmIotsDevInfo.sensor_arm_type = SENSOR_HOME_ARM;
							 needSaveFlashFlag = 1;
							 devOption.GWStat.armtype = 1;
						 }
					}else if(sysTimerBuf[timeIndex].output.devData.gwOutput.armType == SENSOR_ARM){
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
							 devOption.GWStat.armtype = 1;
						 }
					}else{
						printf("in timer for gw arm type was not valid\r\n");
						memset(&sysTimerBuf[timeIndex],0,sizeof(SYSTIMER));
					}

					if(needSaveFlashFlag == 1)
					{
						userFlashWriteSysInfoFunc((uint8_t*)&hmIotsDevInfo);
					}
				}
				if(sysTimerBuf[timeIndex].output.devAction.GWStat.gwlightonoff == 1)
				{
					hmIotsDevInfo.light_onoff = sysTimerBuf[timeIndex].output.devData.gwOutput.lightOnoff;
				}
				if(devOption.modeByte != 0)//撤布防的定时操作也写入cloud
				{
					writeUserControlDevStateToCloud(USER_CONTROL_DEV_TYPE_GW,0,devOption,"timer","");
				}
				break;
			case SCENE_LIST_ZB_DEV_PAYLOAD:
				pni = findInDevListByIndex(sysTimerBuf[timeIndex].output.targetId);
				if(pni == NULL)
				{
					printf("the timer deal dev was not in dev list:%d\r\n",sysTimerBuf[timeIndex].output.targetId);
					memset(&sysTimerBuf[timeIndex],0,sizeof(SYSTIMER));
				}else{
					
					switch(pni->devType)
					{
						case NODE_INFO_TYPE_LIGHT_RGB:
							printf("deal timer output for RGB\r\n");
							if(sysTimerBuf[timeIndex].output.devAction.RGBSet.onoff == 1)
							{
								//if(pni->epList[0].devData.colorStateApp.onOffStatus != sysTimerBuf[timeIndex].output.devData.rgbOutput.onoff)
								{
									userFillUartCmdForOnOff(pni->shortAddr,pni->epList[0].ep,sysTimerBuf[timeIndex].output.devData.rgbOutput.onoff);	
								}
								qcom_thread_msleep(20);
							}
							
							if(sysTimerBuf[timeIndex].output.devAction.RGBSet.level == 1)
							{
								//if(pni->epList[0].devData.colorStateApp.level != sysTimerBuf[timeIndex].output.devData.rgbOutput.level)
								{
									userFillUartCmdForLevel(pni->shortAddr,pni->epList[0].ep,MOVE_TO_LEVEL_WTONOFF,sysTimerBuf[timeIndex].output.devData.rgbOutput.level,0);	
								}
								qcom_thread_msleep(20);
							}
							if((sysTimerBuf[timeIndex].output.devAction.RGBSet.colour_r == 1) ||
							   (sysTimerBuf[timeIndex].output.devAction.RGBSet.colour_g == 1) ||
							    (sysTimerBuf[timeIndex].output.devAction.RGBSet.colour_b == 1))
							{
								pni->epList[0].ep,pni->epList[0].devData.colorStateApp.rgb_r = sysTimerBuf[timeIndex].output.devData.rgbOutput.rgb_r;
								pni->epList[0].ep,pni->epList[0].devData.colorStateApp.rgb_g = sysTimerBuf[timeIndex].output.devData.rgbOutput.rgb_g;
								pni->epList[0].ep,pni->epList[0].devData.colorStateApp.rgb_b = sysTimerBuf[timeIndex].output.devData.rgbOutput.rgb_b;
								userFillUartCmdForColorControl(pni->shortAddr,pni->epList[0].ep,pni->epList[0].devData.colorStateApp.rgb_r,
								pni->epList[0].devData.colorStateApp.rgb_g,pni->epList[0].devData.colorStateApp.rgb_b);
								qcom_thread_msleep(20);
							}
							break;
						case NODE_INFO_TYPE_AI1SW:
						case NODE_INFO_TYPE_AI2SW:
						case NODE_INFO_TYPE_AI3SW:
							printf("deal timer output for switch\r\n");
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
							if(sysTimerBuf[timeIndex].output.devAction.SWSet.onoff1 == 1)
							{
								if(tmEp1 != 0xff)
								{
									//if(sysTimerBuf[timeIndex].output.devData.switchOutput.onoff1 != pni->epList[tmEp1].devData.witchStateApp.onOffStatus)
									{
										userFillUartCmdForOnOff(pni->shortAddr,pni->epList[tmEp1].ep,sysTimerBuf[timeIndex].output.devData.switchOutput.onoff1);
									}
									qcom_thread_msleep(20);
								}
							}
							if(sysTimerBuf[timeIndex].output.devAction.SWSet.onoff2 == 1)
							{
								if(tmEp2 != 0xff)
								{
									//if(sysTimerBuf[timeIndex].output.devData.switchOutput.onoff2 != pni->epList[tmEp2].devData.witchStateApp.onOffStatus)
									{
										userFillUartCmdForOnOff(pni->shortAddr,pni->epList[tmEp2].ep,sysTimerBuf[timeIndex].output.devData.switchOutput.onoff2);
									}
									qcom_thread_msleep(20);
								}
							}
							if(sysTimerBuf[timeIndex].output.devAction.SWSet.onoff3 == 1)
							{
								if(tmEp3 != 0xff)
								{
									//if(sysTimerBuf[timeIndex].output.devData.switchOutput.onoff3 != pni->epList[tmEp3].devData.witchStateApp.onOffStatus)
									{
										userFillUartCmdForOnOff(pni->shortAddr,pni->epList[tmEp3].ep,sysTimerBuf[timeIndex].output.devData.switchOutput.onoff3);
									}
									qcom_thread_msleep(20);
								}
							}
							break;
						case NODE_INFO_TYPE_RELAY_CONTROL:
							printf("timer output for relay\r\n");
							break;
						case NODE_INFO_TYPE_AI_SOCKET_USB:
							printf("timer output for usb plug\r\n");
							for(j = 0; j < pni->epNum; j ++)
							{
								if(pni->epList[j].ep == 1)
								{
									tmEp1 = j;
								}else if(pni->epList[j].ep == 2){
									tmEp2 = j;
								}
							}
							if(sysTimerBuf[timeIndex].output.devAction.UPlugSet.relayOnoff == 1)
							{
								if(tmEp1 != 0xff)
								{
									//if(sysTimerBuf[timeIndex].output.devData.uplugOutput.onoff_r != pni->epList[tmEp1].devData.onoffOutStateApp.onOffStatus)
									{
										userFillUartCmdForOnOff(pni->shortAddr,pni->epList[tmEp1].ep,sysTimerBuf[timeIndex].output.devData.uplugOutput.onoff_r);
									}
									qcom_thread_msleep(20);
								}
							}
							if(sysTimerBuf[timeIndex].output.devAction.UPlugSet.usbOnoff == 1)
							{
								if(tmEp2 != 0xff)
								{
									//if(sysTimerBuf[timeIndex].output.devData.uplugOutput.onoff_u != pni->epList[tmEp2].devData.onoffOutStateApp.onOffStatus)
									{
										userFillUartCmdForOnOff(pni->shortAddr,pni->epList[tmEp2].ep,sysTimerBuf[timeIndex].output.devData.uplugOutput.onoff_u);
									}
									qcom_thread_msleep(20);
								}
							}
							break;
						case NODE_INFO_TYPE_AI_SOCKET_MEASURE:
							printf("timer output for smart plug\r\n");
							if(sysTimerBuf[timeIndex].output.devAction.SPlugSet.relayOnoff == 1)
							{
								//if(pni->epList[0].devData.splugStateApp.onOffStatus != sysTimerBuf[timeIndex].output.devData.splugOutput.onoff)
								{
									userFillUartCmdForOnOff(pni->shortAddr,pni->epList[0].ep,sysTimerBuf[timeIndex].output.devData.splugOutput.onoff);	
								}
								qcom_thread_msleep(20);
							}
							break;
						case NODE_INFO_TYPE_WARNING_DEV: 
							printf("timer output for wd\r\n");
							if(sysTimerBuf[timeIndex].output.devAction.WDSet.onoff == 1)
							{
								userAppControlWDFunc(pni->shortAddr,pni->epList[0].ep,sysTimerBuf[timeIndex].output.devData.wdOutput.onoff,pni->epList[0].devData.wdStateApp.duration);
							}

							break;
						case NODE_INFO_TYPE_CUSTOM_RGB_AREA:
							printf("deal timer output for cut rgb\r\n");
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
									tmEp4= j;
								}
							}
							//on off
							if(sysTimerBuf[timeIndex].output.devAction.CUTRGBSet.onoffList1 == 1)
							{

								//if(sysTimerBuf[timeIndex].output.devData.cutRgbOutput.onoff1 != pni->epList[tmEp1].devData.cstRgbStateApp.onOffStatus)
								{
									userFillUartCmdForOnOff(pni->shortAddr,pni->epList[tmEp1].ep,sysTimerBuf[timeIndex].output.devData.cutRgbOutput.onoff1);
								}
								qcom_thread_msleep(20);
								
							}
							if(sysTimerBuf[timeIndex].output.devAction.CUTRGBSet.onoffList2 == 1)
							{

								//if(sysTimerBuf[timeIndex].output.devData.cutRgbOutput.onoff2 != pni->epList[tmEp2].devData.cstRgbStateApp.onOffStatus)
								{
									userFillUartCmdForOnOff(pni->shortAddr,pni->epList[tmEp2].ep,sysTimerBuf[timeIndex].output.devData.cutRgbOutput.onoff2);
								}
								qcom_thread_msleep(20);
								
							}
							if(sysTimerBuf[timeIndex].output.devAction.CUTRGBSet.onoffList3 == 1)
							{

								//if(sysTimerBuf[timeIndex].output.devData.cutRgbOutput.onoff3 != pni->epList[tmEp3].devData.cstRgbStateApp.onOffStatus)
								{
									userFillUartCmdForOnOff(pni->shortAddr,pni->epList[tmEp3].ep,sysTimerBuf[timeIndex].output.devData.cutRgbOutput.onoff3);
								}
								qcom_thread_msleep(20);
								
							}
							if(sysTimerBuf[timeIndex].output.devAction.CUTRGBSet.onoffList4 == 1)
							{

								//if(sysTimerBuf[timeIndex].output.devData.cutRgbOutput.onoff4 != pni->epList[tmEp4].devData.cstRgbStateApp.onOffStatus)
								{
									userFillUartCmdForOnOff(pni->shortAddr,pni->epList[tmEp4].ep,sysTimerBuf[timeIndex].output.devData.cutRgbOutput.onoff4);
								}
								qcom_thread_msleep(20);
								
							}


							//brightness 
							if(sysTimerBuf[timeIndex].output.devAction.CUTRGBSet.brightnessList1 == 1)
							{

								//if(sysTimerBuf[timeIndex].output.devData.cutRgbOutput.brightness1 != pni->epList[tmEp1].devData.cstRgbStateApp.brightness)
								{
									userFillUartCmdForBrightness(pni->shortAddr,pni->epList[tmEp1].ep,sysTimerBuf[timeIndex].output.devData.cutRgbOutput.brightness1);
								}
								qcom_thread_msleep(20);
								
							}
							if(sysTimerBuf[timeIndex].output.devAction.CUTRGBSet.brightnessList2 == 1)
							{

								//if(sysTimerBuf[timeIndex].output.devData.cutRgbOutput.brightness2 != pni->epList[tmEp2].devData.cstRgbStateApp.brightness)
								{
									userFillUartCmdForBrightness(pni->shortAddr,pni->epList[tmEp2].ep,sysTimerBuf[timeIndex].output.devData.cutRgbOutput.brightness2);
								}
								qcom_thread_msleep(20);
								
							}
							if(sysTimerBuf[timeIndex].output.devAction.CUTRGBSet.brightnessList3 == 1)
							{

								//if(sysTimerBuf[timeIndex].output.devData.cutRgbOutput.brightness3 != pni->epList[tmEp3].devData.cstRgbStateApp.brightness)
								{
									userFillUartCmdForBrightness(pni->shortAddr,pni->epList[tmEp3].ep,sysTimerBuf[timeIndex].output.devData.cutRgbOutput.brightness3);
								}
								qcom_thread_msleep(20);
								
							}
							if(sysTimerBuf[timeIndex].output.devAction.CUTRGBSet.brightnessList4 == 1)
							{

								//if(sysTimerBuf[timeIndex].output.devData.cutRgbOutput.brightness4 != pni->epList[tmEp4].devData.cstRgbStateApp.brightness)
								{
									userFillUartCmdForBrightness(pni->shortAddr,pni->epList[tmEp4].ep,sysTimerBuf[timeIndex].output.devData.cutRgbOutput.brightness4);
								}
								qcom_thread_msleep(20);
								
							}

							//color temperature
							if(sysTimerBuf[timeIndex].output.devAction.CUTRGBSet.colorTempList1 == 1)
							{

								//if(sysTimerBuf[timeIndex].output.devData.cutRgbOutput.colorTemp1!= pni->epList[tmEp1].devData.cstRgbStateApp.brightness)
								{
									userFillUartCmdForColorTemperature(pni->shortAddr,pni->epList[tmEp1].ep,sysTimerBuf[timeIndex].output.devData.cutRgbOutput.colorTemp1);
								}
								qcom_thread_msleep(20);
								
							}
							if(sysTimerBuf[timeIndex].output.devAction.CUTRGBSet.colorTempList2 == 1)
							{

								//if(sysTimerBuf[timeIndex].output.devData.cutRgbOutput.colorTemp2!= pni->epList[tmEp2].devData.cstRgbStateApp.brightness)
								{
									userFillUartCmdForColorTemperature(pni->shortAddr,pni->epList[tmEp2].ep,sysTimerBuf[timeIndex].output.devData.cutRgbOutput.colorTemp2);
								}
								qcom_thread_msleep(20);
								
							}

							if(sysTimerBuf[timeIndex].output.devAction.CUTRGBSet.colorTempList3 == 1)
							{

								//if(sysTimerBuf[timeIndex].output.devData.cutRgbOutput.colorTemp3!= pni->epList[tmEp3].devData.cstRgbStateApp.brightness)
								{
									userFillUartCmdForColorTemperature(pni->shortAddr,pni->epList[tmEp3].ep,sysTimerBuf[timeIndex].output.devData.cutRgbOutput.colorTemp3);
								}
								qcom_thread_msleep(20);
								
							}
							if(sysTimerBuf[timeIndex].output.devAction.CUTRGBSet.colorTempList1 == 4)
							{

								//if(sysTimerBuf[timeIndex].output.devData.cutRgbOutput.colorTemp4!= pni->epList[tmEp4].devData.cstRgbStateApp.brightness)
								{
									userFillUartCmdForColorTemperature(pni->shortAddr,pni->epList[tmEp4].ep,sysTimerBuf[timeIndex].output.devData.cutRgbOutput.colorTemp4);
								}
								qcom_thread_msleep(20);
								
							}
							break;
						default:
							printf("the timer output was unknow dev,remove it\r\n");
							memset(&sysTimerBuf[timeIndex],0,sizeof(SYSTIMER));
							break;
					}	
				}
				
				break;
			case SCENE_LIST_SCENE_PAYLOAD:
				printf("timer do scene\r\n");
				userFillSceneExeBuf(sysTimerBuf[timeIndex].output.targetId,1);
				break;
			case SCENE_LIST_LINKAGE_PAYLOAD:
				printf("timer trigger linkage\r\n");
				tempIndex = sysTimerBuf[timeIndex].output.targetId % 10;
				if(devNumInfo.linkageIndex[tempIndex] == sysTimerBuf[timeIndex].output.targetId)
				{
					if(pgw_info->linkage_list[tempIndex].valid == 1)
					{
						if(pgw_info->linkage_list[tempIndex].Enable == 1)
						{
							pgw_info->linkage_list[tempIndex].inputInfo.triggering = 1;
							printf("the linkage:%d was trigeet\r\n",sysTimerBuf[timeIndex].output.targetId);
						}
					}
				}
				break;
			default:
				printf("the timer output unknow type,remove it\r\n");
				memset(&sysTimerBuf[timeIndex],0,sizeof(SYSTIMER));
				break;
		}
	}
}

/**************************************************************
查询最近的一个定时器
***************************************************************/
uint8_t userFindNearestTimerEvent(uint32_t *times,uint8_t *timerIndex)
{
	uint8_t i;
	HMTM inTime;
	uint8_t nearestIndex = 0xff;
	uint32_t nearestTimes = 0xffffffff;
	uint32_t tempTimes;
	inTime.tm_year = gwStateForRealTime.sysRealTime.year;
	for(i = 0; i < MAX_SYS_TIMER_EVENT_NUM; i ++)
	{
		if(sysTimerBuf[i].timerValid == 1)
		{
			if(!(sysTimerBuf[i].t_timer.twf & 0x80))//定时器使能位
			{
				printf("the timer was disable\r\n");
				continue;
			}
			if(sysTimerBuf[i].t_timer.twf & 0x7f)//周期定时
			{
				if(is_today(gwStateForRealTime.sysRealTime.week, sysTimerBuf[i].t_timer.twf) == 0)
				{
					printf("the wk timer was not today\r\n");
					continue;
				}
				inTime.tm_mon = gwStateForRealTime.sysRealTime.month;
				inTime.tm_mday = gwStateForRealTime.sysRealTime.day;
				inTime.tm_hour = sysTimerBuf[i].t_timer.thour;
				inTime.tm_min = sysTimerBuf[i].t_timer.tmin;
				inTime.tm_sec = sysTimerBuf[i].t_timer.tsec;
			}else{
				inTime.tm_mon = sysTimerBuf[i].t_timer.tmonth;
				inTime.tm_mday = sysTimerBuf[i].t_timer.tday;
				inTime.tm_hour = sysTimerBuf[i].t_timer.thour;
				inTime.tm_min = sysTimerBuf[i].t_timer.tmin;
				inTime.tm_sec = sysTimerBuf[i].t_timer.tsec;
			}
			tempTimes = userMakeGlwzTimeForSecond(&inTime);
			if(tempTimes > gwStateForRealTime.sysRealTimeFors)//时间还未到
			{
				//tempTimes = tempTimes - gwStateForRealTime.sysRealTimeFors;
				if(tempTimes < nearestTimes)
				{
					nearestTimes = tempTimes;
					nearestIndex = i;
				}
			}else{
				if((gwStateForRealTime.sysRealTimeFors - tempTimes) < 20)//时间过了，但在10s内
				{
					if(sysTimerBuf[i].t_timer.dealFlag == 0)//如果定时器还未处理过
					{
						*timerIndex = i;
						return TIMER_EVENT_ACTION_DEAL_NOW;
					}
				}else{//时间过了，但超过了10，清理该定时器
					if(sysTimerBuf[i].t_timer.dealFlag == 1)
					{
						sysTimerBuf[i].t_timer.dealFlag = 0;
					}
					if(!(sysTimerBuf[i].t_timer.twf & 0x80))
					{
						sysTimerBuf[i].timerValid = 0;
						memset(&sysTimerBuf[i],0,sizeof(SYSTIMER));
					}
				}
			}

		}
	}

	if(nearestIndex != 0xff)
	{
		*times = nearestTimes;
		*timerIndex = nearestIndex;
		return TIMER_EVENT_ACTION_TIMERS;
	}else{
		return TIMER_EVENT_ACTION_NONE;
	}
}


void userCheckDevOnOffLineByTimerEvent()
{
	HM_GW_LIST *pgw_info=&gw_info;
	HM_DEV_RAM_LIST *pni= (HM_DEV_RAM_LIST *)pgw_info->pnode_list;
	HM_DEV_RAM_LIST *pni_tmp;
	static uint8_t checkDevIndex = 0;
	uint8_t cpCheckDevIndex = 0;

	if (pni==NULL)
		return ;
	pni_tmp = pni;
	do{//pni_tmp->onLine == 1
		if (cpCheckDevIndex >= checkDevIndex)
		{
			printf("the check on off index:%d\r\n",checkDevIndex);
			checkDevIndex ++;
			if((pni_tmp->lastCommunicationTimes >=  gwStateForRealTime.sysRealTimeFors) || (pni_tmp->lastCommunicationTimes == 0))
			{
				pni_tmp->lastCommunicationTimes = gwStateForRealTime.sysRealTimeFors;
				break;
			}
			switch(pni_tmp->devType)
			{
				case NODE_INFO_TYPE_LIGHT_RGB:
				case NODE_INFO_TYPE_AI1SW:
				case NODE_INFO_TYPE_AI2SW:
				case NODE_INFO_TYPE_AI3SW:
				case NODE_INFO_TYPE_RELAY_CONTROL:
				case NODE_INFO_TYPE_AI_SOCKET_USB:
				case NODE_INFO_TYPE_AI_SOCKET_MEASURE:
				case NODE_INFO_TYPE_CUSTOM_RGB_AREA:
					if(pni_tmp->onLine == 1)
					{
						if(gwStateForRealTime.sysRealTimeFors - pni_tmp->lastCommunicationTimes > 90)
						{
							if(pni_tmp->onOffLineState == 0)
							{
								pni_tmp->onOffLineState = 1;//
								userFillUartCmdForReadAttr(pni_tmp->shortAddr,pni_tmp->epList[0].ep,ZCL_ON_OFF_CLUSTER_ID,ZCL_ON_OFF_ATTRIBUTE_ID);
							}else{//下次进入该函数，为下一轮轮询到该设备，比如有5个设备，check一次为2s，则下一次轮询到该设备需要10s
								pni_tmp->onOffLineState = 0;
								setNodeOnlineUpdateDetectedEvent(pni_tmp,0);//设置该设备为离线
								pni_tmp->lastCommunicationTimes = gwStateForRealTime.sysRealTimeFors; 
							}
						}
					}else{
						if(gwStateForRealTime.sysRealTimeFors - pni_tmp->lastCommunicationTimes > 300)
						{
							userFillUartCmdForReadAttr(pni_tmp->shortAddr,pni_tmp->epList[0].ep,ZCL_ON_OFF_CLUSTER_ID,ZCL_ON_OFF_ATTRIBUTE_ID);
							pni_tmp->lastCommunicationTimes = gwStateForRealTime.sysRealTimeFors; //更新处理时间，如果设备在线，会在uart cmd处设置该设备在线
						}
					}
					break;
				case NODE_INFO_TYPE_WARNING_DEV: 
					if(pni_tmp->onLine == 1)
					{
						if(gwStateForRealTime.sysRealTimeFors - pni_tmp->lastCommunicationTimes > 7000)
						{
							if(pni_tmp->onOffLineState == 0)
							{
								pni_tmp->onOffLineState = 1;//
								userFillUartCmdForReadAttr(pni_tmp->shortAddr,pni_tmp->epList[0].ep,ZCL_POWER_CONFIG_CLUSTER_ID,ZCL_BATTERY_PERCENTAGE_REMAINING_ATTRIBUTE_ID);
							}else{//下次进入该函数，为下一轮轮询到该设备，比如有5个设备，check一次为2s，则下一次轮询到该设备需要10s
								pni_tmp->onOffLineState = 0;
								setNodeOnlineUpdateDetectedEvent(pni_tmp,0);//设置该设备为离线
								pni_tmp->lastCommunicationTimes = gwStateForRealTime.sysRealTimeFors; 
							}
						}
					}else{
						if(gwStateForRealTime.sysRealTimeFors - pni_tmp->lastCommunicationTimes > 9600)
						{
							userFillUartCmdForReadAttr(pni_tmp->shortAddr,pni_tmp->epList[0].ep,ZCL_POWER_CONFIG_CLUSTER_ID,ZCL_BATTERY_PERCENTAGE_REMAINING_ATTRIBUTE_ID);
							pni_tmp->lastCommunicationTimes = gwStateForRealTime.sysRealTimeFors; //更新处理时间，如果设备在线，会在uart cmd处设置该设备在线
						}
					}
					break;
				case NODE_INFO_TYPE_MAGNET_DOOR:
				case NODE_INFO_TYPE_WATER_SENSOR:
				case NODE_INFO_TYPE_PIR_SENSOR:
				case NODE_INFO_TYPE_SMOKE_SENSOR:
				case NODE_INFO_TYPE_TEMP_HUM_SENSOR:
				case NODE_INFO_TYPE_CO_SENSOR:
				case NODE_INFO_TYPE_VIBRATION:
				case NODE_INFO_TYPE_SOS:
				case NODE_INFO_TYPE_REMOTE_CONTROL:
					if(pni_tmp->onLine == 1)
					{
						if(gwStateForRealTime.sysRealTimeFors - pni_tmp->lastCommunicationTimes > 7200 * 3)
						{
							setNodeOnlineUpdateDetectedEvent(pni_tmp,0);//设置该设备为离线
						}
					}
					break;
				default:
					break;
			}
			break;
		}
		
		pni_tmp=(HM_DEV_RAM_LIST *)pni_tmp->pnext_dev_info;
		if(pni_tmp == pni)
		{
			checkDevIndex = 0;
		}else{
			cpCheckDevIndex ++;
		}
	}while (pni_tmp != pni);
}


extern char TCPSocket_connectd;
extern int qca_get_station_state(void);	
void timer_work_task(unsigned int arg)
{
	int8_t ret;
	uint8_t timerInitFlag = 0;
	uint8_t memCheckTime;
	uint8_t wdtNumTime;
	uint8_t checkDevOnOffLineTime;
	uint16_t saveAllFlashTime;
	uint8_t wifiConnectCheckTime;
	uint8_t wifiSpecialCmdWaitNum,i;
	//wifi_connect_flag=0;
	qcom_thread_msleep(500);//qcom_thread_msleep(3000);
	ap_connect();// add  wifi connect fail   //hardy add ap_connect
	while(1)
	{	//printf("timer_work_task------------000-----------\r\n");
		qca_calendar_get_time(&gwStateForRealTime.sysRealTime);
		if((gwStateForRealTime.sysRealTime.year > 3000) || (gwStateForRealTime.sysRealTime.year < 2010))
		{
			gwStateForRealTime.sysRealTimerValid = 0;
			gwStateForRealTime.sysRealTimeFors = 0;
		//	printf("the sys real time was not valid\r\n");
		}else{
			gwStateForRealTime.sysRealTimerValid = 1;
			//gwStateForRealTime.sysRealTimeFors = get_user_times();//格林威治时间
			gwStateForRealTime.sysRealTimeFors = get_user_times()+((hmIotsDevInfo.time_zones / 100) * 3600 + (hmIotsDevInfo.time_zones % 100) * 60);//
		}

		if(gwStateForRealTime.sysRealTimerValid == 1)
		{
			if(timerInitFlag == 0)
			{
				timerInitFlag = 1;
				userInitAllTimerEvent();
			}

			
			if((t_action == TIMER_EVENT_ACTION_NONE) || (sysTimerEventChangFlag == 1))
			{
				t_action = userFindNearestTimerEvent(&t_times,&t_index);
				sysTimerEventChangFlag = 0;
				//printf("the near est timer id:%d,times:%d,action:%d\r\n",t_index,t_times,t_action);//cesi guan
				if(t_action == TIMER_EVENT_ACTION_DEAL_NOW)
				{
					//action 
					userDealTimerAction(t_index);
					printf("the timer event was action now\r\n");
					sysTimerBuf[t_index].t_timer.dealFlag = 1;
					t_action = TIMER_EVENT_ACTION_NONE;
				}else if(t_action == TIMER_EVENT_ACTION_TIMERS){
					printf("timering\r\n");
				}
			}else if(t_action == TIMER_EVENT_ACTION_TIMERS){
				if(gwStateForRealTime.sysRealTimeFors >= t_times) 
				{
					ret = userCheckTimerEventHasTrigger(t_index);
					if(ret == TIMER_CHECK_STATE_TRIGGER_NOW)
					{
						//action
						userDealTimerAction(t_index);
						printf("the timer event was action now\r\n");
						sysTimerBuf[t_index].t_timer.dealFlag = 1;
						t_action = TIMER_EVENT_ACTION_NONE;
					}else{
						printf("the timer test for not trigger\r\n");
						t_action = TIMER_EVENT_ACTION_NONE;
					}
				}
			}
		}

		memCheckTime ++;
		if(memCheckTime >= 60)
		{
			memCheckTime = 0;
			printf("-->Mem : %d.\r\n", qcom_mem_heap_get_free_size());
			
		}
		
		wdtNumTime ++;
		if(wdtNumTime >= 10)
		{
			wdtNumTime = 0;
			qcom_watchdog_feed();
		}

		saveAllFlashTime ++;
		//printf("------saveAllFlashTime=%d,%d\r\n",saveAllFlashTime,TCPSocket_connectd);
		if((saveAllFlashTime >= USER_TIMER_SAVE_ALL_FLASH_TIMR) && //7000秒，将近两小时USER_TIMER_SAVE_ALL_FLASH_TIMR
		(gwStateForRealTime.gwIsAlarning == 0) && //设备没有报警
		(gwStateForRealTime.gwDevIsOta == 0) && //设备没有处于OTA状态
		(gwStateForRealTime.gwZbIsJoining == 0) &&//设备没有处于加网状态
		(netSendInfo.netsendflag == 0) &&//设备没有需要发送的网络数据
		(netRcvInfo.netrcvflag == 0))//设备没有还未处理的网络数据
		{
			printf("for timer save all flash\r\n");
			saveAllFlashTime = 0;
			userSaveAllInfoToFlash();
		}
		/*if(saveAllFlashTime >= USER_TIMER_SAVE_ALL_FLASH_TIMR) 
		{
			printf("for timer save all flash USER_TIMER_SAVE_ALL_FLASH_TIMR\r\n");
			saveAllFlashTime = 0;
			//userSaveAllInfoToFlash();
		}*/

		wifiConnectCheckTime ++;
		if(wifiConnectCheckTime > 4)
		{
			wifiConnectCheckTime = 0;
			if(qca_get_station_state() == 0)
			{
				gwStateForRealTime.wifiConnect = 1;
				printf("wifi connect ok\r\n");
			}else{
				gwStateForRealTime.wifiConnect = 0;
				printf("wifi connect fail\r\n");
			}
		}

		wifiSpecialCmdWaitNum ++;
		if(wifiSpecialCmdWaitNum >= 2)//write attr时等待，目前用于调节震动灵敏度
		{
			wifiSpecialCmdWaitNum = 0;
			for(i = 0; i < SPECIALCMDNUM; i ++)
			{
				if(wifiCmdWaitRspBuf[i].valid == 1)
				{
					if(gwStateForRealTime.sysRealTimeFors > wifiCmdWaitRspBuf[i].rcvTimems)
					{
						if(gwStateForRealTime.sysRealTimeFors - wifiCmdWaitRspBuf[i].rcvTimems > 5)
						{
							userDealWriteAttrCmdRspEvent(wifiCmdWaitRspBuf[i].zbShortAddr,wifiCmdWaitRspBuf[i].zbClusterId,wifiCmdWaitRspBuf[i].zbEp,0);
							wifiCmdWaitRspBuf[i].valid = 0;
							wifiCmdWaitRspBuf[i].cmdSn = 0;
							wifiCmdWaitRspBuf[i].rcvTimems = 0;
							wifiCmdWaitRspBuf[i].zbClusterId = 0;
							wifiCmdWaitRspBuf[i].zbCmd = 0;
							wifiCmdWaitRspBuf[i].zbShortAddr = 0;
							wifiCmdWaitRspBuf[i].zbEp = 0;
						}
					}
				}
			}
		}

		checkDevOnOffLineTime ++;
		if(checkDevOnOffLineTime >= 4)
		{
			checkDevOnOffLineTime = 0;
			userCheckDevOnOffLineByTimerEvent();
		}
		
		
		qcom_thread_msleep(500);	
	}
}
