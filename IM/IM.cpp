#include "pch.h"

#include <algorithm>
#include "fArchive.h"
#include "IM.h"
#include "MTH/Define.h"
#include "MTH/LSFit.h"
#include "MTH/LSF3D.h"
#include "PSI/ICC.h" // 05302023 - Mortuja
#include <map> // 07122023

using namespace IMGL;

#define REDSHFT 0
#define GRNSHFT 8
#define BLUSHFT 16

#define FULLWHITE

#ifndef PIE
#define PIE     3.1415926f
#define PIE15   4.7123889f
#define PIE2    6.2831853f
#define PIE4   12.5663706f
#define PIE6   18.8495559f
#define PIE6D   0.5235987f
#define PIE2D   1.5707963f
#define PIE4D   0.7853981f
#define DEG2RAD 0.0174533f
#endif

// Macros
#define NINT(f)   ((f >= 0) ? (int)(f + .5) : (int)(f - .5))
#define THRESH(d) ((d > 255) ? 255 : ((d < 0) ? 0 : d))
#define INRANGE() (m_image.minmax.min >= 0 && m_image.minmax.max <= 255)

BYTE CIM::GetWhite(COLORREF cr) {
#ifdef FULLWHITE
#ifdef HALFWHITE
	return ((cr & 0xFF) + ((cr >> GRNSHFT) & 0xFF) + ((cr >> BLUSHFT) & 0xFF)) / 3;
#else
	return ((cr & 0xFF) + 2 * ((cr >> GRNSHFT) & 0xFF) + ((cr >> BLUSHFT) & 0xFF)) / 4;
#endif
#else
#ifdef HALFWHITE
	return (((cr >> GRNSHFT) & 0xFF) + ((cr >> BLUSHFT) & 0xFF)) / 2;
#else
	return (2 * ((cr >> GRNSHFT) & 0xFF) + ((cr >> BLUSHFT) & 0xFF)) / 3;
#endif
#endif
}

BYTE CIM::GetRed(COLORREF cr) {
	return cr & 0xFF;
}

BYTE CIM::GetGreen(COLORREF cr) {
	return (cr >> GRNSHFT) & 0xFF;
}

BYTE CIM::GetBlue(COLORREF cr) {
	return (cr >> BLUSHFT) & 0xFF;
}

//////////////////////////////////////////////////////////////////////////
// buffer access [11/3/2019 FSM]
bool CIM::SetBuffer(PBYTE pBuffer, int wd, int ht, int bpp) {
	// Assume pBuffer is 8bpp rectangular image [1/3/2020 yuenl]
	if (bpp != 24) { ASSERT(0); return false; }
	if (IsNull()) {
		if (!Create(wd, ht, bpp)) return false;
	}
	else {
		int w, h, b;
		GetDim(w, h, b);
		if ((w != wd) || (h != ht) || (b != 24)) {
			Destroy();
			if (!Create(wd, ht, 24)) return false;
		}
	}
	if (IsNull()) return false;
	// convert 8 bpp to 24 bpp [1/3/2020 yuenl]
	for (int y = 0; y < ht; y++) {
		PBYTE q = pBuffer + y * wd;
		PBYTE p = (PBYTE)GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++) {
			*(p++) = *q; *(p++) = *q; *(p++) = *q;
			q++;
		}
	}
	return true;
}

void CIM::DumpPoints(std::vector<SLoc>& ledge, std::vector<SLoc>& redge, char* filename) {
	size_t n;
	if (!ledge.size()) return;
	if (!redge.size()) return;

	FILE* fp = fopen(filename, "wb"); if (!fp) return;
	n = redge.size();
	fwprintf(fp, _T("LEFT\n"));
	fwprintf(fp, _T("X,Y,X\n"));
	for (size_t i = 0; i < n; i++) {
		fwprintf(fp, _T("%d,%d,%d\n"), redge[i].x, redge[i].y, redge[i].x);
	}
	n = ledge.size();
	fwprintf(fp, _T("\n\nRIGHT\n"));
	fwprintf(fp, _T("X,Y,X\n"));
	for (size_t i = 0; i < n; i++) {
		fwprintf(fp, _T("%d,%d,%d\n"), ledge[i].x, ledge[i].y, ledge[i].x);
	}
	fclose(fp);
}

void CIM::DumpEdge(char* filename, int* pX, int* pY, short n) {
	FILE* fp = fopen(filename, "wb"); if (!fp) return;
	fwprintf(fp, _T("X,Y\n"));
	for (int i = 0; i < n; i++) {
		fwprintf(fp, _T("%d,%d\n"), *(pX++), *(pY++));
	}
	fclose(fp);
}

bool CIM::Crop(int left, int top, int right, int bottom) {
	// no sanity checks [10/6/2020 FSM]
	int dx = right - left, dy = bottom - top;
	int wd, ht, bpp; GetDim(wd, ht, bpp);
	CIM INew;
	INew.Create(dx, dy, bpp); if (INew.IsNull()) return false;
#pragma omp parallel for
	for (int y = top; y < bottom; y++) {
		INew.SetLine(y - top, (PBYTE)GetPixelAddress(left, y), dx, bpp);
	}
	*this = INew;
	return true;
}

bool IMGL::CIM::Crop(IMGL::CIM& INew, int left, int top, int right, int bottom) {
	// no sanity checks [10/6/2020 FSM]
	int dx = right - left, dy = bottom - top;
	int wd, ht, bpp; GetDim(wd, ht, bpp);
	INew.Create(dx, dy, bpp); if (INew.IsNull()) return false;
#pragma omp parallel for
	for (int y = top; y < bottom; y++) {
		INew.SetLine(y - top, (PBYTE)GetPixelAddress(left, y), dx, bpp);
	}
	return true;
}

int CIM::GetBYPP() {// Get bytes per pixel [11/3/2019 FSM]
	return GetBPP() / 8;
}

int CIM::GetBYPR() {// Get bytes per row [11/3/2019 FSM]
	return GetPitch();
}

bool CIM::ReduceSize(CIM& Image, short factor) {
	if (factor < 2) return false;
	if (Image.IsNull()) return false;
	int wd, ht, bpp; GetDim(wd, ht, bpp);
	if (bpp != 24) return false; // only support 24 bits image [6/18/2020 yuenl]
	if (!IsNull()) Destroy();
	if (!Create(wd / factor, ht / factor, bpp)) return false;
	if (IsNull()) return false;
	int w = wd / factor, h = ht / factor;
	for (int y = 0; y < h; y++) {
		PBYTE q = (PBYTE)Image.GetPixelAddress(0, y * factor);
		PBYTE p = (PBYTE)GetPixelAddress(0, y);
		for (int x = 0; x < w; x++) {
			*(p++) = *(q++); *(p++) = *(q++); *(p++) = *(q++);
			q += 3 * (factor - 1);
		}
	}
	return true;
}

CString CIM::Analyse() {
	// Total pixel above noise threshold count [10/7/2020 FSM]
	SAVal tCount = ImgProperties(24);
	std::vector<SAVal> qCount = ImgRegionProperties(24);
	CString str, str1 = _T(""), str2 = _T(""), str3 = _T(""), str4 = _T("");
	if (qCount.size() == 9) {
		str1.Format(_T("%% pixel changed\nTotal(%%): % .2f\n"),
			tCount.PCount * 1e2f);
		str2.Format(_T("Quadrants(%%)\n% .2f, % .2f, % .2f\n% .2f, % .2f, % .2f\n% .2f, % .2f, % .2f\n\n"),
			qCount[0].PCount * 1e2f, qCount[1].PCount * 1e2f, qCount[2].PCount * 1e2f,
			qCount[3].PCount * 1e2f, qCount[4].PCount * 1e2f, qCount[5].PCount * 1e2f,
			qCount[6].PCount * 1e2f, qCount[7].PCount * 1e2f, qCount[8].PCount * 1e2f);
		str3.Format(_T("Brightness(%%)\nTotal(%%): % .2f\n"), tCount.Bright);
		str4.Format(_T("Quadrants(%%)\n% .2f, % .2f, % .2f\n% .2f, % .2f, % .2f\n% .2f, % .2f, % .2f\n\n"),
			qCount[0].Bright, qCount[1].Bright, qCount[2].Bright,
			qCount[3].Bright, qCount[4].Bright, qCount[5].Bright,
			qCount[6].Bright, qCount[7].Bright, qCount[8].Bright);
	}
	else {
		str1.Format(_T("Total: % .2f"), tCount.PCount);
	}
	return str1 + str2 + str3 + str4;
}

void CIM::GetDim(int& wd, int& ht, int& bpp) const {
	wd = GetWidth(); ht = GetHeight(); bpp = GetBPP();
}

ULONG CIM::ImageSize() {
	return GetWidth() * GetHeight();
}

ULONG CIM::BufferSize() const {
	return abs(GetPitch()) * GetHeight();
}

PVOID CIM::BufferStart() {
	if (GetPitch() < 0) return (PVOID)GetPixelAddress(0, GetHeight() - 1);
	return (PVOID)GetPixelAddress(0, 0);
}

short IMGL::CIM::GetIntensity(int x, int y, eCH nChan) {
	// No sanity check [2/13/2021 FSM]
	PBYTE b = (PBYTE)GetPixelAddress(x, y);
	switch (GetBPP()) {
	case 8: // 8 bits image [11/3/2019 FSM]
		return short(*b);
		break;
	case 24: // 24 bits image [11/3/2019 FSM]
	case 32: // 32 bits image [11/3/2019 FSM]
		switch (nChan) {
		case eCH::REDC:
			return short(*(b + 2));
			break;
		case eCH::GRNC:
			return short(*(b + 1));
			break;
		case eCH::BLUC:
			return short(*b);
			break;
		case eCH::WHTC:
#ifdef FULLWHITE
#ifdef HALFWHITE
			return short((*(b + 2) + *(b + 1) + *b) / 3);
#else
			return short((*(b + 2) + *(b + 1) * 2 + *b) / 4);
#endif
#else
#ifdef HALFWHITE
			return short((*(b + 1) + *b) / 2);
#else
			return short((*(b + 1) * 2 + *b) / 3);
#endif
#endif
			break;
		}
		break;
	default: ASSERT(0); break;
	}
	return 0;
}

BYTE IMGL::CIM::GetPixelG(int x, int y) {
	COLORREF cr = GetPixel(x, y);
#ifdef FULLWHITE
	return ((cr & 0xFF) + 2 * ((cr >> 8) & 0xFF) + ((cr >> 16) & 0xFF)) / 4;
#else
	return ((cr & 0xFF) + ((cr >> 8) & 0xFF) + ((cr >> 16) & 0xFF)) / 3;
#endif
}

COLORREF IMGL::CIM::GetPixel(int x, int y) {
	//? ////////////////////////////////////////////////////////////////////////
	//? sanity check is a must [8/3/2021 FSM]
	//? ////////////////////////////////////////////////////////////////////////
	int wd = GetWidth(), ht = GetHeight();
	if ((x < 0) || (y < 0)) return RGB(0, 0, 0);
	if ((x >= wd) || (y >= ht)) return RGB(0, 0, 0);
	PBYTE b = (PBYTE)GetPixelAddress(x, y);
	switch (GetBPP()) {
	case 8: // 8 bits image [11/3/2019 FSM]
		return RGB(*b, *b, *b);
		break;
	case 24: // 24 bits image [11/3/2019 FSM]
	case 32: // 32 bits image [11/3/2019 FSM]
		return RGB(*(b + 2), *(b + 1), *b);
		break;
	default: ASSERT(0); break;
	}
	return RGB(0, 0, 0);
}

BYTE IMGL::CIM::GetGray(int x, int y, int bpp) {
	PBYTE b = (PBYTE)GetPixelAddress(x, y);
	switch (bpp) {
	case 8: // 8 bits image [11/3/2019 FSM]
		return *b;
		break;
	case 24: // 24 bits image [11/3/2019 FSM]
	case 32: // 32 bits image [11/3/2019 FSM]
		return (*(b + 2) + *(b + 1) * 2 + *b) / 4;
		break;
	default: ASSERT(0);  break;
	}
	return 0;
}

void IMGL::CIM::RGBAddTo(int x, int y, int& R, int& G, int& B) {
	PBYTE b = (PBYTE)GetPixelAddress(x, y);
	B += *(b++); G += *(b++); R += *(b);
}

bool CIM::SetPixel(int x, int y, COLORREF& cr) {
	PBYTE b = (PBYTE)GetPixelAddress(x, y);
	switch (GetBPP()) {
	case 8: // 8 bits image [11/3/2019 FSM]
		*b = BYTE((GetRValue(cr) + GetGValue(cr) + GetBValue(cr)) / 3);
		return true;
		break;
	case 24: // 24 bits image [11/3/2019 FSM]
	case 32: // 32 bits image [11/3/2019 FSM]
		*b = GetBValue(cr);
		*(b + 1) = GetGValue(cr);
		*(b + 2) = GetRValue(cr);
		return true;
		break;
	default: ASSERT(0); break;
	}
	return false;
}

void CIM::AddDiv(CIM& Img) {// Img1 = (Img1 + Img2) / 2 [11/3/2019 FSM]
	if (Img.IsNull()) return;
	int wd, ht, bpp; GetDim(wd, ht, bpp);
	if (IsNull() || (ImageSize() != Img.ImageSize())) {
		if (!Create(wd, ht, bpp)) return;
		memset(BufferStart(), 0, BufferSize());
	}
#pragma omp parallel for
	for (int y = 0; y < ht; y++) {
		PBYTE des = (PBYTE)GetPixelAddress(0, y);
		PBYTE src = (PBYTE)Img.GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++) {
			switch (bpp) {
			case 8:
				*(des++) = (*(des)+*(src++)) / 2;
				break;
			case 15:
			case 16:
				break;
			case 24:
				for (int i = 0; i < 3; i++) {
					*(des++) = (*(des)+*(src++)) / 2;
				}
				break;
			case 32:
				for (int i = 0; i < 3; i++) {
					*(des++) = (*(des)+*(src++)) / 2;
				}
				*(des++) = *(src++);
				break;
			}
		}
	}
}

bool CIM::Sub(CIM& Img) {
	if (IsNull()) return false;
	if (Img.IsNull()) return false;
	if (ImageSize() != Img.ImageSize()) return false;
	int wd, ht, bpp; Img.GetDim(wd, ht, bpp);
	if ((bpp == 15) || (bpp == 16)) return false;

#pragma omp parallel for
	for (int y = 0; y < ht; y++) {
		short v;
		PBYTE des = (PBYTE)GetPixelAddress(0, y);
		PBYTE src = (PBYTE)Img.GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++) {
			switch (bpp) {
			case 8: v = *(des)-*(src++); if (v < 0) *(des++) = 0; *(des++) = BYTE(v); break;
			case 24:
				for (int i = 0; i < 3; i++) {
					v = *des - *src;
					if (v >= 0) *(des++) = BYTE(v); else *(des++) = 0;
				}
				break;
			case 32:
				for (int i = 0; i < 3; i++) {
					v = *des - *src;
					if (v >= 0) *(des++) = BYTE(v); else *(des++) = 0;
				}
				src++; des++;
				break;
			}
		}
	}
}

void IMGL::CIM::Copy(CIM& Img) {
	if (Img.IsNull()) return;
	int wd, ht, bpp; GetDim(wd, ht, bpp);
	if (IsNull()) {
		if (!Create(wd, ht, bpp)) return;
	}
	if (ImageSize() != Img.ImageSize()) {
		Destroy();
		if (!Create(wd, ht, bpp)) return;
	}
	memcpy(GetBufferStart(), Img.GetBufferStart(), wd * ht * bpp / 8);
}

bool IMGL::CIM::MaxMin() {
	// assume 24 bit gray image
	if (IsNull()) return false;
	int wd, ht, bpp; GetDim(wd, ht, bpp);
	bmax = bmin = *(PBYTE)GetPixelAddress(0, 0);
	switch (bpp) {
	case 8:
		for (int y = 0; y < ht; y++) {
			PBYTE p = (PBYTE)GetPixelAddress(0, y);
			for (int x = 0; x < wd; x++, p++) {
				if (*p > bmax) bmax = *p;
				if (*p < bmin) bmin = *p;
			}
		}
		break;
	case 24:
	case 32:
		for (int y = 0; y < ht; y++) {
			PBYTE p = (PBYTE)GetPixelAddress(0, y);
			for (int x = 0; x < wd; x++) {
				if (*p > bmax) bmax = *p;
				if (*p < bmin) bmin = *p;
				if (bpp == 24) p += 3; else p += 4;
			}
		}
		break;
	default: return false; break;
	}
	return true;
}

bool IMGL::CIM::Color2Gray() {
	if (IsNull()) return false;
	int wd, ht, bpp; GetDim(wd, ht, bpp);
	if (bpp == 8) return false;
	//#pragma omp parallel for
	for (int y = 0; y < ht; y++) {
		BYTE v;
		PBYTE des = (PBYTE)GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++) {
			switch (bpp) {
			case  24:
				v = (*(des)+*(des + 1) * 2 + *(des + 2)) / 4;
				*(des++) = v; *(des++) = v; *(des++) = v;
				break;
			case 32:
				v = (*(des)+*(des + 1) * 2 + *(des + 2)) / 4;
				*(des++) = v; *(des++) = v; *(des++) = v; des++;
				break;
			}
		}
	}
	return true;
}

bool IMGL::CIM::Normalize() {
	// apply to 24 bit gray only
	if (IsNull()) return false;
	int wd, ht, bpp; GetDim(wd, ht, bpp);
	if (!MaxMin()) return false;
	float sf = 255 / float(bmax - bmin);
	BYTE v;
	switch (bpp) {
	case 8:
		for (int y = 0; y < ht; y++) {
			PBYTE des = (PBYTE)GetPixelAddress(0, y);
			for (int x = 0; x < wd; x++, des++) {
				*des = BYTE(sf * (*des - bmin));
			}
		}
		break;
	case 24:
	case 32:
		for (int y = 0; y < ht; y++) {
			PBYTE des = (PBYTE)GetPixelAddress(0, y);
			for (int x = 0; x < wd; x++) {
				v = BYTE(sf * (*des - bmin));
				*(des++) = v; *(des++) = v; *(des++) = v;
				if (bpp == 32) des++;
			}
		}
		break;
	default: return false; break;
	}
	return true;
}

void IMGL::CIM::Morpho(IMGL::CIM& Im, IMGL::CIM::EMOR Op, IMGL::SMat& K, USHORT N, BYTE Thre) {
	int wd, ht, bpp; GetDim(wd, ht, bpp);
	if (!((bpp == 8) || (bpp == 24))) { ASSERT(0); return; }
	if (!IsSameSize(Im)) { Im.Destroy(); }
	if (Im.IsNull()) {
		Im.Create(wd, ht, bpp);
		if (bpp == 8) Im.MakeGrayLUT();
	}
	if (Im.IsNull()) return;
	memset(Im.GetBufferStart(), 0, Im.BufferSize());

	USHORT wdw = K.W / 2;
	// only works with 24 bits gray image [6/5/2021 FSM]
#pragma omp parallel for
	for (int y = wdw; y < ht - wdw - 1; y++) {
		for (int x = wdw; x < wd - 1 - wdw; x++) {
			BYTE v = 0;
			PBYTE pK = K.K;
			int yy = y - wdw;
			for (int i = 0; i < K.W; i++, yy++) { // y [6/5/2021 FSM]
				int xx = x - wdw;
				for (int j = 0; j < K.W; j++, xx++, pK++) { // x [6/5/2021 FSM]
					v += ((GetPixelG(x, yy) * (*pK)) > 0 ? 1 : 0);
				}
			}
			switch (Op) {
			case IMGL::CIM::MISS:
				if (v == 0) {
					Im.SetPixel(x, y, 255, 255, 255);
				}
				break;
			case IMGL::CIM::HIT:
				if (v) {
					Im.SetPixel(x, y, 255, 255, 255);
				}
				break;
			case IMGL::CIM::FIT:
				if (v >= N) {
					Im.SetPixel(x, y, 255, 255, 255);
				}
				break;
			case IMGL::CIM::ERODE:
				if (v >= N) {
					if (GetPixelG(x, y)) Im.SetPixel(x, y, 255, 255, 255);
				}
				break;
			case IMGL::CIM::DILATE:
				if (v != 0) {
					Im.SetPixel(x, y, 255, 255, 255);
				}
				break;
			case IMGL::CIM::EDGE:
				if (GetPixelG(x, y) >= Thre) {
					if (v > 1) Im.SetPixel(x, y, 255, 255, 255);
				}
				break;
			}
		}
	}
}

void CIM::ContrastStretch(int Max, int Min) {
	if (IsNull()) return;
	int wd, ht, bpp; GetDim(wd, ht, bpp);

	float df = 255.0f / float(Max - Min);

#pragma omp parallel for
	for (int y = 0; y < ht; y++) {
		short v;
		PBYTE p = (PBYTE)GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++) {
			switch (bpp) {
			case 8:
				v = short((*p - Min) * df);
				if (v < 0) v = 0; if (v > 255) v = 255;
				*(p++) = BYTE(v);
				break;
			case 15:
			case 16:
				break;
			case 24:
			case 32:
				for (int i = 0; i < 3; i++) {
					v = short((*p - Min) * df);
					if (v < 0) v = 0; else if (v > 255) v = 255;
					*(p++) = BYTE(v);
				}
				if (bpp == 32) p++;
				break;
			}
		}
	}
}

