#include "pch.h"

#include "Ims.h"

#include "fArchive.h"

COLORREF WLI::SIms::GetPixRGB(float x, float y) {
	return Im.GetPixel(int(x * Im.GetWidth()), int(y * Im.GetHeight()));
}

COLORREF WLI::SIms::GetPixRGB(int x, int y) {
	return Im.GetPixel(x, y);
}

short WLI::SIms::GetI(int x, int y, IMGL::eCH nChan) {
	return Im.GetI(x, y, nChan);
}

bool WLI::SIms::GetDim(int& x, int& y, int& bpp) {
	if (Im.IsNull()) {
		x = y = -1; return false;
	}
	x = Im.GetWidth();
	y = Im.GetHeight();
	bpp = Im.GetBPP();
	return true;
}

WLI::SIms::SIms() {}

WLI::SIms::~SIms() {
	if (!Im.IsNull()) Im.Destroy();
}

void WLI::SIms::Serialize(CArchive& ar) {
	BOOL bAlloc = FALSE;
	if (!Im.IsNull()) bAlloc = TRUE;
	USHORT magic = 1;
	if (ar.IsStoring()) {
		ar << magic;
		ar << PzPos_um;
		ar << bAlloc;
		if (bAlloc) Im.Serialize(ar);
	}
	else {
		ar >> magic;
		ar >> PzPos_um;
		ar >> bAlloc;
		if (bAlloc) Im.Serialize(ar);
	}
}