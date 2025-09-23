#pragma once

struct SPxy {
	int i = 0;
	float x = 0, y = 0;
	SPxy() {}
	SPxy(float xx, float yy) : x(xx), y(yy) {}
	SPxy(int ii, float xx, float yy) : i(ii), x(xx), y(yy) {}
};
