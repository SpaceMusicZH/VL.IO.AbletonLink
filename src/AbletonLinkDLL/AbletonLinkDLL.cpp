#include "pch.h"
#include "AbletonLinkDLL.h"


extern "C" {


    void* CreateAbletonLink()
    {
        return new MyAbletonLink();
    }

    void DestroyAbletonLink(void* ptr)
    {
        MyAbletonLink* link = static_cast<MyAbletonLink*>(ptr);
        delete link;
    }

    void setup(void* ptr, double bpm)
    {
        static_cast<MyAbletonLink*>(ptr)->setup(bpm);
    }

    void setTempo(void* ptr, double bpm)
    {
        static_cast<MyAbletonLink*>(ptr)->setTempo(bpm);
    }

    double tempo(void* ptr)
    {
        return static_cast<MyAbletonLink*>(ptr)->tempo();
    }

    void setQuantum(void* ptr, double quantum)
    {
        static_cast<MyAbletonLink*>(ptr)->setQuantum(quantum);
    }

    double quantum(void* ptr)
    {
        return static_cast<MyAbletonLink*>(ptr)->quantum();
    }

    void forceBeatAtTime(void* ptr, double beat)
    {
        static_cast<MyAbletonLink*>(ptr)->forceBeatAtTime(beat);
    }

    void requestBeatAtTime(void* ptr, double beat)
    {
        static_cast<MyAbletonLink*>(ptr)->requestBeatAtTime(beat);
    }

    void enable(void* ptr, bool bEnable)
    {
        static_cast<MyAbletonLink*>(ptr)->enable(bEnable);
    }

    bool isEnabled(void* ptr)
    {
        return static_cast<MyAbletonLink*>(ptr)->isEnabled();
    }

    void startPlaying(void* ptr)
    {
        static_cast<MyAbletonLink*>(ptr)->startPlaying();
    }

    void stopPlaying(void* ptr)
    {
        static_cast<MyAbletonLink*>(ptr)->stopPlaying();
    }

    bool isPlaying(void* ptr)
    {
        return static_cast<MyAbletonLink*>(ptr)->isPlaying();
    }

    void enableStartStopSync(void* ptr, bool bEnable)
    {
        static_cast<MyAbletonLink*>(ptr)->enableStartStopSync(bEnable);
    }

    int numPeers(void* ptr)
    {
        return static_cast<int>(static_cast<MyAbletonLink*>(ptr)->numPeers());
    }

    void update(void* ptr, double* rbeat, double* rphase, double* rtempo, double* rquantum, double* rtime, int* rnumPeers)
    {
        MyAbletonLink::Status s = static_cast<MyAbletonLink*>(ptr)->update();
        *rbeat = s.beat;
        *rphase = s.phase;
        *rtempo = s.tempo;
        *rquantum = s.quantum;
        *rtime = s.time;
        *rnumPeers = s.numPeers;
    }

    // ----- Link Audio --------------------------------------------------

    void enableLinkAudio(void* ptr, bool bEnable)
    {
        static_cast<MyAbletonLink*>(ptr)->enableLinkAudio(bEnable);
    }

    bool isLinkAudioEnabled(void* ptr)
    {
        return static_cast<MyAbletonLink*>(ptr)->isLinkAudioEnabled();
    }

    void setPeerName(void* ptr, const char* utf8)
    {
        static_cast<MyAbletonLink*>(ptr)->setPeerName(utf8);
    }

    const char* peerName(void* ptr)
    {
        return static_cast<MyAbletonLink*>(ptr)->peerName();
    }

    uint64_t channelsChangeCounter(void* ptr)
    {
        return static_cast<MyAbletonLink*>(ptr)->channelsChangeCounter();
    }

    int refreshChannels(void* ptr)
    {
        return static_cast<MyAbletonLink*>(ptr)->refreshChannels();
    }

    bool getChannel(void* ptr, int index, uint64_t* outId, const char** outName, uint64_t* outPeerId, const char** outPeerName)
    {
        return static_cast<MyAbletonLink*>(ptr)->getChannel(index, outId, outName, outPeerId, outPeerName);
    }

    int createSink(void* ptr, const char* name, int maxNumSamples)
    {
        return static_cast<MyAbletonLink*>(ptr)->createSink(name, maxNumSamples);
    }

    void setSinkName(void* ptr, int sinkHandle, const char* name)
    {
        static_cast<MyAbletonLink*>(ptr)->setSinkName(sinkHandle, name);
    }

    bool sinkSend(void* ptr, int sinkHandle, const float* interleaved, int numFrames, int numChannels, uint32_t sampleRate)
    {
        return static_cast<MyAbletonLink*>(ptr)->sinkSend(sinkHandle, interleaved, numFrames, numChannels, sampleRate);
    }

    void destroySink(void* ptr, int sinkHandle)
    {
        static_cast<MyAbletonLink*>(ptr)->destroySink(sinkHandle);
    }

    int createSource(void* ptr, uint64_t channelId, int ringCapacitySamples)
    {
        return static_cast<MyAbletonLink*>(ptr)->createSource(channelId, ringCapacitySamples);
    }

    int sourceRead(void* ptr, int sourceHandle, float* dest, int requestedFrames, int numChannels)
    {
        return static_cast<MyAbletonLink*>(ptr)->sourceRead(sourceHandle, dest, requestedFrames, numChannels);
    }

    int sourceAvailable(void* ptr, int sourceHandle)
    {
        return static_cast<MyAbletonLink*>(ptr)->sourceAvailable(sourceHandle);
    }

    int sourceChannels(void* ptr, int sourceHandle)
    {
        return static_cast<MyAbletonLink*>(ptr)->sourceChannels(sourceHandle);
    }

    int sourceSampleRate(void* ptr, int sourceHandle)
    {
        return static_cast<MyAbletonLink*>(ptr)->sourceSampleRate(sourceHandle);
    }

    void destroySource(void* ptr, int sourceHandle)
    {
        static_cast<MyAbletonLink*>(ptr)->destroySource(sourceHandle);
    }
}
