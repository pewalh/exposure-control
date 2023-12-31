#include "timer.h"
#include <chrono>
#include <cstdint>

Timer::Timer() {
	running = false;
}

Timer::~Timer() {
}

void Timer::start() {
	startTime = std::chrono::high_resolution_clock::now();
	running = true;
}

void Timer::stop() {
	running = false;
}

void Timer::reset() {
	startTime = std::chrono::high_resolution_clock::now();
}

bool Timer::isRunning() {
	return running;
}

uint64_t Timer::getElapsedNano() {
	if (running) {
		auto elapsed = std::chrono::high_resolution_clock::now() - startTime;
		return std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count();
	}
	else {
		return 0;
	}
}

uint64_t Timer::getElapsedMilli() {
	if (running) {
		auto elapsed = std::chrono::high_resolution_clock::now() - startTime;
		return std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
	}
	else {
		return 0;
	}
}

uint64_t Timer::getElapsedSeconds() {
	if (running) {
		auto elapsed = std::chrono::high_resolution_clock::now() - startTime;
		return std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();
	}
	else {
		return 0;
	}
}

void Timer::getElapsedTime(int& days, int& hours, int& minutes, int& seconds, int& milliseconds, int& microseconds, int& nanoseconds) {
	if (running) {
		auto elapsed = std::chrono::high_resolution_clock::now() - startTime;
		uint64_t _nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count();
		nanoseconds = _nanoseconds % 1000;
		microseconds = (_nanoseconds / 1000) % 1000;
		milliseconds = (_nanoseconds / 1000000) % 1000;
		seconds = (_nanoseconds / 1000000000) % 60;
		minutes = (_nanoseconds / 60000000000) % 60;
		hours = (_nanoseconds / 3600000000000) % 24;
		days = (_nanoseconds / 86400000000000);
	}
	else {
		days = 0;
		hours = 0;
		minutes = 0;
		seconds = 0;
		milliseconds = 0;
		microseconds = 0;
		nanoseconds = 0;
	}
}

