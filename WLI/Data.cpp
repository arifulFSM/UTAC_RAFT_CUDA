#include "pch.h"
#include "Data.h"

float CData::Get(short idx) {
	if ((idx >= 0) && (idx < MAXMEPASET * MaXPA)) {
		return DVal[idx];
	}
	ASSERT(0);
	return -1;
}

void CData::Serialize(CArchive& ar) {
	int i;
	if (ar.IsStoring()) {
		magic = 0;
		ar << magic;
		for (i = 0; i < MAXMEPASET * MaXPA; i++) {
			ar << DVal[i];
		}
	}
	else {
		ar >> magic;
		for (i = 0; i < 24; i++) {
			ar >> DVal[i];
			// Patch for loading old recipe [9/5/2012 Administrator]
			if (DVal[i] == 0) {
				DVal[i] = BADDATANEG;
			}
		}
		for (i = 24; i < MAXMEPASET * MaXPA; i++) {
			ar >> DVal[i];
		}
	}
}
