#include "pch.h"
#include "DataList.h"

void CDataList::SetData(CData* pDat) {
	ASSERT(pDat);
	DatLst.AddHead(pDat);
}

CData* CDataList::GetData(short index) {
	POSITION pos = DatLst.FindIndex(index);
	if (pos) {
		return (CData*)DatLst.GetAt(pos);
	}
	return NULL;
}

void CDataList::ClearList() {
	POSITION pos = DatLst.GetHeadPosition();
	while (pos) {
		CData* pDat = (CData*)DatLst.GetNext(pos);
		if (pDat) {
			delete pDat;
		}
	}
	if (DatLst.GetCount()) {
		DatLst.RemoveAll();
	}
}

void CDataList::Serialize(CArchive& ar) {
	if (ar.IsStoring()) {
		magic = 0;
		ar << magic;
		short n = (short)DatLst.GetCount();
		ar << n;
		if (n > 0) {
			POSITION pos = DatLst.GetHeadPosition();
			while (pos) {
				CData* p = (CData*)DatLst.GetNext(pos);
				if (p) p->Serialize(ar);
			}
		}
	}
	else {
		ClearList();
		ar >> magic;
		short n;
		ar >> n;
		for (int i = 0; i < n; i++) {
			CData* p = new CData(); // 20 for TM413 [6/24/2010 Yuen]
			if (p) {
				p->Serialize(ar);
				DatLst.AddTail(p);
			}
		}
	}
}