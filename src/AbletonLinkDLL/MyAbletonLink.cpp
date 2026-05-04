#include "pch.h"

#include "MyAbletonLink.h"

#include <algorithm>
#include <cmath>
#include <cstring>


MyAbletonLink::MyAbletonLink()
    : link_(nullptr),
    quantum_(4.0),
    isNumPeersChanged_(false),
    numPeers_(0),
    isTempoChanged_(false),
    tempo_(0.0),
    channelsChangeCounter_(0)
{
}

MyAbletonLink::~MyAbletonLink() {
    {
        std::lock_guard<std::mutex> lock(sinksMutex_);
        sinks_.clear();
    }
    {
        std::lock_guard<std::mutex> lock(sourcesMutex_);
        sources_.clear();
    }
    if (link_) {
        link_->enableLinkAudio(false);
        link_->enable(false);
        link_.reset();
    }
}

void MyAbletonLink::setup(double tempo) {
    {
        std::lock_guard<std::mutex> lock(sinksMutex_);
        sinks_.clear();
    }
    {
        std::lock_guard<std::mutex> lock(sourcesMutex_);
        sources_.clear();
    }
    if (link_) {
        link_->enableLinkAudio(false);
        link_->enable(false);
        link_.reset();
    }

    link_ = std::make_unique<ableton::LinkAudio>(tempo, std::string{"vvvv"});

    link_->setNumPeersCallback([this](std::size_t peers) {
        isNumPeersChanged_.store(true, std::memory_order_release);
        numPeers_.store(static_cast<int>(peers), std::memory_order_release);
    });

    link_->setTempoCallback([this](const double bpm) {
        isTempoChanged_.store(true, std::memory_order_release);
        tempo_.store(bpm, std::memory_order_release);
    });

    link_->setChannelsChangedCallback([this]() {
        channelsChangeCounter_.fetch_add(1, std::memory_order_acq_rel);
    });

    link_->enable(true);
}

void MyAbletonLink::setTempo(double bpm) {
    if (!link_) return;
    auto state = link_->captureAppSessionState();
    state.setTempo(bpm, link_->clock().micros());
    link_->commitAppSessionState(state);
}

double MyAbletonLink::tempo() {
    if (!link_) return 0.0;
    return link_->captureAppSessionState().tempo();
}

void MyAbletonLink::setQuantum(double quantum) {
    quantum_ = (std::min)((std::max)(quantum, 2.0), 16.0);
}

double MyAbletonLink::quantum() {
    return quantum_;
}

void MyAbletonLink::forceBeatAtTime(double beat) {
    if (!link_) return;
    auto state = link_->captureAppSessionState();
    state.forceBeatAtTime(beat, link_->clock().micros(), quantum_);
    link_->commitAppSessionState(state);
}

void MyAbletonLink::requestBeatAtTime(double beat) {
    if (!link_) return;
    auto state = link_->captureAppSessionState();
    state.requestBeatAtTime(beat, link_->clock().micros(), quantum_);
    link_->commitAppSessionState(state);
}

void MyAbletonLink::enable(bool bEnable) {
    if (!link_) return;
    link_->enable(bEnable);
}

bool MyAbletonLink::isEnabled() const {
    if (!link_) return false;
    return link_->isEnabled();
}

void MyAbletonLink::enableStartStopSync(bool bEnable) {
    if (!link_) return;
    link_->enableStartStopSync(bEnable);
}

void MyAbletonLink::startPlaying() {
    if (!link_) return;
    auto state = link_->captureAppSessionState();
    state.setIsPlaying(true, link_->clock().micros());
    link_->commitAppSessionState(state);
}

void MyAbletonLink::stopPlaying() {
    if (!link_) return;
    auto state = link_->captureAppSessionState();
    state.setIsPlaying(false, link_->clock().micros());
    link_->commitAppSessionState(state);
}

bool MyAbletonLink::isPlaying() {
    if (!link_) return false;
    return link_->captureAppSessionState().isPlaying();
}

std::size_t MyAbletonLink::numPeers() {
    if (!link_) return 0;
    return link_->numPeers();
}

MyAbletonLink::Status MyAbletonLink::update() {
    Status status;
    if (!link_) return status;

    auto state = link_->captureAppSessionState();
    const auto time = link_->clock().micros();

    status.beat = state.beatAtTime(time, quantum_);
    status.phase = state.phaseAtTime(time, quantum_);
    status.quantum = quantum_;
    status.tempo = state.tempo();
    status.time = static_cast<double>(
        std::chrono::duration_cast<std::chrono::milliseconds>(time).count());
    status.numPeers = static_cast<int>(link_->numPeers());

    return status;
}

// ----- Link Audio --------------------------------------------------------

