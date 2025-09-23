#pragma once

struct SScanPar {
	// Total z scan distance (um)
	float ZRange = 3.0f;
	// Center wavelength (0.4 - 0.7) um
	float Cwlen = 0.580f;
	// Number of Z steps per wavelength
	int NSteps = 4;
	// Frame average
	int NSlice = 1;
};
