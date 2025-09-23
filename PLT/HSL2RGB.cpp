#include "pch.h"
#include "stdafx.h"
#include <math.h>
#include "HSL2RGB.h"

CHslRgb::CHslRgb() {
	R = G = B = 0;
	H = S = L = 0;
}

CHslRgb::~CHslRgb() {}

// H,S,L [0 .. 1] [5/16/2013 user]
// R,G,B [0 .. 255] [5/16/2013 user]
void CHslRgb::RgB1(float H1, float S1, float L1) {
	// 	if (H1 < 0)
	// 	{
	// 		H1 += 1;
	// 	}
	// 	if (H1 > 1)
	// 	{
	// 		H1 -= 1;
	// 	}
	// 	if (L1 > 1)
	// 	{
	// 		L1 = 1;
	// 	}
	// 	if (L1 < 0)
	// 	{
	// 		L1 = 0;
	// 	}

	H = H1;
	S = S1;
	L = L1;

	if (S1 == 0) {
		R = G = B = L1 * 255;
		return;
	}
	if (L1 == 0) {
		R = G = B = 0;
		return;
	}
	short i = short(H1 * 6);
	float f = H1 * 6 - i;
	float p = L1 * (1 - S1);
	float q = L1 * (1 - S1 * f);
	float t = L1 * (1 - S1 * (1 - f));

	switch (i) {
	case 0:
		R = L1 * 255; G = t * 255; B = p * 255;
		break;
	case 1:
		R = q * 255; G = L1 * 255; B = p * 255;
		break;
	case 2:
		R = p * 255; G = L1 * 255; B = t * 255;
		break;
	case 3:
		R = p * 255; G = q * 255; B = L1 * 255;
		break;
	case 4:
		R = t * 255; G = p * 255; B = L1 * 255;
		break;
	case 5:
		R = L1 * 255; G = p * 255; B = q * 255;
		break;
	}
}

void CHslRgb::RgB(float H1, float S1, float L1) {
	if (H1 < 0) {
		H1 += 1;
	}
	if (H1 > 1) {
		H1 -= 1;
	}
	if (L1 > 1) {
		L1 = 1;
	}
	if (L1 < 0) {
		L1 = 0;
	}
	if (S1 > 1) {
		S1 = 1;
	}
	if (S1 < 0) {
		S1 = 0;
	}
	H = H1;
	S = S1;
	L = L1;
	if (S1 == 0) {
		R = G = B = L1 * 255;
		return;
	}
	float q = L1 < 0.5f ? L1 * (1 + S1) : L1 + S1 - L1 * S1;
	float p = 2 * L1 - q;
	R = Hue2RGB(p, q, H1 + 0.33333333f) * 255;
	G = Hue2RGB(p, q, H1) * 255;
	B = Hue2RGB(p, q, H1 - 0.33333333f) * 255;
}

float CHslRgb::Hue2RGB(float p, float q, float t) {
	if (t < 0) {
		t += 1;
	}
	if (t > 1) {
		t -= 1;
	}
	if (t < 0.166666666f)		// 1/6 [5/16/2013 user]
	{
		return p + (q - p) * 6 * t;
	}
	if (t < 0.5f) {
		return q;
	}
	if (t < 0.666666666f)	// 2/3 [5/16/2013 user]
	{
		return p + (q - p) * (0.666666666f - t) * 6;	// 2/3 [5/16/2013 user]
	}
	return p;
}

void CHslRgb::HsL(short R1, short G1, short B1) {
	R = R1;
	G = G1;
	B = B1;

	float fR = R1 / 255.0f;
	float fG = G1 / 255.0f;
	float fB = B1 / 255.0f;

	short n = 0;
	float max, min;
	max = min = fR;
	if (fG > max) {
		n = 1;
		max = fG;
	}
	if (fB > max) {
		n = 2;
		max = fB;
	}
	if (fG < min) {
		min = fG;
	}
	if (fB < min) {
		min = fB;
	}
	L = (max + min) / 2.0f;

	if (max == min) {
		H = S = 0;
	}
	else {
		float d = max - min;
		S = L > 0.5f ? d / (2 - max - min) : d / (max + min);
		//S = (d) / (1-fabs(2*L-1));
		switch (n) {
		case 0:
			H = (fG - fB) / d + (fG < fB ? 6.0f : 0);
			//H = (fG - fB) / d;
			break;
		case 1:
			H = (fB - fR) / d + 2.0f;
			break;
		case 2:
			H = (fR - fG) / d + 4.0f;
			break;
		}
		H /= 6.0f;
	}
}

float CHslRgb::GetL(short R, short G, short B) {
	HsL(R, G, B);
	return L;
}

float CHslRgb::GetH(short R, short G, short B) {
	HsL(R, G, B);
	return H;
}

float CHslRgb::GetS(short R, short G, short B) {
	HsL(R, G, B);
	return S;
}