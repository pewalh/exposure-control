#pragma once
#include <chrono>

class Timer
{
	public:
		Timer();
		~Timer();
		void start();
		void stop();
		void reset();
		bool isRunning();
		long long getElapsedNano();
		long long getElapsedMilli();
		long long getElapsedSeconds();
		void getElapsedTime(long& days, long& hours, long& minutes, long& seconds, long& milliseconds, long& microseconds, long& nanoseconds);
	private:
		bool running;
		std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
		
};

