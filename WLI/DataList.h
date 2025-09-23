#pragma once
#include "Data.h"

class CDataList
{
public:
	short magic;
	CPtrList DatLst;
	void SetData(CData* pDat);
	CData* GetData(short index);
	void Serialize(CArchive& ar);
	void ClearList();
};

