#include "pch.h"
#include "Pattern.h"
#include "MPoint.h"

CCoor* CPattern::GetCoor(short index) {
	CMPoint* p = MP.Get(index);
	if (!p) return NULL;
	return p->GetCoor();
}

BOOL CPattern::DelPoint(CCoor& p) {
	return MP.DeletePoint(p);
}

void CPattern::Renumber() {
	MP.Renumber();
}

void CPattern::UpdateControl(CListCtrl& Ctrl) {
	MP.UpdateControl(Ctrl);
}