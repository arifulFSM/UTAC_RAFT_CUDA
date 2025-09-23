// WaferMap.cpp : implementation file
//
#include "pch.h"
#include "stdafx.h"
#include <math.h>
#include "WaferMap.h"
#include "resource.h"
/* HAQUE/ADDED/WAFER MAP
#include "e95.h"
*/
#include "WaferParam.h"
#include "Pattern.h"
#include "Coor.h"

#include "RecipeRAFT.h"
#include "MPoint.h"

#include "RAFTApp.h"
/*
#include "Data.h"
*/
#include "ChuckMask.h"

#include "..\PLT\HSL2RGB.h"	// Added by ClassView
/* HAQUE/ADDED/WAFER MAP
#include "..\413\MotorS2.h"
*/
#include "Dev.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define PIE180     0.0174532925199432957692369076849
#define RULER_CX	(16)
#define RULER_CY	(16)
#define FRAME_SIZE	(300.f)
#define IDT_POPUPMENU	(0x100)
#define sqr(x) ((x)*(x))
#define CELL_SIZE	(2)

/////////////////////////////////////////////////////////////////////////////
// CWaferMap

#define FRAME_SIZE	(300.f)

CWaferMap::CWaferMap() {
	MeSet = 0;
	m_bFitScreen = TRUE;
	m_fLog2Device = 1;
	m_nWaferSize = 300;
	bRedraw = FALSE;
	m_nImageType = 0;

	dmMode = dmENGINNER;

	m_bMask = TRUE;

	bDrawBackground = TRUE;
	bDrawPoint = TRUE;	// Draw measurement point on the wafer [6/25/2010 Yuen]
	bDrawPatt = FALSE;   // Draw a grid pattern on the wafer [6/25/2010 Yuen]
	bDrawText = TRUE;
	bDrawValues = TRUE;
	bLandscape = TRUE;
	bShowStat = TRUE;
	bSiteView = TRUE;	// True if displaying result and False if just want to display measurement point [6/25/2010 Yuen]

	pRcp = NULL;
	pParent = NULL;

	CHslRgb H;
	for (int i = 0; i < FILL_LEVEL; i++) {
		H.RgB(i / float(FILL_LEVEL + FILL_LEVEL / 6 + 3), 1.0f, 0.5f);
		// color map
		m_crFill[FILL_LEVEL - i - 1] = RGB(H.R, H.G, H.B);
		// b/w map
		//m_crFill[FILL_LEVEL - i - 1] = RGB(i/(float)FILL_LEVEL*255,i/(float)FILL_LEVEL*255,i/(float)FILL_LEVEL*255);
	}

	MapCol = 3;
	DataCol = 0;
	Title = "";
}

CWaferMap::~CWaferMap() {}

BEGIN_MESSAGE_MAP(CWaferMap, CStatic)
	//{{AFX_MSG_MAP(CWaferMap)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWaferMap message handlers

CPoint CWaferMap::Log2Dev(CCoor* point) {
	CPoint pt;
	pt.x = long(point->x * m_fLog2Device) + cpt.x;
	pt.y = cpt.y - long(point->y * m_fLog2Device);	// Flipped coordinate
	return pt;
}

CCoor& CWaferMap::Dev2Log(CPoint point) {
	static CCoor pt;
	pt.x = (point.x - cpt.x) / m_fLog2Device;
	pt.y = (cpt.y - point.y) / m_fLog2Device;	// Flipped y coordinate
	return pt;
}

void CWaferMap::OnPaint() {
	CPaintDC dc(this); // device context for painting
	OnDraw(dc);
	// Do not call CStatic::OnPaint() for painting messages
}

void CWaferMap::OnSize(UINT nType, int cx, int cy) {
	CStatic::OnSize(nType, cx, cy);
	Redraw();
}

void CWaferMap::OnDraw(CDC& dc) {
	CRect rc;
	GetClientRect(&rc);
	Draw(dc, rc);
}

void CWaferMap::DrawPattern(CDC* pDC, CRect& rc) {
	/* HAQUE/ADDED/WAFER MAP
	if (pRcp) {
		CWaferParam* pWP = pRcp->GetWp();
		if (!pWP) return;

		CRect rect(rc.CenterPoint(), CSize(0, 0));
		int nWaferRadius = (int)(pWP->mapsize * m_fLog2Device / 2);
		rect.InflateRect(CSize(nWaferRadius, nWaferRadius));
	}
	*/
}

void CWaferMap::DrawBackground(CDC* pDC, CRect& rect) {
	if (!pDC || rect.IsRectEmpty())  return;
	pDC->FillSolidRect(rect, /*::GetSysColor(COLOR_3DLIGHT)*/ RGB(165, 191, 200));
}

void CWaferMap::DrawWafer(CDC* pDC, CRect&) {
	if (!pDC->IsPrinting() && !m_bFitScreen && (m_nWaferSize < FRAME_SIZE)) {
		CPen* pOldPen = pDC->SelectObject(new CPen(PS_INSIDEFRAME, 2, RGB(0, 0, 0)));
		CBrush* pOldBrush = pDC->SelectObject(new CBrush(RGB(0xC0, 0xC0, 0xC0)));

		int nFrameSize = (int)(m_fLog2Device * FRAME_SIZE / 2);
		CRect rcFrame(cpt, CSize(0, 0));

		rcFrame.InflateRect(CSize(nFrameSize, nFrameSize));
		pDC->Ellipse(rcFrame);

		delete pDC->SelectObject(pOldPen);
		delete pDC->SelectObject(pOldBrush);
	}
	int nWaferSize = (int)(m_fLog2Device * m_nWaferSize / 2);
	CRect rcWafer(cpt, CSize(0, 0));

	if ((m_nImageType == 1) && bLandscape) {
		nWaferSize = (int)(0.9 * nWaferSize);
		rcWafer.OffsetRect(0, (int)(0.05 * nWaferSize));
	}

	rcWafer.InflateRect(CSize(nWaferSize, nWaferSize));

	if (pDC->IsPrinting() || (m_nImageType == 1)) {
		CBrush br;
		br.CreateStockObject(HOLLOW_BRUSH);
		CBrush* pOldBrush = pDC->SelectObject(&br);

		if ((m_nImageType == 1) && bLandscape) {
			int nLogicalDiameter = (int)(m_nWaferSize * m_fLog2Device);
			rcWafer.OffsetRect(3 * RULER_CX / 2 - nLogicalDiameter / 10 + 2 * CELL_SIZE, -4 * CELL_SIZE);
		}

		pDC->Ellipse(rcWafer);

		//draw notch
		CPen* pOldPen = pDC->SelectObject(new CPen(PS_INSIDEFRAME, 2, RGB(0, 0, 0)));
		int nSize = rcWafer.Width() / 50;
		if (nSize < 5)
			nSize = 5;
		int nX = (rcWafer.left + rcWafer.right) / 2;
		int nY = rcWafer.bottom;
		pDC->MoveTo(nX, nY);
		pDC->LineTo(nX + (int)(0.5 * nSize), nY + (int)(0.866 * nSize));
		pDC->LineTo(nX - (int)(0.5 * nSize), nY + (int)(0.866 * nSize));
		pDC->LineTo(nX, nY);

		delete pDC->SelectObject(pOldPen);
		pDC->SelectObject(pOldBrush);
	}
	else {
		CBrush* pOldBrush = pDC->SelectObject(new CBrush(RGB(109, 147, 87)));
		pDC->Ellipse(rcWafer);

		//draw notch
		CPen* pOldPen = pDC->SelectObject(new CPen(PS_INSIDEFRAME, 2, RGB(0, 0, 0)));
		int nSize = rcWafer.Width() / 50;
		if (nSize < 5) {
			nSize = 5;
		}
		int nX = (rcWafer.left + rcWafer.right) / 2;
		int nY = rcWafer.bottom;
		pDC->MoveTo(nX, nY);
		pDC->LineTo(nX + (int)(0.5 * nSize), nY + (int)(0.866 * nSize));
		pDC->LineTo(nX - (int)(0.5 * nSize), nY + (int)(0.866 * nSize));
		pDC->LineTo(nX, nY);

		delete pDC->SelectObject(pOldPen);
		delete pDC->SelectObject(pOldBrush);
	}
}

