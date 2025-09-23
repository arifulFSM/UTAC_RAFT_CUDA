#pragma once

class CHslRgb {
public:
	float GetS(short R, short G, short B);
	float GetH(short R, short G, short B);
	float GetL(short R, short G, short B);
	short R;
	short G;
	short B;
	float H;
	float S;
	float L;

	void HsL(short R, short G, short B);
	void RgB(float H, float S, float L);
	void RgB1(float H, float S, float L);
	float Hue2RGB(float p, float q, float t);

	CHslRgb();
	virtual ~CHslRgb();
};
