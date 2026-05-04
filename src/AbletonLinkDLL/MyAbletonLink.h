#ifndef MyAbletonLink_h
#define MyAbletonLink_h

#if defined(__APPLE__)
#define LINK_PLATFORM_MACOSX 1
#elif defined(_WIN32)
#define LINK_PLATFORM_WINDOWS 1
#elif defined(__linux__)
#define LINK_PLATFORM_LINUX 1
#else
#error This platform is not supported.
#endif

#include "AudioRingBuffer.h"

#include "link\include\ableton\LinkAudio.hpp"

#include <atomic>
#include <chrono>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

class MyAbletonLink {

public:
    struct Status
    {
        double beat;
        double phase;
        double tempo;
        double quantum;
        double time;
        int numPeers;
        Status() : beat(0.0), phase(0.0), tempo(0.0), quantum(0.0), time(0.0), numPeers(0) {}
    };

    struct ChannelInfo
    {
        uint64_t idAsU64;
        std::string name;
        uint64_t peerIdAsU64;
        std::string peerName;
    };

    MyAbletonLink();
    ~MyAbletonLink();

    MyAbletonLink(const MyAbletonLink&) = delete;
    MyAbletonLink& operator=(const MyAbletonLink&) = delete;
    MyAbletonLink(MyAbletonLink&&) = delete;
    MyAbletonLink& operator=(MyAbletonLink&&) = delete;

    void setup(double bpm);

    void setTempo(double bpm);
    double tempo();

    void setQuantum(double quantum);
    double quantum();

    void enable(bool bEnable);
    bool isEnabled() const;

    void forceBeatAtTime(double beat);
    void requestBeatAtTime(double beat);

    void startPlaying();
    void stopPlaying();
    bool isPlaying();

    void enableStartStopSync(bool bEnable);

    std::size_t numPeers();

    Status update();

    // ----- Link Audio (4.0+) -----------------------------------------------

    void enableLinkAudio(bool bEnable);
    bool isLinkAudioEnabled() const;

    void setPeerName(const char* utf8);
    const char* peerName();

    // Refresh the channels snapshot. Returns the number of channels visible
    // in the session (after refresh). Channel data is stable until the next
    // call to refreshChannels.
    int refreshChannels();
    bool getChannel(int index,
                    uint64_t* outId,
                    const char** outName,
                    uint64_t* outPeerId,
                    const char** outPeerName);

    // Monotonic counter that bumps every time the channels-changed callback
    // fires. C# polls this to know when to call refreshChannels.
    uint64_t channelsChangeCounter() const;

    // ----- Sinks (publish) -------------------------------------------------

    int createSink(const char* name, int maxNumSamples);
    void setSinkName(int sinkHandle, const char* name);
    bool sinkSend(int sinkHandle,
                  const float* interleaved,
                  int numFrames,
                  int numChannels,
                  uint32_t sampleRate);
    void destroySink(int sinkHandle);

    // ----- Sources (subscribe) ---------------------------------------------

    int createSource(uint64_t channelIdAsU64, int ringCapacitySamples);
    int sourceRead(int sourceHandle, float* dest, int requestedFrames, int numChannels);
    int sourceAvailable(int sourceHandle);
    int sourceChannels(int sourceHandle);
    int sourceSampleRate(int sourceHandle);
    void destroySource(int sourceHandle);

private:
    struct SinkEntry
    {
        std::unique_ptr<ableton::LinkAudioSink> sink;
        std::vector<int16_t> stagingI16; // float -> int16 staging buffer
    };

    struct SourceEntry
    {
        // Declaration order matters for destruction: members are destroyed in
        // reverse, so `source` (last) is destroyed first, ensuring its callback
        // is cancelled before the `ring` it writes into goes away.
        std::unique_ptr<AudioRingBuffer> ring;
        std::atomic<int> numChannels{0};
        std::atomic<int> sampleRate{0};
        std::unique_ptr<ableton::LinkAudioSource> source;
    };

    std::unique_ptr<ableton::LinkAudio> link_;
    double quantum_;

    std::atomic<bool> isNumPeersChanged_;
    std::atomic<int> numPeers_;
    std::atomic<bool> isTempoChanged_;
    std::atomic<double> tempo_;

    std::atomic<uint64_t> channelsChangeCounter_;

    std::mutex channelsSnapshotMutex_;
    std::vector<ChannelInfo> channelsSnapshot_;

    // peerName(): returns a pointer that stays valid until the next call.
    std::string peerNameCache_;

    std::mutex sinksMutex_;
    std::unordered_map<int, std::unique_ptr<SinkEntry>> sinks_;
    int nextSinkHandle_ = 1;

    std::mutex sourcesMutex_;
    std::unordered_map<int, std::unique_ptr<SourceEntry>> sources_;
    int nextSourceHandle_ = 1;

    static uint64_t idToU64(const ableton::link_audio::Id& id);
    static ableton::link_audio::Id u64ToId(uint64_t value);
};


#endif /* MyAbletonLink_h */