void CWaferMap::DrawPoint(CDC* pDC, CRect& rect) {
	if (!pRcp || (m_nImageType > 0)) {
		return;
	}

	CPattern* pPattern = &pRcp->Patt;
	if (!pPattern) return;

	if (bSiteView && pRcp->Me.matrixspacing > 0) {
		CSize szRect;
		szRect.cx = (int)(pRcp->Me.xdicesize * m_fLog2Device / 2);
		szRect.cy = (int)(pRcp->Me.ydicesize * m_fLog2Device / 2);
		if (szRect.cx < 2) szRect.cx = 2;
		if (szRect.cy < 2) szRect.cy = 2;

		CPen* pOldPen = pDC->SelectObject(new CPen(PS_SOLID, 1, RGB(0, 0, 0)));
		for (short i = 0; i < pPattern->MP.GetCount(); i++) {
			CCoor* co = pPattern->GetCoor(i);
			if (!co) continue;

			CPoint pt = Log2Dev(co);
			pDC->Rectangle(CRect(pt.x - szRect.cx, pt.y - szRect.cy, pt.x + szRect.cx, pt.y + szRect.cy));
		}

		delete pDC->SelectObject(pOldPen);
	}
	else {
		int dx;
		if (rect.Width() <= rect.Height()) {
			dx = rect.Width() / 100;
		}
		else {
			dx = rect.Height() / 100;
		}
		CPen pen(PS_SOLID, 1, RGB(255, 0, 0)), * pPen = NULL;
		CPen pen1(PS_SOLID, 1, RGB(255, 255, 0));
		CPen pen2(PS_SOLID, 1, RGB(0, 0, 255));
		for (short i = 0; i < pPattern->MP.GetCount(); i++) {
			CCoor* co = pPattern->GetCoor(i);
			if (!co) continue;

			int x1 = int(cpt.x + co->x * m_fLog2Device) - dx;
			int y1 = int(cpt.y - co->y * m_fLog2Device) - dx;
			int x2 = int(cpt.x + co->x * m_fLog2Device) + dx;
			int y2 = int(cpt.y - co->y * m_fLog2Device) + dx;

			if (co->status == CCoor::MEASURE)
			{
				pPen = (CPen*)pDC->SelectObject(&pen);
			}
			else if (co->status == CCoor::MEASURING)
			{
				pPen = (CPen*)pDC->SelectObject(&pen1);
			}
			else if (co->status == CCoor::MEASURED)
			{
				pPen = (CPen*)pDC->SelectObject(&pen2);
			}
			pDC->MoveTo(x1 - 3, (y1 + y2) / 2);
			pDC->LineTo(x2 + 3, (y1 + y2) / 2);
			pDC->MoveTo((x1 + x2) / 2, y1 - 3);
			pDC->LineTo((x1 + x2) / 2, y2 + 3);
			pDC->SelectObject(pPen);
		}
	}
}

