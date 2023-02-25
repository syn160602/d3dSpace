#include "myTimer.h"

using namespace std::chrono;

myTimer::myTimer() noexcept
{
	last = steady_clock::now();
}

float myTimer::Mark() noexcept
{
	const auto old = last;
	last = steady_clock::now();
	const duration<float> frameTime = last - old;
	return frameTime.count();
}

float myTimer::Peek() const noexcept
{
	return duration<float>(steady_clock::now() - last).count();
}