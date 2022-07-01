#ifndef MyAbletonLink_h
#define MyAbletonLink_h

//#if __cplusplus<201103L
//#error C++11 features are required to compile this source code.
//#endif

#if defined(__APPLE__)
#define LINK_PLATFORM_MACOSX 1
#elif defined(_WIN32)
#define LINK_PLATFORM_WINDOWS 1
#elif defined(__linux__)
#define LINK_PLATFORM_LINUX 1
#else
#error This platform is not supported.
#endif

#include "link\include\ableton\Link.hpp"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>

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
    MyAbletonLink();
    ~MyAbletonLink();
    
    MyAbletonLink(const MyAbletonLink&) = delete;
    MyAbletonLink& operator=(const ableton::Link&) = delete;
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
    
private:
    ableton::Link* link_;
    double quantum_;

    bool isNumPeersChanged_;
    int numPeers_;

    bool isTempoChanged_;
    double tempo_;
};


#endif /* MyAbletonLink_h */
