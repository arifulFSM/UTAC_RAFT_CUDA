#include "pch.h"
#include "MPList.h"
#include "../MTH/Stat.h"
#include <vector>
#include <algorithm>

int CMPList::GetCount()
{
	return (MPLst.GetCount());
}

CMPoint* CMPList::Get(short index) {
	if (index < 0) {
		return NULL;
	}
	if (index >= MPLst.GetCount()) {
		return NULL;
	}
	POSITION pos = MPLst.FindIndex(index);
	return (CMPoint*)MPLst.GetAt(pos);
}

// Return CMPoint and Distance [7/19/2013 Yuen]
CMPoint* CMPList::FindNearest(CCoor& point, float& Distance) {
	CMPoint* pNearest = NULL;
	Distance = -1;
	float a, b, t;
	POSITION pos = MPLst.GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)MPLst.GetNext(pos);
		if (p) {
			a = p->Co.x - point.x;
			b = p->Co.y - point.y;
			t = a * a + b * b;
			if (Distance == -1) {
				// First point [7/19/2013 Yuen]
				Distance = t;
				pNearest = p;
			}
			if (t < Distance) {
				// subsequent point [7/19/2013 Yuen]
				Distance = t;
				pNearest = p;
			}
		}
	}
	Distance = sqrt(Distance);
	return pNearest;
}

BOOL CMPList::DeletePoint(CCoor& Point) {
	POSITION pos = MPLst.GetHeadPosition(), posFound = NULL;
	while (pos) {
		posFound = pos;
		CMPoint* p = (CMPoint*)MPLst.GetNext(pos);
		if ((p) && (p->GetCoor()->IsPoint(Point))) {
			// The numbers will be out of order so needs readjustment !
			while (pos) {
				p = (CMPoint*)MPLst.GetNext(pos);
				if (p) p->GetCoor()->n--;
			}
			p = (CMPoint*)MPLst.GetAt(posFound);
			MPLst.RemoveAt(posFound);
			delete p;
			return TRUE;
		}
	}
	return FALSE;
}

void CMPList::AddTailPoint(CMPoint* p) {
	MPLst.AddTail(p);
}

void CMPList::Renumber() {
	int i = 1;
	CString str;

	// Put points with marker at the head of list [7/23/2013 Yuen]
	POSITION opos;
	POSITION pos = MPLst.GetHeadPosition();
	while (pos) {
		opos = pos;
		CMPoint* p = (CMPoint*)MPLst.GetNext(pos);
		if (p && (p->Co.p == 3)) {
			MPLst.RemoveAt(opos);
			MPLst.AddHead(p);
			break;
		}
	}
	pos = MPLst.GetHeadPosition();
	while (pos) {
		opos = pos;
		CMPoint* p = (CMPoint*)MPLst.GetNext(pos);
		if (p && (p->Co.p == 2)) {
			MPLst.RemoveAt(opos);
			MPLst.AddHead(p);
			break;
		}
	}
	pos = MPLst.GetHeadPosition();
	while (pos) {
		opos = pos;
		CMPoint* p = (CMPoint*)MPLst.GetNext(pos);
		if (p && (p->Co.p == 1)) {
			MPLst.RemoveAt(opos);
			MPLst.AddHead(p);
			break;
		}
	}

	pos = MPLst.GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)MPLst.GetNext(pos);
		if (p) {
			p->Co.n = i;
			i++;
		}
	}
}

void CMPList::UpdateControl(CListCtrl& Ctrl) {
	int i = 0;
	CString str;

	Ctrl.DeleteAllItems();

	POSITION pos = MPLst.GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)MPLst.GetNext(pos);
		if (p) {
			short n = 1;
			str.Format(L"%d", p->Co.n);
			int idx = Ctrl.InsertItem(i++, str);
			Ctrl.SetItemText(idx, n++, p->GetPropStr());
			/*Ctrl.SetItemText(idx, n++, p->GetMeSetStr());
			Ctrl.SetItemText(idx, n++, p->GetPatSetStr());*/
		}
	}
}

BOOL CMPList::GetMinMaxY(float& YMin, float& YMax) {
	POSITION pos = MPLst.GetHeadPosition();
	YMin = FLT_MAX, YMax = FLT_MIN;
	while (pos) {
		CMPoint* pMPoint = (CMPoint*)MPLst.GetNext(pos);
		if (pMPoint) {
			CCoor* pCo = pMPoint->GetCoor();
			if (YMin > pCo->y) {
				YMin = pCo->y;
			}
			if (YMax < pCo->y) {
				YMax = pCo->y;
			}
		}
	}
	return (YMin != FLT_MAX) && (YMax != FLT_MIN);
}

