#include "pch.h"

#include <cmath>

#include "Ang.h"

float CAng::AN1P(float rad) {
	// convert angle radian to 1 PIE [7/2/2021 FSM]
	if (rad < PIE) rad = PIE2 + rad;
	if (rad > PIE) rad = rad - PIE2;
	return rad;
}

float CAng::AN2P(float rad) {
	// convert angle radian to 2 PIE [6/28/2021 FSM]
	rad = fmod(rad, PIE2);
	if (rad < 0) rad += PIE2;
	return rad;
}

float CAng::AN360(float deg) {
	// convert angle degree to 0 to 360 [7/2/2021 FSM]
	deg -= 360.f * std::floor(deg / 360.f);
	return deg;
}

float CAng::AN180(float deg) {
	// convert angle degree to -180 to 180 [7/2/2021 FSM]
	deg -= 360.f * std::floor((deg + 180.f) / 360.f);
	return deg;
}

// angle arithmetics [7/2/2021 FSM]

float CAng::SubR(float rd1, float rd2) {
#if 0
	const float c = PIE;
	//const float c = 180.f;
	return c - fabsf(fmodf(fabsf(rd1 - rd2), 2 * c) - c);
#endif

#if 0
	return  PIE - abs(abs(rd1 - rd2) - PIE);
#endif

#if 0
	// return the smallest angle [7/2/2021 FSM]
	return AR1P(min((2 * PIE) - fabsf(rd1 - rd2), fabsf(rd1 - rd2)));
#endif

#if 0
	float a = rd1 - rd2;
	a += (a > PIE) ? -PIE2 : (a < -PIE) ? PIE2 : 0;
	return a;
#endif

#if 1
	return atan2(sin(rd1 - rd2), cos(rd1 - rd2));
#endif
}