void CWaferMap::DrawValues(CDC* pDC, CRect&) {
	if (!pRcp || (MapCol < 0)) return;

	CPattern* pPattern = &pRcp->Patt;
	if (!pPattern) return;

	CWaferParam* Wp = pRcp->GetWp();
	if (!Wp) return;

	CFont font;
	CFont* pfont = NULL;

	if (pDC->IsPrinting()) {
		font.CreateFont(64, 0, 0, 0, FW_LIGHT, 0, 0, 0, ANSI_CHARSET, 0, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Times New Roman");
		pfont = pDC->SelectObject(&font);
	}
	else {
		font.CreateFont(14, 0, 0, 0, FW_LIGHT, 0, 0, 0, ANSI_CHARSET, 0, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Times New Roman");
		pfont = pDC->SelectObject(&font);
	}

	TEXTMETRIC tm;
	pDC->GetTextMetrics(&tm);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextAlign(TA_CENTER | TA_TOP);

	int nCount = 0;
	CString dec1;
	dec1.Format(L"%%.%df", pRcp->MeParam[MeSet].MPa[MapCol].D);

	for (short i = 0; i < pPattern->MP.GetCount(); i++) {
		CMPoint* mp = pPattern->MP.Get((short)i);
		if (!mp) continue;

		CCoor* co = mp->GetCoor();
		if (!co) continue;

		CString str;
		if (bSiteView) {
			float fSFPD;
			if (mp->GetSFPD(fSFPD)) {
				str.Format(dec1, fSFPD);
				CPoint pt;
				pt = Log2Dev(co);
				pDC->TextOut(pt.x, pt.y, str);
			}
		}
		else {
			if (dmMode == dmENGINNER) {
				str.Format(L"%d", co->n);
			}
			else {
				CData* d = mp->GetData(0, FALSE);
				if (!d) {
					continue;
				}
				str = "NA";
				if (DataCol > -1) {
					if (DataCol != 999) {
						float fData = d->Get(DataCol);  // This may be a problem because DataCol may not correspond to the real data [11/8/2011 Administrator]
						if (fData > 0) {
							str.Format(dec1, fData);
						}
					}
					else {
						str.Format(L"%.2f", mp->baseline);
					}
				}
			}
			CPoint pt;
			pt = Log2Dev(co);
			pDC->TextOut(pt.x, pt.y, str);
		}
		nCount++;
	}

	if (pfont) pDC->SelectObject(pfont);
	if (font.m_hObject) {
		DeleteObject(font);
	}
}

void CWaferMap::DrawOtherText(CDC* pDC, LPCRECT lpRect) {
	CRect rc(lpRect);
	CFont font, font2, * pfont = NULL;
	if (pDC->IsPrinting()) {
		font.CreateFont(64, 0, 0, 0, FW_BOLD, 0, 0, 0, ANSI_CHARSET, 0, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Times New Roman");
		font2.CreateFont(96, 0, 0, 0, FW_BOLD, 0, 0, 0, ANSI_CHARSET, 0, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Times New Roman");
		int w = (int)(pDC->GetDeviceCaps(HORZSIZE) / 25.4 * pDC->GetDeviceCaps(LOGPIXELSX));
		int h = (int)(pDC->GetDeviceCaps(VERTSIZE) / 25.4 * pDC->GetDeviceCaps(LOGPIXELSY));
		if (h > (w * 3 / 4)) h = int(w * 3.2 / 4);
		rc.SetRect(0, 0, w, h);
		rc.InflateRect(int(-w * 0.15), int(-h * 0.075));
		rc.OffsetRect(int(w * 0.025), int(h * 0.025));
	}
	else {
		font.CreateFont(14, 0, 0, 0, FW_BOLD, 0, 0, 0, ANSI_CHARSET, 0, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Times New Roman");
		font2.CreateFont(22, 0, 0, 0, FW_BOLD, 0, 0, 0, ANSI_CHARSET, 0, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Times New Roman");
		pDC->SetBkMode(TRANSPARENT);
	}
	pfont = pDC->SelectObject(&font);

	CString msg;
	TEXTMETRIC tm;

	if (bShowStat) {
		pDC->SetTextColor(RGB(200, 128, 0));
		pDC->GetTextMetrics(&tm);
		pDC->SetTextAlign(TA_LEFT | TA_TOP);
		int y = rc.top;
		int off;
		if (m_nImageType == 2) {
			off = rc.left;
		}
		else {
			off = rc.left + tm.tmAveCharWidth;
		}
		POSITION pos = strList.GetHeadPosition();
		while (pos) {
			CString str;
			str = strList.GetNext(pos);
			pDC->TextOut(off, y, str);
			y += tm.tmHeight;
		}
	}

	if (MapCol >= 0) {
		pDC->SetTextColor(RGB(200, 64, 64));
		pDC->SelectObject(&font2);
		pDC->SetTextAlign(TA_CENTER | TA_TOP);
		pDC->TextOut(rc.CenterPoint().x, tm.tmHeight, Title);
		pDC->SetTextAlign(TA_LEFT | TA_BOTTOM);
		if (pRcp) {
			msg = pRcp->GetRecipeName();
			if (msg.GetLength() > 0) {
				msg.Format(L"Recipe: %s", pRcp->GetRecipeName());
				pDC->TextOut(rc.left, rc.bottom, msg);
			}
		}
	}

	pDC->SelectObject(pfont);
	DeleteObject(font);
	DeleteObject(font2);
}

void CWaferMap::DrawMain(CDC* pDC, CRect& rc) {
	CRect rcWafer = rc;
	if (pDC->IsPrinting()) {
		cpt = rc.CenterPoint();
		DrawWafer(pDC, rc);
	}
	else {
		DrawBackground(pDC, rc);
		rcWafer.InflateRect(-rcWafer.Width() * 0.1, -rcWafer.Height() * 0.1);
		cpt = rcWafer.CenterPoint();
		if (rcWafer.Width() <= rcWafer.Height()) {
			m_fLog2Device = rcWafer.Width() / (m_bFitScreen ? m_nWaferSize : FRAME_SIZE);
		}
		else {
			m_fLog2Device = rcWafer.Height() / (m_bFitScreen ? m_nWaferSize : FRAME_SIZE);
		}
		DrawWafer(pDC, rcWafer);
	}

	switch (m_nImageType) {
	case 0:
		if (!pDC->IsPrinting()) {
			DrawStage(pDC, rcWafer);
		}
		break;
	case 1:
		DrawColorMap(pDC, rc);
		break;
	case 2:
		DrawProfiles(pDC, rc);
		break;
	default:
		if (!pDC->IsPrinting()) {
			DrawStage(pDC, rcWafer);
		}
		break;
	}
}

#define LINE_WIDTH 20.0f
#define LINE_LENGTH 150.0f

void CWaferMap::DrawStage(CDC* pDC, CRect& rect) {
	CPen* pOldPen = pDC->SelectObject(new CPen(PS_SOLID, 1, RGB(0x7F, 0xBF, 0xBF)));
	CBrush* pOldBrush = pDC->SelectObject(new CBrush(RGB(0x7F, 0xBF, 0xBF)));
	int nROP = pDC->SetROP2(R2_MERGEPEN);

	if (rect.Width() <= rect.Height()) {
		m_fLog2Device = rect.Width() / (m_bFitScreen ? m_nWaferSize : FRAME_SIZE);
	}
	else {
		m_fLog2Device = rect.Height() / (m_bFitScreen ? m_nWaferSize : FRAME_SIZE);
	}

	BOOL bMask = m_bMask;
	if (bMask) {
		CChuckMask* pChuckMask = pRAFTApp->GetChuckMask();
		if (pChuckMask) {
			int WfrSize = m_nWaferSize;
			if (pRcp) {
				CWaferParam* pWP = pRcp->GetWp();
				WfrSize = pWP->mapsize / 2;
				bMask = pChuckMask->Draw(pDC, rect.CenterPoint(), m_fLog2Device, (float)WfrSize);
			}
		}
		else {
			bMask = FALSE;
		}
	}
	if (!bMask) {
		int nWaferRadius = (int)(m_fLog2Device * m_nWaferSize / 2);
		CRect rcWafer(cpt, CSize(0, 0));
		rcWafer.InflateRect(CSize(nWaferRadius, nWaferRadius));
		pDC->Ellipse(rcWafer);
	}
	delete pDC->SelectObject(pOldBrush);
	delete pDC->SelectObject(pOldPen);
	pDC->SetROP2(nROP);
}

BOOL CWaferMap::DrawColorMap(CDC* pDC, LPCRECT lpRect) {
	if (!pRcp) {
		return FALSE;
	}
	if (DataCol < 0) {
		return FALSE;
	}
	CPattern* pPattern = &pRcp->Patt;
	if (!pPattern) return FALSE;

	short nCount = pPattern->MP.GetCount();
	if (nCount <= 0) return FALSE;

	CWaferParam* Wp = pRcp->GetWp();
	if (!Wp) return FALSE;

	ColorMapPoint* mp2 = NULL;
	mp2 = new ColorMapPoint[nCount + 1];

	int nLogicalDiameter = (int)(m_nWaferSize * m_fLog2Device);
	if (bLandscape) {
		nLogicalDiameter = (int)(0.9 * nLogicalDiameter);
	}
	int nStep = nLogicalDiameter / CELL_SIZE;

	int nGridTableSize = (nStep + 1) * (nStep + 1);
	float(*fGridTable)[2] = new float[nGridTableSize][2];
	if (!fGridTable) {
		delete[] mp2;
		ASSERT(0);
		return FALSE;
	}
	memset(fGridTable, 0, nGridTableSize * 2 * sizeof(float));

	int nRadius = m_nWaferSize / 2;
	int nSquareRadius = nRadius * nRadius;
	double dfZone = 3.14 * nSquareRadius / (double)nCount;
	double dfCellSize = CELL_SIZE / m_fLog2Device;
	double dfRatio = dfCellSize * dfCellSize / dfZone;

	float c2 = (nStep + 1) / 2;
	float c1 = nStep / m_nWaferSize;
	for (short n = 0; n < nCount; n++) {
		CMPoint* p = pPattern->MP.Get(n);
		if (!p) continue;
		float fX, fY;
		p->GetCoor(fX, fY);

		mp2[n].fX = fX;
		mp2[n].fY = fY;

		CData* pData = p->GetData(0, FALSE);
		if (pData) {
			if (DataCol != 999) {
				mp2[n].fT = pData->Get(DataCol);
			}
			else {
				mp2[n].fT = p->baseline;
			}
		}
	}
	float fMin = 1e20f, fMax = -1e20f;

	int i;
	int nIndex = 0;
	float fX, fY, fX2, fY2;
	float df, df0, df1, df2, df3;
	int k, k0, k1, k2, k3;
	float fDist, fDist0, fDist1, fDist2, fDist3;
	short nStep2 = nStep / 2;
	c1 = m_nWaferSize / float(nStep);
	for (i = 0; i <= nStep; i++) {
		fX2 = float(i - nStep2) * c1;
		for (int j = 0; j <= nStep; j++) {
			fDist0 = m_nWaferSize * m_nWaferSize;
			for (k = 0; k < nCount; k++) {
				fX = mp2[k].fX;
				fY = mp2[k].fY;
				df = mp2[k].fT;
				fY2 = (j - nStep2) * c1;
				fDist = float(fY2 - fY) * (fY2 - fY) + (fX2 - fX) * (fX2 - fX);
				if (fDist < fDist0) {
					fDist0 = fDist;
					k0 = k;
					df0 = df;
				}
			}
			if ((nCount == 1) || (fDist0 == 0)) {
				fGridTable[nIndex][1] = df0;
			}
			if ((nCount > 1) && (fDist0 > 0)) {
				fDist1 = m_nWaferSize * m_nWaferSize;
				for (k = 0; k < nCount; k++) {
					df = mp2[k].fT;
					if ((k != k0)) {
						fX = mp2[k].fX;
						fY = mp2[k].fY;
						//fX2 = 1.0 * (i - nStep/2) * c1;
						fY2 = float(j - nStep2) * c1;
						fDist = (fY2 - fY) * (fY2 - fY) + (fX2 - fX) * (fX2 - fX);
						if (fDist < fDist1) {
							fDist1 = fDist;
							k1 = k;
							df1 = df;
						}
					}
				}
				if (nCount == 2) {
					fGridTable[nIndex][1] = (df0 / fDist0 + df1 / fDist1) / (1 / fDist0 + 1 / fDist1);
				}
				if (nCount > 2) {
					fDist2 = m_nWaferSize * m_nWaferSize;
					for (k = 0; k < nCount; k++) {
						df = mp2[k].fT;
						if ((k != k0) && (k != k1)) {
							fX = mp2[k].fX;
							fY = mp2[k].fY;
							//fX2 = 1.0 * (i - nStep/2) * c1;
							fY2 = float(j - nStep2) * c1;
							fDist = (fY2 - fY) * (fY2 - fY) + (fX2 - fX) * (fX2 - fX);
							if (fDist < fDist2) {
								fDist2 = fDist;
								k2 = k;
								df2 = df;
							}
						}
					}
				}
				if (nCount == 3) {
					fGridTable[nIndex][1] = (df0 / fDist0 + df1 / fDist1 + df2 / fDist2)
						/ (1 / fDist0 + 1 / fDist1 + 1 / fDist2);
				}
				if (nCount > 3) {
					fDist3 = m_nWaferSize * m_nWaferSize;
					for (k = 0; k < nCount; k++) {
						df = mp2[k].fT;
						if ((k != k0) && (k != k1) && (k != k2)) {
							fX = mp2[k].fX;
							fY = mp2[k].fY;
							//fX2 = 1.0 * (i - nStep/2) * c1;
							fY2 = float(j - nStep2) * c1;
							fDist = (fY2 - fY) * (fY2 - fY) + (fX2 - fX) * (fX2 - fX);
							if (fDist < fDist3) {
								fDist3 = fDist;
								k3 = k;
								df3 = df;
							}
						}
					}
					fGridTable[nIndex][1] = (df0 / fDist0 + df1 / fDist1 + df2 / fDist2 + df3 / fDist3)
						/ (1 / fDist0 + 1 / fDist1 + 1 / fDist2 + 1 / fDist3);
				}
			}
			if (fGridTable[nIndex][1] < fMin) {
				fMin = fGridTable[nIndex][1];
			}
			if (fGridTable[nIndex][1] > fMax) {
				fMax = fGridTable[nIndex][1];
			}
			nIndex++;
		}
	}

	// Draw data points [3/11/2011 FSMT]
	if (fMin < fMax) {
		CBrush brush[FILL_LEVEL];
		int i;
		for (i = 0; i < FILL_LEVEL; i++) {
			brush[i].CreateSolidBrush(m_crFill[i]);
		}

		CPoint pt;

		pt.x = cpt.x - (nLogicalDiameter + CELL_SIZE) / 2;
		if (bLandscape) {
			pt.x += (3 * RULER_CX / 2 - nLogicalDiameter / 10);
		}
		nIndex = 0;
		float fRange = fMax - fMin;
		int nSquareStep = nStep * nStep;
		for (i = 0; i <= nStep; i++) {
			int nX = 2 * i - nStep;
			pt.y = cpt.y + (nLogicalDiameter + CELL_SIZE) / 2;
			for (int j = 0; j <= nStep; j++) {
				int nY = 2 * j - nStep;
				if (nX * nX + nY * nY <= nSquareStep) {
					int nLevel = (int)(FILL_LEVEL * ((fGridTable[nIndex][1] - fMin) / (fRange)));
					if (nLevel < 0) nLevel = 0;
					if (nLevel >= FILL_LEVEL) nLevel = FILL_LEVEL - 1;
					pDC->FillRect(CRect(pt.x, pt.y - CELL_SIZE, pt.x + CELL_SIZE, pt.y), brush + nLevel);
				}
				nIndex++;
				pt.y -= CELL_SIZE;
			}
			pt.x += CELL_SIZE;
		}

		CRect rc(lpRect);
		CFont font, * pfont = NULL;

		if (pDC->IsPrinting()) {
			int w = (int)(pDC->GetDeviceCaps(HORZSIZE) / 25.4 * pDC->GetDeviceCaps(LOGPIXELSX));
			int h = (int)(pDC->GetDeviceCaps(VERTSIZE) / 25.4 * pDC->GetDeviceCaps(LOGPIXELSY));
			BOOL bLandscape = (w > h);

			if (h > (w * 3 / 4)) h = int(w * 3.2 / 4);
			rc.SetRect(0, 0, w, h);
			rc.InflateRect(int(-w * 0.15), int(-h * 0.075));
			rc.OffsetRect(int(w * 0.025), int(h * 0.15));
			if (bLandscape) {
				font.CreateFont(42, 0, 0, 0, FW_BOLD, 0, 0, 0, ANSI_CHARSET, 0, CLIP_DEFAULT_PRECIS,
					PROOF_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Times New Roman");
			}
			else {
				font.CreateFont(42, 0, -900, 0, FW_BOLD, 0, 0, 0, ANSI_CHARSET, 0, CLIP_DEFAULT_PRECIS,
					PROOF_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Times New Roman");
			}
		}
		else {
			if (bLandscape) {
				font.CreateFont(14, 0, 0, 0, FW_BOLD, 0, 0, 0, ANSI_CHARSET, 0, CLIP_DEFAULT_PRECIS,
					PROOF_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Times New Roman");
			}
			else {
				font.CreateFont(14, 0, -900, 0, FW_BOLD, 0, 0, 0, ANSI_CHARSET, 0, CLIP_DEFAULT_PRECIS,
					PROOF_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Times New Roman");
			}
			pDC->SetBkMode(TRANSPARENT);
			if (bDrawBackground) {
				pDC->SetTextColor(RGB(255, 255, 255));
			}
			else {
				pDC->SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
			}
		}
		pfont = pDC->SelectObject(&font);

		TEXTMETRIC tm;
		pDC->GetTextMetrics(&tm);
		short ic = short(tm.tmHeight / 2);

		int dx;
		CRect rcTemp;

		if (bLandscape) {
			rcTemp.left = rc.left + int(rc.Width() * 0.90);
			rcTemp.top = rc.top + int(rc.Height() * 0.125);
			rcTemp.right = rcTemp.left + int(rc.Width() * 0.03);
			rcTemp.bottom = rcTemp.top + int(rc.Height() * 0.75);
			dx = (int)(0.75 * rc.Height() / FILL_LEVEL);
			pDC->SetTextAlign(TA_LEFT | TA_TOP);
		}
		else {
			rcTemp.left = rc.left + int(rc.Width() * 0.05);
			rcTemp.top = rc.top + int(rc.Height() * 0.95);
			rcTemp.right = rcTemp.left + int(rc.Width() * 0.9);
			rcTemp.bottom = rcTemp.top + int(rc.Height() * 0.03);
			dx = (int)(0.9 * rc.Width() / FILL_LEVEL);
			pDC->SetTextAlign(TA_RIGHT | TA_TOP);
		}

		// Draw legend [3/11/2011 FSMT]
		if (DataCol > -1) {
			SStat* sta = &pPattern->MP.Stats[DataCol];
			for (i = 0; i <= FILL_LEVEL; i++) {
				CString str;
				str.Format(_T("%.2f"), i * (sta->Max - sta->Min) / FILL_LEVEL + sta->Min);

				CBrush* pBrsh = pDC->SelectObject(&brush[i]);
				if (bLandscape) {
					rcTemp.top = rcTemp.bottom - dx;
					if (i < FILL_LEVEL) pDC->FillRect(rcTemp, brush + i);
					pDC->TextOut(rcTemp.right + 1, rcTemp.bottom - ic, str);
					rcTemp.bottom = rcTemp.top;
				}
				else {
					rcTemp.right = rcTemp.left + dx;
					if (i < FILL_LEVEL) pDC->FillRect(rcTemp, brush + i);
					pDC->TextOut(rcTemp.left + ic, rcTemp.top, str);
					rcTemp.left = rcTemp.right;
				}
				pDC->SelectObject(pBrsh);
			}
		}
		pDC->SelectObject(pfont);
		if (font.m_hObject) {
			DeleteObject(font);
		}
	}
	if (mp2) {
		delete[] mp2;
	}
	delete[] fGridTable;

	return TRUE;
}

BOOL CWaferMap::DrawProfiles(CDC* pDC, LPCRECT lpRect) {
	if (!pRcp) {
		return FALSE;
	}
	if (DataCol < 0) {
		return FALSE;
	}

	CPattern* pPattern = &pRcp->Patt;
	if (!pPattern) return FALSE;

	short nCount = pPattern->MP.GetCount();
	if (nCount <= 0) return FALSE;

	CWaferParam* Wp = pRcp->GetWp();
	if (!Wp) return FALSE;

	ColorMapPoint* mp2 = NULL;
	if (nCount > 0) {
		mp2 = new ColorMapPoint[nCount];
	}

	int nLogicalDiameter = (int)(0.75 * m_nWaferSize * m_fLog2Device);
	int nStep = nLogicalDiameter / CELL_SIZE;

	int nGridTableSize = (nStep + 1) * (nStep + 1);
	float(*fGridTable)[2] = new float[nGridTableSize][2];
	if (!fGridTable) {
		delete[] mp2;
		ASSERT(0);
		return FALSE;
	}
	memset(fGridTable, 0, nGridTableSize * 2 * sizeof(float));

	int nRadius = m_nWaferSize / 2;
	int nSquareRadius = nRadius * nRadius;
	double dfZone = 3.14 * nSquareRadius / (double)nCount;
	double dfCellSize = CELL_SIZE / m_fLog2Device;
	double dfRatio = dfCellSize * dfCellSize / dfZone;

	for (short n = 0; n < nCount; n++) {
		CMPoint* p = pPattern->MP.Get(n);
		if (!p) continue;
		float fX, fY;
		p->GetCoor(fX, fY);

		mp2[n].fX = fX;
		mp2[n].fY = fY;

		CData* pData = p->GetData(0, FALSE);
		if (!pData) continue;

		if (DataCol != 999) {
			mp2[n].fT = pData->Get(DataCol);
		}
		else {
			mp2[n].fT = p->baseline;
		}
	}

	float fMin = 1e20f;
	float fMax = -1e20f;
	int i;
	int nIndex = 0;
	float fX, fY, fX2, fY2;
	double df, df0, df1, df2, df3;
	int k, k0, k1, k2, k3;
	float fDist, fDist0, fDist1, fDist2, fDist3;
	float c1 = m_nWaferSize / float(nStep);
	for (i = 0; i <= nStep; i++) {
		for (int j = 0; j <= nStep; j++) {
			fDist0 = m_nWaferSize * m_nWaferSize;
			for (k = 0; k < nCount; k++) {
				fX = mp2[k].fX;
				fY = mp2[k].fY;
				df = mp2[k].fT;
				fX2 = 1.0 * (j - nStep / 2) * m_nWaferSize / nStep;
				fY2 = 1.0 * (i - nStep / 2) * m_nWaferSize / nStep;
				fDist = (fY2 - fY) * (fY2 - fY) + (fX2 - fX) * (fX2 - fX);
				if (fDist < fDist0) {
					fDist0 = fDist;
					k0 = k;
					df0 = df;
				}
			}
			if ((nCount == 1) || (fDist0 == 0)) {
				fGridTable[nIndex][1] = df0;
			}
			if ((nCount > 1) && (fDist0 > 0)) {
				fDist1 = m_nWaferSize * m_nWaferSize;
				for (k = 0; k < nCount; k++) {
					df = mp2[k].fT;
					if ((k != k0)) {
						fX = mp2[k].fX;
						fY = mp2[k].fY;
						fX2 = 1.0 * (j - nStep / 2) * m_nWaferSize / nStep;
						fY2 = 1.0 * (i - nStep / 2) * m_nWaferSize / nStep;
						fDist = (fY2 - fY) * (fY2 - fY) + (fX2 - fX) * (fX2 - fX);
						if (fDist < fDist1) {
							fDist1 = fDist;
							k1 = k;
							df1 = df;
						}
					}
				}
				if (nCount == 2) {
					fGridTable[nIndex][1] = (df0 / fDist0 + df1 / fDist1) / (1 / fDist0 + 1 / fDist1);
				}
				if (nCount > 2) {
					fDist2 = m_nWaferSize * m_nWaferSize;
					for (k = 0; k < nCount; k++) {
						df = mp2[k].fT;
						if ((k != k0) && (k != k1)) {
							fX = mp2[k].fX;
							fY = mp2[k].fY;
							fX2 = 1.0 * (j - nStep / 2) * m_nWaferSize / nStep;
							fY2 = 1.0 * (i - nStep / 2) * m_nWaferSize / nStep;
							fDist = (fY2 - fY) * (fY2 - fY) + (fX2 - fX) * (fX2 - fX);
							if (fDist < fDist2) {
								fDist2 = fDist;
								k2 = k;
								df2 = df;
							}
						}
					}
				}
				if (nCount == 3) {
					fGridTable[nIndex][1] = (df0 / fDist0 + df1 / fDist1 + df2 / fDist2) / (1 / fDist0 + 1 / fDist1 + 1 / fDist2);
				}
				if (nCount > 3) {
					fDist3 = m_nWaferSize * m_nWaferSize;
					for (k = 0; k < nCount; k++) {
						df = mp2[k].fT;
						if ((k != k0) && (k != k1) && (k != k2)) {
							fX = mp2[k].fX;
							fY = mp2[k].fY;
							fX2 = 1.0 * (j - nStep / 2) * m_nWaferSize / nStep;
							fY2 = 1.0 * (i - nStep / 2) * m_nWaferSize / nStep;
							fDist = (fY2 - fY) * (fY2 - fY) + (fX2 - fX) * (fX2 - fX);
							if (fDist < fDist3) {
								fDist3 = fDist;
								k3 = k;
								df3 = df;
							}
						}
					}
					fGridTable[nIndex][1] = (df0 / fDist0 + df1 / fDist1 + df2 / fDist2 + df3 / fDist3)
						/ (1 / fDist0 + 1 / fDist1 + 1 / fDist2 + 1 / fDist3);
				}
			}
			if (fGridTable[nIndex][1] < fMin) {
				fMin = fGridTable[nIndex][1];
			}
			if (fGridTable[nIndex][1] > fMax) {
				fMax = fGridTable[nIndex][1];
			}
			nIndex++;
		}
	}

	CBrush brush;
	brush.CreateSolidBrush(RGB(0xFF, 0xFF, 0xFF));

	CRect rc(lpRect);

	pDC->FillRect(rc, &brush);

	if (fMin < fMax) {
		CPoint pt;

		pt.x = cpt.x - (nLogicalDiameter + CELL_SIZE) / 2;

		CPen pen;
		HPEN oldPen = NULL;

		pen.CreatePen(PS_SOLID, 1, RGB(0x00, 0x00, 0x00));
		oldPen = (HPEN)pDC->SelectObject(pen);

		int nY0;
		int nX0;
		int i, nLevel;
		int nDiam = 4;
		int YMAX = nLogicalDiameter / nDiam - 2;
		int nY2 = (int)(nStep / 2 / sqrt(2.0f));

		nX0 = cpt.x - (nLogicalDiameter + CELL_SIZE) / 2;

		CFont font, * pfont = NULL;

		if (pDC->IsPrinting()) {
			int w = (int)(pDC->GetDeviceCaps(HORZSIZE) / 25.4 * pDC->GetDeviceCaps(LOGPIXELSX));
			int h = (int)(pDC->GetDeviceCaps(VERTSIZE) / 25.4 * pDC->GetDeviceCaps(LOGPIXELSY));
			if (h > (w * 3 / 4)) h = int(w * 3.2 / 4);
			rc.SetRect(0, 0, w, h);
			rc.InflateRect(int(-w * 0.15), int(-h * 0.075));
			rc.OffsetRect(int(w * 0.025), int(h * 0.15));
			font.CreateFont(42, 0, 0, 0, FW_BOLD, 0, 0, 0, ANSI_CHARSET, 0, CLIP_DEFAULT_PRECIS,
				PROOF_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Times New Roman");
		}
		else {
			font.CreateFont(14, 0, 0, 0, FW_BOLD, 0, 0, 0, ANSI_CHARSET, 0, CLIP_DEFAULT_PRECIS,
				PROOF_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Times New Roman");
			pDC->SetBkMode(TRANSPARENT);
			if (bDrawBackground) {
				pDC->SetTextColor(RGB(255, 255, 255));
			}
			else {
				pDC->SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
			}
		}
		pfont = pDC->SelectObject(&font);
		pDC->SetTextColor(RGB(0x0, 0x0, 0x0));

		//diameter 0 deg
		nY0 = cpt.y - nLogicalDiameter / 2 + 1 * nLogicalDiameter / nDiam;
		nIndex = (nStep + 1) * (nStep / 2);

		nLevel = (int)(YMAX * ((fGridTable[nIndex][1] - fMin) / (fMax - fMin)));
		if (nLevel < 0) nLevel = 0;
		if (nLevel > YMAX) nLevel = YMAX;

		pDC->MoveTo(nX0, nY0 - nLevel);

		for (i = 1; i <= nStep; i++) {
			nLevel = (int)(YMAX * ((fGridTable[nIndex][1] - fMin) / (fMax - fMin)));
			if (nLevel < 0) nLevel = 0;
			if (nLevel > YMAX) nLevel = YMAX;

			pDC->LineTo(nX0 + CELL_SIZE * i, nY0 - nLevel);

			nIndex++;
		}
		pDC->TextOut(nX0 + 10, nY0 - YMAX + 2, L"W");
		pDC->TextOut(nX0 + CELL_SIZE * nStep - 20, nY0 - YMAX + 2, L"E");
		//diameter 90 deg
		nY0 = cpt.y - nLogicalDiameter / 2 + 2 * nLogicalDiameter / nDiam;
		nIndex = +(nStep + 1) / 2;

		nLevel = (int)(YMAX * ((fGridTable[nIndex][1] - fMin) / (fMax - fMin)));
		if (nLevel < 0) nLevel = 0;
		if (nLevel > YMAX) nLevel = YMAX;

		pDC->MoveTo(nX0, nY0 - nLevel);

		for (i = 1; i <= nStep; i++) {
			nLevel = (int)(YMAX * ((fGridTable[nIndex][1] - fMin) / (fMax - fMin)));
			if (nLevel < 0) nLevel = 0;
			if (nLevel > YMAX) nLevel = YMAX;

			pDC->LineTo(nX0 + CELL_SIZE * i, nY0 - nLevel);

			nIndex += (nStep + 1);
		}
		pDC->TextOut(nX0 + 10, nY0 - YMAX + 2, L"S");
		pDC->TextOut(nX0 + CELL_SIZE * nStep - 20, nY0 - YMAX + 2, L"N");
		//diameter 135 deg
		nY0 = cpt.y - nLogicalDiameter / 2 + 3 * nLogicalDiameter / nDiam;
		nIndex = ((nStep + 1) / 2 + nY2) * (nStep + 1) + ((nStep - 1) / 2 - nY2);

		nLevel = (int)(YMAX * ((fGridTable[nIndex][1] - fMin) / (fMax - fMin)));
		if (nLevel < 0) nLevel = 0;
		if (nLevel > YMAX) nLevel = YMAX;

		pDC->MoveTo(nX0, nY0 - nLevel);

		for (i = 1; i <= 2 * nY2; i++) {
			nLevel = (int)(YMAX * ((fGridTable[nIndex][1] - fMin) / (fMax - fMin)));
			if (nLevel < 0) nLevel = 0;
			if (nLevel > YMAX) nLevel = YMAX;

			pDC->LineTo(nX0 + CELL_SIZE * i * nStep / 2 / nY2, nY0 - nLevel);

			nIndex -= nStep;
		}
		pDC->TextOut(nX0 + 10, nY0 - YMAX + 2, L"NW");
		pDC->TextOut(nX0 + CELL_SIZE * nStep - 25, nY0 - YMAX + 2, L"SE");
		//diameter 45 deg
		nY0 = cpt.y - nLogicalDiameter / 2 + 4 * nLogicalDiameter / nDiam;
		nIndex = ((nStep + 1) / 2 - nY2) * (nStep + 1) + ((nStep + 1) / 2 - nY2);

		nLevel = (int)(YMAX * ((fGridTable[nIndex][1] - fMin) / (fMax - fMin)));
		if (nLevel < 0) nLevel = 0;
		if (nLevel > YMAX) nLevel = YMAX;

		pDC->MoveTo(nX0, nY0 - nLevel);

		for (i = 1; i <= 2 * nY2; i++) {
			nLevel = (int)(YMAX * ((fGridTable[nIndex][1] - fMin) / (fMax - fMin)));
			if (nLevel < 0) nLevel = 0;
			if (nLevel > YMAX) nLevel = YMAX;

			pDC->LineTo(nX0 + CELL_SIZE * i * nStep / 2 / nY2, nY0 - nLevel);

			nIndex += (nStep + 2);
		}
		pDC->TextOut(nX0 + 10, nY0 - YMAX + 2, L"SW");
		pDC->TextOut(nX0 + CELL_SIZE * nStep - 25, nY0 - YMAX + 2, L"NE");

		CPen pen2;
		pen2.CreatePen(PS_SOLID, 1, RGB(0x40, 0x40, 0x40));
		pDC->SelectObject(pen2);

		pDC->MoveTo(cpt.x - nLogicalDiameter / 2 - 1, cpt.y - nLogicalDiameter / 2 - 1);
		pDC->LineTo(cpt.x + nLogicalDiameter / 2 + 1, cpt.y - nLogicalDiameter / 2 - 1);
		for (int j = 0; j < nDiam; j++) {
			pDC->MoveTo(cpt.x - nLogicalDiameter / 2, cpt.y - nLogicalDiameter / 2 + (j + 1) * nLogicalDiameter / nDiam + 1);
			pDC->LineTo(cpt.x + nLogicalDiameter / 2, cpt.y - nLogicalDiameter / 2 + (j + 1) * nLogicalDiameter / nDiam + 1);
		}
		pDC->MoveTo(cpt.x - nLogicalDiameter / 2 - 1, cpt.y - nLogicalDiameter / 2 - 1);
		pDC->LineTo(cpt.x - nLogicalDiameter / 2 - 1, cpt.y + nLogicalDiameter / 2);
		pDC->MoveTo(cpt.x + nLogicalDiameter / 2 + 1, cpt.y - nLogicalDiameter / 2 - 1);
		pDC->LineTo(cpt.x + nLogicalDiameter / 2 + 1, cpt.y + nLogicalDiameter / 2);

		if (oldPen) {
			pDC->SelectObject(oldPen);
		}

		if (pen2.m_hObject) {
			DeleteObject(pen2);
		}
		if (pen.m_hObject) {
			DeleteObject(pen);
		}

		pDC->SelectObject(pfont);
		if (font.m_hObject) {
			DeleteObject(font);
		}
	}
	if (mp2) {
		delete[] mp2;
	}
	delete[] fGridTable;

	return TRUE;
}

void CWaferMap::Redraw() {
	if (m_bmpWaferMap.DeleteObject()) {
		m_bmpWaferMap.m_hObject = NULL;
	}
	Invalidate(TRUE);
}

void CWaferMap::OnRButtonUp(UINT nFlags, CPoint point) {
	CMenu menu;
	CMenu* pPopUp;
	menu.LoadMenu(IDR_MENURAFT);
	pPopUp = menu.GetSubMenu(0);
	if (!pPopUp) return;
	CPoint ScrnPt = point;
	ClientToScreen(&ScrnPt);
	int res = pPopUp->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD, ScrnPt.x, ScrnPt.y, this);
	switch (res) {
	case IDC_GOTOPOINT:
		if (dmMode != 0) {
			GotoPoint(point);
		}
		break;
	case IDC_EDITPOINT:
		if (dmMode != 0) {
			EditPoint(point);
		}
		break;
	case IDC_DELETEPOINT:
		if (dmMode != 0) {
			DeletePoint(point);
		}
		break;
	case IDC_ADDPOINT:
		if (dmMode != 0) {
			AddPoint(point);
		}
		break;
	case IDC_ADDALGNPOINT:
		if (dmMode != 0) {
			AddPointAlgn(point);
		}
		break;
		//case ID_RELOCATETOBACK:
		//	if (dmMode != 0) {
		//		RelocateToBack(point);
		//	}
		//	break;
		//case ID_RELOCATETOTOP:
		//	if (dmMode != 0) {
		//		RelocateToTop(point);
		//	}
		//	break;
		//case ID_DUPLICATE:
		//	if (pParent) {
		//		// [6/15/2020] =====================================
		//		CCoor& pt = Dev2Log(point);
		//		float Distance;

		//		if (pRcp) {
		//			CPattern* pPat = &pRcp->Patt;
		//			CMPoint* mp = pPat->FindNearestMP(pt, Distance);
		//			if (mp && (Distance < 5.0f)) {
		//				// do nothing
		//			}
		//			else {
		//				AddPointAlgn(point);
		//			}
		//			pParent->PostMessage(ID_DUPLICATE, 0, (LPARAM)mp);
		//		}
		//		else {
		//			AfxMessageBox("No Recipe created yet");
		//		}
		//		// =================================================

		//		// cause to crash program:
		//		//		point is CPoint type, so it does not cast to proper type on pParent->OnDuplicate() which need a CMPoint type
		//		// pParent->PostMessage(ID_DUPLICATE,0,(LPARAM)&point);
		//	}
		//	break;
	case ID_UPDATEPOINT:
		UpdatePoint(point);
		break;
		/*case ID_POINTVIEW:
			m_nImageType = 0;
			dmMode = CWaferMap::dmENGINNER;
			Redraw();
			break;
		case ID_RESULTVIEW:
			m_nImageType = 0;
			MapCol = 0;
			DataCol = 0;
			dmMode = CWaferMap::dmUSER;
			Redraw();
			break;
		case ID_3DRESULTVIEW:
			m_nImageType = 1;
			MapCol = 0;
			DataCol = 0;
			dmMode = CWaferMap::dmUSER;
			Redraw();
			break;
		case ID_ZVIEW:
			MapCol = 15;
			DataCol = 15;
			GenerateZPosData();
			dmMode = CWaferMap::dmUSER;
			Redraw();
			break;*/
	}
	return;
}

#define DS_BITMAP_FILEMARKER  ((WORD) ('M' << 8) | 'B')

BOOL CWaferMap::SaveBitmap(CFile& fp, CRect& rc) {
	/* HAQUE/ADDED/WAFER MAP
	CDC* pDC = GetDC();
	m_bmpWaferMap.DeleteObject();
	Draw(*pDC, rc, TRUE);
	ReleaseDC(pDC);

	BITMAP bitmapInfo;
	m_bmpWaferMap.GetBitmap(&bitmapInfo);
	BITMAPFILEHEADER hdr;
	BITMAPINFOHEADER pbih;
	UCHAR* buf2;
	DWORD dwFileHeaderSize = sizeof(BITMAPINFOHEADER) + sizeof(hdr);

	long BytesPerRow = bitmapInfo.bmWidth * (bitmapInfo.bmBitsPixel / 8);
	BytesPerRow += (4 - BytesPerRow % 4) % 4;

	pbih.biSize = sizeof(BITMAPINFOHEADER);
	pbih.biWidth = bitmapInfo.bmWidth;
	pbih.biHeight = bitmapInfo.bmHeight;
	pbih.biPlanes = bitmapInfo.bmPlanes;
	pbih.biBitCount = bitmapInfo.bmBitsPixel;
	pbih.biCompression = 0;
	pbih.biSizeImage = BytesPerRow * abs(pbih.biHeight);
	pbih.biXPelsPerMeter = 4000;
	pbih.biYPelsPerMeter = 4000;
	pbih.biClrUsed = 0;
	pbih.biClrImportant = 0;

	hdr.bfType = DS_BITMAP_FILEMARKER;
	hdr.bfSize = dwFileHeaderSize + pbih.biSizeImage;
	hdr.bfReserved1 = 0;
	hdr.bfReserved2 = 0;
	hdr.bfOffBits = dwFileHeaderSize;

	buf2 = new UCHAR[pbih.biSizeImage];
	int iByteToTransfer;
	iByteToTransfer = BytesPerRow * abs(bitmapInfo.bmHeight);
	UCHAR* buf3 = new UCHAR[iByteToTransfer];
	m_bmpWaferMap.GetBitmapBits(iByteToTransfer, buf3);
	UCHAR* pSrc, * pDest;
	int i, j; // LYF:  [12/27/2006]
	for (i = 0, j = bitmapInfo.bmHeight - 1; i < bitmapInfo.bmHeight; i++, j--) {
		pSrc = buf3 + i * bitmapInfo.bmWidthBytes;
		pDest = buf2 + j * bitmapInfo.bmWidthBytes;
		memcpy(pDest, pSrc, bitmapInfo.bmWidthBytes);
	}
	if (buf3) { delete[] buf3; }

	fp.Write(&hdr, sizeof(BITMAPFILEHEADER));
	fp.Write(&pbih, sizeof(BITMAPINFOHEADER));
	fp.Write(buf2, pbih.biSizeImage);

	if (buf2) delete[](buf2);

	m_bmpWaferMap.DeleteObject();
	Invalidate(TRUE);
	return TRUE;
	*/
	return TRUE;
}

void CWaferMap::Draw(CDC& dc, CRect rc, BOOL bDoNotShow) {
	CDC dc1;
	if (!dc1.CreateCompatibleDC(&dc)) return;
	CBitmap* pOldBitmap = NULL;
	CRect rcTemp;

	if (!m_bmpWaferMap.m_hObject) {
		if (pRcp) {
			CWaferParam* Wp = pRcp->GetWp();
			if (!Wp) return;
		}
		if (!m_bmpWaferMap.CreateCompatibleBitmap(&dc, rc.Width(), rc.Height())) return;
		pOldBitmap = dc1.SelectObject(&m_bmpWaferMap);
		DrawMain(&dc1, rcTemp = rc);
	}
	else {
		pOldBitmap = dc1.SelectObject(&m_bmpWaferMap);
		if (bRedraw == TRUE) {
			bRedraw = FALSE;
			DrawMain(&dc1, rcTemp = rc);
		}
	}

	// ====================================================== [7/24/2010 Yuen]

	CRect rcWafer;
	rcWafer = rc;

	rcWafer.InflateRect(-rcWafer.Width() * 0.1, -rcWafer.Height() * 0.1);
	if (rcWafer.Width() <= rcWafer.Height()) {
		m_fLog2Device = rcWafer.Width() / (m_bFitScreen ? m_nWaferSize : FRAME_SIZE);
	}
	else {
		m_fLog2Device = rcWafer.Height() / (m_bFitScreen ? m_nWaferSize : FRAME_SIZE);
	}

	if (m_nImageType == 0) {
		if (bDrawPatt) {
			DrawPattern(&dc1, rcTemp = rcWafer);
		}
		if (bDrawPoint) {
			DrawPoint(&dc1, rcTemp = rcWafer);
		}
		if (bDrawValues) {
			DrawValues(&dc1, rcTemp = rcWafer);
		}
	}
	if (bDrawText) {
		DrawOtherText(&dc1, rcTemp = rc);
	}
	bRedraw = FALSE;

	// ====================================================== [7/24/2010 Yuen]
	if (!bDoNotShow) {
		dc.BitBlt(rc.left, rc.top, rc.Width(), rc.Height(), &dc1, 0, 0, SRCCOPY);
	}
	if (pOldBitmap) {
		dc1.SelectObject(pOldBitmap);
	}
}

void CWaferMap::GotoPoint(CPoint& point) {
	CCoor& pt = Dev2Log(point);
	if (pRcp) {
		float Distance;
		CPattern* pPat = &pRcp->Patt;

		CMPoint* mp = pPat->FindNearestMP(pt, Distance);
		if (mp && (Distance < 5.0f)) {
			Dev.MC.get()->stage.GotoXY(mp->Co.x, mp->Co.y, 10000, true);
		}
	}
	if (pParent) {
		pParent->PostMessage(IDC_GOTOPOINT, 0, (LPARAM)&pt);
	}
}

void CWaferMap::EditPoint(CPoint& point) {
	/* HAQUE/ADDED/WAFER MAP
	CCoor& pt = Dev2Log(point);
	if (pRcp) {
		float Distance;
		CPattern* pPat = &pRcp->Patt;

		CMPoint* mp = pPat->FindNearestMP(pt, Distance);
		if (mp && (Distance < 5.0f)) {
			if (pPat->EditMP(mp->Co, mp->Co.x, mp->Co.y)) {
				Redraw();
			}
		}
	}
	if (pParent) {
		pParent->PostMessage(IDC_EDITPOINT, 0, (LPARAM)&pt);
	}
	*/
}

void CWaferMap::DeletePoint(CPoint& point) {
	CCoor& pt = Dev2Log(point);
	if (pRcp) {
		float Distance;
		CPattern* pPat = &pRcp->Patt;

		CMPoint* mp = pPat->FindNearestMP(pt, Distance);
		if (mp && (Distance < 5.0f)) {
			pPat->DelPoint(mp->Co);
			Redraw();
		}
	}
	if (pParent) {
		pParent->PostMessage(IDC_DELETEPOINT, 0, (LPARAM)&pt);
	}
}

void CWaferMap::AddPoint(CPoint& point) {
	CCoor& pt = Dev2Log(point);
	if (pRcp) {
		float Distance;
		CPattern* pPat = &pRcp->Patt;

		CMPoint* mp = pPat->FindNearestMP(pt, Distance);
		if (mp && Distance < 0.005f) {
		}
		else {
			//02282024/HAQUE/Selected point distance from center to check selected point is within the wafer range
			if (sqrt(pow(pt.x, 2) + pow(pt.y, 2)) < (pRcp->GetWp()->size / 2))
			{
				CMPoint* pMPoint = new CMPoint;
				pMPoint->SetCoor(&pt);
				pPat->MP.AddTailPoint(pMPoint);
				Redraw();
			}
		}
	}
	if (pParent) {
		pParent->PostMessage(IDC_ADDPOINT, 0, (LPARAM)&pt);
	}
}

void CWaferMap::AddPointAlgn(CPoint& point) {
	CCoor& pt = Dev2Log(point);
	if (pRcp) {
		float Distance;
		CPattern* pPat = &pRcp->Patt;

		CMPoint* mp = pPat->FindNearestMP(pt, Distance);
		if (mp && Distance < 0.005f) {
		}
		else {
			if (pRAFTApp->GetChuckMask()->GetCenter(pt.x, pt.y)) {
				CMPoint* pMPoint = new CMPoint;
				pMPoint->SetCoor(&pt);
				pPat->MP.AddTailPoint(pMPoint);
				Redraw();
			}
		}
	}
	if (pParent) {
		pParent->PostMessage(IDC_ADDALGNPOINT, 0, (LPARAM)&pt);
	}
}

void CWaferMap::RelocateToBack(CPoint& point) {
	/* HAQUE/ADDED/WAFER MAP
	if (pParent) {
		CCoor& pt = Dev2Log(point);
		pParent->PostMessage(ID_RELOCATETOBACK, 0, (LPARAM)&pt);
	}
	*/
}

void CWaferMap::RelocateToTop(CPoint& point) {
	/* HAQUE/ADDED/WAFER MAP
	if (pParent) {
		CCoor& pt = Dev2Log(point);
		pParent->PostMessage(ID_RELOCATETOTOP, 0, (LPARAM)&pt);
	}
	*/
}

void CWaferMap::UpdatePoint(CPoint& point) {
	/* HAQUE/ADDED/WAFER MAP
	CCoor& pt = Dev2Log(point);
	if (pRcp) {
		float Distance;
		CPattern* pPat = &pRcp->Patt;

		CMPoint* mp = pPat->FindNearestMP(pt, Distance);
		if (mp && (Distance < 5.0f)) {
			C413Global* g = &p413App->Global;
			mp->Co.x = g->LocXY.X;
			mp->Co.y = g->LocXY.Y;
			Redraw();
		}
	}
	if (pParent) {
		pParent->PostMessage(ID_UPDATEPOINT, 0, (LPARAM)&pt);
	}
	*/
}

void CWaferMap::OnLButtonDblClk(UINT nFlags, CPoint point) {
	/* HAQUE/ADDED/WAFER MAP
	if (dmMode != 0) {
		CCoor& pt = Dev2Log(point);
		if (pRcp) {
			float Distance;
			CPattern* pPat = &pRcp->Patt;

			CMPoint* mp = pPat->FindNearestMP(pt, Distance);
			if (mp && (Distance < 5.0f)) {
				DeletePoint(point);
			}
			else {
				AddPoint(point);
			}
		}
		if (pParent) {
			pParent->PostMessage(ID_UPDATEPOINT, 0, (LPARAM)&pt);
		}
	}
	*/
}

void CWaferMap::OnLButtonDown(UINT nFlags, CPoint point) {
	if (dmMode != 0) {
		CCoor& pt = Dev2Log(point);
		if (pRcp) {
			float Distance;
			CPattern* pPat = &pRcp->Patt;

			CMPoint* mp = pPat->FindNearestMP(pt, Distance);
			if (mp && (Distance < 5.0f)) {
				DeletePoint(point);
			}
			else {
				AddPoint(point);		//AddPointAlgn(point) - to allign point in the hole
			}
		}
		if (pParent) {
			pParent->PostMessage(ID_UPDATEPOINT, 0, (LPARAM)&pt);
		}
	}
}

void CWaferMap::GenerateZPosData() {
	/* HAQUE/ADDED/WAFER MAP
	for (short i = 0; i < pRcp->Patt.MP.GetCount(); i++) {
		CMPoint* mp = pRcp->Patt.MP.Get((short)i);
		if (mp) {
			CData* d = mp->GetData(0, TRUE);
			if (d) {
				d->DVal[DataCol] = mp->Co.z * 1e3;	// display in micron [7/17/2013 Yuen]
			}
		}
	}
	*/
}