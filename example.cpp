#include "mono_wedge.h"

#include <deque>
#include <cmath>
#include <iostream>
#include <fstream>

float generateSignal(int time)
{
	return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

struct Sample
{
	float value;
	int   time;

	bool operator<(const Sample& o) const { return value < o.value; }
	bool operator>(const Sample& o) const { return value > o.value; }
};

std::ostream& operator<<(std::ostream& os, const Sample& sample)
{
	os << sample.time << '/' << sample.value;
	return os;
}

void example()
{
	std::deque<Sample> max_wedge;
	std::ofstream out_file("output.csv");
	const int rangeSize = 20;

	for (int time = 0; time < 1000; ++time)
	{
		// Generate a new sample with timestamp
		Sample sample = { generateSignal(time), time };

		// Add the new sample to our wedge
		mono_wedge::max_wedge_update(max_wedge, sample);

		// Get rid of old samples outside our rolling range
		const auto window_border = time - rangeSize;
		while (max_wedge.front().time <= window_border) {
			auto front = max_wedge.front();
			std::cout << "  - Remove " << front << " (older than " << rangeSize << ")\n";
			max_wedge.pop_front();
		}

		// The maximum value is at the front of the wedge.
		auto maximumInRange = max_wedge.front();

		std::cout << sample << "\tMax=" << maximumInRange << "\n   Wedge: ";
		for(auto &i : max_wedge)
		{
			std::cout << i << '\t';
		}
		std::cout << "\n\n";

		out_file << sample.time << ";" << sample.value << ";" << maximumInRange.value << "\n";
	}
}

int main(void)
{
	example();
}