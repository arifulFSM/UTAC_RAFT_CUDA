#include "pch.h"
#include "Coor.h"

CCoor::CCoor() {
	//magic = 0;
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
	n = 0;
	p = 0;	// 0 means no marker set [7/23/2013 Yuen]
	status = MEASURE;
}

BOOL CCoor::IsPoint(CCoor& point) {
	if ((fabs(point.x - x) < 2.0f) && (fabs(point.y - y) < 2.0f)) {
		return TRUE;
	}
	return FALSE;
}

void CCoor::Serialize(CArchive& ar) {
	if (ar.IsStoring()) {
		magic = 0;
		ar << magic;
		ar << x;
		ar << y;
		ar << n;
		ar << p;
		ar << z;
	}
	else {
		ar >> magic;
		ar >> x;
		ar >> y;
		ar >> n;
		ar >> p;
		ar >> z;
	}
}
