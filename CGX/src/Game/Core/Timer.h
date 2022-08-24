#pragma once

#include <chrono>


#define GET_TIME std::chrono::steady_clock::now();
#define SECONDS(period) std::chrono::duration<double, std::milli> (period).count() * (1.0 / 1000.0)
#define MILLISECONDS(period) std::chrono::duration<double, std::milli> (period).count()
#define NANOSECONDS(period) std::chrono::duration<double, std::nano> (period).count()

class Timer 
{
public:
	Timer() = default; 

public:
	float Get_Deltatime(); 
public:
	float time_since_last_frame;
};