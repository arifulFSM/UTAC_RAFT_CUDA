#include "pch.h"

#include "In.h"

void SIn::clear() {
	nStp = 0;
	bActive = false;
	bSuccess = false;
	reason = "";
	for (int i = 0; i < 8; i++) {
		I[i].clear();
	}
}

int SIn::size() {
	return nStp;
}

// intensity or position or height [6/21/2021 FSM]

void SAmplf::clear() {
	i = 0;
	fPos = 0;
	memset(y, 0, 5 * sizeof(float));
}