BOOL CIM::IsSameSize(const CIM& DiB) {
	if (IsNull() || DiB.IsNull()) return FALSE;
	if ((DiB.GetWidth() != GetWidth()) || (DiB.GetHeight() != GetHeight()) ||
		(DiB.GetBPP() != GetBPP())) {
		return FALSE;
	}
	return TRUE;
}

BOOL IMGL::CIM::Histo(SBin& Bin, eCH Ch) {
	if (IsNull()) return FALSE;
	if (Bin.nBin < 256) { ASSERT(0);  return FALSE; }

	Bin.Clear();
	int wd, ht, bpp; GetDim(wd, ht, bpp);
	switch (bpp) {
	case 8:
		for (int y = 0; y < ht; y++) {
			PBYTE p = (PBYTE)GetPixelAddress(0, y);
			for (int x = 0; x < wd; x++) {
				Bin.psRed[*(p++)]++;
			}
		}
		break;
	case 24:
		switch (Ch) {
		case IMGL::BLUC:
			for (int y = 0; y < ht; y++) {
				SRGB* p = (SRGB*)GetPixelAddress(0, y);
				for (int x = 0; x < wd; x++, p++) {
					Bin.psBlu[p->B]++;
				}
			}
			break;
		case IMGL::GRNC:
			for (int y = 0; y < ht; y++) {
				SRGB* p = (SRGB*)GetPixelAddress(0, y);
				for (int x = 0; x < wd; x++, p++) {
					Bin.psGrn[p->G]++;
				}
			}
			break;
		case IMGL::REDC:
			for (int y = 0; y < ht; y++) {
				SRGB* p = (SRGB*)GetPixelAddress(0, y);
				for (int x = 0; x < wd; x++, p++) {
					Bin.psRed[p->R]++;
				}
			}
			break;
		case IMGL::WHTC:
			for (int y = 0; y < ht; y++) {
				SRGB* p = (SRGB*)GetPixelAddress(0, y);
				for (int x = 0; x < wd; x++, p++) {
					Bin.psBlu[p->B]++; Bin.psGrn[p->G]++; Bin.psRed[p->R]++;
				}
			}
			break;
		default: ASSERT(0); return FALSE; break;
		}
	}
	return TRUE;
}

double IMGL::CIM::Intensity() {
	if (IsNull()) return 0;
	int wd, ht, bpp; GetDim(wd, ht, bpp);
	if ((bpp == 15) || (bpp == 16)) { ASSERT(0); return 0; }
	long gsum = 0;
	long* sum = new long[ht]; if (!sum) return 0;
	memset(sum, 0, ht * sizeof(long));
#pragma omp parallel for
	for (int y = 0; y < ht; y++) {
		BYTE* p;
		switch (bpp) {
		case 8:
			p = (BYTE*)GetPixelAddress(0, y);
			for (int x = 0; x < wd; x++, p++) {
				sum[y] += *p;
			}
			break;
		case 24:
		case 32:
			for (int x = 0; x < wd; x++) {
				sum[y] += GetGray(x, y, bpp);
			}
			break;
		}
	}
	for (int y = 0; y < ht; y++) gsum += sum[y];
	delete[] sum;
	return gsum / double(wd * ht);
}

PBYTE CIM::GetBufferStart() const {
	// Get beginning of image buffer [11/21/2015 Yuen]
	// Caller must lock [1/24/2016 Yuen]
	if (IsNull()) return NULL;
	if (GetPitch() < 0) return (PBYTE)GetPixelAddress(0, GetHeight() - 1);
	return (PBYTE)GetPixelAddress(0, 0);
}

void CIM::Dump(UINT sz, int* pX, int* pY, char* Filename) {
	FILE* fp = fopen(Filename, "wb");
	if (!fp) return;
	fwprintf(fp, _T("X,Y\n"));
	for (UINT i = 0; i < sz; i++) {
		fwprintf(fp, _T("%d,%d\n"), *(pX++), *(pY++));
	}
	fclose(fp);
}

void CIM::RangeLEdge(std::vector<SLoc>& Inner, int& dx, int& dy, int* pX, int* pY) {// determine whether the edge is horizontal or vertical
	dx = 0; dy = 0;
	size_t sz = Inner.size(); if (!sz) return;
	int minX, maxX, minY, maxY;
	minX = maxX = Inner[0].x;
	minY = maxY = Inner[0].y;
	*(pX++) = minX; *(pY++) = minY;
	for (size_t i = 1; i < sz; i++) {
		int v = Inner[i].x; *(pX++) = v;
		if (v < minX) minX = v; if (v > maxX) maxX = v;
		v = Inner[i].y; *(pY++) = v;
		if (v < minY) minY = v; if (v > maxY) maxY = v;
	}
	dx = maxX - minX; dy = maxY - minY;
}

void CIM::DrawCross(int x1, int y1, int len, BYTE r, BYTE g, BYTE b) {
	int wd, ht, bpp; GetDim(wd, ht, bpp);
	len /= 2;
	int stY = y1 - len; if (stY < 0) stY = 0;
	int edY = y1 + len; if (edY >= ht) edY = ht - 1;
	int stX = x1 - len; if (stX < 0) stX = 0;
	int edX = x1 + len; if (edX >= wd) edX = wd - 1;

	DrawLine(stX, y1, edX, y1, r, g, b);
	DrawLine(x1, stY, x1, edY, r, g, b);
}

void CIM::DrawLine(int x1, int y1, int x2, int y2, BYTE r, BYTE g, BYTE b) {
	int xx, yy;
	int wd, ht, bpp; GetDim(wd, ht, bpp);

	if (x1 == x2) {// draw vertical line [6/4/2021 FSM]
		if ((x1 < 0) || (x1 >= wd)) return;
		for (int y = y1; y <= y2; y++) {
			if ((y < 0) || (y >= ht)) continue;
			SetPixel(x1, y, r, g, b);
		}
		return;
	}
	if (y1 == y2) {// draw horizontal line [6/4/2021 FSM]
		if ((y1 < 0) || (y1 >= ht)) return;
		for (int x = x1; x <= x2; x++) {
			if ((x < 0) || (x >= wd)) continue;
			SetPixel(x, y1, r, g, b);
		}
		return;
	}
	// draw line with gradient [6/4/2021 FSM]
	float m = (y2 - y1) / float(x2 - x1);
	float c = y2 - m * x2;
	if (fabsf(m) > 1) {// vary y [11/5/2019 FSM]
		if (y1 < y2) {
			for (int i = y1; i < y2; i++) {
				xx = int((i - c) / m);
				yy = i;
				if ((xx >= 0) && (xx < wd)) {
					if ((yy > 0) && (yy < ht)) {
						PBYTE des = (PBYTE)GetPixelAddress(xx, yy);
						switch (bpp) {
						case 8: *(des) = (r + b + g) / 3; break;
						case 15: case 16: break;
						case 24: case 32:
							*(des++) = b; *(des++) = g; *(des) = r;
							break;
						}
					}
				}
			}
		}
		else {
			for (int i = y2; i < y1; i++) {
				xx = int((i - c) / m);
				yy = i;
				if ((xx >= 0) && (xx < wd)) {
					if ((yy > 0) && (yy < ht)) {
						PBYTE des = (PBYTE)GetPixelAddress(xx, yy);
						switch (bpp) {
						case 8: *(des) = (r + b + g) / 3; break;
						case 15: case 16: break;
						case 24: case 32:
							*(des++) = b; *(des++) = g; *(des) = r;
							break;
						}
					}
				}
			}
		}
	}
	else {// vary x [11/5/2019 FSM]
		if (x1 < x2) {
			for (int i = x1; i < x2; i++) {
				xx = i;
				yy = int(m * i + c);
				if ((xx >= 0) && (xx < wd)) {
					if ((yy > 0) && (yy < ht)) {
						PBYTE des = (PBYTE)GetPixelAddress(xx, yy);
						switch (bpp) {
						case 8: *(des) = (r + b + g) / 3; break;
						case 15: case 16: break;
						case 24: case 32:
							*(des++) = b; *(des++) = g; *(des) = r;
							break;
						}
					}
				}
			}
		}
		else {
			for (int i = x2; i < x1; i++) {
				xx = i;
				yy = int(m * i + c);
				if ((xx >= 0) && (xx < wd)) {
					if ((yy > 0) && (yy < ht)) {
						PBYTE des = (PBYTE)GetPixelAddress(xx, yy);
						switch (bpp) {
						case 8: *(des) = (r + b + g) / 3; break;
						case 15: case 16: break;
						case 24: case 32:
							*(des++) = b; *(des++) = g; *(des) = r;
							break;
						}
					}
				}
			}
		}
	}
}

void IMGL::CIM::DrawCircle(int cx, int cy, int cr, BYTE r, BYTE g, BYTE b) {
	//////////////////////////////////////////////////////////////////////////
	//+ x = cx + r * cos (t)
	//+ y = cy + r * sin (t)
	//////////////////////////////////////////////////////////////////////////
	if (GetBPP() != 24) { return; }
	int wd = GetWidth() - 1;
	int ht = GetHeight() - 1;
	float sf = 3.141592f / 180.0f;
	short dr = short(360 / (2 * 3.141592f * cr));
	if (dr < 1) dr = 1;
#pragma omp parallel for
	for (int t = 0; t <= 360; t += dr) {
		int x = int(cx + cr * cos(t * sf));
		int y = int(cy + cr * sin(t * sf));
		if ((x < 0) || (y < 0)) continue;
		if ((x > wd) || (y > ht)) continue;
		PBYTE p = (PBYTE)GetPixelAddress(x, y);
		*(p++) = b; *(p++) = g; *(p) = r;
	}
}

void IMGL::CIM::DrawSinuGridPattern(float px, float py, float shift, eCH ch, BOOL bBin) {
	if (IsNull()) return;
	int wd, ht, bpp; GetDim(wd, ht, bpp);
	if (bpp != 24) return;
	float kx = PIE2 / px, ky = PIE2 / py;
	for (int y = 0; y < ht; y++) {
		SRGB* p = (SRGB*)GetPixelAddress(0, y);
		switch (ch) {
		case eCH::REDC:
			for (int x = 0; x < wd; x++, p++) {
				p->R = BYTE(255 * (0.5f + cos(kx * x + shift) / 4.f + cos(ky * y + shift) / 4.f));
				if (bBin) { if (p->R < 127) p->R = 0; else p->R = 255; }
			}
			break;
		case eCH::GRNC:
			for (int x = 0; x < wd; x++, p++) {
				p->G = BYTE(255 * (0.5f + cos(kx * x + shift) / 4.f + cos(ky * y + shift) / 4.f));
				if (bBin) { if (p->G < 127) p->G = 0; else p->G = 255; }
			}
			break;
		case eCH::BLUC:
			for (int x = 0; x < wd; x++, p++) {
				p->B = BYTE(255 * (0.5f + cos(kx * x + shift) / 4.f + cos(ky * y + shift) / 4.f));
				if (bBin) { if (p->B < 127) p->B = 0; else p->B = 255; }
			}
			break;
		default:
			for (int x = 0; x < wd; x++, p++) {
				BYTE By = BYTE(255 * (0.5f + cos(kx * x + shift) / 4.f + cos(ky * y + shift) / 4.f));
				if (bBin) { if (By < 127) By = 0; else By = 255; }
				p->R = p->G = p->B = By;
			}
			break;
		}
	}
}

void IMGL::CIM::DrawSinuHorzPattern(float py, float shift, eCH ch) {
	if (IsNull()) return;
	int wd, ht, bpp; GetDim(wd, ht, bpp);
	if (bpp != 24) return;
	float ky = PIE2 / py;
	for (int y = 0; y < ht; y++) {
		BYTE By;
		SRGB* p = (SRGB*)GetPixelAddress(0, y);
		switch (ch) {
		case eCH::REDC:
			By = BYTE(255 * (0.5f + cos(ky * y + shift) / 2.f));
			for (int x = 0; x < wd; x++, p++) {
				p->R = By;
			}
			break;
		case eCH::GRNC:
			By = BYTE(255 * (0.5f + cos(ky * y + shift) / 2.f));

			for (int x = 0; x < wd; x++, p++) {
				p->G = By;
			}
			break;
		case eCH::BLUC:
			By = BYTE(255 * (0.5f + cos(ky * y + shift) / 2.f));
			for (int x = 0; x < wd; x++, p++) {
				p->B = By;
			}
			break;
		default:
			By = BYTE(255 * (0.5f + cos(ky * y + shift) / 2.f));
			for (int x = 0; x < wd; x++, p++) {
				p->R = p->G = p->B = By;
			}
			break;
		}
	}
}

void IMGL::CIM::DrawSinuVertPattern(float px, float shift, eCH ch) {
	if (IsNull()) return;
	int wd, ht, bpp; GetDim(wd, ht, bpp); if (bpp != 24) return;
	float u0 = PIE2 / px; // spacial frequency in radians per pixel [12/29/2021 yuenl]
	for (int x = 0; x < wd; x++) {
		BYTE By = BYTE(127 * (1.0f + cos(u0 * x + shift)));
		switch (ch) {
		case eCH::REDC:
			for (int y = 0; y < ht; y++) {
				((SRGB*)GetPixelAddress(x, y))->R = By;
			}
			break;
		case eCH::GRNC:
			for (int y = 0; y < ht; y++) {
				((SRGB*)GetPixelAddress(x, y))->G = By;
			}
			break;
		case eCH::BLUC:
			for (int y = 0; y < ht; y++) {
				((SRGB*)GetPixelAddress(x, y))->B = By;
			}
			break;
		default:
			for (int y = 0; y < ht; y++) {
				SRGB* p = (SRGB*)GetPixelAddress(x, y);
				p->R = p->G = p->B = By;
			}
			break;
		}
	}
}

void IMGL::CIM::DrawBinHorzPattern(int n, float shift, eCH ch) {
	if (IsNull()) return;
	int wd, ht, bpp; GetDim(wd, ht, bpp); if (bpp != 24) return;
	int mask = (2 << n);
	for (int y = 0; y < ht; y++) {
		SRGB* p = (SRGB*)GetPixelAddress(0, y);
		BYTE By;
		if (y & mask) By = 255; else By = 0;
		switch (ch) {
		case eCH::REDC: for (int x = 0; x < wd; x++, p++) { p->R = By; } break;
		case eCH::GRNC: for (int x = 0; x < wd; x++, p++) { p->G = By; } break;
		case eCH::BLUC: for (int x = 0; x < wd; x++, p++) { p->B = By; } break;
		default: for (int x = 0; x < wd; x++, p++) { p->R = p->G = p->B = By; } break;
		}
	}
}

void IMGL::CIM::DrawBinVertPattern(int n, float shift, eCH ch) {
	if (IsNull()) return;
	int wd, ht, bpp; GetDim(wd, ht, bpp); if (bpp != 24) return;
	int mask = int(powf(2.f, float(n)));
	for (int x = 0; x < wd; x++) {
		BYTE By;
		if (x & mask) By = 255; else By = 0;
		switch (ch) {
		case eCH::REDC:
			for (int y = 0; y < ht; y++) { ((SRGB*)GetPixelAddress(x, y))->R = By; } break;
		case eCH::GRNC:
			for (int y = 0; y < ht; y++) { ((SRGB*)GetPixelAddress(x, y))->G = By; } break;
		case eCH::BLUC:
			for (int y = 0; y < ht; y++) { ((SRGB*)GetPixelAddress(x, y))->B = By; } break;
		default:
			for (int y = 0; y < ht; y++) {
				SRGB* p = (SRGB*)GetPixelAddress(x, y); p->R = p->G = p->B = By;
			}
			break;
		}
	}
}

void IMGL::CIM::DrawBinCheckerPattern(int n, float shift, eCH ch) {
	if (IsNull()) return;
	int wd, ht, bpp; GetDim(wd, ht, bpp); if (bpp != 24) return;
	int div = int(powf(2, float(n))); // divider [12/1/2021 yuenl]
	for (int y = 0; y < ht; y++) {
		for (int x = 0; x < wd; x++) {
			int yy = y / div, xx = x / div;
			SRGB cr; cr.SetGray(0);
			if ((yy + xx) % 2) cr.SetGray(255);
			SRGB* p = (SRGB*)GetPixelAddress(x, y); *p = cr;
		}
	}
}

void IMGL::CIM::DrawFringePattern(float lamda, float shift, short A, short B, bool vert, eCH ch) {
	if (IsNull()) return;
	int wd, ht, bpp; GetDim(wd, ht, bpp); if (bpp != 24) return;
	float C = PIE2 / float(lamda);
	if (vert) {
		BYTE By;
		SRGB* p = (SRGB*)GetPixelAddress(0, 0);
		for (int x = 0; x < wd; x++, p++) {
			switch (ch) {
			case eCH::WHTC:
				By = BYTE(A + B * cos(C * x + shift));
				p->R += By; if (p->R > 255) p->R = 255;
				p->G += By; if (p->G > 255) p->G = 255;
				p->B += By; if (p->B > 255) p->B = 255;
				break;
			case eCH::REDC:
				p->R += BYTE(A + B * cos(C * x + shift));
				if (p->R > 255) p->R = 255;
				break;
			case eCH::GRNC:
				p->G += BYTE(A + B * cos(C * x + shift));
				if (p->G > 255) p->G = 255;
				break;
			case eCH::BLUC:
				p->B += BYTE(A + B * cos(C * x + shift));
				if (p->B > 255) p->B = 255;
				break;
			}
		}
		p = (SRGB*)GetPixelAddress(0, 0);
		for (int y = 1; y < ht; y++) {
			memcpy((SRGB*)GetPixelAddress(0, y), p, wd * sizeof(SRGB));
		}
	}
	else {
		for (int y = 0; y < ht; y++) {
			BYTE By = BYTE(A + B * cos(C * y + shift));
			SRGB* p = (SRGB*)GetPixelAddress(0, y);
			for (int x = 0; x < wd; x++, p++) {
				switch (ch) {
				case eCH::WHTC:
					p->R += By; if (p->R > 255) p->R = 255;
					p->G += By; if (p->G > 255) p->G = 255;
					p->B += By; if (p->B > 255) p->B = 255;
					break;
				case eCH::REDC:
					p->R += By; if (p->R > 255) p->R = 255;
					break;
				case eCH::GRNC:
					p->G += By; if (p->G > 255) p->G = 255;
					break;
				case eCH::BLUC:
					p->B += By; if (p->B > 255) p->B = 255;
					break;
				}
			}
		}
	}
}

bool CIM::BThres(short lvl, BYTE low, BYTE hi) {// binary thresholding [11/3/2019 FSM]
	if (IsNull() || (lvl > 255)) return false;
	int wd, ht, bpp; GetDim(wd, ht, bpp);
	//#pragma omp parallel for
	switch (bpp) {
	case 8:
		for (int y = 0; y < ht; y++) {
			PBYTE des = (PBYTE)GetPixelAddress(0, y);
			for (int x = 0; x < wd; x++, des++) {
				*des = (*des >= lvl) ? hi : low;
			}
		}
		break;
	case 24:
	case 32:
		for (int y = 0; y < ht; y++) {
			BYTE v;
			PBYTE des = (PBYTE)GetPixelAddress(0, y);
			for (int x = 0; x < wd; x++) {
				SRGB* p = (SRGB*)des;
				v = (p->R + 2 * p->G + p->B) / 4;
				if (v < lvl) v = low; else v = hi;
				p->R = p->G = p->B = v;
				if (bpp == 24) des += 3; else des += 4;
			}
		}
		break;
	default: ASSERT(0); break;
	}
	return true;
}

bool IMGL::CIM::BThres32() {
	if (IsNull()) return false;
	int wd, ht, bpp; GetDim(wd, ht, bpp);
	MaxMin();
	//#pragma omp parallel for
	for (int y = 0; y < ht; y++) {
		BYTE v, * des = (PBYTE)GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++) {
			switch (bpp) {
			case 24:
			case 32:
				v = (((*des) + *(des + 1) * 2 + *(des + 2)) / 256) * 64;
				*(des++) = v; *(des++) = v; *(des++) = v;
				if (bpp == 32) des++;
				break;
			default: return false; break;
			}
		}
	}
	return true;
}

BYTE CIM::Set() {
	if (IsNull()) return 0;
	long sum = 0;
	int wd, ht, bpp; GetDim(wd, ht, bpp);

#pragma omp parallel for reduction(+:sum)
	for (int y = 0; y < ht; y++) {
		PBYTE des = (PBYTE)GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++) {
			switch (bpp) {
			case 8:
				sum += *(des++);
				break;
			case 15: case 16: break;
			case 24:
				sum += *(des++); sum += *(des++); sum += *(des++);
				break;
			case 32:
				sum += *(des++); sum += *(des++); sum += *(des++); des++;
				break;
			}
		}
	}
	return BYTE(sum / (wd * ht * (bpp / 8)));
}