void MyAbletonLink::enableLinkAudio(bool bEnable) {
    if (!link_) return;
    link_->enableLinkAudio(bEnable);
}

bool MyAbletonLink::isLinkAudioEnabled() const {
    if (!link_) return false;
    return link_->isLinkAudioEnabled();
}

void MyAbletonLink::setPeerName(const char* utf8) {
    if (!link_) return;
    link_->setPeerName(utf8 ? std::string{utf8} : std::string{});
}

const char* MyAbletonLink::peerName() {
    if (!link_) return "";
    peerNameCache_ = link_->peerName();
    return peerNameCache_.c_str();
}

uint64_t MyAbletonLink::channelsChangeCounter() const {
    return channelsChangeCounter_.load(std::memory_order_acquire);
}

uint64_t MyAbletonLink::idToU64(const ableton::link_audio::Id& id) {
    uint64_t out = 0;
    std::memcpy(&out, id.data(), sizeof(uint64_t));
    return out;
}

ableton::link_audio::Id MyAbletonLink::u64ToId(uint64_t value) {
    ableton::link_audio::Id id{};
    std::memcpy(id.data(), &value, sizeof(uint64_t));
    return id;
}

int MyAbletonLink::refreshChannels() {
    if (!link_) return 0;

    const auto channels = link_->channels();

    std::lock_guard<std::mutex> lock(channelsSnapshotMutex_);
    channelsSnapshot_.clear();
    channelsSnapshot_.reserve(channels.size());
    for (const auto& c : channels) {
        ChannelInfo info;
        info.idAsU64 = idToU64(c.id);
        info.name = c.name;
        info.peerIdAsU64 = idToU64(c.peerId);
        info.peerName = c.peerName;
        channelsSnapshot_.push_back(std::move(info));
    }
    return static_cast<int>(channelsSnapshot_.size());
}

bool MyAbletonLink::getChannel(int index,
                               uint64_t* outId,
                               const char** outName,
                               uint64_t* outPeerId,
                               const char** outPeerName) {
    std::lock_guard<std::mutex> lock(channelsSnapshotMutex_);
    if (index < 0 || static_cast<size_t>(index) >= channelsSnapshot_.size()) {
        return false;
    }
    const auto& c = channelsSnapshot_[index];
    if (outId)       *outId       = c.idAsU64;
    if (outName)     *outName     = c.name.c_str();
    if (outPeerId)   *outPeerId   = c.peerIdAsU64;
    if (outPeerName) *outPeerName = c.peerName.c_str();
    return true;
}

// ----- Sinks --------------------------------------------------------------

int MyAbletonLink::createSink(const char* name, int maxNumSamples) {
    if (!link_) return 0;
    if (maxNumSamples <= 0) maxNumSamples = 8192;

    auto entry = std::make_unique<SinkEntry>();
    entry->sink = std::make_unique<ableton::LinkAudioSink>(
        *link_,
        name ? std::string{name} : std::string{"vvvv"},
        static_cast<size_t>(maxNumSamples));
    entry->stagingI16.reserve(static_cast<size_t>(maxNumSamples));

    std::lock_guard<std::mutex> lock(sinksMutex_);
    const int handle = nextSinkHandle_++;
    sinks_.emplace(handle, std::move(entry));
    return handle;
}

void MyAbletonLink::setSinkName(int sinkHandle, const char* name) {
    std::lock_guard<std::mutex> lock(sinksMutex_);
    auto it = sinks_.find(sinkHandle);
    if (it == sinks_.end()) return;
    it->second->sink->setName(name ? std::string{name} : std::string{});
}

bool MyAbletonLink::sinkSend(int sinkHandle,
                             const float* interleaved,
                             int numFrames,
                             int numChannels,
                             uint32_t sampleRate) {
    if (!link_ || !interleaved) return false;
    if (numFrames <= 0) return false;
    if (numChannels != 1 && numChannels != 2) return false;

    ableton::LinkAudioSink* sinkPtr = nullptr;
    {
        std::lock_guard<std::mutex> lock(sinksMutex_);
        auto it = sinks_.find(sinkHandle);
        if (it == sinks_.end()) return false;
        sinkPtr = it->second->sink.get();
    }

    const size_t totalSamples = static_cast<size_t>(numFrames) * static_cast<size_t>(numChannels);

    // Grow the sink's max buffer if needed (no-op if already large enough).
    if (sinkPtr->maxNumSamples() < totalSamples) {
        sinkPtr->requestMaxNumSamples(totalSamples);
    }

    ableton::LinkAudioSink::BufferHandle handle{*sinkPtr};
    if (!handle) {
        // No matching source on the network, or no buffer available.
        return false;
    }
    if (handle.maxNumSamples < totalSamples) {
        return false;
    }

    // float [-1.0, 1.0] -> int16 with clamp
    int16_t* dst = handle.samples;
    for (size_t i = 0; i < totalSamples; ++i) {
        float v = interleaved[i];
        if (v > 1.0f) v = 1.0f;
        else if (v < -1.0f) v = -1.0f;
        // Asymmetric scale: 32767 keeps perfect 0 mapping and avoids overflow.
        dst[i] = static_cast<int16_t>(std::lrintf(v * 32767.0f));
    }

    auto state = link_->captureAppSessionState();
    const auto now = link_->clock().micros();
    const double beatsAtBufferBegin = state.beatAtTime(now, quantum_);

    return handle.commit(state,
                         beatsAtBufferBegin,
                         quantum_,
                         static_cast<size_t>(numFrames),
                         static_cast<size_t>(numChannels),
                         sampleRate);
}

