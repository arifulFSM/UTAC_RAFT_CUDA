#pragma once
#include "MPoint.h"
#include "../MTH/Stat.h"

class CMPList
{
public:
	short magic;
	CPtrList MPLst;
	SStat Stats[MaXPA * MAXMEPASET];

	int GetCount();
	CMPoint* Get(short index);
	CMPoint* FindNearest(CCoor& point, float& Distance);
	BOOL DeletePoint(CCoor& Point);
	void AddTailPoint(CMPoint* p);
	void Renumber();
	void UpdateControl(CListCtrl& Ctrl);
	BOOL Sort(/*BOOL bASc*/);
	BOOL GetMinMaxY(float& YMin, float& YMax);
	void EnsureData();
	void Serialize(CArchive& ar);
	void Clear();
};