bool CIM::FillEdge(BYTE Thres) {
	if (IsNull()) return false;
	int wd, ht, bpp; GetDim(wd, ht, bpp);
	int bypp = bpp / 8;
	// Fill left side with black [11/4/2019 FSM]
	for (int y = 0; y < ht; y++) {
		PBYTE des = (PBYTE)GetPixelAddress(0, y);
		switch (bpp) {
		case 8:
			for (int x = 0; x < wd; x++, des++) {
				if (*des > Thres) *des = 0;
				else break;
			}
			break;
		case 24:
		case 32:
			for (int x = 0; x < wd; x++, des += bypp) {
				if (*des > Thres) memset(des, 0, 3);
				else break;
			}
			break;
		default: ASSERT(0); return false;  break;
		}
	}
	// fill right side with black [11/4/2019 FSM]
	for (int y = 0; y < ht; y++) {
		PBYTE des = (PBYTE)GetPixelAddress(wd - 1, y);
		switch (bpp) {
		case 8:
			for (int x = 0; x < wd; x++, des--) {
				if (*des > Thres) *des = 0;
				else break;
			}
			break;
		case 24:
		case 32:
			for (int x = 0; x < wd; x++, des += bypp) {
				if (*des > Thres) memset(des, 0, 3);
				else break;
			}
			break;
		default: ASSERT(0); return false; break;
		}
	}
	return true;
}

short CIM::CountObject(short u, BYTE Obj, int x, int y) {
	// no sanity check
	// assume Isrc is properly thresholded binary
	int stx = x - u, sty = y - u, edx = x + u, edy = y + u, a = 0;
	int bypp = GetBPP() / 8;
	int sum = 0;
	for (int j = sty; j <= edy; j++) {
		PBYTE src = (PBYTE)GetPixelAddress(stx, y);
		for (int i = stx; i <= edx; i++, src += bypp) {
			sum += (*src == Obj);
		}
	}
	return sum;
}

//////////////////////////////////////////////////////////////////////////
// Convolution operation [1/5/2020 yuenl]

void CIM::ConvlHSobel() {
	Convl(-1.0, -2.0, -1.0, 0.0, 0.0, 0.0, 1.0, 2.0, 1.0);
	//Convl(-1.0, -3.0, -1.0, 0.0, 0.0, 0.0, 1.0, 3.0, 1.0);
	//Convl(-2.0, -3.0, -2.0, 0.0, 0.0, 0.0, 2.0, 3.0, 2.0);
}

void CIM::ConvlVSobel() {
	Convl(-1.0, 0.0, 1.0, -2.0, 0.0, 2.0, -1.0, 0.0, 1.0);
	//Convl(-1.0, 0.0, 1.0, -3.0, 0.0, 3.0, -1.0, 0.0, 1.0);
	//Convl(-2.0, 0.0, 2.0, -3.0, 0.0, 3.0, -2.0, 0.0, 2.0);
}

bool CIM::Sobel() {
	if (IsNull()) return false;
	CIM I1, I2;
	I1 = *this; I2 = *this;
	I1.ConvlVSobel();
	I2.ConvlHSobel();
	Magnitude(I1, I2);
	return true;
}

bool CIM::Magnitude(CIM& I1, CIM& I2) {// Note all 3 images must have same dimension and must be 24 bits gray image [1/5/2020 yuenl]
	int w, h, b;
	GetDim(w, h, b); b /= 8; if (b != 3) { ASSERT(0); return false; }
#pragma omp parallel for
	for (int y = 0; y < h; y++) {
		PBYTE pDes = (PBYTE)GetPixelAddress(0, y);
		PBYTE pI1 = (PBYTE)I1.GetPixelAddress(0, y);
		PBYTE pI2 = (PBYTE)I2.GetPixelAddress(0, y);
		for (int x = 0; x < w; x++) {
			BYTE v = BYTE(sqrt((*pI1) * (*pI1) + (*pI2) * (*pI2)));
			if (v < 0)v = 0; if (v > 255) v = 255;
			*(pDes++) = v; *(pDes++) = v; *(pDes++) = v;
			pI1 += b; pI2 += b;
		}
	}
	return false;
}

bool IMGL::CIM::Contour(short nlvl) {
	return true;
}

void CIM::Convl(float k1, float k2, float k3, float k4, float k5, float k6, float k7, float k8, float k9) {
	if (IsNull()) return;

	PBYTE p;						// Image ptr
	int nxx;
	unsigned long nx, ny;
	PBYTE m1, m2, m3;				// Pointers to buffers to free()
	PBYTE old_r1, r1, r2, r3;	// Cycling pointers to rows
	float fsum;
	short byPP;

	nx = GetWidth();
	ny = GetHeight();
	byPP = GetBPP() / 8;
	nxx = (nx)*byPP;

	p = (PBYTE)GetBufferStart();// Ptr to bitmap
	// Allocate row buffers
	m1 = new BYTE[(nx + 2) * byPP];
	m2 = new BYTE[(nx + 2) * byPP];
	m3 = new BYTE[(nx + 2) * byPP];
	if (!m1 || !m2 || !m3) {
		if (m1) delete[] m1; if (m2) delete[] m2; if (m3) delete[] m3;
		return;
	}
	r1 = m1; r2 = m2; r3 = m3;

	int width = nx * byPP;
	// Initialize rows
	p = (PBYTE)GetPixelAddress(0, 0);
	memcpy_s(&r1[3], width, p, width);
	p = (PBYTE)GetPixelAddress(0, 1);
	memcpy_s(&r2[3], width, p, width);

	r1[0] = r1[3]; r1[1] = r1[4]; r1[2] = r1[5];
	r1[width + 0] = r1[width - 3];
	r1[width + 1] = r1[width - 2];
	r1[width + 2] = r1[width - 1];
	r2[0] = r2[3]; r2[1] = r2[4]; r2[2] = r2[5];
	r2[width + 0] = r2[width - 3];
	r2[width + 1] = r2[width - 2];
	r2[width + 2] = r2[width - 1];

	// Calc. sum of kernel
	fsum = k1 + k2 + k3 + k4 + k5 + k6 + k7 + k8 + k9;
	if (fsum == 0) fsum = 1; // Avoid div. by 0
	else fsum = 1 / fsum; // Invert so can mult.

	for (int j = 1; j < int(ny - 1); j++) {
		if (j == ny) r3 = r2;
		else  /* Read next row (into the 3rd row) */
		{
			p = (PBYTE)GetPixelAddress(0, j + 1);
			memcpy_s(&r3[3], width, p, width);
			r3[0] = r3[3]; r3[1] = r3[4]; r3[2] = r3[5];
			r3[width + 0] = r3[width - 3];
			r3[width + 1] = r3[width - 2];
			r3[width + 2] = r3[width - 1];
		}

		PBYTE pp = (PBYTE)GetPixelAddress(0, j);
#pragma omp parallel for
		for (int i = 0; i < int(nx); i++) {
			short nByte = i * byPP;
			PBYTE p0 = pp + nByte;
			PBYTE r11 = r1 + nByte;
			PBYTE r22 = r2 + nByte;
			PBYTE r33 = r3 + nByte;
			if (byPP > 3) { r11++; r22++; r33++; p0++; }
			// Red component
			float s;
			s = k1 * r11[0] + k2 * r11[3] + k3 * r11[6]
				+ k4 * r22[0] + k5 * r22[3] + k6 * r22[6]
				+ k7 * r33[0] + k8 * r33[3] + k9 * r33[6];
			BYTE v = (BYTE)THRESH(NINT(s * fsum));
			*(p0++) = v;
			*(p0++) = v;
			*(p0++) = v;
			r11 += byPP; r22 += byPP; r33 += byPP;

			// Green component
			// s = k1 * r11[0] + k2 * r11[3] + k3 * r11[6]
			// 	+ k4 * r22[0] + k5 * r22[3] + k6 * r22[6]
			// 	+ k7 * r33[0] + k8 * r33[3] + k9 * r33[6];
			// *p0 = (BYTE)THRESH(NINT(s * fsum));
			// r11++; r22++; r33++; p0++;

			// Blue component
			// s = k1 * r11[0] + k2 * r11[3] + k3 * r11[6]
			// 	+ k4 * r22[0] + k5 * r22[3] + k6 * r22[6]
			// 	+ k7 * r33[0] + k8 * r33[3] + k9 * r33[6];
			// *p = (BYTE)THRESH(NINT(s * fsum));
		}
		/* Cycle row pointers */
		old_r1 = r1;
		r1 = r2;
		r2 = r3;
		r3 = old_r1;
	}

	delete[] m1; delete[] m2; delete[] m3;
}

short CIM::Set(PBYTE src, short wdw, short bpp) {
	int sum = 0;
	switch (bpp) {
	case 8:
		for (int i = 0; i < wdw; i++) {
			sum += *(src++);
		}
		return sum / wdw;
		break;
	case 15: case 16: break;
	case 24:
		for (int i = 0; i < wdw; i++) {
			sum += (*(src)+*(src + 1) + *(src + 2)) / 3;
			src += 3;
		}
		return sum / wdw;
		break;
	case 32:
		for (int i = 0; i < wdw; i++) {
			sum += (*(src)+*(src + 1) + *(src + 2)) / 3;
			src += 4;
		}
		return sum / wdw;
		break;
	}
	return 0;
}

bool CIM::IsInRect(int x, int y) {// _wd and _ht must be valid on entry [1/7/2020 yuenl]
	int wd = GetWidth(), ht = GetHeight();
	if ((x < 0) || (x >= wd)) return false;
	if ((y < 0) || (y >= ht)) return false;
	return true;
}

int CIM::GetPx(float angle, int dy) {
	return int(dy / tan(angle * DEG2RAD));
}

int CIM::GetPy(float angle, int dx) {
	if (angle == 90) return 0;
	else if (angle == 270) return 0;
	return int(dx * tan(angle * DEG2RAD));
}

float IMGL::CIM::IScore() {
	// no sanity check
	if (IsNull()) return -1;
	int wd, ht, bpp; GetDim(wd, ht, bpp); if (bpp != 24) return -1;
	//#pragma omp parallel for
	int cnt = 0;
	long sum = 0;
	for (int y = 0; y < ht; y++) {
		SRGB* p = (SRGB*)GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++, p++) {
			sum += p->White(); cnt++;
		}
	}
	//return sum / 256;
	return sum / float(cnt);
}

void CIM::SmoothLeft(std::vector<SLoc>& Inner, short wdw, short ntime, short bpp) {
	size_t ndata = Inner.size(); if (ndata < 1) return;
	int* pdata = new int[ndata];
	int* ydd = new int[ndata];
	if (!pdata || !ydd) {
		if (pdata) delete[] pdata; if (ydd) delete[] ydd;
		return;
	}
	int* pX = pdata;
	for (size_t i = 0; i < ndata; i++) {
		*(pX++) = Inner[i].x;
	}
	int sum;
	for (int j = 0; j < ntime; j++) {
		pX = pdata;
		for (size_t i = 0; i < ndata; i++) {
			sum = 0;
			int l = 0;
			for (int k = -wdw; k <= wdw; k++) {
				if ((i + k >= 0) && (i + k < ndata)) {
					sum += pX[i + k];
					l++;
				}
			}
			if (l > 0) ydd[i] = sum / l;
			else ydd[i] = pX[i];
		}
		// Copy back to main buffer [2/19/2019 yuenl]
		memcpy(pdata, ydd, ndata * sizeof(int));
	}
	// copy back to edge [11/7/2019 yuenl]
	pX = pdata;
	for (size_t i = 0; i < ndata; i++) {
		Inner[i].x = *(pX++);
		if (bpp == 8)
			SetPixel(Inner[i].x, Inner[i].y, 255, 255, 255);
		else
			SetPixel(Inner[i].x, Inner[i].y, 0, 255, 0);
	}

	if (ydd) delete[] ydd; if (pdata) delete[] pdata;
}

bool CIM::SetPixel(int x, int y, BYTE r, BYTE g, BYTE b) {
	PBYTE p = (PBYTE)GetPixelAddress(x, y);
	switch (GetBPP()) {
	case 8: *p = r; break;
	case 24:
	case 32: *(p++) = b; *(p++) = g; *(p) = r; break;
	default: ASSERT(0); return false;
	}
	return true;
}

bool CIM::SetLine(int y, PBYTE pBuffer, int len, int bpp) {
	PBYTE p = (PBYTE)GetPixelAddress(0, y);
	switch (bpp) {
	case 8: memcpy(p, pBuffer, len); break;
	case 24: memcpy(p, pBuffer, 3 * len); break;
	default: ASSERT(0); return false; break;
	}
	return true;
}

COLORREF IMGL::CIM::GetColorRef(PBYTE p) {
	return *p + (*(p + 1) << 8) + (*(p + 2) << 16);
}

COLORREF* IMGL::CIM::GetLine(CPoint& st, CPoint& ed, int& sz, eCH Ch) {
	// caller delete allocation [12/20/2020 FSM]
	sz = 0;
	int wd, ht, bpp; GetDim(wd, ht, bpp);
	if ((st.x == ed.x) && (st.y == ed.y)) return nullptr;
	int dx = abs(ed.x - st.x), dy = abs(ed.y - st.y);
	float sf = 0;
	if (dx > dy) {
		if (dx > 1) { sz = dx; sf = 1 / float(dx); }
		else return nullptr;
	}
	else if (dx < dy) {
		if (dy > 1) { sz = dy; sf = 1 / float(dy); }
		else return nullptr;
	}
	else {
		// vertical line [3/1/2021 FSM]
		return nullptr;
	}
	BYTE(*GetPix)(COLORREF);
	switch (Ch) {
	case eCH::REDC: GetPix = &GetRed; break;
	case eCH::GRNC: GetPix = &GetGreen; break;
	case eCH::BLUC: GetPix = &GetBlue; break;
	default: GetPix = &GetWhite; break;
	}
	COLORREF* pbuf = new COLORREF[sz]; if (!pbuf) return nullptr;
	COLORREF* p = pbuf;
	for (int i = 0; i < sz; i++, p++) {
		int x = int(st.x + i * sf * dx); if (x < 0) x = 0; else if (x >= wd) x = wd - 1;
		int y = int(st.y + i * sf * dy); if (y < 0) y = 0; else if (y >= ht) y = ht - 1;
		*p = *(COLORREF*)GetPixelAddress(x, y);
	}
	return pbuf;
}

BOOL IMGL::CIM::GetLine(std::vector<SPixel>& line, CPoint& st, CPoint& ed) {
	if ((st.x == ed.x) && (st.y == ed.y)) return FALSE;
	int dx = abs(ed.x - st.x), dy = abs(ed.y - st.y);
	int sz = 0;
	float sf = 0;
	if (dx > dy) {
		if (dx != 1) { sz = dx; sf = 1 / float(dx - 1); }
		else return FALSE;
	}
	else {
		if (dy != 1) { sz = dy; sf = 1 / float(dy - 1); }
		else return FALSE;
	}
	for (int i = 0; i < sz; i++) {
		int x = int(st.x + i * sf * dx);
		int y = int(st.y + i * sf * dy);
		//float v = *(float *)GetPixelAddress(x, y);
		line.push_back(SPixel(x, y, *(float*)(GetPixelAddress(x, y))));
	}
	return TRUE;
}

bool IMGL::CIM::ReplacePixel(USHORT from, COLORREF to) {
	if (IsNull()) return false;
	BYTE R = (to >> 16) & 0xFF, G = (to >> 8) & 0xFF, B = to & 0xFF;
	int wd = GetWidth(), ht = GetHeight();
	switch (GetBPP()) {
	case 8:
		for (int y = 0; y < ht; y++) {
			PBYTE p = (PBYTE)GetPixelAddress(0, y);
			for (int x = 0; x < wd; x++, p++) {
				if (*p) continue;
				*p = from && 0xFF;
			}
		}
		break;
	case 24:
		for (int y = 0; y < ht; y++) {
			SRGB* p = (SRGB*)GetPixelAddress(0, y);
			for (int x = 0; x < wd; x++, p++) {
				if (p->Get() == from) {
					p->B = B; p->G = G; p->R = R;
				}
			}
		}
		break;
	default: ASSERT(0); return false; break;
	}
	return true;
}

int IMGL::CIM::ReplacePixel(IMGL::CIM2& Im2, USHORT from, COLORREF to) {
	if (IsNull()) return -1;
	BYTE R = (to >> 16) & 0xFF, G = (to >> 8) & 0xFF, B = to & 0xFF;
	int cnt = 0;
	int wd = GetWidth(), ht = GetHeight();
	switch (GetBPP()) {
	case 8:
		for (int y = 0; y < ht; y++) {
			PBYTE p = (PBYTE)GetPixelAddress(0, y);
			USHORT* q = Im2.GetPixelAddress(0, y);
			for (int x = 0; x < wd; x++, p++, q++) {
				if (*q == from) continue;
				*p = R; cnt++;
			}
		}
		break;
	case 24:
		for (int y = 0; y < ht; y++) {
			SRGB* p = (SRGB*)GetPixelAddress(0, y);
			USHORT* q = Im2.GetPixelAddress(0, y);
			for (int x = 0; x < wd; x++, p++, q++) {
				if (*q == from) {
					p->R = R; p->G = G; p->B = B; cnt++;
				}
			}
		}
		break;
	default: ASSERT(0); return -1; break;
	}
	return cnt;
}

bool CIM::GetInnerEdge(std::vector<SLoc>& Outer, float& x, float& y) {
	x = 0; y = 0;
	size_t sz = Outer.size(); if (sz < 1) return false;
	for (size_t i = 0; i < sz; i++) {
		x += Outer[i].x;
		y += Outer[i].y;
	}
	x /= float(sz);
	y /= float(sz);
	return false;
}

bool CIM::GetOuterEdge(std::vector<SLoc>& Inner, float& x, float& y) {
	x = 0; y = 0;
	size_t sz = Inner.size(); if (sz < 1) return false;
	for (size_t i = 0; i < sz; i++) {
		x += Inner[i].x;
		y += Inner[i].y;
	}
	x /= float(sz);
	y /= float(sz);
	return true;
}

int CIM::AverW() {
	if (IsNull()) return -1;
	int wd, ht, bpp; GetDim(wd, ht, bpp);
	long sum = 0;
	switch (bpp) {
	case 8:
		for (int y = 0; y < ht; y++) {
			BYTE* p = (BYTE*)GetPixelAddress(0, y);
			for (int x = 0; x < wd; x++, p++) {
				sum += *p;
			}
		}
		return sum / (wd * ht);
		break;
	case 24:
		for (int y = 0; y < ht; y++) {
			SRGB* p = (SRGB*)GetPixelAddress(0, y);
			for (int x = 0; x < wd; x++, p++) {
				sum += p->White();
			}
		}
		return sum / (wd * ht);
		break;
	default: ASSERT(0); break;
	}
	return -1;
}

void CIM::AverStdev(size_t sz, int* pD, float& Ave, float& Stdev) {
	Ave = 0; Stdev = 0;
	float sum = 0;
	int* p = pD;
	for (UINT i = 0; i < sz; i++) {
		sum += *(p++);
	}
	Ave = sum / float(sz);
	p = pD;
	for (UINT i = 0; i < sz; i++) {
		float dif = *(p++) - Ave;
		Stdev += (dif * dif);
	}
	Stdev = float(sqrt(Stdev / float(sz)));
}

//////////////////////////////////////////////////////////////////////////

void CIM::ConvlLowPass() {
	Convl(0.0625f, 0.125f, 0.0625f, 0.125f, 0.25f, 0.125f, 0.0625f, 0.125f, 0.0625f);
}

bool IMGL::CIM::Smooth(USHORT N) {
	if (IsNull()) return false;
	for (USHORT i = 0; i < N; i++) {
		ConvlLowPass();
	}
	return true;
}

void CIM::DumpEdge(PBYTE pSrc, PBYTE pDes, short wd, short bpp, char* filename) {
	FILE* fp = fopen(filename, "wb"); if (!fp) return;
	for (int i = 0; i < wd; i++) {
		fwprintf(fp, _T("%d,%d\n"), *(pSrc++) ? 1 : 0, *(pDes++) ? 1 : 0);
	}
	fclose(fp);
}

bool CIM::Compare(CIM& Img) {
	if (IsNull()) return false;
	if (Img.IsNull()) return false;

	int pitch = abs(GetPitch());
	int wd, ht, bpp; GetDim(wd, ht, bpp);

#pragma omp parallel for
	for (int y = 0; y < ht; y++) {
		PBYTE des = (PBYTE)GetPixelAddress(0, y);
		PBYTE src = (PBYTE)Img.GetPixelAddress(0, y);
		switch (bpp) {
		case 8:
			for (int x = 0; x < wd; x++) {
				*des = (*des == *src) ? 0 : 255;
				src++; des++;
			}
			break;
		case 15:
		case 16:
			break;
		case 24:
			for (int x = 0; x < wd; x++) {
				*des = (*des == *src) ? 0 : 255;
				src++; des++;
				*des = (*des == *src) ? 0 : 255;
				src++; des++;
				*des = (*des == *src) ? 0 : 255;
				src++; des++;
			}
			break;
		case 32:
			for (int x = 0; x < wd; x++) {
				*des = (*des == *src) ? 0 : 255;
				src++; des++;
				*des = (*des == *src) ? 0 : 255;
				src++; des++;
				*des = (*des == *src) ? 0 : 255;
				src += 2; des += 2;
			}
			break;
		}
	}
	return true;
}