void MyAbletonLink::destroySink(int sinkHandle) {
    std::lock_guard<std::mutex> lock(sinksMutex_);
    sinks_.erase(sinkHandle);
}

// ----- Sources ------------------------------------------------------------

int MyAbletonLink::createSource(uint64_t channelIdAsU64, int ringCapacitySamples) {
    if (!link_) return 0;
    if (ringCapacitySamples <= 0) ringCapacitySamples = 32768;

    auto entry = std::make_unique<SourceEntry>();
    entry->ring = std::make_unique<AudioRingBuffer>(static_cast<size_t>(ringCapacitySamples));

    AudioRingBuffer* ringPtr = entry->ring.get();
    SourceEntry* entryPtr = entry.get();

    entry->source = std::make_unique<ableton::LinkAudioSource>(
        *link_,
        u64ToId(channelIdAsU64),
        [ringPtr, entryPtr](ableton::LinkAudioSource::BufferHandle bufferHandle) {
            const size_t total = bufferHandle.info.numFrames * bufferHandle.info.numChannels;
            // int16 -> float on a small stack/heap buffer, then write to ring.
            // We avoid allocations on the hot path by using a thread_local vector.
            thread_local std::vector<float> tlBuffer;
            if (tlBuffer.size() < total) tlBuffer.resize(total);

            const int16_t* src = bufferHandle.samples;
            const float kScale = 1.0f / 32767.0f;
            for (size_t i = 0; i < total; ++i) {
                tlBuffer[i] = static_cast<float>(src[i]) * kScale;
            }
            ringPtr->write(tlBuffer.data(), total);
            entryPtr->numChannels.store(static_cast<int>(bufferHandle.info.numChannels),
                                        std::memory_order_release);
            entryPtr->sampleRate.store(static_cast<int>(bufferHandle.info.sampleRate),
                                       std::memory_order_release);
        });

    std::lock_guard<std::mutex> lock(sourcesMutex_);
    const int handle = nextSourceHandle_++;
    sources_.emplace(handle, std::move(entry));
    return handle;
}

int MyAbletonLink::sourceRead(int sourceHandle, float* dest, int requestedFrames, int numChannels) {
    if (!dest || requestedFrames <= 0 || numChannels <= 0) return 0;

    AudioRingBuffer* ringPtr = nullptr;
    {
        std::lock_guard<std::mutex> lock(sourcesMutex_);
        auto it = sources_.find(sourceHandle);
        if (it == sources_.end()) return 0;
        ringPtr = it->second->ring.get();
    }
    const size_t want = static_cast<size_t>(requestedFrames) * static_cast<size_t>(numChannels);
    return static_cast<int>(ringPtr->read(dest, want));
}

int MyAbletonLink::sourceAvailable(int sourceHandle) {
    AudioRingBuffer* ringPtr = nullptr;
    {
        std::lock_guard<std::mutex> lock(sourcesMutex_);
        auto it = sources_.find(sourceHandle);
        if (it == sources_.end()) return 0;
        ringPtr = it->second->ring.get();
    }
    return static_cast<int>(ringPtr->available());
}

int MyAbletonLink::sourceChannels(int sourceHandle) {
    std::lock_guard<std::mutex> lock(sourcesMutex_);
    auto it = sources_.find(sourceHandle);
    if (it == sources_.end()) return 0;
    return it->second->numChannels.load(std::memory_order_acquire);
}

int MyAbletonLink::sourceSampleRate(int sourceHandle) {
    std::lock_guard<std::mutex> lock(sourcesMutex_);
    auto it = sources_.find(sourceHandle);
    if (it == sources_.end()) return 0;
    return it->second->sampleRate.load(std::memory_order_acquire);
}

void MyAbletonLink::destroySource(int sourceHandle) {
    std::lock_guard<std::mutex> lock(sourcesMutex_);
    sources_.erase(sourceHandle);
}
