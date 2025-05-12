
#include "Ducker.h"
#include "AudioDelayer.h"
#include "AudioEvent.h"
#include "AudioBitmap.h"
#include "AudioMain.h"
#include "IAudio.h"

typedef enum __DuckType_t{		
    PRI_DUCK_REVERSE,
	PRI_DUCK_NAVIGATION,
	PRI_DUCK_INSTRUMENT,	
	DUCK_COUNTS
}DuckType_t;

DuckVolumeCtrl_t dvc;
Bitmap_t s_duckerBitMap;
static AudioEventConfig_t* s_eventTbl = NULL;

DuckVolume_t s_duckVolume[] = {
	[PRI_DUCK_REVERSE] = {
		.duckPri = PRI_DUCK_REVERSE,
		.callBack = ReverseDuck,
	},	
	[PRI_DUCK_NAVIGATION] = {
		.duckPri = PRI_DUCK_NAVIGATION,
		.callBack = NavigationDuck,
	},	
	[PRI_DUCK_INSTRUMENT] = {
		.duckPri = PRI_DUCK_INSTRUMENT,
		.callBack = InstrumentDuck,
	},
};

void DuckerConfig(void)
{	
	//Ducker控制器初始化
	int i = 0;
	for(i = 0;i<DUCK_COUNTS;i++){
		dvc.changed[i] = false;
	}	
	dvc.tempVolume = 0;
	dvc.ducking = false;
	dvc.duckVolumeArray = s_duckVolume;
	//优先级位图初始化
	BitmapInit(&s_duckerBitMap);
	//拿到事件表
	s_eventTbl = GetAudioEventTable();
	//注册事件处理器
	AudioEventRegister(AUDIO_EVENT_NAVIGATION_DUCK,NavigationDuckHandler,false);
	AudioEventRegister(AUDIO_EVENT_DASH_DUCK,InstrumentDuckHandler,false);
	AudioEventRegister(AUDIO_EVENT_MEDIA_UNDUCK,VolumeUnDuckHandler,false);
	AudioEventRegister(AUDIO_EVENT_REVERSE_DUCK,ReverseDuckHandler,false);
    
	//创建仪表音压低媒体的任务
	BareMetalTaskCreater(PRIORITY_CRITICAL,VolumeDuckServer,true);
}

static void VolumeDuckServer(void)
{	
	int i = 0;
	static int s_preHighest = MAX_PRIORITY_COUNTS;
	int highest = GetFirstBitSetPos(&s_duckerBitMap);

	if(s_preHighest != highest){
		s_preHighest = highest;
		dvc.changed[highest] = true;
	}
	if(highest != BitmapPositionCount()){
		if(s_duckVolume[highest].callBack != NULL){
			s_duckVolume[highest].callBack(); 
		}
	}else{//没有压低事件
		dvc.ducking = false;
		for(i = 0;i<DUCK_COUNTS;i++){
			dvc.changed[i] = false;
		}			
	}
}

static void NavigationDuckHandler(void)		//导航压低事件处理器
{
	bool isDone = false;
	//int data = *((int*)(s_eventTbl[AUDIO_EVENT_NAVIGATION_DUCK].pParam));
	//user code start

	if(!dvc.ducking){
		dvc.ducking = true;
		dvc.tempVolume = GetIState()->mainVolume;
	}	
	BitmapSet(&s_duckerBitMap,s_duckVolume[PRI_DUCK_NAVIGATION].duckPri);
	isDone = true;
	
	//user code end
	s_eventTbl[AUDIO_EVENT_NAVIGATION_DUCK].eventFlags = !isDone;	
}

static void NavigationDuck(void)			//导航压低回调
{
	static uint32_t data = 0;
	if(dvc.changed[PRI_DUCK_NAVIGATION]){
		dvc.changed[PRI_DUCK_NAVIGATION] = false;
		data = 6;//测试音量值
		AudioEventTrigger(AUDIO_EVENT_MEDIA_FADE,&data);
	}
}

static void InstrumentDuckHandler(void)		//仪表压低事件处理器
{
	bool isDone = false;
	//int data = *((int*)(s_eventTbl[AUDIO_EVENT_DASH_DUCK].pParam));
	//user code start
	
	if(!dvc.ducking){
		dvc.ducking = true;
		dvc.tempVolume = GetIState()->mainVolume;
	}		
	BitmapSet(&s_duckerBitMap,s_duckVolume[PRI_DUCK_INSTRUMENT].duckPri);
	isDone = true;
	
	//user code end
	s_eventTbl[AUDIO_EVENT_DASH_DUCK].eventFlags = !isDone;		
}

static void InstrumentDuck(void)			//仪表压低回调
{
	static uint32_t data = 0;
	if(dvc.changed[PRI_DUCK_INSTRUMENT]){
		dvc.changed[PRI_DUCK_INSTRUMENT] = false;
		data = 12;//测试音量值
		AudioEventTrigger(AUDIO_EVENT_MEDIA_FADE,&data);
	}
}

static void VolumeUnDuckHandler(void)	//音量压低解除事件处理器
{
	bool isDone = false;
	int data = *((int*)(s_eventTbl[AUDIO_EVENT_MEDIA_UNDUCK].pParam));
	//user code start

	int highest = GetFirstBitSetPos(&s_duckerBitMap);
	BitmapClear(&s_duckerBitMap,s_duckVolume[data].duckPri);
	if(highest == s_duckVolume[data].duckPri)
		AudioEventTrigger(AUDIO_EVENT_MEDIA_FADE,&dvc.tempVolume);
	isDone = true;
	
	//user code end
	s_eventTbl[AUDIO_EVENT_MEDIA_UNDUCK].eventFlags = !isDone;	
}

static void ReverseDuckHandler(void)
{
	bool isDone = false;
	//int data = *((int*)(s_eventTbl[AUDIO_EVENT_REVERSE_DUCK].pParam));
	//user code start

	if(!dvc.ducking){
		dvc.ducking = true;
		dvc.tempVolume = GetIState()->mainVolume;
	}	
	BitmapSet(&s_duckerBitMap,s_duckVolume[PRI_DUCK_REVERSE].duckPri);
	isDone = true;
	
	//user code end
	s_eventTbl[AUDIO_EVENT_REVERSE_DUCK].eventFlags = !isDone;	
}

static void ReverseDuck(void)			//R档压低回调
{
	static uint32_t data = 0;
	if(dvc.changed[PRI_DUCK_REVERSE]){
		dvc.changed[PRI_DUCK_REVERSE] = false;
		data = 1;//测试音量值
		AudioEventTrigger(AUDIO_EVENT_MEDIA_FADE,&data);
	}
}