UINT CIM::Thining(short wdw, short lmt, BYTE Obj) {
	if (IsNull()) return 0;
	const short u = wdw / 2;
	int wd, ht, bpp; GetDim(wd, ht, bpp);
	int bypp = bpp / 8;

	CIM ImB; ImB = *this;
	BYTE Bg = 255 - Obj;
	UINT cnt = 0;

	switch (bpp) {
	case 8:
#pragma omp parallel for
		for (int y = u; y <= ht - u; y++) {
			PBYTE des = (PBYTE)GetPixelAddress(u, y);
			for (int x = u; x <= wd - u; x++, des++) {
				if (*des != Obj) continue;
				if (ImB.CountObject(u, Obj, x, y) < lmt) {
					memset(des, Bg, bypp); cnt++;
				}
			}
		}
		break;
	case 24:
	case 32:
#pragma omp parallel for
		for (int y = u; y <= ht - u; y++) {
			PBYTE des = (PBYTE)GetPixelAddress(u, y);
			for (int x = u; x < wd - u; x++, des += bypp) {
				if (*des != Obj) continue;
				if (ImB.CountObject(u, Obj, x, y) < lmt) {
					memset(des, Bg, bypp); cnt++;
				}
			}
		}
		break;
	default: ASSERT(0); return 0; break;
	}
	return cnt;
}

bool CIM::SavePic() {
	if (IsNull()) return false;
	CFileDialog dlg(
		FALSE, _T("BMP"), NULL,
		OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_NOCHANGEDIR,
		_T("BMP|*.BMP|PNG file|*.PNG|JPG|*.JPG|All|*.*|"));
	CString fname = GetProgramDir(); fname += _T("IMG"); int a = _wmkdir(fname);
	dlg.m_ofn.lpstrInitialDir = fname.GetBuffer();
	dlg.m_ofn.lpstrTitle = _T("Save image");
	if (dlg.DoModal() == IDOK) {
		Save(dlg.GetPathName()); return true;
	}
	return false;
}

CString CIM::SavePicTime() {
	if (IsNull()) return L"";
	CString fname = GetProgramDir(); fname += _T("IMG");
	int a = _wmkdir(fname);
	CString str = CTime::GetCurrentTime().Format("IM_%d%H%M%S.BMP");
	fname += L"\\" + str;
	Save(fname); return str;
	return L"";
}

bool CIM::SavePic(CString name) {
	Save(name);
	return true;
}

bool IMGL::CIM::SaveCSV(CString name) {
	if (IsNull()) return false;
	FILE* fp = _wfopen(name, L"wb"); if (!fp) return false;
	int wd, ht, bpp; GetDim(wd, ht, bpp);
	for (int y = 0; y < ht; y++) {
		PBYTE p = (PBYTE)GetPixelAddress(0, y);
		fprintf(fp, "<%d>", y + 1);
		for (int x = 0; x < wd; x++, p += 3) {
			fprintf(fp, ",%d", *p);
		}
		fprintf(fp, "\n");
	}
	fclose(fp);
	return true;
}

CString CIM::GetProgramDir() {
	TCHAR dirname[_MAX_PATH], drive[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT];
	GetModuleFileName(NULL, dirname, _MAX_PATH);
	_wsplitpath(dirname, drive, dir, fname, ext);
	_wmakepath(dirname, drive, dir, _T(""), _T(""));
	return dirname;
}

void IMGL::CIM::Serialize(CArchive& ar) {
	USHORT magic = 1;
	ULONG BufSize;
	int wd, ht, bpp, pitch;
	BOOL IsAlloc = !IsNull();
	if (ar.IsStoring()) {
		ar << magic;
		ar << IsAlloc;
		if (IsAlloc) {
			BufSize = BufferSize();
			pitch = GetPitch();
			GetDim(wd, ht, bpp);
			ar << wd;
			ar << ht;
			ar << bpp;
			ar << pitch;
			ar << BufSize;
			ar.Write(BufferStart(), BufSize);
		}
	}
	else {
		ar >> magic;
		ar >> IsAlloc;
		if (IsAlloc) {
			if (!IsNull()) Destroy();
			ar >> wd;
			ar >> ht;
			ar >> bpp;
			ar >> pitch;
			ar >> BufSize;
			Create(wd, ht, bpp);
			if (!IsNull()) {
				ar.Read(BufferStart(), BufSize);
			}
		}
	}
}

void CIM::BestRect(CRect& rc) {
	int nwd, nht;
	float aspImg = this->GetWidth() / float(this->GetHeight());
	float aspScr = rc.Width() / float(rc.Height());
	if (aspImg > aspScr) {
		nwd = rc.Width(); nht = int(nwd / aspImg);
	}
	else {
		nht = rc.Height(); nwd = int(nht * aspImg);
	}
	int x = (rc.Width() - nwd) / 2;
	int y = (rc.Height() - nht) / 2;
	rc.SetRect(x, y, x + nwd, y + nht);  // shift y up to 0, ie y is always start from 0 [3/9/2015 Yuen]
}

SAVal CIM::ImgProperties(USHORT thres) {
	SAVal V;
	int wd, ht, bpp; GetDim(wd, ht, bpp); if (bpp != 24) return V;

	UINT count = 0;
	LONG bright = 0;
#pragma omp parallel for reduction(+:count) reduction(+:bright)
	for (int y = 0; y < ht; y++) {
		PBYTE des = (PBYTE)GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++) {
			USHORT v = (*(des++) + *(des++) + *(des++)) / 3;
			bright += v;
			if (v > thres) count++;
		}
	}
	float area = float(wd * ht);
	V.PCount = count / area;
	V.Bright = bright / area;
	return V;
}

std::vector<SAVal> CIM::ImgRegionProperties(USHORT thres) {
	float area;
	std::vector<SAVal>vCount;
	SAVal V;
	int wd, ht, bpp; GetDim(wd, ht, bpp);
	if (bpp != 24)  return std::vector<SAVal>();

	int dht = ht / 3, dwd = wd / 3;
	int dh2 = 2 * dht, dw2 = 2 * dwd;
	UINT count = 0;
	LONG bright = 0;
#pragma omp parallel for reduction(+:count) reduction(+:bright)
	// quadrant 1 [10/7/2020 FSM]
	for (int y = 0; y < dht; y++) {
		PBYTE des = (PBYTE)GetPixelAddress(0, y);
		for (int x = 0; x < dwd; x++) {
			USHORT v = (*(des++) + *(des++) + *(des++)) / 3;
			bright += v;
			if (v > thres) count++;
		}
	}
	area = float(dwd * dht);
	V.PCount = count / area;
	V.Bright = bright / area;
	vCount.push_back(V);

	count = 0; bright = 0;
#pragma omp parallel for reduction(+:count) reduction(+:bright)
	// quadrant 2 [10/7/2020 FSM]
	for (int y = 0; y < dht; y++) {
		PBYTE des = (PBYTE)GetPixelAddress(dwd, y);
		for (int x = dwd; x < dw2; x++) {
			USHORT v = (*(des++) + *(des++) + *(des++)) / 3;
			bright += v;
			if (v > thres) count++;
		}
	}
	area = float(dwd * dht);
	V.PCount = count / area;
	V.Bright = bright / area;
	vCount.push_back(V);

	count = 0; bright = 0;
#pragma omp parallel for reduction(+:count) reduction(+:bright)
	// quadrant 3 [10/7/2020 FSM]
	for (int y = 0; y < dht; y++) {
		PBYTE des = (PBYTE)GetPixelAddress(dw2, y);
		for (int x = dw2; x < wd; x++) {
			USHORT v = (*(des++) + *(des++) + *(des++)) / 3;
			bright += v;
			if (v > thres) count++;
		}
	}
	area = float(dwd * dht);
	V.PCount = count / area;
	V.Bright = bright / area;
	vCount.push_back(V);

	count = 0; bright = 0;
#pragma omp parallel for reduction(+:count) reduction(+:bright)
	// quadrant 4 [10/7/2020 FSM]
	for (int y = dht; y < dh2; y++) {
		PBYTE des = (PBYTE)GetPixelAddress(0, y);
		for (int x = 0; x < dwd; x++) {
			USHORT v = (*(des++) + *(des++) + *(des++)) / 3;
			bright += v;
			if (v > thres) count++;
		}
	}
	area = float(dwd * dht);
	V.PCount = count / area;
	V.Bright = bright / area;
	vCount.push_back(V);

	count = 0; bright = 0;
#pragma omp parallel for reduction(+:count) reduction(+:bright)
	// quadrant 5 [10/7/2020 FSM]
	for (int y = dht; y < dh2; y++) {
		PBYTE des = (PBYTE)GetPixelAddress(dwd, y);
		for (int x = dwd; x < dw2; x++) {
			USHORT v = (*(des++) + *(des++) + *(des++)) / 3;
			bright += v;
			if (v > thres) count++;
		}
	}
	area = float(dwd * dht);
	V.PCount = count / area;
	V.Bright = bright / area;
	vCount.push_back(V);

	count = 0; bright = 0;
#pragma omp parallel for reduction(+:count) reduction(+:bright)
	// quadrant 6 [10/7/2020 FSM]
	for (int y = dht; y < dh2; y++) {
		PBYTE des = (PBYTE)GetPixelAddress(dw2, y);
		for (int x = dw2; x < wd; x++) {
			USHORT v = (*(des++) + *(des++) + *(des++)) / 3;
			bright += v;
			if (v > thres) count++;
		}
	}
	area = float(dwd * dht);
	V.PCount = count / area;
	V.Bright = bright / area;
	vCount.push_back(V);

	count = 0; bright = 0;
#pragma omp parallel for reduction(+:count) reduction(+:bright)
	// quadrant 7 [10/7/2020 FSM]
	for (int y = dh2; y < ht; y++) {
		PBYTE des = (PBYTE)GetPixelAddress(0, y);
		for (int x = 0; x < dwd; x++) {
			USHORT v = (*(des++) + *(des++) + *(des++)) / 3;
			bright += v;
			if (v > thres) count++;
		}
	}
	area = float(dwd * dht);
	V.PCount = count / area;
	V.Bright = bright / area;
	vCount.push_back(V);

	count = 0; bright = 0;
#pragma omp parallel for reduction(+:count) reduction(+:bright)
	// quadrant 8 [10/7/2020 FSM]
	for (int y = dh2; y < ht; y++) {
		PBYTE des = (PBYTE)GetPixelAddress(dwd, y);
		for (int x = dwd; x < dw2; x++) {
			USHORT v = (*(des++) + *(des++) + *(des++)) / 3;
			bright += v;
			if (v > thres) count++;
		}
	}
	area = float(dwd * dht);
	V.PCount = count / area;
	V.Bright = bright / area;
	vCount.push_back(V);

	count = 0; bright = 0;
#pragma omp parallel for reduction(+:count) reduction(+:bright)
	// quadrant 9 [10/7/2020 FSM]
	for (int y = dh2; y < ht; y++) {
		PBYTE des = (PBYTE)GetPixelAddress(dw2, y);
		for (int x = dw2; x < wd; x++) {
			USHORT v = (*(des++) + *(des++) + *(des++)) / 3;
			bright += v;
			if (v > thres) count++;
		}
	}
	area = float(dwd * dht);
	V.PCount = count / area;
	V.Bright = bright / area;
	vCount.push_back(V);

	return vCount;
}

CIM::CIM() {}

CIM::~CIM() {}

//////////////////////////////////////////////////////////////////////////
// Display [11/3/2019 FSM]

void IMGL::CIM::SetGrayScale() {
	RGBQUAD RGB[256];
	for (int i = 0; i < 256; i++) {
		RGB[i].rgbRed = RGB[i].rgbGreen = RGB[i].rgbBlue = i;
	}
	SetColorTable(0, 256, RGB);
}

void IMGL::CIM::MakeGrayLUT() {
	if (GetBPP() == 8) {
		int n = 256;
		RGBQUAD* rgbColors = new RGBQUAD[n];
		for (int i = 0; i < n; i++) {
			rgbColors[i].rgbRed = i;
			rgbColors[i].rgbGreen = i;
			rgbColors[i].rgbBlue = i;
		}
		SetColorTable(0, n, rgbColors);
		delete[]rgbColors;
	}
}

void CIM::CopyLUT(CIM Image) {
	const int n = Image.GetMaxColorTableEntries();
	if (n) {
		RGBQUAD* rgbColors = new RGBQUAD[n];
		Image.GetColorTable(0, n, rgbColors);
		SetColorTable(0, n, rgbColors);
		delete[]rgbColors;
	}
}

BOOL CIM::DivideGrey(CIM& Im1, CIM& Im2) {
	// implement this = Im1 / Im2 [12/24/2020 FSM]
	if (Im1.IsNull()) return FALSE;
	if (Im2.IsNull()) return FALSE;
	if (!Im1.IsSameSize(Im2)) return FALSE;
	if (IsNull()) {
		if (!Create(Im1.GetWidth(), Im1.GetHeight(), Im1.GetBPP())) return FALSE;
	}
	if (IsNull()) return FALSE;
	if (!IsSameSize(Im1)) return FALSE;
	CopyLUT(Im1);

	// require a higher bpp to operate [1/7/2020 yuenl]
	int wd, ht, bpp; GetDim(wd, ht, bpp);
	if (bpp != 24) { ASSERT(0); return FALSE; }

	CIM16 Im16; Im16.Create(wd, ht); if (Im16.IsNull()) return FALSE;
#pragma omp parallel for
	for (int y = 0; y < ht; y++) {
		PBYTE pS1 = (PBYTE)Im1.GetPixelAddress(0, y);
		PBYTE pS2 = (PBYTE)Im2.GetPixelAddress(0, y);
		float* p = Im16.GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++, p++, pS1 += 3, pS2 += 3) {
			float u = (*(pS1)+*(pS1 + 1) * 2 + *(pS1 + 2)) / 4.f;
			float v = (*(pS2)+*(pS2 + 1) * 2 + *(pS2 + 2)) / 4.f;
			if (v) *p = u / v;
			else *p = 0;
		}
	}
	Im16.Make24BPPStretch(*this);
	//edge = Im1.edge;
	return TRUE;
}

BOOL CIM::DivideColor(CIM& Im1, CIM& Im2) {
	// implement this = Im1 / Im2 [12/24/2020 FSM]
	if (Im1.IsNull()) return FALSE;
	if (Im2.IsNull()) return FALSE;
	if (!Im1.IsSameSize(Im2)) return FALSE;
	if (IsNull()) {
		if (!Create(Im1.GetWidth(), Im1.GetHeight(), Im1.GetBPP())) return FALSE;
	}
	if (IsNull()) return FALSE;
	if (!IsSameSize(Im1)) return FALSE;
	CopyLUT(Im1);

	// require a higher bpp to operate [1/7/2020 yuenl]
	int wd, ht, bpp; GetDim(wd, ht, bpp);
	if (bpp != 24) { ASSERT(0); return FALSE; }

	CIM16x3 Im16; Im16.Create(wd, ht); if (Im16.IsNull()) return FALSE;
#pragma omp parallel for
	for (int y = 0; y < ht; y++) {
		PBYTE pS1 = (PBYTE)Im1.GetPixelAddress(0, y);
		PBYTE pS2 = (PBYTE)Im2.GetPixelAddress(0, y);
		float* p = Im16.GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++) {
			for (int i = 0; i < 3; i++) {
				if (*pS2) *p = (*pS1 / float(*pS2));
				else *p = 0;
				p++, pS1++, pS2++;
			}
		}
	}
	Im16.Make24BPPStretch(*this);
	//edge = Im1.edge;
	return TRUE;
}

BOOL IMGL::CIM::SubAbs(CIM& Im1, CIM& Im2) {
	if (Im1.IsNull() || Im2.IsNull()) return FALSE;
	if (!Im1.IsSameSize(Im2)) return FALSE;
	int wd, ht, bpp;
	Im1.GetDim(wd, ht, bpp);
	if (IsNull()) {
		if (!Create(wd, ht, bpp)) return FALSE;
		if (IsNull()) return FALSE;
	}
	if (!IsSameSize(Im1)) {
		Destroy();
		if (!Create(wd, ht, bpp)) return FALSE;
		if (IsNull()) return FALSE;
	}
	if (bpp == 8) CopyLUT(Im1);
	for (int y = 0; y < ht; y++) {
		SRGB* pSrc1 = (SRGB*)Im1.GetPixelAddress(0, y);
		SRGB* pSrc2 = (SRGB*)Im2.GetPixelAddress(0, y);
		SRGB* pDes = (SRGB*)GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++, pDes++, pSrc1++, pSrc2++) {
			short wh1 = pSrc1->White(), wh2 = pSrc2->White();
			pDes->SetGray(abs(wh1 - wh2));
		}
	}
	return TRUE;
}

// remove background [10/7/2020 FSM]

BOOL IMGL::CIM::CreateA(int wd, int ht, int bpp) {
	if (!IsNull()) {
		if ((GetWidth() != wd) || (GetHeight() != ht) || (GetBPP() != bpp)) {
			Destroy();
		}
	}
	if (IsNull()) return Create(wd, ht, bpp);

	return TRUE;
}

CIM& CIM::operator-=(CIM& Rhs) {
	if (this == &Rhs) return *this;
	if (Rhs.IsNull()) return *this;
	int wd, ht, bpp; GetDim(wd, ht, bpp);
	if (IsNull()) {
		if (!Create(wd, ht, bpp)) { ASSERT(0); return *this; }
		if (IsNull()) return *this;
	}
	if (!IsSameSize(Rhs)) {
		Destroy();
		if (!Create(wd, ht, bpp)) { ASSERT(0); return *this; }
		if (IsNull()) { ASSERT(0); return *this; }
	}
	if (bpp == 8) CopyLUT(Rhs);

	switch (bpp) {
	case 8:
		break;
	case 24:
	case 32:
#pragma omp parallel for
		for (int y = 0; y < ht; y++) {
			PBYTE pSrc = (PBYTE)this->GetPixelAddress(0, y);
			PBYTE pDes = (PBYTE)GetPixelAddress(0, y);
			for (int x = 0; x < wd; x++) {
				short v;
				v = *pDes - *(pSrc++); if (v >= 0) *(pDes++) = BYTE(v); else *(pDes++) = 0;
				v = *pDes - *(pSrc++); if (v >= 0) *(pDes++) = BYTE(v); else *(pDes++) = 0;
				v = *pDes - *(pSrc++); if (v >= 0) *(pDes++) = BYTE(v); else *(pDes++) = 0;
				if (bpp == 32) { pSrc++; pDes++; }
			}
		}
		break;
	default: ASSERT(0); return *this; break;
	}
	return *this;
}

const CIM& CIM::operator=(const CIM& Im) {
	if (this == &Im) return *this;
	if (Im.IsNull()) { ASSERT(0); return *this; }
	int wd, ht, bpp; Im.GetDim(wd, ht, bpp);
	if (IsNull()) {
		if (!Create(wd, ht, bpp)) { ASSERT(0); return *this; }
	}
	if (!IsSameSize(Im)) {
		Destroy(); if (!Create(wd, ht, bpp)) { ASSERT(0); return *this; }
	}
	if (bpp == 8) SetGrayScale();
	memcpy(GetBufferStart(), Im.GetBufferStart(), wd * ht * bpp / 8);
	return *this;
}

const CIM& CIM::operator=(const CIM2& Im) {
	if (Im.IsNull()) return *this;
	int wd, ht, bpp = 24; Im.GetDim(wd, ht);
	if (!IsNull()) { Destroy(); }
	if (!Create(wd, ht, bpp)) { ASSERT(0); return *this; }
	USHORT Mn, Mx; int ave;
	Im.MaxMinAve(Mn, Mx, ave);
	float sf = 255 / float(Mx - Mn);
	for (int y = 0; y < ht; y++) {
		USHORT* p = Im.GetPixelAddress(0, y);
		SRGB* q = (SRGB*)GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++, p++, q++) {
			q->SetGray(BYTE(sf * (*p - Mn)));
		}
	}
	return *this;
}

void SEdge::Clear() {
	Inner.clear(); Outer.clear(); width.clear();
}

BOOL SEdge::IsValid(int i) {
	if ((Inner[i].x != -1) && (Inner[i].y != -1) &&
		(Outer[i].x != -1) && (Outer[i].y != -1)) return TRUE;
	return FALSE;
}

void SEdge::DumpConst(FILE* fp) {
	fwprintf(fp, _T("Distance:,%.3f\n"), AveWidth());
	fwprintf(fp, _T("Px:,%.3f\n"), px);
	fwprintf(fp, _T("Py:,%.3f\n\n"), py);
}

void SEdge::DumpHeader(FILE* fp) {
	fwprintf(fp, _T("Length, Inner X,Inner Y,Outer X,Outer Y\n"));
}

void SEdge::Dump(FILE* fp) {
	size_t sz = width.size();
	for (size_t i = 0; i < sz; i++) {
		fwprintf(fp, _T("%.4f,%s,%s\n"), width[i], Inner[i].str().GetBuffer(), Outer[i].str().GetBuffer());
	}
}

float SEdge::AveWidth() {
	float sum = 0;
	size_t sz = width.size(); if (!sz) return 0;
	for (size_t i = 0; i < sz; i++) {
		sum += width[i];
	}
	return sum / float(sz);
}

// location of edge on image [1/7/2020 yuenl]

CString SLoc::str() {
	CString s; s.Format(_T("%d,%d"), x, y); return s;
}

