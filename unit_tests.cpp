//
//  main.cpp
//  unit tests for monotonic wedge utility
//
//  Created by Evan Balster on 9/1/16.
//  Copyright © 2016 Evan Balster. All rights reserved.
//

#include <iostream>

#include <vector>
#include <deque>

#include <cstdlib> // For rand()
#include <cmath>   // For sin()

#include "mono_wedge.h"

#include "stl_ringbuffer.h"

using namespace mono_wedge;

struct Sample
{
	unsigned time;
	float    value;
	
	bool operator<(const Sample &o) const    {return value < o.value;}
	bool operator>(const Sample &o) const    {return value > o.value;}
};

typedef std::vector<float> Signal;
typedef fixed_ringbuffer<Sample> Wedge;

bool test(const Signal &signal, unsigned interval = 0)
{
	bool success = true;
	
	if (interval == 0) interval = unsigned(signal.size());
	
	Wedge
		min_wedge(interval), //, Wedge::resize_policy::no_resize),
		max_wedge(interval); //, Wedge::resize_policy::no_resize);
	
	//std::deque<Sample>
	//	min_wedge, //, Wedge::resize_policy::no_resize),
	//	max_wedge;
	
	for (unsigned t = 0; t < signal.size(); ++t)
	{
		if ((t&255)==0) std::cout << '.' << std::flush;
		
		float value = signal[t];
		
		Sample sample = {t, value};
		
		/*if (min_wedge_search(min_wedge.begin(), min_wedge.end(), sample) == min_wedge.begin())
		{
			std::cout << "      New global min: t=" << t << ", value=" << value << std::endl;
		}*/
		
		// Update the wedge
		min_wedge_update(min_wedge, sample);
		max_wedge_update(max_wedge, sample);
		
		// Pop old samples
		while (t - min_wedge.front().time >= interval) min_wedge.pop_front();
		while (t - max_wedge.front().time >= interval) max_wedge.pop_front();
		
		// Compare wedge result with actual min/max
		float refMin = 1e18f, refMax = -1e18f;
		for (unsigned ot = t-std::min(t, interval-1); ot <= t; ++ot)
		{
			refMin = std::min(refMin, signal[ot]);
			refMax = std::max(refMax, signal[ot]);
		}
		
		if (refMin != min_wedge.front().value)
		{
			std::cout << "      (min inconsistent at t=" << t
				<< ": wedge-min=" << min_wedge.front().value << ", actual=" << refMin
				<< std::endl;
			success = false;
			//break;
		}
		if (refMax != max_wedge.front().value)
		{
			std::cout << "      (max inconsistent at t=" << t
				<< ": wedge-max=" << max_wedge.front().value << ", actual=" << refMax
				<< std::endl;
			success = false;
			//break;
		}
	}
	
	std::cout << std::endl;
	
	std::cout << "      " << (success ? "...OK" : "...FAILED") << std::endl;
		
	return success;
}

int main(int argc, const char * argv[])
{
	Signal white, brown, red, whiteUp, whiteDn, sine, square, noisySine;
	
	std::cout << "Synthesizing test signals..." << std::endl;
	
	float randP = 0.f, randC = 0.f, brownState = 0.f;
	for (unsigned i = 0; i < 16384; ++i)
	{
		// Generate white noise signal
		randC = float(std::rand()) / float(RAND_MAX);
		randC = 2.0f*randC - 1.0f;
		
		float sineC = std::sin(.01f * i);
		
		brownState += randC;
		
		white  .push_back(randC);
		whiteUp.push_back(.01f * float(i) + randC);
		whiteDn.push_back(.01f * float(i) + randC);
		brown  .push_back(brownState);
		red    .push_back(randC - randP);
		sine   .push_back(sineC);
		square .push_back((i&64u) ? 1.0f : -1.0f);
		noisySine.push_back(sineC+randC);
		
		randP = randC;
	}
	
	std::cout << "Testing..." << std::endl;
	
	bool success = true;
	
	for (unsigned w = 0; w < 3; ++w)
	{
		unsigned interval = 0;
		switch (w)
		{
			case 0: interval = 32;   break;
			case 1: interval = 512;  break;
			case 2: interval = 4096; break;
		}
		
		std::cout << "  Interval = " << interval << std::endl;
		
		std::cout << "    White:" << std::endl;
		success &= test(white);
		std::cout << "    White ascending:" << std::endl;
		success &= test(whiteUp);
		std::cout << "    White descending:" << std::endl;
		success &= test(whiteDn);
		std::cout << "    Brown:" << std::endl;
		success &= test(brown);
		std::cout << "    Red:" << std::endl;
		success &= test(red);
		std::cout << "    Sine:" << std::endl;
		success &= test(sine);
		std::cout << "    Square:" << std::endl;
		success &= test(square);
		std::cout << "    Noisy Sine:" << std::endl;
		success &= test(noisySine);
	}
	
	return success ? 0 : 1;
}
