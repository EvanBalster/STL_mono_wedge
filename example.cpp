#include "mono_wedge.h"

#include <deque>
#include <cmath>

float generateSignal(int time)
{
	return std::sin(.1f*time) * std::exp(-.1f*time);
}

struct Sample
{
	float value;
	int   time;
  
	bool operator<(const Sample &o) const    {return value < o.value;}
}

void example()
{
	std::deque<float> max_wedge;
	
	int rangeSize = 100;
	
	for (int time = 0; time < 1000; ++time)
	{
		// Generate a new sample with timestamp
		Sample sample = {generateSignal(time), time};
		
		// Add the new sample to our wedge
		max_wedge_update(wedge, sample);
		
		// Get rid of old samples outside our rolling range
		while (wedge.front().time <= time-rangeSize) wedge.pop_front();
		
		// The maximum value is at the front of the wedge.
		float maximumInRange = wedge.front();
	}
}