BOOL CIM::Convert32Bit() {
	//-----------------------------------------------------------------------
	// This function converts 8, 24, & 32 bit DIBs to 32 bit
	// flip image buffer top-down. No padding bytes
	//-----------------------------------------------------------------------
	byte* t, r, g, b;
	UINT32* buf, * p, * q;
	ULONG i, j, nx, ny;
	RGBQUAD* pRGB = new RGBQUAD[256];   // For GetDIBColorTable()

	nx = GetWidth();
	ny = GetHeight();
	unsigned long n = nx * ny;   // No. of pixels

	// Allocate n sized buffer for temp storage
	buf = new UINT32[n]; if (!buf) return FALSE;
	memset(buf, 0, n * sizeof(UINT32));

	switch (GetBPP()) {
	case 8:
		if (!(i = GetDIBColorTable(GetDC(), 0, 256, pRGB))) {
			ReleaseDC(); goto End;
		}
		ReleaseDC();

		for (j = 0, q = buf; j < ny; j++) {
			t = (byte*)GetPixelAddress(0, j);
			for (i = 0; i < nx; i++, t++, q++) {
				r = pRGB[*t].rgbRed;
				g = pRGB[*t].rgbGreen;
				b = pRGB[*t].rgbBlue;
				*q = RGB(b, g, r);    // CImage is BGR
			}
		}
		break;
	case 24:
		for (j = 0, q = buf; j < ny; j++) {
			t = (byte*)GetPixelAddress(0, j);
			for (i = 0; i < nx; i++, t++, q++) {
				b = *t;        // CImage is BGR
				g = *(++t);
				r = *(++t);
				*q = RGB(b, g, r);
			}
		}
		break;
	case 32:   // Just need to make top-down
		for (j = 0, q = buf; j < ny; j++) {
			p = (UINT32*)GetPixelAddress(0, j);
			for (i = 0; i < nx; i++, p++, q++) {
				*q = *p;
			}
		}
		break;
	}

	// Start a new CImage
	Destroy();
	if (!Create(nx, -(int)ny, 32, 0)) {
		goto End;
	}
	p = (UINT32*)GetBits();   // Ptr to new bitmap (top-down DIB)
	memcpy_s(p, n * sizeof(UINT32), buf, n * sizeof(UINT32)); // Copy buf to bitmap
	//ptype = cRGB;        // Update pixel type

End:
	if (buf) delete[]buf;
	return TRUE;
}

BOOL CIM::Convert24Bit() {
	//-----------------------------------------------------------------------
	// This function converts 8, 24, & 32 bit DIBs to 32 bit
	// flip image buffer top-down. No padding bytes
	//-----------------------------------------------------------------------
	byte* t, r, g, b;
	byte* buf, * p, * q;
	ULONG i, j, nx, ny;
	RGBQUAD* pRGB = new RGBQUAD[256];   // For GetDIBColorTable()

	nx = GetWidth();
	ny = GetHeight();
	unsigned long n = 3 * (nx * ny);   // No. of pixels

	// Allocate n sized buffer for temp storage
	buf = new byte[n]; if (!buf) return FALSE;
	memset(buf, 0, n * sizeof(byte));

	switch (GetBPP()) {
	case 8:
		if (!(i = GetDIBColorTable(GetDC(), 0, 256, pRGB))) {
			ReleaseDC(); goto End;
		}
		ReleaseDC();

		for (j = 0, q = buf; j < ny; j++) {
			t = (byte*)GetPixelAddress(0, j);
			for (i = 0; i < nx; i++, t++, q += 3) {
				r = pRGB[*t].rgbRed;
				g = pRGB[*t].rgbGreen;
				b = pRGB[*t].rgbBlue;
				*q = RGB(b, g, r);    // CImage is BGR
			}
		}
		break;
	case 24:
		for (j = 0, q = buf; j < ny; j++) {
			t = (byte*)GetPixelAddress(0, j);
			for (i = 0; i < nx; i++, t++, q += 3) {
				b = *t;        // CImage is BGR
				g = *(++t);
				r = *(++t);
				*q = RGB(b, g, r);
			}
		}
		break;
	case 32:   // Just need to make top-down
		for (j = 0, q = buf; j < ny; j++) {
			t = (byte*)GetPixelAddress(0, j);
			for (i = 0; i < nx; i++, t += 2, q += 3) {
				b = *t;        // CImage is BGR
				g = *(++t);
				r = *(++t);
				*q = RGB(b, g, r);
			}
		}
		break;
	}

	// Start a new CImage
	Destroy();
	if (!Create(nx, -(int)ny, 24, 0)) {
		goto End;
	}
	p = (byte*)GetBits();   // Ptr to new bitmap (top-down DIB)
	memcpy_s(p, n, buf, n); // Copy buf to bitmap

End:
	if (buf) delete[]buf;
	return TRUE;
}

void IMGL::CIM::RetainRed() {
	if (IsNull()) return;
	int wd, ht, bpp; GetDim(wd, ht, bpp); if (bpp != 24) return;
#pragma omp parallel for
	for (int y = 0; y < ht; y++) {
		PBYTE p = (PBYTE)GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++) {
			*(p++) = 0; *(p++) = 0; p++;
		}
	}
}

void IMGL::CIM::RetainGreen() {
	if (IsNull()) return;
	int wd, ht, bpp; GetDim(wd, ht, bpp); if (bpp != 24) return;
#pragma omp parallel for
	for (int y = 0; y < ht; y++) {
		PBYTE p = (PBYTE)GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++) {
			*(p++) = 0; p++; *(p++) = 0;
		}
	}
}

void IMGL::CIM::RetainBlue() {
	if (IsNull()) return;
	int wd, ht, bpp; GetDim(wd, ht, bpp); if (bpp != 24) return;
#pragma omp parallel for
	for (int y = 0; y < ht; y++) {
		PBYTE p = (PBYTE)GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++) {
			p++;  *(p++) = 0; *(p++) = 0;
		}
	}
}

void IMGL::CIM::RetainGrey() {
	if (IsNull()) return;
	int wd, ht, bpp; GetDim(wd, ht, bpp); if (bpp != 24) return;
#pragma omp parallel for
	for (int y = 0; y < ht; y++) {
		PBYTE p = (PBYTE)GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++) {
			BYTE v = BYTE((*p + *(p + 1) * 2 + *(p + 2)) / 4);
			*(p++) = v;  *(p++) = v; *(p++) = v;
		}
	}
}

void IMGL::CIM::Select(bool bRed, bool bGrn, bool bBlu) {
	if (IsNull()) return;
	int wd, ht, bpp;
	GetDim(wd, ht, bpp);
	if (bpp == 8) return;
	if (bpp != 24) { ASSERT(0); return; }
	for (int y = 0; y < ht; y++) {
		PBYTE p = (PBYTE)GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++) {
			if (!bBlu) *p = 0; p++;
			if (!bGrn) *p = 0; p++;
			if (!bRed) *p = 0; p++;
		}
	}
}

short CIM::GetI(int x, int y, eCH nChan) {
	// No sanity check [2/13/2021 FSM]
	PBYTE b = (PBYTE)GetPixelAddress(x, y);
	switch (GetBPP()) {
	case 8: // 8 bits image [11/3/2019 FSM]
		return short(*b);
		break;
	case 24: // 24 bits image [11/3/2019 FSM]
	case 32: // 32 bits image [11/3/2019 FSM]
		switch (nChan) {
		case eCH::REDC:
			return short(*(b + 2));
			break;
		case eCH::GRNC:
			return short(*(b + 1));
			break;
		case eCH::BLUC:
			return short(*b);
			break;
		default:
#ifdef FULLWHITE
#ifdef HALFWHITE
			return short((*(b + 2) + *(b + 1) + *b) / 3);
#else
			return short((*(b + 2) + *(b + 1) * 2 + *b) / 4);
#endif
#else
#ifdef HALFWHITE
			return short((*(b + 1) + *b) / 2);
#else
			return short((*(b + 1) * 2 + *b) / 3);
#endif
#endif
			break;
		}
		break;
	default: ASSERT(0); break;
	}
	return 0;
}

BOOL IMGL::CIM16::IsNull() {
	if (!pPix) return TRUE;
	if ((wd < 1) || (ht < 1)) return TRUE;
	return FALSE;
}

BOOL IMGL::CIM16::IsSameSize(CIM16& Image) {
	if ((wd != Image.wd) || (ht != Image.ht)) return FALSE;
	return TRUE;
}

BOOL IMGL::CIM16::Create(int w, int h) {
	if (pPix) {
		if ((w != wd) || (h != ht)) {
			if (pPix) delete[]pPix; //if (pPhMo) delete[]pPhMo;
			pPix = new float[(w + 1) * (h + 1)]; if (!pPix) return FALSE;
			//pPhMo = new SPhMo[w * h];
		}
	}
	else {
		if (pPix) delete[]pPix; //if (pPhMo) delete[]pPhMo;
		pPix = new float[(w + 1) * (h + 1)]; if (!pPix) return FALSE;
	}
	if (!pPix) {
		if (pPix) delete[]pPix; pPix = nullptr;
		return FALSE;
	}
	wd = w; ht = h;
	memset(pPix, 0, wd * ht * sizeof(float));
	return TRUE;
}

void IMGL::CIM16::Dealloc() {
	wd = ht = 0;
	if (pPix) delete[]pPix; pPix = nullptr;
}

BOOL IMGL::CIM16::SetBuffer(float* pBuf, int len) {
	if (len != (wd * ht)) return FALSE;
	memcpy(pPix, pBuf, len * sizeof(float));
	return TRUE;
}

BOOL IMGL::CIM16::WavinessH(CIM16& Im) {
	GetDim(wd, ht);
	if (!Im.Create(wd, ht)) return FALSE;
	int wdw = wd / 6; // window shall be 10% of total len [4/29/2021 FSM]
	int wdw2 = wdw / 2;
	for (int y = 0; y < ht; y++) {
		for (int i = 0; i < wd; i++) {
			int st = i - wdw2; if (st < 0) st = 0;
			int ed = i + wdw2; if (ed >= wd) ed = wd - 1;
			int n = 0;
			float sum = 0;
			float* p16 = GetPixelAddress(st, y);
			for (int j = st; j <= ed; j++, p16++) {
				if (*p16 == BADDATA) continue;
				sum += *p16; n++;
			}
			Im.SetPixel(i, y, sum / float(n));
		}
	}
	return TRUE;
}

BOOL IMGL::CIM16::WavinessV(CIM16& Im) {
	GetDim(wd, ht);
	if (!Im.Create(wd, ht)) return FALSE;
	int wdw = ht / 6; // window shall be 10% of total len [4/29/2021 FSM]
	int wdw2 = wdw / 2;
	for (int x = 0; x < wd; x++) {
		for (int i = 0; i < ht; i++) {
			int st = i - wdw2; if (st < 0) st = 0;
			int ed = i + wdw2; if (ed >= ht) ed = ht - 1;
			int n = 0;
			float sum = 0;
			for (int j = st; j <= ed; j++) {
				float* p16 = (float*)GetPixelAddress(x, j);
				if (*p16 == BADDATA) continue;
				sum += *p16; n++;
			}
			Im.SetPixel(x, i, sum / float(n));
		}
	}
	return TRUE;
}

BOOL IMGL::CIM16::SetBuffer(CIM16& Im1, CIM16& Im2) {
	if (IsNull() || Im1.IsNull() || Im2.IsNull()) return FALSE;
	if (Im1.size() != Im2.size()) return FALSE;
	if (size() != Im1.size()) return FALSE;

#pragma omp parallel for
	for (int y = 0; y < ht; y++) {
		float* pS = GetPixelAddress(0, y);
		float* pX = Im1.GetPixelAddress(0, y);
		float* pY = Im2.GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++, pS++, pX++, pY++) {
			if ((*pX == BADDATA) || (*pY == BADDATA)) { *pS = BADDATA; continue; }
			*pS = (*pX + *pY) / 2.f;
		}
	}
	return TRUE;
}

float* IMGL::CIM16::GetBuffer() { return pPix; }

float IMGL::CIM16::GetPixel(int x, int y) {
	// no sanity check [2/26/2021 FSM]
	return pPix[y * wd + x];
}

void IMGL::CIM16::SetPixel(int x, int y, float val) {
	// no sanity check [2/26/2021 FSM]
	pPix[y * wd + x] = val;
}

void IMGL::CIM16::AddPixel(int x, int y, float val) {
	// no sanity check [2/26/2021 FSM]
	pPix[y * wd + x] += val;
}

void IMGL::CIM16::OffsetPixel(int x, int y, float val) {
	// no sanity check [2/26/2021 FSM]
	pPix[y * wd + x] += val;
}

void IMGL::CIM16::Smooth(USHORT N) {
	for (int i = 0; i < N; i++) {
		// 5x5 moving average [10/31/2020 yuenl]
		// int GaussBlur[25] = {
		// 	1, 4,  6,  4,  1,
		// 	4, 16, 24, 16, 4,
		// 	6, 24, 36, 24, 6,
		// 	4, 16, 24, 16, 4,
		// 	1, 4,  6,  4,  1
		// };
		int GaussBlur[25] = {
			2,  4,  5,  4,  2,
			4,  9, 12,  9,  4,
			5, 12, 15, 12,  5,
			4,  9, 12,  9,  4,
			2,  4,  5,  4,  2
		};
		Convl5x5(GaussBlur);
	}
}

BOOL IMGL::CIM16::DumpVLine(char* fname, int x) {
	FILE* fp = fopen(fname, "wb"); if (!fp) return FALSE;
	for (int y = 0; y < ht; y++) {
		fprintf(fp, "%d,%f\n", y + 1, GetPixel(x, y));
	}
	fclose(fp);
	return TRUE;
}

#include <fstream>

BOOL IMGL::CIM16::GetHLn(Sln& ln, int y) {
	if (IsNull()) return FALSE;
	std::ofstream myFile("C:\\WLI\\all_height_data.txt"); // 05302023 - Mortuja
	int wd, ht; GetDim(wd, ht);
	SPtS q;
	ln.Pts.clear();

	// 05302023 - Mortuja

	q.y = 0.0, q.t = 0.0;
	for (int idx = 0; idx < ICC.x1; idx++) {
		q.x = idx;
		ln.Pts.push_back(q);
	}

	//float* p = GetPixelAddress(0, y);
	float* p = GetPixelAddress(ICC.x1, y);
	myFile << "X1 = " << ICC.x1 << " X2 = " << ICC.x2 << " Y1 = " << ICC.y1 << std::endl;
	for (int x = ICC.x1; x < ICC.x2; x++, p++) {
		if (*p == BADDATA) continue;
		q.x = float(x); q.y = *p;
		ln.Pts.push_back(q);
	}

	q.y = 0.0, q.t = 0.0;
	for (int idx = ICC.x2; idx < wd; idx++) {
		q.x = idx;
		ln.Pts.push_back(q);
	}

	for (auto X : ln.Pts) myFile << X.x << " " << X.y << std::endl;
	return TRUE;
}

BOOL IMGL::CIM16::GetAreaAvgHLn(Sln& ln, int y) { // 07122023
	if (IsNull()) return FALSE;
	std::ofstream myFile("C:\\WLI\\Area_Avg_height_data.txt");
	int wd, ht; GetDim(wd, ht);
	SPtS q;
	ln.Pts.clear();

	q.y = 0.0, q.t = 0.0;
	for (int idx = 0; idx < ICC.x1; idx++) {
		q.x = idx;
		ln.Pts.push_back(q);
	}

	std::map<int, double>AvgHeightData;
	myFile << "X1 = " << ICC.x1 << " X2 = " << ICC.x2 << " Y1 = " << ICC.y1 << std::endl;

	//20230801=======================================
	if (!ICC.LineData.empty()) {
		for (int i = 0; i < ICC.LineData.size(); i++) {
			ICC.LineData.clear();
		}
	}
	ICC.LineData.clear();
	ICC.LineData = std::vector<std::vector<double>>(ICC.y2 - ICC.y1 + 1, std::vector<double>(ICC.x2 - ICC.x1 + 1));
	//20230801 ===================================================

	for (int yy = ICC.y1; yy < ICC.y2; yy++) {
		float* p = GetPixelAddress(ICC.x1, yy);
		for (int x = ICC.x1; x < ICC.x2; x++, p++) {
			ICC.LineData[yy - ICC.y1][x - ICC.x1] = *p; //20230801
			if (*p == BADDATA) continue;
			AvgHeightData[x] += *p;
		}
	}

	for (int x = ICC.x1; x < ICC.x2; x++) {
		q.x = float(x); q.y = AvgHeightData[x] / (ICC.y2 - ICC.y1 + 1);
		ln.Pts.push_back(q);
	}

	q.y = 0.0, q.t = 0.0;
	for (int idx = ICC.x2 + 1; idx < wd; idx++) {
		q.x = idx;
		ln.Pts.push_back(q);
	}

	for (auto X : ln.Pts) myFile << X.x << " " << X.y << std::endl;
	myFile.close();

	return TRUE;
}

BOOL IMGL::CIM16::GetVLn(Sln& ln, int x) {
	if (IsNull()) return FALSE;
	std::ofstream myFile("C:\\WLI\\all_height_data.txt"); // 05302023 - Mortuja
	int wd, ht; GetDim(wd, ht);
	SPtS q;
	ln.Pts.clear();

	// 05302023 - Mortuja

	q.y = 0.0, q.t = 0.0;
	for (int idx = 0; idx < ICC.y1; idx++) {
		q.x = idx;
		ln.Pts.push_back(q);
	}

	myFile << "X1 = " << ICC.x1 << " Y1 = " << ICC.y1 << " Y2 = " << ICC.y2 << std::endl;
	for (int y = ICC.y1; y < ICC.y2; y++) {
		float* p = GetPixelAddress(x, y);
		if (*p == BADDATA) continue;
		q.x = float(y); q.y = *p;
		ln.Pts.push_back(q);
	}

	q.y = 0.0, q.t = 0.0;
	for (int idx = ICC.y2; idx < ht; idx++) {
		q.x = idx;
		ln.Pts.push_back(q);
	}

	for (auto X : ln.Pts) myFile << X.x << " " << X.y << std::endl;
	myFile.close();

	return TRUE;
}

BOOL IMGL::CIM16::DumpHLine(char* fname, int y) {
	FILE* fp = fopen(fname, "wb"); if (!fp) return FALSE;
	for (int x = 0; x < wd; x++) {
		fprintf(fp, "%d,%f\n", x + 1, GetPixel(x, y));
	}
	fclose(fp);
	return TRUE;
}

void CIM16::ConvlLowPass() {
	Convl(0.0625f, 0.125f, 0.0625f, 0.125f, 0.25f, 0.125f, 0.0625f, 0.125f, 0.0625f);
}

void IMGL::CIM16::Level() {
	if (IsNull())return;
	CLSF3D LSF3D;
	// make use of height image [10/27/2020 yuenl]
	// Find LS plane [3/10/2011 FSMT]
	int skip = 8;
	for (int y = 0; y < ht; y += skip) {
		float* p16 = GetPixelAddress(0, y);
		for (int x = 0; x < wd; x += skip, p16 += skip) {
			if (*p16 == BADDATA) continue;
			LSF3D.Input(x, y, *p16);
		}
	}
	// Normalize data: All points [3/10/2011 FSMT]
	float fMin = FLT_MAX; float fMax = -(FLT_MAX - 1);
	double dfA, dfB, dfC;
	if (LSF3D.GetResult(dfA, dfB, dfC)) {
		//double dfDivider = sqrt(dfA * dfA + dfB * dfB + 1);	// This is wrong [5/10/2013 user]
		for (int y = 0; y < ht; y++) {
			float dfNorDist;
			float* p = GetPixelAddress(0, y);
			for (int x = 0; x < wd; x++, p++) {
				if (*p != BADDATA) {
					dfNorDist = float(*p - dfC - dfA * x - dfB * y);
					if (dfNorDist > fMax) fMax = dfNorDist;
					if (dfNorDist < fMin) fMin = dfNorDist;
					SetPixel(x, y, dfNorDist);
				}
			}
		}
		float off = float(fabs(fMin));
		for (int y = 0; y < ht; y++) {
			float* p = GetPixelAddress(0, y);
			for (int x = 0; x < wd; x++, p++) {
				if (*p != BADDATA) {
					*p += off;
				}
			}
		}
		fMax += off; fMin += off;
	}
}

void CIM16::Convl(float k1, float k2, float k3, float k4, float k5, float k6, float k7, float k8, float k9) {
	// 3x3 kernel operation [10/31/2020 yuenl]
	if (IsNull()) return;

	// Calculate sum of kernel
	float fsum = k1 + k2 + k3 + k4 + k5 + k6 + k7 + k8 + k9;
	if (fsum == 0) fsum = 1; // Avoid div. by 0
	else fsum = 1 / fsum; // Invert so can multiply.

	float* buf = new float[wd * ht];
#pragma omp parallel for
	for (int j = 1; j < int(ht - 1); j++) {
		float* p;
		float* r1 = GetPixelAddress(0, j - 1);
		float* r2 = GetPixelAddress(0, j);
		float* r3 = GetPixelAddress(0, j + 1);
		p = buf + j * wd + 1;
		// convolve here [10/31/2020 yuenl]
		for (int i = 1; i < (wd - 1); i++, r1++, r2++, r3++, p++) {
			*p = (k1 * r1[0] + k2 * r1[1] + k3 * r1[2]
				+ k4 * r2[0] + k5 * r2[1] + k6 * r2[2]
				+ k7 * r3[0] + k8 * r3[1] + k9 * r3[2]) * fsum;
		}
	}
	//replace original pixels [10/31/2020 yuenl]
	for (int j = 1; j < (ht - 1); j++) {
		memcpy(GetPixelAddress(1, j), buf + j * wd + 1, (wd - 2) * sizeof(float));
	}
	if (buf) delete[]buf;

	//CIM Im; Convert24BPP(Im, 0); Im.Save(_T("C:\\TEMP\\AA.BMP"));
}

