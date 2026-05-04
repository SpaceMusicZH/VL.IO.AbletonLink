#pragma once

#include "MyAbletonLink.h"

#include <cstdint>


#ifdef ABLETONLINKDLL_EXPORTS
#define ABLETONLINKDLL_API __declspec(dllexport)
#else
#define ABLETONLINKDLL_API __declspec(dllimport)
#endif


extern "C" {

    // ----- Lifecycle / existing v1 API ------------------------------------

    ABLETONLINKDLL_API void* CreateAbletonLink();
    ABLETONLINKDLL_API void DestroyAbletonLink(void* ptr);

    ABLETONLINKDLL_API void setup(void* ptr, double bpm);

    ABLETONLINKDLL_API void setTempo(void* ptr, double bpm);
    ABLETONLINKDLL_API double tempo(void* ptr);

    ABLETONLINKDLL_API void setQuantum(void* ptr, double quantum);
    ABLETONLINKDLL_API double quantum(void* ptr);

    ABLETONLINKDLL_API bool isEnabled(void* ptr);
    ABLETONLINKDLL_API void enable(void* ptr, bool bEnable);

    ABLETONLINKDLL_API void startPlaying(void* ptr);
    ABLETONLINKDLL_API void stopPlaying(void* ptr);
    ABLETONLINKDLL_API bool isPlaying(void* ptr);
    ABLETONLINKDLL_API void enableStartStopSync(void* ptr, bool bEnable);

    ABLETONLINKDLL_API void forceBeatAtTime(void* ptr, double beat);
    ABLETONLINKDLL_API void requestBeatAtTime(void* ptr, double beat);

    ABLETONLINKDLL_API int numPeers(void* ptr);
    ABLETONLINKDLL_API void update(void* ptr,
                                   double* rbeat,
                                   double* rphase,
                                   double* rtempo,
                                   double* rquantum,
                                   double* rtime,
                                   int* rnumPeers);

    // ----- Link Audio (4.0+) ----------------------------------------------

    ABLETONLINKDLL_API void enableLinkAudio(void* ptr, bool bEnable);
    ABLETONLINKDLL_API bool isLinkAudioEnabled(void* ptr);

    ABLETONLINKDLL_API void setPeerName(void* ptr, const char* utf8);
    ABLETONLINKDLL_API const char* peerName(void* ptr);

    ABLETONLINKDLL_API uint64_t channelsChangeCounter(void* ptr);
    ABLETONLINKDLL_API int refreshChannels(void* ptr);
    ABLETONLINKDLL_API bool getChannel(void* ptr,
                                       int index,
                                       uint64_t* outId,
                                       const char** outName,
                                       uint64_t* outPeerId,
                                       const char** outPeerName);

    ABLETONLINKDLL_API int createSink(void* ptr, const char* name, int maxNumSamples);
    ABLETONLINKDLL_API void setSinkName(void* ptr, int sinkHandle, const char* name);
    ABLETONLINKDLL_API bool sinkSend(void* ptr,
                                     int sinkHandle,
                                     const float* interleaved,
                                     int numFrames,
                                     int numChannels,
                                     uint32_t sampleRate);
    ABLETONLINKDLL_API void destroySink(void* ptr, int sinkHandle);

    ABLETONLINKDLL_API int createSource(void* ptr,
                                        uint64_t channelId,
                                        int ringCapacitySamples);
    ABLETONLINKDLL_API int sourceRead(void* ptr,
                                      int sourceHandle,
                                      float* dest,
                                      int requestedFrames,
                                      int numChannels);
    ABLETONLINKDLL_API int sourceAvailable(void* ptr, int sourceHandle);
    ABLETONLINKDLL_API int sourceChannels(void* ptr, int sourceHandle);
    ABLETONLINKDLL_API int sourceSampleRate(void* ptr, int sourceHandle);
    ABLETONLINKDLL_API void destroySource(void* ptr, int sourceHandle);

}
