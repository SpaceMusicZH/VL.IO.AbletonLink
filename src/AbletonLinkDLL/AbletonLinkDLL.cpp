// AbletonLinkDLL.cpp : Definiert die exportierten Funktionen für die DLL-Anwendung.
//

#include "stdafx.h"
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
			MyAbletonLink* link = static_cast<MyAbletonLink*>(ptr);
			link->setup(bpm);
		}

		void setTempo(void* ptr, double bpm)
		{
			MyAbletonLink* link = static_cast<MyAbletonLink*>(ptr);
			link->setTempo(bpm);
		}

		double tempo(void* ptr)
		{
			MyAbletonLink* link = static_cast<MyAbletonLink*>(ptr);
			return link->tempo();
		}

		void setQuantum(void* ptr, double quantum)
		{
			MyAbletonLink* link = static_cast<MyAbletonLink*>(ptr);
			link->setQuantum(quantum);
		}

		double quantum(void* ptr)
		{
			MyAbletonLink* link = static_cast<MyAbletonLink*>(ptr);
			return link->quantum();
		}

		bool isEnabled(void* ptr)
		{
			MyAbletonLink* link = static_cast<MyAbletonLink*>(ptr);
			return link->isEnabled();
		}

		void enable(void* ptr, bool bEnable)
		{
			MyAbletonLink* link = static_cast<MyAbletonLink*>(ptr);
			link->enable(bEnable);
		}


		void startPlaying(void* ptr)
		{
			MyAbletonLink* link = static_cast<MyAbletonLink*>(ptr);
			link->startPlaying();
		}

		void stopPlaying(void* ptr)
		{
			MyAbletonLink* link = static_cast<MyAbletonLink*>(ptr);
			link->stopPlaying();
		}

		bool isPlaying(void* ptr)
		{
			MyAbletonLink* link = static_cast<MyAbletonLink*>(ptr);
			return link->isPlaying();
		}

		void enableStartStopSync(void* ptr, bool bEnable)
		{
			MyAbletonLink* link = static_cast<MyAbletonLink*>(ptr);
			link->enableStartStopSync(bEnable);
		}

		int numPeers(void* ptr)
		{
			MyAbletonLink* link = static_cast<MyAbletonLink*>(ptr);
			return static_cast<int>(link->numPeers());
		}

		void update(void* ptr, double* rbeat, double* rphase)
		{
			MyAbletonLink* link = static_cast<MyAbletonLink*>(ptr);
			MyAbletonLink::Status s = link->update();
			*rbeat = s.beat;
			*rphase = s.phase;
		}

	
}



