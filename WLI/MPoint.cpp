#include "pch.h"
#include "MPoint.h"

CMPoint::CMPoint() 
{
	zN = 0;
	MeSet = 0;
	PatSet = 0;
}

CCoor* CMPoint::GetCoor() {
	return &Co;
}

void CMPoint::GetCoor(float& x, float& y)
{
	x = Co.x;
	y = Co.y;
}

CData* CMPoint::GetData(short index, BOOL bCreate)
{
	if (index < 0) {
		// illegal call [5/14/2013 user]
		return NULL;
	}
	if (index == 0) {
		if (Dat.DatLst.GetCount() < 1) {
			if (bCreate) {
				CData* pDat = new CData;
				if (pDat) {
					Dat.SetData(pDat);
					return pDat;
				}
			}
			return NULL;
		}
		return (CData*)Dat.DatLst.GetTail();
	}
	return Dat.GetData((short)(index - 1));
}

BOOL CMPoint::GetSFPD(float& fSFPD) {
	fSFPD = zN;
	return TRUE;
}

void CMPoint::SetCoor(CCoor* p) {
	Co = *p;
}

CString CMPoint::GetPropStr() {
	CString str;
	// [ 20230511 ZHIMING below code will cause 1ST-TRIM.RCP loading to crash
	//CString mkr[] = { "", "M1", "M2", "M3" };
	//str.Format("%.3f  %.3f  %.3f  %s", Co.x, Co.y, Co.z, mkr[Co.p]);
	str.Format(L"%.3f  %.3f", Co.x, Co.y);
	// ]
	return str;
}

CString CMPoint::GetMeSetStr() {
	CString str;
	str.Format(L"%d", MeSet);
	return str;
}

CString CMPoint::GetPatSetStr() {
	CString str;
	str.Format(L"%d", PatSet);
	return str;
}

void CMPoint::Serialize(CArchive& ar) {
	if (ar.IsStoring()) {
		magic = 0;
		ar << magic;
		Co.Serialize(ar);
		Dat.Serialize(ar);
		ar << zN;
		ar << MeSet;
		ar << PatSet;
		ar << baseline;
	}
	else {
		ar >> magic;
		Co.Serialize(ar);
		Dat.Serialize(ar);
		ar >> zN;
		ar >> MeSet;
		ar >> PatSet;
		ar >> baseline;
	}
}