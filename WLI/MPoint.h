#pragma once
#include "Coor.h"
#include "Data.h"
#include "DataList.h"

class CMPoint
{
protected:
	CDataList Dat;

public:
	short magic;
	short MeSet;
	short PatSet;
	float baseline;
	float zN;
	CCoor Co;

	CMPoint();
	CCoor* GetCoor();
	void GetCoor(float& x, float& y);
	CData* GetData(short index, BOOL bCreate);
	BOOL GetSFPD(float& fSFPD);
	void SetCoor(CCoor* p);
	CString GetPropStr();
	CString GetMeSetStr();
	CString GetPatSetStr();
	void Serialize(CArchive& ar);
};

