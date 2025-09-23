#pragma once
#include <iostream>
#include <vector>
#define MeData 3
#define BADDATA -9999

struct DATA {
	float x;
	float y;
	float ResultData[MeData];
};
class CResultRAFT
{
public:
	CResultRAFT();
	~CResultRAFT();
	std::vector<DATA>ResultList;
	CString RecipeName;
	
	void Clear();
	void InsertResult(float x, float y, float Ra, float Rrms, float Rmax);
	float GetRa(int index);
	float GetRrms(int index);
	float GetRmax(int index);

	BOOL SaveResult();
	BOOL LoadResult();
	void Serialize(CArchive& ar);
	void UpdateControl(CListCtrl& Ctrl);
};

