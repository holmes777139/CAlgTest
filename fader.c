#include "Fader.h"
#include "AudioDelayer.h"
#include "AudioEvent.h"
#include "AudioMain.h"
#include "McuApi.h"

FaderContext volFader;
static AudioEventConfig_t* s_eventTbl = NULL;

void FaderConfig(void)
{
	volFader.current = volFader.target = 0;
	//拿到事件表
	s_eventTbl = GetAudioEventTable();
	//注册事件处理器
	AudioEventRegister(AUDIO_EVENT_MEDIA_FADE,VolumeFadeHandler,false);
	AudioEventRegister(AUDIO_EVENT_MEDIA_FADE_UPDATE,VolumeFadeUpdateHandler,false);
	//创建音量缓升缓降的任务
	BareMetalTaskCreater(1,FaderProcess,true);//1毫秒周期
}

static void FaderProcess(void)
{
	//媒体缓升缓降算法
    if(Get1MsTick() - volFader.lastTick >= volFader.intervalMs 
		&& volFader.current != volFader.target) { 
        volFader.current += volFader.step;
	    if(GetIAmp()->fp_AmpMasterVolumeSet != NULL){
			GetIAmp()->fp_AmpMasterVolumeSet(volFader.current);
		}
        volFader.lastTick = Get1MsTick();
        if((volFader.step > 0 && volFader.current >= volFader.target) ||
           (volFader.step < 0 && volFader.current <= volFader.target)){
		    if(GetIAmp()->fp_AmpMasterVolumeSet != NULL){
				GetIAmp()->fp_AmpMasterVolumeSet(volFader.target);
			}
			GetIState()->mainVolume = volFader.target;	
			volFader.intervalMs = 0;
		
			
            LOGI("[@AUDIO]Fade completed to %d\n", volFader.target);
        }
		//更新媒体音量的UI
		indcrev_03DE_01_2_0_2_7 = volFader.step > 0 ? 5 : 4;//4:减 5:加
		indcrev_03DE_01_3_0_3_7 = 1;//0：短 1：长
		indcrev_03DE_01_4_0_4_7 = 1;//abs(volFader.target - volFader.current);//次数	
		indelm_flush_ind(&indcrelm_03DE_01_2_0_2_7);
		indelm_flush_ind(&indcrelm_03DE_01_3_0_3_7);
		indelm_flush_ind(&indcrelm_03DE_01_4_0_4_7);		
    }
}

static void VolumeFadeHandler(void)
{
	bool isDone = false;
	int data = *((int*)(s_eventTbl[AUDIO_EVENT_MEDIA_FADE].pParam));
	//user code start
	
	static int preData = 0;
	if(preData !=data){
		preData = data;
		
		FadeVolume(data,2000);//2000毫秒完成缓升缓降
		
		isDone = true;
	}
	
	//user code end
	s_eventTbl[AUDIO_EVENT_MEDIA_FADE].eventFlags = !isDone;	
}

static void VolumeFadeUpdateHandler(void)
{
	bool isDone = false;
	//int data = *((int*)(s_eventTbl[AUDIO_EVENT_MEDIA_FADE_UPDATE].pParam));
	//user code start
	if(volFader.intervalMs !=0){
		volFader.target = GetIState()->mainVolume;
	}
	isDone = true;
	
	//user code end
	s_eventTbl[AUDIO_EVENT_MEDIA_FADE_UPDATE].eventFlags = !isDone;	
}

static void FadeVolume(uint32_t target, uint32_t durationMs)
{
    if(durationMs == 0) { // 立即设置
	    if(GetIAmp()->fp_AmpMasterVolumeSet != NULL){
			GetIAmp()->fp_AmpMasterVolumeSet(target);
		}
        return;
    }
    
    volFader.current = GetIState()->mainVolume;
    volFader.target = target;
    volFader.step = (target > volFader.current) ? 1 : -1;
    volFader.intervalMs = MAX(durationMs / abs(target - volFader.current), 10);
    volFader.lastTick = Get1MsTick();
}
