#ifndef AUDIO_EVENT_H
#define AUDIO_EVENT_H
#include "AudioGlobal.h"

typedef void(*EventCallBack)(void);

typedef struct __AudioEventConfig_t{
	void*	pParam;
	EventCallBack fp_CB;
	bool 	needDelayer;
	bool 	eventFlags;	
}AudioEventConfig_t;

void AudioEventConfig(void);
AudioEventConfig_t* GetAudioEventTable(void);
bool AudioEventRegister(int eventID,EventCallBack fp_CB,bool needDelayer);
void AudioEventTrigger(int eventID, void* pParam);
void AudioEventServer(void);

#endif

#include "AudioEvent.h"

AudioEventConfig_t s_audioEventTbl[AUDIO_EVENT_COUNTS];

void AudioEventConfig(void)
{
	int i = 0;
	for(i = 0;i < AUDIO_EVENT_COUNTS;i++){
		s_audioEventTbl[i].pParam = NULL;
		s_audioEventTbl[i].fp_CB = NULL;
		s_audioEventTbl[i].needDelayer = false;
		s_audioEventTbl[i].eventFlags = false;		
	}
}

void AudioEventServer(void)
{
	AudioEventConfig_t* pEvent = NULL;
	int i = 0;
	for(i=0;i<AUDIO_EVENT_COUNTS;i++){
		pEvent = &s_audioEventTbl[i];
		if(pEvent->needDelayer == true){
			if((pEvent->fp_CB != NULL) && (pEvent->eventFlags == true)){
				pEvent->fp_CB();
			}
		}
	}	
}

bool AudioEventRegister(int eventID,EventCallBack fp_CB,bool needDelayer)
{
	bool res =  false;
	if(eventID < AUDIO_EVENT_COUNTS){
		AudioEventConfig_t* pEvent = &s_audioEventTbl[eventID];
		if(pEvent->fp_CB != NULL && fp_CB != NULL){
			res = false;
			LOGI("[@AUDIO][EEROR] %s Failed to register the eventID:%d.\n",__func__,eventID);
		}else{
			pEvent->needDelayer = needDelayer;
			if(fp_CB != NULL){
				pEvent->fp_CB = fp_CB;
			}
			res = true;
		}
	}
	return res;
}

void AudioEventTrigger(int eventID, void* pParam)
{
	if(eventID >= AUDIO_EVENT_COUNTS){
		return;
	}
	AudioEventConfig_t* pEvent = &s_audioEventTbl[eventID];
	pEvent->pParam = pParam;
	pEvent->eventFlags = true;
	if(pEvent->fp_CB != NULL){
		if(pEvent->needDelayer == false){
			pEvent->fp_CB();
		}
	}
}

AudioEventConfig_t* GetAudioEventTable(void)
{
	return s_audioEventTbl;
}
