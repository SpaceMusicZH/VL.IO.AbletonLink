#pragma once


#include "MyAbletonLink.h"


#ifdef ABLETONLINKDLL_EXPORTS
#define ABLETONLINKDLL_API __declspec(dllexport) 
#else
#define ABLETONLINKDLL_API __declspec(dllimport) 
#endif


extern "C" {

    ABLETONLINKDLL_API  void* CreateAbletonLink();
    ABLETONLINKDLL_API void DestroyAbletonLink(void* ptr);

    ABLETONLINKDLL_API void setup(void* ptr, double bpm);

    ABLETONLINKDLL_API void setTempo(void* ptr, double bpm);
    double ABLETONLINKDLL_API tempo(void* ptr);

    void ABLETONLINKDLL_API setQuantum(void* ptr, double quantum);
    double ABLETONLINKDLL_API quantum(void* ptr);

    bool ABLETONLINKDLL_API isEnabled(void* ptr);
    void ABLETONLINKDLL_API enable(void* ptr, bool bEnable);

    void ABLETONLINKDLL_API startPlaying(void* ptr);
    void ABLETONLINKDLL_API stopPlaying(void* ptr);
    bool ABLETONLINKDLL_API isPlaying(void* ptr);
    void ABLETONLINKDLL_API enableStartStopSync(void* ptr, bool bEnable);

    void ABLETONLINKDLL_API forceBeatAtTime(void* ptr, double beat);
    void ABLETONLINKDLL_API requestBeatAtTime(void* ptr, double beat);

    int ABLETONLINKDLL_API numPeers(void* ptr);
    void ABLETONLINKDLL_API update(void* ptr, double* rbeat, double* rphase, double* rtempo, double* rquantum, double* rtime, int* rnumPeers);

}