void CIM16::Convl5x5(int* kernel) {
	// 5x5 kernel operation [10/31/2020 yuenl]
	if (IsNull()) return;

	// Calculate sum of kernel
	float fsum = 0;
	for (int i = 0; i < 25; i++) {
		fsum += kernel[i];
	}
	if (fsum != 0) fsum = 1 / fsum; // Invert so can multiply.
	else fsum = 1; // Avoid div. by 0

	const int* k11 = kernel, * k12 = k11 + 1, * k13 = k12 + 1, * k14 = k13 + 1, * k15 = k14 + 1;
	const int* k21 = kernel + 5, * k22 = k21 + 1, * k23 = k22 + 1, * k24 = k23 + 1, * k25 = k24 + 1;
	const int* k31 = kernel + 10, * k32 = k31 + 1, * k33 = k32 + 1, * k34 = k33 + 1, * k35 = k34 + 1;
	const int* k41 = kernel + 15, * k42 = k41 + 1, * k43 = k42 + 1, * k44 = k43 + 1, * k45 = k44 + 1;
	const int* k51 = kernel + 20, * k52 = k51 + 1, * k53 = k52 + 1, * k54 = k53 + 1, * k55 = k54 + 1;
	float* buf = new float[wd * ht]; // new image buffer [11/20/2020 FSM]
	//memset(buf, 0, wd * ht * sizeof(float));
#pragma omp parallel for
	for (int j = 2; j < int(ht - 2); j++) {
		float* r1 = GetPixelAddress(0, j - 2);
		float* r2 = GetPixelAddress(0, j - 1);
		float* r3 = GetPixelAddress(0, j);
		float* r4 = GetPixelAddress(0, j + 1);
		float* r5 = GetPixelAddress(0, j + 2);
		float* p = buf + j * wd + 2;
		// convolve here [10/31/2020 yuenl]
		for (int i = 2; i < (wd - 2); i++, r1++, r2++, r3++, r4++, r5++, p++) {
			if (*p == 0) continue;
			*p = (
				(*k11) * r1[0] + (*(k12)) * r1[1] + (*(k13)) * r1[2] + (*(k14)) * r1[3] + (*(k15)) * r1[4] +
				(*k21) * r2[0] + (*(k22)) * r2[1] + (*(k23)) * r2[2] + (*(k24)) * r2[3] + (*(k25)) * r2[4] +
				(*k31) * r3[0] + (*(k32)) * r3[1] + (*(k33)) * r3[2] + (*(k34)) * r3[3] + (*(k35)) * r3[4] +
				(*k41) * r4[0] + (*(k42)) * r4[1] + (*(k43)) * r4[2] + (*(k44)) * r4[3] + (*(k45)) * r4[4] +
				(*k51) * r5[0] + (*(k52)) * r5[1] + (*(k53)) * r5[2] + (*(k54)) * r5[3] + (*(k55)) * r5[4]
				) * fsum;
		}
		//replace original pixels [10/31/2020 yuenl
		memcpy(GetPixelAddress(2, j), buf + j * wd + 2, (wd - 4) * sizeof(float));
	}
	//replace original pixels [10/31/2020 yuenl
	// for (int j = 2; j < (ht - 2); j++) {
	// 	memcpy(GetPixelAddress(2, j), buf + j * wd + 2, (wd - 4) * sizeof(float));
	// }
	if (buf) delete[]buf;
}

void IMGL::CIM16::SubtractNoise(CIM16& ImSmth) {
	int wd = GetWidth(); int ht = GetHeight();
	for (int y = 0; y < ht; y++) {
		float* p = GetPixelAddress(0, y);
		float* q = ImSmth.GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++, p++, q++) {
			//if ((*p - *q) > 0.005f) *p = *q;
			if ((*p - *q) > 0.0015f) *p = 0;
		}
	}
}

void IMGL::CIM16::FlipX() {
	for (int y = 0; y < ht; y++) {
		float* p1 = GetPixelAddress(0, y);
		float* p2 = GetPixelAddress(wd - 1, y);
		int wd2 = wd / 2;
		for (int x = 0; x < wd2; x++, p1++, p2--) {
			float v = *p1; *p1 = *p2; *p2 = v;
		}
	}
}

void IMGL::CIM16::FlipY() {
	CIM16 Im; Im.Create(wd, ht); if (Im.IsNull()) return;
	for (int y = 0; y < ht; y++) {
		float* p1 = Im.GetPixelAddress(0, y);
		float* p2 = GetPixelAddress(0, ht - 1 - y);
		memcpy(Im.GetPixelAddress(0, y), GetPixelAddress(0, ht - 1 - y), wd * sizeof(float));
	}
	memcpy(GetPixelAddress(0, 0), Im.GetPixelAddress(0, 0), wd * ht * sizeof(float));
}

void IMGL::CIM16::Add(CIM16& Lhs, CIM16& Rhs) {
	if (Lhs.IsNull() || Rhs.IsNull()) return;
	if (!Lhs.IsSameSize(Rhs)) return;
	wd = Lhs.GetWidth(); ht = Lhs.GetHeight();
	if (!Create(wd, ht)) return;
	for (int y = 0; y < ht; y++) {
		float* pLHS = (float*)Lhs.GetPixelAddress(0, y);
		float* pRHS = (float*)Rhs.GetPixelAddress(0, y);
		float* pDes = (float*)GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++, pLHS++, pRHS++, pDes++) {
			*pDes = *pLHS + *pRHS;
		}
	}
}

void IMGL::CIM16::Destroy() {
	wd = ht = 0;
	if (pPix) delete[]pPix; pPix = nullptr;
}

bool CIM16::UnwrapTB() {
	if (IsNull()) { ASSERT(0); return false; }
	float lmt = PIE;
	float lmt2 = 2 * lmt, lmt3 = 3 * lmt, lmt4 = 2 * lmt2;

	// seeds [2/21/2021 FSM]
	float w = GetPixel(0, 0);
	for (int x = 1; x < wd; x++) {
		int c = 0;
		float u = 0, v;
		v = GetPixel(x, 0);
		while ((fabs(v - w) >= lmt)) {
			c++; if (c > IRPT) break;
			if (v > w) u = -lmt2; else if (v < w) u = lmt2; else break;
			for (int i = x; i < wd; i++) {
				*(pPix + i) += u;
			}
			v += u;
		}
		w = v;
	}

#pragma omp parallel for
	for (int x = 0; x < wd; x++) {
		int c = 0;
		float u = 0, v;
		float w = GetPixel(x, 0);
		// odd line [2/28/2021 FSM]
		for (int y = 1; y < ht; y++) {
			v = GetPixel(x, y);
			while (float(fabs(v - w)) >= lmt) {
				c++; if (c > IRPT) break;
				if (v > w) u = -lmt2; else if (v < w) u = lmt2; else break;
				for (int i = y; i < ht; i++) {
					*(pPix + i * wd + x) += u;
				}
				v += u;
			}
			w = v;
		}
	}
	return true;
}

bool CIM16::UnwrapRL() {
	if (IsNull()) { ASSERT(0); return false; }
	float lmt = PIE;
	float lmt2 = 2 * lmt, lmt3 = 3 * lmt, lmt4 = 2 * lmt2;

	// seeding [2/21/2021 FSM]
	int x = wd - 1;
	float w = GetPixel(x, 0);
	for (int y = 1; y < ht; y++) {
		int c = 0;
		float u = 0, v;
		v = GetPixel(x, y);
		while (float(fabs(v - w)) >= lmt) {
			c++; if (c > IRPT) break;
			if (v > w) u = -lmt2; else if (v < w) u = lmt2; else break;
			for (int i = y; i < ht; i++) {
				*(pPix + i * wd + x) += u;
			}
			v += u;
		}
		w = v;
	}

#pragma omp parallel for
	for (int y = 0; y < ht; y++) {
		int c = 0;
		float u = 0, v;
		float w = GetPixel(wd - 1, y);
		// odd line [2/28/2021 FSM]
		for (int x = wd - 1; x >= 1; x--) {
			v = GetPixel(x, y);
			while ((float(fabs(v - w)) >= lmt)) {
				c++; if (c > IRPT) break;
				if (v > w) u = -lmt2; else if (v < w) u = lmt2; else break;
				for (int i = x; i >= 1; i--) {
					*(pPix + y * wd + i) += u;
				}
				v += u;
			}
			w = v;
		}
	}
	return true;
}

void IMGL::CIM16::ZeroMean() {
	int n = 0, wd = GetWidth(), ht = GetHeight();
	double sum = 0;
	for (int y = 0; y < ht; y++) {
		float* p16 = GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++, p16++) {
			if (*p16 == BADDATA) continue;
			sum += *p16; n++;
		}
	}
	sum /= float(n);
	Offset(float(sum));
}

void IMGL::CIM16::ZeroShift() {
	if (IsNull()) return;
	float min = FLT_MAX;
	int wd = GetWidth(), ht = GetHeight();
	for (int y = 0; y < ht; y++) {
		float* p16 = GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++, p16++) {
			if (*p16 < min) {
				if (*p16 == BADDATA) continue;
				min = *p16;
			}
		}
	}
	Offset(min);
}

void IMGL::CIM16::Offset(float off) {
	int wd = GetWidth(), ht = GetHeight();
	for (int y = 0; y < ht; y++) {
		float* p = GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++, p++) {
			if (*p == BADDATA) continue;
			*p -= off;
		}
	}
}

float* IMGL::CIM16::GetBufferStart() {
	return pPix;
}

float* IMGL::CIM16::GetPixelAddress(int x, int y) {
	return pPix + y * wd + x;
}

int IMGL::CIM16::GetHeight() { return ht; }

int IMGL::CIM16::GetWidth() { return wd; }

int IMGL::CIM16::size() { return wd * ht; }

TCHAR* IMGL::CIM16::Info() {
	static TCHAR info[384];
	MaxMin();
	swprintf(info, 384, L"Dimension: %d(wd) %d(ht)\nIntensity: %.4f(max) %.4f(min)",
		wd, ht, mx, mn);
	return info;
}

ULONG IMGL::CIM16::GetBufferSize() {
	return wd * ht;
}

void IMGL::CIM16::MaxMin() {
	if (!pPix) return;
	int sz = wd * ht;
	float* p = pPix;
	mx = *p, mn = *p;
	for (int i = 0; i < sz; i++, p++) {
		if (*p == BADDATA) continue;
		if (*p > mx) mx = *p;
		if (*p < mn) mn = *p;
	}
}

BOOL IMGL::CIM16::GetMaxMin(float& max, float& min) {
	if (!pPix) return FALSE;
	float* p = pPix;
	max = min = *p;
	for (int y = 0; y < ht; y++) {
		for (int x = 0; x < wd; x++) {
			if (*p > max) max = *p;
			if (*p > 0) { // ignore 0 value [11/3/2020 yuenl]
				if (*p < min) min = *p;
			}
			p++;
		}
	}
	return TRUE;
}

BOOL IMGL::CIM16::GetMaxMin2(float& max, float& min) {
	// Ignore 0 value [11/3/2020 yuenl]
	if (!pPix) return FALSE;
	max = min = 0;
	float* p = pPix;
	int sz = wd * ht;
	for (int i = 0; i < sz; i++, p++) {
		if (*p == BADDATA) continue;
		max = min = *p;
		break;
	}
	//if (max == 0) return FALSE;

	p = pPix;
	for (int y = 0; y < ht; y++) {
		for (int x = 0; x < wd; x++, p++) {
			if (*p == BADDATA) continue;
			if (*p > max) max = *p;
			if (*p < min) {
				if (*p) min = *p;
			}
		}
	}
	return TRUE;
}

BOOL IMGL::CIM16::Make8BPP() {
	if (!pPix) return FALSE;
	if (!Im8.IsNull()) Im8.Destroy();
	Im8.Create(wd, ht, 8); if (Im8.IsNull()) return FALSE;
	Im8.SetGrayScale();

	float max, min;
	GetMaxMin(max, min);
	float sf = 255 / (max - min);
#pragma omp parallel for
	for (int y = 0; y < ht; y++) {
		float* p16 = GetPixelAddress(0, y);
		PBYTE p8 = (PBYTE)Im8.GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++, p8++, p16++) {
			if (*p16 == BADDATA) *(p8) = 0;
			*(p8) = BYTE(sf * (*p16 - min));
		}
	}
	return TRUE;
}

BOOL IMGL::CIM16::IsSameSize(CIM& Im) {
	if (wd != Im.GetWidth()) return FALSE;
	if (ht != Im.GetWidth()) return FALSE;
	return TRUE;
}

BOOL IMGL::CIM16::ConvertPhase2Height_um(float fac_um, CIM16& Im_rad) {
	if (Im_rad.IsNull()) return FALSE;
	int wd, ht; Im_rad.GetDim(wd, ht);
	if (IsNull()) {
		Create(wd, ht); if (IsNull()) return FALSE;
	}
	if (!IsSameSize(Im_rad)) {
		Destroy(); Create(wd, ht); if (IsNull()) return FALSE;
	}
	zHtFac_um = fac_um;
#pragma omp parallel for
	for (int y = 0; y < ht; y++) {
		float* p = Im_rad.GetPixelAddress(0, y);
		float* q = GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++, p++, q++) {
			if (*p == BADDATA) { *q = BADDATA; continue; }
			*q = *p * zHtFac_um;
		}
	}
}

BOOL IMGL::CIM16::Resampling(IMGL::CIM& Img, int cx, int cy) {
	Destroy();
	if (Img.IsNull()) return FALSE;
	// To be completed [4/29/2021 FSM]
	// decide aspect ratio [4/29/2021 FSM]
	ASSERT(0);
	int ch = cx, cw = cy;
	for (int y = 0; y < ch; y++) {
		for (int x = 0; x < cw; x++) {
		}
	}
	return TRUE;
}

bool IMGL::CIM16::Unwrap(IMGL::eDIR Dir) {
	switch (Dir) {
	case IMGL::LR: return UnwrapLR(); break;
	case IMGL::RL: return UnwrapRL(); break;
	case IMGL::TB: return UnwrapTB(); break;
	case IMGL::BT: return UnwrapBT(); break;
	default: ASSERT(0); break;
	}
	return false;
}

bool CIM16::UnwrapBT() {
	if (IsNull()) { ASSERT(0); return false; }
	float lmt = PIE;
	float lmt2 = 2 * lmt, lmt3 = 3 * lmt, lmt4 = 2 * lmt2;

	// seeds [2/21/2021 FSM]
	int y = ht - 1;
	float w = GetPixel(0, y);
	for (int x = 1; x < wd; x++) {
		int c = 0;
		float u = 0, v;
		v = GetPixel(x, y);
		while ((fabs(v - w) >= lmt)) {
			c++; if (c > IRPT) break;
			if (v > w) u = -lmt2; else if (v < w) u = lmt2; else break;
			for (int i = x; i < wd; i++) {
				*(pPix + y * wd + i) += u;
			}
			v += u;
		}
		w = v;
	}

#pragma omp parallel for
	for (int x = 0; x < wd; x++) {
		int c = 0;
		float u = 0, v;
		float w = GetPixel(x, ht - 1);
		for (int y = ht - 2; y >= 0; y--) {
			v = GetPixel(x, y);
			while (float(fabs(v - w)) >= lmt) {
				c++; if (c > IRPT) break;
				if (v > w) u = -lmt2; else if (v < w) u = lmt2; else break;
				for (int i = y; i >= 0; i--) {
					*(pPix + i * wd + x) += u;
				}
				v += u;
			}
			w = v;
		}
	}
	return true;
}

bool CIM16::UnwrapLR() {
	if (IsNull()) { ASSERT(0); return false; }
	float lmt = PIE;
	float lmt2 = 2 * lmt, lmt3 = 3 * lmt, lmt4 = 2 * lmt2;

	// seeding [2/21/2021 FSM]
	float w = GetPixel(0, 0);
	for (int y = 1; y < ht; y++) {
		int c = 0;
		float u = 0, v;
		v = GetPixel(0, y);
		while (fabsf(v - w) >= lmt) {
			c++; if (c > IRPT) break;
			if (v > w) u = -lmt2; else if (v < w) u = lmt2; else break;
			for (int i = y; i < ht; i++) *(pPix + i * wd) += u;
			v += u;
		}
		w = v;
	}

#pragma omp parallel for
	for (int y = 0; y < ht; y++) {
		int c = 0;
		float u = 0, v;
		float w = GetPixel(0, y);
		for (int x = 1; x < wd; x++) {
			v = GetPixel(x, y);
			while (fabsf(v - w) >= lmt) {
				c++; if (c > IRPT) break;
				if (v > w) u = -lmt2; else if (v < w) u = lmt2; else break;
				for (int i = x; i < wd; i++) {
					*(pPix + y * wd + i) += u;
				}
				v += u;
			}
			w = v;
		}
	}
	return true;
}

BOOL IMGL::CIM16::Histo256() {
	if (IsNull()) return FALSE;
	memset(His5.His, 0, His5.nbins * sizeof(int));
	float mx, mn, sf;
	mx = mn = *GetPixelAddress(0, 0);
	for (int y = 0; y < ht; y++) {
		float* p = GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++, p++) {
			if (*p == BADDATA) continue;
			if (*p > mx) mx = *p;
			if (*p < mn) mn = *p;
		}
	}
	if (mx == mn) return FALSE;

	His5.mx = mx; His5.mn = mn;
	His5.sf = sf = His5.nbins / (mx - mn);
	for (int y = 0; y < ht; y++) {
		float* p = GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++, p++) {
			if (*p == BADDATA) continue;
			His5.His[int(sf * (*p - mn))]++;
		}
	}
	return TRUE;
}

BOOL IMGL::CIM16::Histo(int* Bin, int nBin) {
	if (IsNull()) return FALSE;
	memset(Bin, 0, nBin * sizeof(int));
	float mx, mn, sf;
	mx = mn = *GetPixelAddress(0, 0);
	for (int y = 0; y < ht; y++) {
		float* p = GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++, p++) {
			if (*p == BADDATA) continue;
			if (*p > mx) mx = *p;
			if (*p < mn) mn = *p;
		}
	}
	if (mx == mn) return FALSE;

	His5.mx = mx; His5.mn = mn;
	sf = (nBin - 1) / (mx - mn);
	for (int y = 0; y < ht; y++) {
		float* p = GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++, p++) {
			Bin[int(sf * (*p - mn))]++;
		}
	}
	return TRUE;
}

void IMGL::CIM16::Normalize() {
	if (IsNull()) return;
	// find max, min [4/9/2021 FSM]
	double max = 0, min = 0;
	max = min = *GetPixelAddress(0, 0);
	for (int y = 0; y < ht; y++) {
		float* p0 = GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++, p0++) {
			if (*p0 > max) max = *p0;
			if (*p0 < min) min = *p0;
		}
	}
	// normalize [4/9/2021 FSM]
	float dd = float(max - min);
	for (int y = 0; y < ht; y++) {
		float* p0 = GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++, p0++) {
			*p0 = float((*p0 - min) / dd);
		}
	}
}

void IMGL::CIM16::GetDim(int& w, int& h) {
	w = wd; h = ht;
}

BOOL IMGL::CIM16::Add(IMGL::CIM16* Im) {
	if (IsNull()) return FALSE;
	if (!Im) return FALSE;
#pragma omp parallel for
	for (int y = 0; y < ht; y++) {
		float* p0 = GetPixelAddress(0, y);
		float* p1 = Im->GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++, p0++, p1++) {
			*p0 += *p1;
		}
	}
	return TRUE;
}

bool IMGL::CIM16::BThres(float lvl) {
	if (IsNull()) return false;
	int wd = GetWidth(), ht = GetHeight();
	//#pragma omp parallel for
	for (int y = 0; y < ht; y++) {
		float* des = GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++) {
			*(des++) = (*des >= lvl) ? 255.f : 0;
		}
	}
	return true;
}

BOOL IMGL::CIM16::Devide(int n) {
	if (IsNull()) return FALSE;
	if (n <= 1) return TRUE;
#pragma omp parallel for
	for (int y = 0; y < ht; y++) {
		float* p0 = GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++, p0++) {
			*p0 /= float(n);
		}
	}
	return TRUE;
}

BOOL IMGL::CIM16::Mult(float fac) {
	if (IsNull()) return FALSE;
	//#pragma omp parallel for
	for (int y = 0; y < ht; y++) {
		float* p0 = GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++, p0++) {
			*p0 *= fac;
		}
	}
	return TRUE;
}

