#pragma once

#include <string>

struct SAmplf {
	int i = 0;
	float fPos = 0;      // position
	float y[5] = { 0 };  // result intensity or position or height, corresponds to WHTCH, REDCH, ... [6/21/2021 FSM]
	void clear();
};

#define MAXINTSTY 9

struct SIn {
	bool bSpdUp = true;
	bool bActive = false;
	bool bSuccess = false;  // return variable [10/16/2021 yuenl]
	short nStp = 0;  // number of phase shift step [8/3/2021 FSM]
	std::string reason = "";
	SAmplf I[MAXINTSTY];
	const SIn& operator=(const SIn& co) {
		nStp = co.nStp;
		bSpdUp = co.bSpdUp;
		bActive = co.bActive;
		bSuccess = co.bSuccess;
		reason = co.reason;
		//for (int i = 0; i < MAXINTSTY; i++) {
		//	I[i] = co.I[i];
		//}
		memcpy(I, co.I, MAXINTSTY * sizeof(SAmplf));
		return *this;
	}

public:
	int size();
	void clear();
};
