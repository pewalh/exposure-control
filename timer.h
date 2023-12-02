#pragma once
#include <chrono>
#include <cstdint>

class Timer
{
public:
	Timer();
	~Timer();
	void start();
	void stop();
	void reset();
	bool isRunning();
	uint64_t getElapsedNano();
	uint64_t getElapsedMilli();
	uint64_t getElapsedSeconds();
	void getElapsedTime(int& days, int& hours, int& minutes, int& seconds, int& milliseconds, int& microseconds, int& nanoseconds);
	
private:
	bool running;
	std::chrono::time_point<std::chrono::high_resolution_clock> startTime;		

};