BOOL IMGL::CIM16::Make24BPPStretch(CIM& Im) {
	if (!pPix) return FALSE;
	if (!Im.IsNull()) { if (!IsSameSize(Im)) Im.Destroy(); }
	if (Im.IsNull()) Im.Create(wd, ht, 24);
	if (Im.IsNull()) return FALSE;
	float max, min;
	GetMaxMin2(max, min); // Ignore 0 value [11/3/2020 yuenl]
	float sf = 255 / (max - min);
	// #pragma omp parallel for
	for (int y = 0; y < ht; y++) {
		float* p16 = GetPixelAddress(0, y);
		PBYTE p8 = (PBYTE)Im.GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++, p16++) {
			if (*p16 != BADDATA) {
				short v = short(sf * (*p16 - min));
				if (v < 0) v = 0;
				if (v > 255) v = 255;
				*(p8++) = BYTE(v); *(p8++) = BYTE(v); *(p8++) = BYTE(v);
			}
			else {
				*(p8++) = 0; *(p8++) = 0; *(p8++) = 255;
			}
		}
	}
	return TRUE;
}

BOOL IMGL::CIM16::Make24BPP() {
	return Make24BPPStretch(Im8);
}

BOOL IMGL::CIM16::Make24H() {
	return Make24HStretch(Im8);
}

BOOL IMGL::CIM16::Make24HStretch(CIM& Im) {
	if (!pPix) return FALSE;
	if (!Im.IsNull()) { if (!IsSameSize(Im)) Im.Destroy(); }
	if (Im.IsNull()) Im.Create(wd, ht, 24);
	if (Im.IsNull()) return FALSE;

	Level();
	if (!Histo256()) return FALSE;

	int lim = int(0.0000f * wd * ht);
	int sum = 0, imx = 0, imn = 0;
	for (int i = 0; i < 255; i++) {
		sum += His5.His[i];
		if (sum < lim) His5.His[i] = 0;
		else { imn = i; break; }
	}
	sum = 0;
	for (int i = 255; i >= 0; i--) {
		sum += His5.His[i];
		if (sum < lim) His5.His[i] = 0;
		else { imx = i; break; }
	}
	float sf = His5.sf, min = His5.mn;
	float sf2 = 255.f / float(imx - imn);
	//#pragma omp parallel for

	for (int y = 0; y < ht; y++) {
		float* p16 = GetPixelAddress(0, y);
		PBYTE p8 = (PBYTE)Im.GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++, p16++) {
			if (*p16 != BADDATA) {
				short v = int(sf2 * int(sf * (*p16 - min)));
				*(p8++) = BYTE(v); *(p8++) = BYTE(v); *(p8++) = BYTE(v);
			}
			else {
				*(p8++) = 0; *(p8++) = 0; *(p8++) = 255;
			}
		}
	}
	return TRUE;
}

BOOL IMGL::CIM16::SaveBMP(std::wstring& PathName, float Percent) {
	CIM16 IM;
	if (!ResizeIM(IM, Percent)) return FALSE;
	if (IM.Make24BPP()) {
		IM.Im8.Save(PathName.c_str()); return TRUE;
	}
	return FALSE;
}

BOOL IMGL::CIM16::SaveCSV(std::wstring& PathName, float Percent) {
	CIM16 IM;
	if (!ResizeIM(IM, Percent)) return FALSE;
	IM.Normalize();
	IM.Save_CSV(PathName); return TRUE;
	return FALSE;
}

BOOL IMGL::CIM16::Save_CSV(std::wstring& PathName) {
	FILE* fp = _wfopen(PathName.c_str(), _T("wb"));
	if (!fp) return FALSE;
	fprintf(fp, "Width,%d\nHeight,%d\n", wd, ht);
	for (int y = 0; y < ht; y++) {
		float* p = GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++, p++) {
			fprintf(fp, "%.4f,", *p);
		}
		fprintf(fp, "\n");
	}
	fclose(fp);
	return TRUE;
}

BOOL IMGL::CIM16::SaveIM16(std::wstring& PathName, float Percent) {
	CIM16 IM;
	if (!ResizeIM(IM, Percent)) return FALSE;
	return IM.Save(PathName);
}

BOOL IMGL::CIM16::ResizeIM(IMGL::CIM16& newIM, float Percent) {
	if (IsNull()) return FALSE;
	// allocation [4/9/2021 FSM]
	if (!newIM.IsNull()) {
		if (!IsSameSize(newIM)) newIM.Destroy();
	}
	int w = int(GetWidth() * Percent / 100.f);
	int h = int(GetHeight() * Percent / 100.f);
	if (newIM.IsNull()) {
		if (!newIM.Create(w, h)) return FALSE;
	}
	// calculate scaling factors  [4/9/2021 FSM]
	float dw = wd / float(w);
	float dh = ht / float(h);
	//#pragma omp parallel for
	for (int y = 0; y < h; y++) {
		float* p = GetPixelAddress(0, int(dh * y));
		float* q = newIM.GetPixelAddress(0, y);
		for (int x = 0; x < w; x++, q++) {
			*q = *(p + int(dw * x));
		}
	}
	return TRUE;
}

bool IMGL::CIM16::Save(const std::wstring& PathName) {
	CFile theFile;
	if (theFile.Open(PathName.c_str(), CFile::modeCreate | CFile::modeWrite)) {
		CArchive archive(&theFile, CArchive::store);
		Serialize(archive);
		archive.Close(); theFile.Close();
		return true;
	}
	return false;
}

bool IMGL::CIM16::Load(const std::wstring& PathName) {
	CFile theFile;
	if (theFile.Open(PathName.c_str(), CFile::modeRead)) {
		CArchive archive(&theFile, CArchive::load);
		Serialize(archive);
		archive.Close(); theFile.Close();
		return true;
	}
	return false;
}

void IMGL::CIM16::Substract(CIM16& Lhs, CIM16& Rhs) {
	if (Lhs.IsNull() || Rhs.IsNull()) return;
	if (!Lhs.IsSameSize(Rhs)) return;
	wd = Lhs.GetWidth(); ht = Lhs.GetHeight();
	if (!Create(wd, ht)) return;
	for (int y = 0; y < ht; y++) {
		float* pLHS = (float*)Lhs.GetPixelAddress(0, y);
		float* pRHS = (float*)Rhs.GetPixelAddress(0, y);
		float* pDes = (float*)GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++, pLHS++, pRHS++, pDes++) {
			*pDes = *pLHS - *pRHS;
		}
	}
}

IMGL::CIM16::CIM16() {}

IMGL::CIM16::CIM16(int w, int h) {
	Create(w, h);
}

IMGL::CIM16::CIM16(CIM16& Image) {
	if (Image.IsNull()) return;
	if (!Create(Image.GetWidth(), Image.GetHeight())) { ASSERT(0);  return; }
	memcpy(GetBufferStart(), Image.GetBufferStart(), Image.GetBufferSize() * sizeof(float));

	wd = Image.GetWidth();
	ht = Image.GetHeight();
}

IMGL::CIM16::~CIM16() {
	if (pPix) delete[]pPix; pPix = nullptr;
}

void IMGL::CIM16::Serialize(CArchive& ar) {
	short magic = 1;
	if (ar.IsStoring()) {
		ar << magic;
		ar << wd;
		ar << ht;
		int sz = wd * ht;
		if (sz > 0) {
			ar.Write(pPix, sz * sizeof(float));
		}
	}
	else {
		if (pPix) { wd = ht = 0; delete[] pPix; pPix = NULL; }
		ar >> magic;
		ar >> wd;
		ar >> ht;
		int sz = wd * ht;
		if (sz > 0) {
			pPix = new float[sz];
			if (pPix) {
				ar.Read(pPix, sz * sizeof(float));
			}
			else { wd = 0; ht = 0; }
		}
	}
}

IMGL::CIM16 IMGL::CIM16::operator-(IMGL::CIM16& Rhs) {// assume image is monochrome [1/7/2020 yuenl]
	if (IsNull() || Rhs.IsNull()) return Rhs;
	if (!IsSameSize(Rhs)) return Rhs;

	//#pragma omp parallel for
	for (int y = 0; y < ht; y++) {
		//#pragma omp simd
		float* pRHS = (float*)Rhs.GetPixelAddress(0, y);
		float* pDes = (float*)GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++, pRHS++, pDes++) {
			//! Note: could contain -ve value
			*pDes -= *pRHS;
		}
	}
	return *this;
}

IMGL::CIM16 IMGL::CIM16::operator+(IMGL::CIM16& Rhs) {// assume image is monochrome [1/7/2020 yuenl]
	if (IsNull() || Rhs.IsNull()) return Rhs;
	if (!IsSameSize(Rhs)) return Rhs;

	//#pragma omp parallel for
	for (int y = 0; y < ht; y++) {
		// #pragma omp simd
		float* pRHS = (float*)Rhs.GetPixelAddress(0, y);
		float* pDes = (float*)GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++, pRHS++, pDes++) {
			*pDes += *pRHS;
		}
	}
	return *this;
}

BOOL CIM16::Convolve(SKernal& K, int y, float* buf, int wd) {
	short wdw = short(K.Ker.size()); if (wdw < 3) return FALSE;
	int tlen = wd + wdw - 1;
	// create working buffer with padding [4/11/2021 FSM]
	float* pdat = pPix + y * wd;
	// end pad data [4/11/2021 FSM]
	float a = *pdat;
	float b = *(pdat + wd - 1);
	float* p0 = buf;
	float* p1 = buf + tlen - 1;
	for (int i = 0; i < wdw / 2; i++, p0++, p1--) {
		*p0 = a; *p1 = b;
	}
	// copy the rest of data [4/13/2021 FSM]
	float denom = K.denom();
	float* pF = pdat;
	float* p = buf + wdw / 2;
	memcpy(p, pF, wd * sizeof(float));
	pF = pdat; // destination [4/11/2021 FSM]
	p = buf; // source [4/11/2021 FSM]
	for (int i = 0; i < wd; i++, pF++, p++) {
		float sum = 0;
		float* pp = p;
		float* kk = &K.Ker[0];
		for (int j = 0; j < wdw; j++, pp++, kk++) {
			sum += (*pp) * (*kk);
		}
		*pF = sum / denom;
	}
	return TRUE;
}

CIM16& CIM16::operator/(CIM16& Rhs) {// assume image is monochrome [1/7/2020 yuenl]
	if (Rhs.IsNull()) return Rhs;
	if (IsNull()) return Rhs;
	if (this == &Rhs) return Rhs;
	if (!IsSameSize(Rhs)) return Rhs;

	int wd = GetWidth(), ht = GetHeight();
	float* pMax = new float[ht];
	float* pMin = new float[ht];
	float maxV = -(FLT_MAX - 1), minV = FLT_MAX; // extend to RGB processing [10/6/2020 FSM]
#pragma omp parallel for
	for (int y = 0; y < ht; y++) {
		float* pSrc = (float*)Rhs.GetPixelAddress(0, y);
		float* pDes = (float*)GetPixelAddress(0, y);
		float* pMx = &pMax[y]; float* pMn = &pMin[y];
		*pMx = -(FLT_MAX - 1), * pMn = FLT_MAX;
		for (int x = 0; x < wd; x++) {
			for (int i = 0; i < 3; i++) {
				float v;
				if (*pSrc) v = *pDes / float(*pSrc); else v = 0;
				if (v > *pMx) *pMx = v; if (v < *pMn) *pMn = v;
				*(pDes++) = v; pSrc++;
			}
		}
	}
	// perform reduction here [10/12/2020 FSM]
	for (int y = 0; y < ht; y++) {
		if (pMax[y] > maxV) maxV = pMax[y];
		if (pMin[y] < minV) minV = pMin[y];
	}
	if (pMax) delete[]pMax; if (pMin) delete[]pMin;

	// normalize pfImg [1/7/2020 yuenl]
	float sf = 255 / (maxV - minV);
#pragma omp parallel for
	for (int y = 0; y < ht; y++) {
		PBYTE pDes = (PBYTE)GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++) {
			*(pDes++) = BYTE((*(pDes)-minV) * sf);
			*(pDes++) = BYTE((*(pDes)-minV) * sf);
			*(pDes++) = BYTE((*(pDes)-minV) * sf);
		}
	}
	return *this;
}

CIM16& CIM16::operator=(CIM16& Im) {
	if (Im.IsNull()) {
		if (!IsNull()) Destroy(); return *this;
	}
	if (this == &Im) return *this;
	if (IsNull()) {
		if (!Create(Im.GetWidth(), Im.GetHeight())) { ASSERT(0);  return *this; }
	}
	else {
		if (!IsSameSize(Im)) {
			Destroy();
			if (!Create(Im.GetWidth(), Im.GetHeight())) { ASSERT(0); return *this; }
		}
	}
	if (IsNull()) return *this;
	memcpy(GetBufferStart(), Im.GetBufferStart(), Im.GetBufferSize() * sizeof(float));

	wd = Im.wd; ht = Im.ht;
	return *this;
}

BOOL IMGL::CIM16x3::IsNull() {
	if (pPixels) return FALSE;
	return TRUE;
}

BOOL IMGL::CIM16x3::Create(int w, int h) {
	if (pPixels) {
		if ((w == wd) && (h == ht)) return TRUE;
		delete[]pPixels; pPixels = NULL;
	}
	if (!pPixels) {
		pPixels = new float[3 * w * h];
		if (pPixels) { wd = w; ht = h; }
	}
	if (!pPixels) return FALSE;
	return TRUE;
}

float* IMGL::CIM16x3::GetPixelAddress(int x, int y) {
	if (!pPixels) return NULL;
	return pPixels + 3 * (y * wd + x);
}

int IMGL::CIM16x3::size() { return 3 * wd * ht; }

void IMGL::CIM16x3::Destroy() {
	if (pPixels) {
		delete[]pPixels; pPixels = NULL;
	}
	wd = 0; ht = 0;
}

BOOL IMGL::CIM16x3::IsSameSize(CIM& DiB) {
	if (IsNull() || DiB.IsNull()) return FALSE;
	if ((DiB.GetWidth() != wd) || (DiB.GetHeight() != wd)) {
		return FALSE;
	}
	return TRUE;
}

BOOL IMGL::CIM16x3::GetMaxMin(float& max, float& min) {
	// Ignore -1 value [11/3/2020 yuenl]
	if (!pPixels) return FALSE;
	max = min = 0;
	max = min = *pPixels;
	float* p = pPixels;
	for (int y = 0; y < ht; y++) {
		for (int x = 0; x < wd; x++) {
			for (int i = 0; i < 3; i++, p++) {
				if (*p > max)
					max = *p;
				if (*p < min)
					min = *p;
			}
		}
	}
	return TRUE;
}

BOOL IMGL::CIM16x3::Make24BPPStretch(CIM& Im) {
	if (!pPixels) return FALSE;
	if (!Im.IsNull()) { if (!IsSameSize(Im)) Im.Destroy(); }
	if (Im.IsNull()) Im.Create(wd, ht, 24);
	if (Im.IsNull()) return FALSE;
	float max, min;
	GetMaxMin(max, min); // Ignore 0 value [11/3/2020 yuenl]
	float sf = 255 / (max - min);
	// #pragma omp parallel for
	for (int y = 0; y < ht; y++) {
		float* p16 = GetPixelAddress(0, y);
		PBYTE p8 = (PBYTE)Im.GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++) {
			for (int i = 0; i < 3; i++, p16++, p8++) {
				short v = short(sf * (*p16 - min));
				if (v > 255) v = 255;
				*p8 = BYTE(v);
			}
		}
	}
	return TRUE;
}

IMGL::CIM16x3::CIM16x3() {}

IMGL::CIM16x3::~CIM16x3() {
	if (pPixels) delete[] pPixels;
}

float IMGL::SKernal::denom() {
	float sum = 0;
	int sz = int(Ker.size());
	for (int i = 0; i < sz; i++) {
		sum += Ker[i];
	}
	return sum;
}

void IMGL::SKernal::Hat(short N) {
	N = 2 * (N / 2) + 1;
	Ker.clear();
	int n = int(0.20f * N / 2);
	for (int i = 0; i < n; i++) {
		Ker.push_back(0);
	}
	for (int i = 0; i < N; i++) {
		Ker.push_back(1);
	}
	for (int i = 0; i < n; i++) {
		Ker.push_back(0);
	}
}

void IMGL::SKernal::Sine(short N, short pole) {
	N = 2 * (N / 2) + 1;
	Ker.clear();
	int n = int(0.20f * N / 2);
	for (int i = 0; i < n; i++) {
		Ker.push_back(0);
	}
	float sf = PIE / float(N - 1);
	for (int i = 0; i < N; i++) {
		Ker.push_back(FLOAT(sin(i * sf * pole)));
	}
	n = int(0.33f * N / 2);
	for (int i = 0; i < n; i++) {
		Ker.push_back(0);
	}
}

void IMGL::SKernal::Wedge(short N, short inc) {
	N = 2 * (N / 2) + 1;
	Ker.clear();
	int n = int(0.20f * N / 2);
	for (int i = 0; i < n; i++) {
		Ker.push_back(0);
	}
	float num = 0;
	for (int i = 0; i < N; i++) {
		num += inc; Ker.push_back(num);
	}
	n = int(0.33f * N / 2);
	for (int i = 0; i < n; i++) {
		Ker.push_back(0);
	}
}

IMGL::SKernal::SKernal() {}

IMGL::SKernal::~SKernal() {}

USHORT IMGL::SMat::Det() {
	USHORT v = 0;
	if (K) {
		PBYTE p = K;
		for (int i = 0; i < D; i++, p++) {
			v += (*p > 0);
		}
	}
	return v;
}

IMGL::SMat::SMat() {}

IMGL::SMat::SMat(USHORT n) {
	if (n > 0) {
		K = new BYTE[n * n];
		if (K) {
			W = n; D = W * W;
		}
	}
}

IMGL::SMat::~SMat() { if (K) delete[]K; }

void IMGL::Sln::Clear() { Pts.clear(); }

std::vector<SPtS>& IMGL::Sln::Get() { return Pts; }

SPtS* IMGL::Sln::GetPts(int n) {
	if (n < Pts.size()) return &Pts[n];
	return nullptr;
}

float IMGL::Sln::Aver() {
	int sz = int(Pts.size()); if (sz < 1) return 0;
	double sum = 0;
	for (int i = 0; i < sz; i++) {
		sum += Pts[i].y;
	}
	return float(sum / float(sz));
}

IMGL::Sln::Sln() {}

IMGL::Sln::Sln(int sz) { Pts.resize(sz); }

IMGL::Sln::~Sln() {}

SPtS* IMGL::Slin::getPeak(int i) { return &ln.Pts[i]; }

bool IMGL::Slin::dump(char* fname) {
	FILE* fp = fopen(fname, "wb");
	if (!fp) return false;
	fprintf(fp, "legend,%s\n", legnd.c_str());
	fprintf(fp, "Xmin,%.4f,Xmax,%.4f,Ymin,%.4f,Ymax,%4f\n", xmin, xmax, ymin, ymax);
	fprintf(fp, "X,Y\n");
	int sz = int(ln.Pts.size());
	IMGL::SPtS* p = &ln.Pts[0];
	for (int i = 0; i < sz; i++, p++) {
		fprintf(fp, "%.4f,%.4f\n", p->x, p->y);
	}
	fclose(fp);
	return true;
}

int IMGL::Slin::size() { return int(ln.Pts.size()); }

float IMGL::Slin::aver() {
	return ln.Aver();
}

void IMGL::Slin::add(IMGL::SPtS& p) { ln.Pts.push_back(p); }

void IMGL::Slin::clear() { ln.Pts.clear(); }

void IMGL::Slin::legend(std::string& legd) { legnd = legd; }

void IMGL::Slin::maxmin() {
	int sz = int(ln.Pts.size());
	IMGL::SPtS* p = &ln.Pts[0];
	xmax = xmin = p->x;
	ymax = ymin = p->y;
	float v;
	for (int i = 0; i < sz; i++, p++) {
		v = p->x;
		if (v < xmin) xmin = v;
		if (v > xmax) xmax = v;
		v = p->y;
		if (v < ymin) ymin = v;
		if (v > ymax) ymax = v;
	}
}

void IMGL::Slin::resize(int sz) { ln.Pts.resize(sz); }

void IMGL::Slin::setprop(COLORREF color, EDType ty, short marker) {
	cr = color; type = ty; nMkr = marker;
}

void IMGL::Slin::smooth(int n) {
	int sz = int(ln.Pts.size());
	int sz2 = sz - 2;
	for (int i = 0; i < n; i++) {
		float sum;
		IMGL::SPtS* p, * p0;
		for (int j = 2; j <= sz2; j++) {
			sum = 0;
			p = &ln.Pts[j - 2];
			for (int k = 0; k < 5; k++, p++) sum += p->y;
			ln.Pts[j].t = sum / 5.f;
		}
		sum = 0; p0 = &ln.Pts[0];
		for (int k = 0; k < 3; k++, p0++) sum += p0->y;
		ln.Pts[0].t = sum / 3.f;

		sum = 0; p0 = &ln.Pts[0];
		for (int k = 0; k < 4; k++, p0++) sum += p0->y;
		ln.Pts[1].t = sum / 4.f;

		sum = 0; p0 = &ln.Pts[sz2 - 1];
		for (int k = 0; k < 4; k++, p0++) sum += p0->y;
		ln.Pts[sz2 + 1].t = sum / 4.f;

		sum = 0; p0 = &ln.Pts[sz2];
		for (int k = 0; k < 3; k++, p0++) sum += p0->y;
		ln.Pts[sz2 + 2].t = sum / 3.f;

		p = &ln.Pts[0];
		for (int j = 0; j < sz; j++, p++) {
			p->y = p->t;
		}
	}
}

