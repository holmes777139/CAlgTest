#ifndef _NON_BLOCKING_DELAY_H_
#define _NON_BLOCKING_DELAY_H_
#include "AudioGlobal.h"

#define AUDIO_DELAYER_MAX 		128
#define DELAYER_ALLOCATE_FLAG 	0x8000//BIT15
#define DELAYER_COUNTING_FLAG 	1//BIT0
#define DELAYER_PERIOD_FLAG		0x80//BIT7
#define DELAYER_TIMEOUT_FLAG	2//BIT1

// 定义优先级枚举量
typedef enum {
	PRIORITY_BASIC = 1,		// 基础服务，1毫秒调用一次 
    PRIORITY_LOW = 1000,    // 低优先级,1秒钟调用一次
    PRIORITY_MEDIUM = 500,  // 中等优先级，500毫秒调用一次
    PRIORITY_HIGH = 100,    // 高优先级，100毫秒调用一次
    PRIORITY_CRITICAL = 1   // 最高优先级，1毫秒调用一次
} PriorityLevel;

typedef struct __AudioDelayer{
	int delay_time;
	int flags;
	int period_time;
	void(*cb)(void);
}AudioDelayer;

typedef struct __AudioDelayerCtrl_t{
	int usedDelayer;
	AudioDelayer ad[AUDIO_DELAYER_MAX];
}AudioDelayerCtrl_t;

#define FSM_GET()	(__LINE__ - 1);
#define FSM_SET(bp)	fsm_state= bp; return;
#define FSM_RESET()	fsm_state = 0; return;
#define FSM_START()	static int fsm_state = 0; switch(fsm_state) {case 0:
#define FSM_NEXT()	fsm_state = __LINE__; return; case __LINE__:
#define FSM_STOP()	fsm_state = 0; return;}

void AudioDelayerConfig(void); 
void AudioDelayerServer(void);
bool AudioDelayerRegister(AudioDelayer** ppAd);
int AudioDelay(AudioDelayer* pAd, uint32_t delayMs);
void AudioDelayerReset(AudioDelayer* pAd);
void BareMetalTaskCreater(uint32_t delayMs,void(*fp_CallBack)(void),bool isPeriod);

#define NON_BLOCKING_WAIT(ms,ad)  do {  											\
	if(ad == NULL) return;															\
    if((AudioDelay(ad, ms) & DELAYER_COUNTING_FLAG) == DELAYER_COUNTING_FLAG) {   	\
        return;                                                 					\
    }			                                                    				\
	AudioDelayerReset(ad);                       									\
} while(0)

#define NON_BLOCKING_WAIT_RET(ms,ad)  do {  										\
	if(ad == NULL) return 0;														\
    if((AudioDelay(ad, ms) & DELAYER_COUNTING_FLAG) == DELAYER_COUNTING_FLAG) {   	\
        return 0;                                                 					\
    }			                                                    				\
	AudioDelayerReset(ad);                       									\
} while(0)

#endif
#include "AudioDelayer.h"

AudioDelayerCtrl_t s_audioDelayerCtrl;

void AudioDelayerConfig(void)
{
	int i = 0;	
	AudioDelayerCtrl_t *pAdCtrl = &s_audioDelayerCtrl;
	AudioDelayer* pAd = NULL;
	for(i=0;i<AUDIO_DELAYER_MAX;i++){
		pAd = &pAdCtrl->ad[i];
		pAd->delay_time = 0;
		pAd->flags = 0;
		pAd->period_time = 0;
		pAd->cb = NULL;
	}
	pAdCtrl->usedDelayer =0;
}

void AudioDelayerServer(void)
{
	int i = 0;
	AudioDelayerCtrl_t *pAdCtrl = &s_audioDelayerCtrl;
	AudioDelayer* pAd = NULL;
 	for(i=0;i< pAdCtrl->usedDelayer ;i++){
		pAd = &pAdCtrl->ad[i];
		if(pAd->flags & DELAYER_COUNTING_FLAG || pAd->flags & DELAYER_PERIOD_FLAG){
			if(pAd->delay_time-- == 0){
				pAd->flags |=  DELAYER_TIMEOUT_FLAG;
				pAd->flags &=  ~DELAYER_COUNTING_FLAG;
				if(pAd->cb != NULL){
					pAd->cb();
					if(pAd->flags & DELAYER_PERIOD_FLAG){
						pAd->delay_time = pAd->period_time;
					}
				}				
			}
		}
	}	
}

bool AudioDelayerRegister(AudioDelayer** ppAd)
{
	bool res = true;
	AudioDelayerCtrl_t *pAdCtrl = &s_audioDelayerCtrl;
	int ind = pAdCtrl->usedDelayer;
	AudioDelayer* pAd = &pAdCtrl->ad[ind];
	if(ind >= AUDIO_DELAYER_MAX || pAd->flags & DELAYER_ALLOCATE_FLAG){
		ppAd = NULL;
		res = false;
		LOGI("[@AUDIO][EEROR] %s Failed to register the delayer:%d.\n",__func__,ind);
	}else{	
		pAd->flags |= DELAYER_ALLOCATE_FLAG;
		*ppAd = pAd;
		pAdCtrl->usedDelayer++;
	}
	return res;
}

int AudioDelay(AudioDelayer* pAd, uint32_t delayMs)
{
	uint32_t realTicks = 0;
#ifdef 	AUDIO_DELAYER_PERFORMANCE
	realTicks = delayMs == 0 ? 1 : (delayMs + 1) / IHC_AUDIO_PROJECT_TICKS;
#else
	realTicks = delayMs;
#endif
	
	if(!(pAd->flags & DELAYER_TIMEOUT_FLAG) && !(pAd->flags & DELAYER_COUNTING_FLAG))
	{	
		#ifdef 	AUDIO_DELAYER_PERFORMANCE
			pAd->delay_time = realTicks * 10;//100us * 10 = 1ms
		#else
			pAd->delay_time = realTicks;//1ms
		#endif
		pAd->flags |= DELAYER_COUNTING_FLAG;
	}
	return pAd->flags;
}

void AudioDelayerReset(AudioDelayer* pAd)
{
	pAd->flags &= ~DELAYER_COUNTING_FLAG;
	pAd->flags &= ~DELAYER_TIMEOUT_FLAG;
	pAd->delay_time =0;
}

void BareMetalTaskCreater(uint32_t delayMs,void(*fp_CallBack)(void),bool isPeriod)
{
	uint32_t realTicks = 0;
#ifdef 	AUDIO_DELAYER_PERFORMANCE
	realTicks = delayMs == 0 ? 1 : (delay_time + 1) / IHC_AUDIO_PROJECT_TICKS;
#else
	realTicks = delayMs;
#endif
	
	AudioDelayer *  pAd = NULL;
	if(!AudioDelayerRegister(&pAd)){
		LOGI("[@AUDIO][EEROR] %s Failed to create the task.\n",__func__);
		return;
	}
	
	if(isPeriod){
		#ifdef 	AUDIO_DELAYER_PERFORMANCE
			pAd->period_time = realTicks * 10;//100us * 10 = 1ms
		#else
			pAd->period_time = realTicks;//1ms
		#endif
	}else{
		pAd->period_time = 0;
	}
	pAd->flags |= DELAYER_PERIOD_FLAG;//周期性软件定时器
	pAd->cb = fp_CallBack;
}
