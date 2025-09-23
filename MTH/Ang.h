#pragma once

#ifndef PIE
#define PIE     3.1415926f
#define PIE15   4.7123889f
#define PIE2    6.2831853f
#define PIE4   12.5663706f
#define PIE6   18.8495559f
#define PIE6D   0.5235987f
#define PIE2D   1.5707963f
#define PIE4D   0.7853981f
#define DEG2RAD 0.0174533f
#endif

class CAng {
public:
	// angle wrap [7/2/2021 FSM]
	static float AN1P(float rad); // Angle normalize [7/7/2021 FSM]
	static float AN2P(float rad); // Angle normalize [7/7/2021 FSM]
	static float AN360(float deg); // Angle normalize [7/7/2021 FSM]
	static float AN180(float deg); // Angle normalize [7/7/2021 FSM]
	// angle unit conversion [7/2/2021 FSM]

	// angle arithmetics [7/2/2021 FSM]
	static float SubR(float rd1, float rd2);
};
