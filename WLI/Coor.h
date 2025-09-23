#pragma once
class CCoor
{

public:
	enum {MEASURE, MEASURING, MEASURED};
	float x;
	float y;
	float z;
	short p;	// Position Marker (1 to 3 [7/23/2013 Yuen]
	short n;	// serial number of MPoints [11/1/2010 XPMUser]
	short status;
	short magic;

	CCoor();
	BOOL IsPoint(CCoor& point);
	void Serialize(CArchive& ar);
};