bool PointSortXFwd(CMPoint* Lhs, CMPoint* Rhs) {
	CCoor* LCor = Lhs->GetCoor(),
		* RCor = Rhs->GetCoor();
	if (LCor->x < RCor->x) {
		return true;
	}
	else {
		return false;
	}
}

bool PointSortXRev(CMPoint* Lhs, CMPoint* Rhs) {
	CCoor* LCor = Lhs->GetCoor(),
		* RCor = Rhs->GetCoor();
	if (LCor->x > RCor->x) {
		return true;
	}
	else {
		return false;
	}
}

BOOL CMPList::Sort(/*BOOL bASc*/) {
	int BucketSize = sqrt(MPLst.GetCount() * 1.3);
	float YMin, YMax;
	if (!GetMinMaxY(YMin, YMax)) {
		return FALSE;
	}
	float Delta = YMax - YMin;
	if (fabs(Delta) < .05f) {
		return FALSE;
	}
	float Div = Delta / (((BucketSize - 1) == 0) ? 1 : ((BucketSize - 1)));

	//added	20080415
	if (BucketSize < 5) {
		BucketSize = Delta / 12.7f;
		Div = 12.7f;
	}
	if (BucketSize < 1) {
		BucketSize = 1;
	}

	std::vector<std::vector<CMPoint*> > Map(BucketSize);

	POSITION pos = MPLst.GetHeadPosition();
	short Bucket = 0;
	while (pos) {
		CMPoint* pMPoint = (CMPoint*)MPLst.GetNext(pos);
		if (pMPoint) {
			CCoor* pCo = pMPoint->GetCoor();
			Bucket = short((pCo->y - YMin) / Div);
			if (Bucket >= BucketSize) {
				Bucket = BucketSize - 1;
			}
			if (Bucket < 0) {
				Bucket = 0;
			}
			Map[Bucket].push_back(pMPoint);
		}
	}
	//TRACE(">> Unsorted\n");
	MPLst.RemoveAll();
	int Index = 0, Point = 1;
	for (int iBucket = 0; iBucket < BucketSize; iBucket++) {
		if (Map[iBucket].empty()) {
			continue;
		}
		if (Index % 2) {
			std::sort(Map[iBucket].begin(), Map[iBucket].end(), PointSortXFwd);
		}
		else {
			sort(Map[iBucket].begin(), Map[iBucket].end(), PointSortXRev);
		}
		for (unsigned int iPt = 0; iPt < Map[iBucket].size(); iPt++) {
			CMPoint* pMp = Map[iBucket][iPt];
			pMp->Co.n = Point; /*pMp->Co.p = Point;*/
			Point++;
			MPLst.AddTail(pMp);
		}
		Index++;
	}
	return TRUE;
}

void CMPList::EnsureData() {
	POSITION pos = MPLst.GetHeadPosition();
	while (pos) {
		CMPoint* mp = (CMPoint*)MPLst.GetNext(pos);
		if (mp) {
			mp->GetData(0, TRUE);
		}
	}
}

void CMPList::Serialize(CArchive& ar) {
	int i;
	short maxPA;
	if (ar.IsStoring()) {
		magic = 0;
		ar << magic;
		short n = short(MPLst.GetCount());
		ar << n;
		if (n > 0) {
			POSITION pos = MPLst.GetHeadPosition();
			while (pos) {
				CMPoint* p = (CMPoint*)MPLst.GetNext(pos);
				if (p) p->Serialize(ar);
			}
		}
		maxPA = MaXPA * MAXMEPASET;
		ar << maxPA;
		for (i = 0; i < maxPA; i++) {
			Stats[i].Serialize(ar);
		}
	}
	else {
		Clear();
		ar >> magic;
		short n;
		ar >> n;
		for (i = 0; i < n; i++) {
			CMPoint* p = new CMPoint; // Use default contructor
			if (p) {
				p->Serialize(ar);
				MPLst.AddTail(p);
			}
		}
		ar >> maxPA;
		if (maxPA > MaXPA * MAXMEPASET) {
			ASSERT(0);
		}
		for (i = 0; i < maxPA; i++) {
			Stats[i].Serialize(ar);
		}
	}
}

void CMPList::Clear() {
	for (int i = 0; i < MaXPA * MAXMEPASET; i++) {
		Stats[i].Clear();
	}
	POSITION pos = MPLst.GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)MPLst.GetNext(pos);
		if (p) {
			delete p;
		}
	}
	MPLst.RemoveAll();
}