IMGL::SLines::SLines() {}

IMGL::SLines::~SLines() {}

int IMGL::SLines::size() { return int(lins.size()); }

void IMGL::SLines::clear() {
	int sz = int(lins.size());
	for (int i = 0; i < sz; i++) {
		lins[i].clear();
	}
	lins.clear();
}

void IMGL::SLines::maxmin() {
	int siz = int(lins.size()); if (siz < 1) return;
	Slin* pl = &lins[0];
	sz = pl->size();
	xmin = pl->xmin; xmax = pl->xmax; ymin = pl->ymin; ymax = pl->ymax;
	for (int i = 1; i < siz; i++) {
		Slin* pl = &lins[i];
		if (pl->size() > sz) sz = pl->size();
		if (pl->xmin < xmin) xmin = pl->xmin; if (pl->xmax > xmax) xmax = pl->xmax;
		if (pl->ymin < ymin) ymin = pl->ymin; if (pl->ymax > ymax) ymax = pl->ymax;
	}
}

void IMGL::SLines::dump(FILE* fp) {
	fprintf(fp, "\n\n");
	fprintf(fp, "Name,%s\n", name.c_str());
	int sz = int(lins.size());
	int mx = -1;
	for (int i = 0; i < sz; i++) {
		int v = int(lins[i].ln.Pts.size());
		if (v > mx) mx = v;
	}
	for (int i = 0; i < sz; i++) {
		if (i > 0) fprintf(fp, ",");
		fprintf(fp, "ZPos,Val");
	}
	fprintf(fp, "\n");

	for (int j = 0; j < mx; j++) {
		for (int i = 0; i < sz; i++) {
			SPtS* p = lins[i].ln.GetPts(j);
			if (i > 0) fprintf(fp, ",");
			if (p) fprintf(fp, "%.4f,%.1f", p->x, p->y);
			else fprintf(fp, ",");
		}
		fprintf(fp, "\n");
	}
}

void IMGL::SLines::dump1(FILE* fp) {
	int nln = int(lins.size()); if (nln < 1) return;

	fprintf(fp, "Z");
	for (int i = 0; i < nln; i++) { fprintf(fp, ",V%d", i + 1); }
	fprintf(fp, "\n");

	int npt = lins[0].size(); // assume all lines contains same number of points
	for (int j = 0; j < npt; j++) {
		for (int i = 0; i < nln; i++) {
			SPtS* p = lins[i].ln.GetPts(j);
			if (i > 0) {
				if (p) fprintf(fp, ",%.1f", p->y);
				else fprintf(fp, ",");
			}
			else {
				if (p) fprintf(fp, "%.4f,%.1f", p->x, p->y);
				else fprintf(fp, ",");
			}
		}
		fprintf(fp, "\n");
	}
}

void IMGL::SLines::label(const std::string& nam) { name = nam; }

//bool IMGL::CIM2::BThres(short lvl) { return true; }

int IMGL::SNxt::Gets() {
	for (int i = 1; i < nNext; i++) {
		if (!nxt[i].nxt) {
			nxt[i].nxt = true; return i;
		}
	}
	if (nNext < nBin) {
		int i = nNext++;
		nxt[i].nxt = true; return i;
	}
	//ASSERT(0);
	return 0;
}

int IMGL::SNxt::Count() {
	int cnt = 0;
	for (int i = 1; i < nNext; i++) {
		if (nxt[i].nxt) cnt++;
	}
	return cnt;
}

int IMGL::SNxt::Max() {
	if (nNext < nBin) return nNext;
	return -1;
}

void IMGL::SNxt::Reset() {
	for (int i = 1; i < nNext; i++) {
		nxt[i].nxt = 0;
	}
	nNext = 1;
}

bool IMGL::SNxt::Clear(int n) {
	if ((n < 1) || (n >= nBin)) {
		return false;
	}
	nxt[n].nxt = false;
	return true;
}

void SNxt::Identify(IMGL::CIM& Des, IMGL::CIM2& Ref, int lmt, int clmt, std::vector<IMGL::SCgI>& CG, ETYPE nType) {
	int mx = Max(); if (mx < 2) return;
	IMGL::SBinU Bin(mx + 1);
	Ref.Histo(Bin);
	COLORREF Color[12] = {
		BGR(255,0,0),   BGR(0,255,0),   BGR(0,0,255),
		BGR(255,255,0), BGR(0,255,255), BGR(255,0,255),
		BGR(128,0,0),   BGR(0,128,0),   BGR(0,0,128),
		BGR(255,128,0), BGR(0,255,128), BGR(128,0,255)
	};
	for (int i = 0; i < Bin.nBin; i++) {
		USHORT from = Bin.GetMax(1, mx, true);
		if (from > -1) {
			IMGL::SCgI Cg;
			if (!Ref.GetStripsStats(Cg, from)) continue;
			if (Cg.Count < lmt) continue;
			CG.push_back(Cg);
		}
	}
	int sz = int(CG.size());
	if (nType == SNxt::BLOB) {
		std::sort(CG.begin(), CG.end(),
			[](IMGL::SCgI& lhs, IMGL::SCgI& rhs) {
				return lhs.Dst < rhs.Dst;
			}
		);
	}
	else {
		std::sort(CG.begin(), CG.end(),
			[](IMGL::SCgI& lhs, IMGL::SCgI& rhs) {
				return lhs.Count > rhs.Count;
			}
		);
	}
	int cnt = 0;
	for (int i = 0; i < sz; i++) {
		IMGL::SCgI& Cg = CG[i];
		if (Replace(Cg.from, Des, Ref, Cg, Color[cnt], nType)) {
			cnt++; if (cnt > clmt) break;
		}
	}
}

bool SNxt::Replace(USHORT from, IMGL::CIM& Des, IMGL::CIM2& Ref, IMGL::SCgI& Cg, COLORREF cr, ETYPE nType) {
	int wd, ht, bpp; Des.GetDim(wd, ht, bpp); if (bpp != 24) return false;

	IMGL::CRGB C(cr);
	Des.ReplacePixel(Ref, from, cr);
	Cg.cr = cr;
	if (nType) {
		IMGL::SLoc stL, edL;
		if (fabsf(Cg.M) > 1) {
			stL.y = Cg.cy - ht / 4; if (stL.y < 0) stL.y = 0;
			stL.x = int((stL.y - Cg.C) / Cg.M);
			edL.y = Cg.cy + ht / 4; if (stL.y >= ht) stL.y = ht - 1;
			edL.x = int((edL.y - Cg.C) / Cg.M);
		}
		else {
			stL.x = Cg.cx - wd / 4; if (stL.x < 0) stL.x = 0;
			stL.y = int(Cg.M * stL.x + Cg.C);
			edL.x = Cg.cx + wd / 4;  if (stL.x >= wd) stL.x = wd - 1;
			edL.y = int(Cg.M * edL.x + Cg.C);
		}
		Des.DrawLine(stL.x, stL.y, edL.x, edL.y, C.cR, C.cG, C.cB);
	}
	Des.DrawCross(Cg.cx, Cg.cy, int(0.05f * wd), C.cR, C.cG, C.cB);
	return true;
}

bool IMGL::SNxt::Segment(IMGL::CIM& Src, IMGL::CIM2& Des, BYTE Obj) {
	// assume *this is properly thresholded source image
	// background = 0, object >= 250
	if (Src.IsNull()) return false; // can't continue
	int wd, ht, bpp; Src.GetDim(wd, ht, bpp);
	int bypp = bpp / 8;
	if (Des.IsNull()) { if (!Des.Create(wd, ht)) return false; }
	else {
		if ((wd != Des.wd) || (ht != Des.ht)) {
			if (!Des.Create(wd, ht)) return false;
		}
	}
	Des.Clear(); Reset();
	BYTE Bg = 255 - Obj;
	BYTE* q0;
	USHORT* pv, * p0, /** p1,*/* p2/*, * p3*/;
	//NOTE: Cannot be parallelized
	switch (bpp) {
	case 8:
		for (int y = 1; y < ht - 1; y++) {
			q0 = (BYTE*)Src.GetPixelAddress(1, y);
			p0 = (USHORT*)Des.GetPixelAddress(1, y);
			p2 = (USHORT*)Des.GetPixelAddress(1, y - 1);
			for (int x = 1; x < wd - 1; x++, p0++, p2++, q0++) {
				if (*q0 != Obj) continue;
				pv = p0 - 1;
				if (*pv) {
					*p0 = *pv;
					if ((*p2 != *p0) && *p2) {
						Clear(*p2);
						Des.ReplacePixelA(*p2, *p0);
					}
				}
				//USHORT *p1 = p2 - 1; USHORT *p3 = p2 + 1;
				//if (*p2) {
				//	*p0 = *p2; continue;
				//}
				//else if (*p3) {
				//	*p0 = *p3;
				//	if (*p3 && (*p3 != *p0)) {
				//		nxt.Clear(*p3);
				//		Im2.ReplacePixel(*p3, *p0, wd, ht);
				//	}
				//	continue;
				//}
				else {
					*p0 = Gets();
					if (*p0 == 0) return true;
				}
			}
		}
		break;
	case 24:
	case 32:
		for (int y = 1; y < ht - 1; y++) {
			q0 = (BYTE*)Src.GetPixelAddress(1, y);
			p0 = (USHORT*)Des.GetPixelAddress(1, y);
			p2 = (USHORT*)Des.GetPixelAddress(1, y - 1);
			for (int x = 1; x < wd - 1; x++, p0++, p2++, q0 += bypp) {
				if (*q0 != Obj) continue;
				pv = p0 - 1;
				if (*pv) {
					*p0 = *pv;
					if ((*p2 != *p0) && *p2) {
						Clear(*p2);
						Des.ReplacePixelA(*p2, *p0);
					}
					continue;
				}
				//USHORT *p1 = p2 - 1; USHORT *p3 = p2 + 1;
				//if (*p2) {
				//	*p0 = *p2; continue;
				//}
				//else if (*p3) {
				//	*p0 = *p3;
				//	if (*p3 && (*p3 != *p0)) {
				//		nxt.Clear(*p3);
				//		Im2.ReplacePixel(*p3, *p0, wd, ht);
				//	}
				//	continue;
				//}
				else {
					*p0 = Gets();
					if (*p0 == 0) return true;
				}
			}
		}
		break;
	default: ASSERT(0); return false; break;
	}
	return true;
}

// operations

bool IMGL::CIM2::Alloc(int sz) {
	if (pPix) {
		if (PixSz == sz) return true;
		Dealloc();
	}
	pPix = new USHORT[sz];
	if (!pPix) { PixSz = 0; return false; }
	PixSz = sz;
	return true;
}

void IMGL::CIM2::Dealloc() {
	if (pPix) {
		delete[]pPix; pPix = nullptr; PixSz = 0;
	}
}

bool IMGL::CIM2::GetCG(SCgI& CG, USHORT from) {
	if (IsNull()) return false;
	int w, h; GetDim(w, h);
	long cogX = 0, cogY = 0;
	for (int y = 0; y < h; y++) {
		USHORT* p = GetPixelAddress(0, y);
		for (int x = 0; x < w; x++, p++) {
			if (*p == from) {
				cogX += x; cogY += y;
			}
		}
	}
	CG.cx = cogX / (w * h); CG.cy = cogY / (w * h);
	return true;
}

bool IMGL::CIM2::GetStripsStats(SCgI& Cg, USHORT from) {
	if (IsNull()) return false;
	int w, h; GetDim(w, h);
	int xmn = wd + 16, xmx = -1, ymn = h + 16, ymx = -1;

	bool bRev = false;
	for (int y = 0; y < h; y++) {
		USHORT* p = GetPixelAddress(0, y);
		for (int x = 0; x < w; x++, p++) {
			if (*p == from) {
				if (x < xmn) xmn = x; if (x > xmx) xmx = x;
				if (y < ymn) ymn = y; if (y > ymx) ymx = y;
			}
		}
	}
	if ((ymx - ymn) > (xmx - xmn)) bRev = true;
	Cg.xmx = xmx; Cg.xmn = xmn; Cg.ymx = ymx; Cg.ymn = ymn; Cg.bRev = bRev;

	CLSFit Fit; // not valid for vertical line
	int cnt = 0;
	long cogX = 0, cogY = 0;
	for (int y = 0; y < h; y++) {
		USHORT* p = GetPixelAddress(0, y);
		for (int x = 0; x < w; x++, p++) {
			if (*p == from) {
				if (bRev) Fit.Add(float(y), float(x));
				else Fit.Add(float(x), float(y));
				cogX += x; cogY += y; cnt++;
			}
		}
	}
	Cg.Count = cnt;
	if (cnt) {
		Cg.cx = cogX / cnt; Cg.cy = cogY / cnt;
		Fit.Calc();
		if (bRev) {
			Fit.M = 1 / Fit.M;
			Fit.C = Cg.cy - Fit.M * Cg.cx;
		}
		Cg.from = from;
		Cg.M = float(Fit.M); Cg.C = float(Fit.C); Cg.A = atan(Cg.M);
		return true;
	}
	return false;
}

void IMGL::CIM2::ReplacePixelA(USHORT from, USHORT to/*, int wd, int ht*/) {
	for (int y = 1; y < ht - 1; y++) {
		USHORT* p = GetPixelAddress(1, y);
		for (int x = 1; x < wd - 1; x++, p++) {
			if (*p == from) *p = to;
		}
	}
}

bool IMGL::CIM2::Create(int w, int h) {
	if (Alloc(w * h)) {
		wd = w; ht = h;
		return true;
	}
	return false;
}

bool IMGL::CIM2::IsNull() const {
	if (!pPix) return true;
	return false;
}

void IMGL::CIM2::GetDim(int& w, int& h) const {
	w = wd; h = ht;
}

int IMGL::CIM2::BufferSize() {
	return PixSz * sizeof(USHORT);
}

void IMGL::CIM2::Clear() {
	if (pPix) memset(pPix, 0, BufferSize());
}

USHORT* IMGL::CIM2::GetPixelAddress(int x, int y) const {
	if (!pPix) return nullptr;
	return pPix + y * wd + x;
}

int IMGL::CIM2::ReplacePixel(USHORT from, USHORT to) {
	if (IsNull()) return -1;
	int cnt = 0;
	for (int y = 0; y < ht; y++) {
		USHORT* p = GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++, p++) {
			if (*p == from) {
				*p = to; cnt++;
			}
		}
	}
	return cnt;
}

bool IMGL::CIM2::MaxMinAve(USHORT& Mn, USHORT& Mx, int& ave) const {
	if (IsNull()) return false;
	Mn = 65000, Mx = 1, ave = 0;
	for (int y = 0; y < ht; y++) {
		USHORT* p = GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++, p++) {
			//if (*p == 0) continue;
			ave += *p;
			if (*p > Mx) Mx = *p;
			if (*p < Mn) Mn = *p;
		}
	}
	ave /= (wd * ht);
	return true;
}

bool IMGL::CIM2::Histo(SBinU& Bin) {
	//if (!MaxMinAve()) return false;
	//if (!Bin.Alloc(Mx + 1)) return false;
	for (int y = 0; y < ht; y++) {
		USHORT* p = GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++, p++) {
			if (*p < Bin.nBin) Bin.pU[*p]++;
			//else ASSERT(0);
		}
	}
	return true;
}

IMGL::CIM2::CIM2(int w, int h) {
	Create(w, h);
}

IMGL::CIM2::CIM2(CIM2& Image) {
	*this = Image;
}

IMGL::CIM2::~CIM2() { Dealloc(); }

void IMGL::CIM2::Serialize(CArchive& ar) {
	short magic = 1;
	if (ar.IsStoring()) {
		ar << magic;
		ar << wd;
		ar << ht;
		int sz = wd * ht;
		if (sz > 0) {
			ar.Write(pPix, sz * sizeof(USHORT));
		}
	}
	else {
		if (pPix) { wd = ht = 0; delete[] pPix; pPix = NULL; }
		ar >> magic;
		ar >> wd;
		ar >> ht;
		int sz = wd * ht;
		if (sz > 0) {
			pPix = new USHORT[sz];
			if (pPix) {
				ar.Read(pPix, sz * sizeof(USHORT));
			}
			else { wd = 0; ht = 0; }
		}
	}
}

float IMGL::SCgI::Ang() {
	float an = A;
	if (an < 0) an += 3.14159265f;
	return an * 180.f / 3.14159265f;
}

float IMGL::SCgI::Distance(SCgI& co) {
	dx = float(cx - co.cx);
	dy = float(cy - co.cy);
	Dst = sqrtf(dx * dx + dy * dy);
	return Dst;
}

CString IMGL::SCgI::Report(int nType) {
	CString str;
	if (nType) {
		str.Format(L"CG(%03d, %03d), N=% 5d, M=%.3f, C= %.2f, A= %.2f " \
			"from= %d RGB(%d,%d,%d) Rev= %d dX(%d,%d,%d) dY(%d,%d,%d)",
			cx, cy, Count, M, C, A * 180.f / 3.14159265f,
			from, (cr >> 16) & 0xFF, (G >> 8) & 0xFF, B & 0xFF,
			bRev, xmn, xmx, xmx - xmn, ymn, ymx, ymx - ymn);
	}
	else {
		str.Format(L"CG(%d, %d), N= %d", cx, cy, Count);
	}
	return str;
}

void IMGL::SCgI::Clear() {
	Count = R = G = B = cx = cy = 0;
	Dst = dx = dy = A = M = C = 0;
}

void IMGL::SCgI::SetColor(COLORREF cr) {
	R = (cr >> 16); G = (cr >> 8) && 0xFF; B = cr && 0xFF;
}

IMGL::SBin::SBin() {
	Alloc(256);
}

IMGL::SBin::SBin(int sz) {
	Alloc(sz);
}

IMGL::SBin::~SBin() {
	Dealloc();
}

bool IMGL::SBin::Alloc(int n) {
	if (psRed) {
		if (n == nBin) return true;
		Dealloc();
	}
	psRed = new USHORT[n]; psGrn = new USHORT[n]; psBlu = new USHORT[n];
	if (!psRed || !psGrn || !psBlu) {
		Dealloc(); return false;
	}
	nBin = n;
	return true;
}

void IMGL::SBin::Dealloc() {
	if (psRed) delete[]psRed; psRed = nullptr;
	if (psGrn) delete[]psGrn; psGrn = nullptr;
	if (psBlu) delete[]psBlu; psBlu = nullptr;
}

void IMGL::SBin::Clear() {
	if (psRed) memset(psRed, 0, nBin * sizeof(USHORT));
	if (psGrn) memset(psGrn, 0, nBin * sizeof(USHORT));
	if (psBlu) memset(psBlu, 0, nBin * sizeof(USHORT));
}

void IMGL::SBin::Add(BYTE By, eCH Ch) {
	// no sanity check
	switch (Ch) {
	case IMGL::REDC: psRed[By]++; break;
	case IMGL::GRNC: psGrn[By]++; break;
	case IMGL::BLUC: psBlu[By]++; break;
	default: ASSERT(0); break;
	}
}

USHORT IMGL::SBin::GetMax(IMGL::eCH Ch, int st, int ed, bool bErase) {
	// no sanity check
	int mx = -1, imx = -1;
	switch (Ch) {
	case IMGL::REDC:
		for (int i = st; i <= ed; i++) {
			if (psRed[i] > mx) { mx = psRed[i]; imx = i; }
		}
		if (bErase) { psRed[imx] = 0; }
		break;
	case IMGL::GRNC:
		for (int i = st; i <= ed; i++) {
			if (psGrn[i] > mx) { mx = psGrn[i]; imx = i; }
		}
		if (bErase) { psGrn[imx] = 0; }
		break;
	case IMGL::BLUC:
		for (int i = st; i <= ed; i++) {
			if (psBlu[i] > mx) { mx = psBlu[i]; imx = i; }
		}
		if (bErase) { psBlu[imx] = 0; }
		break;
	case IMGL::WHTC:
		for (int i = st; i <= ed; i++) {
			int v = (psRed[i] + psGrn[i] + psBlu[i]) / 3;
			if (v > mx) { mx = v; imx = i; }
		}
		if (bErase) {
			psRed[imx] = 0; psGrn[imx] = 0; psBlu[imx] = 0;
		}
		break;
	default: ASSERT(0); return 0; break;
	}
	return imx;
}

IMGL::SBinU::SBinU() {
	Alloc(256);
}

IMGL::SBinU::SBinU(int sz) {
	Alloc(sz);
}

IMGL::SBinU::~SBinU() {
	Dealloc();
}

bool IMGL::SBinU::Alloc(int n) {
	if (pU) {
		if (n == nBin) return true;
		Dealloc();
	}
	pU = new USHORT[n];
	if (!pU) { Dealloc(); return false; }
	nBin = n;
	return true;
}

void IMGL::SBinU::Dealloc() {
	if (pU) delete[]pU; pU = nullptr;
}

void IMGL::SBinU::Clear() {
	if (pU) memset(pU, 0, nBin * sizeof(USHORT));
}

void IMGL::SBinU::Add(BYTE By) {
	pU[By]++;
}

USHORT IMGL::SBinU::GetMax(int st, int ed, bool bErase) {
	// no sanity check
	int mx = -1, imx = -1;
	for (int i = ed; i >= st; i--) {
		if (!pU[i]) continue;
		if (bErase) pU[i] = 0;
		return i;
	}
	return imx;
}