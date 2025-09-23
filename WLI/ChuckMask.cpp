#include "pch.h"
#include "stdafx.h"
// #include <math.h>
#include <vector>
#include <map>
#include <algorithm>

using std::vector;
using std::map;
using std::sort;

#include "ChuckMask.h"

// CHollow class definition

class CHollow {
public:
	CHollow() {};
	virtual ~CHollow() {}

public:
	virtual BOOL ProcessText(LPCTSTR psz, int& nStage) = NULL;
	virtual BOOL Draw(CDC* pDC, const CPoint& ptCenter, float fRatio) = NULL;
	virtual BOOL DrawRect(CDC* pDC, const CPoint& ptCenter, float fRatio) = NULL;
	virtual int GetHole(float* pfPoint) = NULL;
	virtual int GetRadius(float* pfPoint) = NULL;
	virtual bool GetRegion(CRgn& Region) = NULL;
	virtual bool GetRectRegion(CRgn& Region) = NULL;
	virtual bool HitTest(float& XCoor, float& YCoor) = NULL;
	virtual bool Write2File(FILE* pFile) = NULL;
};

class CHole : public CHollow {
public:
	CHole();
	CHole(float X, float Y, float R) :m_dfX(X), m_dfY(Y), m_dfR(R) {};
	virtual BOOL ProcessText(LPCTSTR psz, int& nStage);
	virtual BOOL Draw(CDC* pDC, const CPoint& ptCenter, float fRatio);
	virtual BOOL DrawRect(CDC* pDC, const CPoint& ptCenter, float fRatio);
	virtual int GetHole(float* pfPoint);
	virtual int GetRadius(float* pfPoint);
	virtual bool GetRegion(CRgn& Region);
	virtual bool GetRectRegion(CRgn& Region);
	virtual bool HitTest(float& XCoor, float& YCoor);
	virtual bool Write2File(FILE* pFile);
protected:
	double m_dfX, m_dfY, m_dfR;
};

CHole::CHole() {
	m_dfX = 0; m_dfY = 0; m_dfR = 0;
}

BOOL CHole::ProcessText(LPCTSTR psz, int& nStage) {
	nStage = 0;
	return (3 == _stscanf(psz, _T("%lf,%lf,%lf"), &m_dfX, &m_dfY, &m_dfR));
}

bool CHole::Write2File(FILE* pFile) {
	ASSERT(pFile);
	TCHAR szHole[] = _T("Hole");
	_ftprintf(pFile, _T("%s\r\n"), szHole);
	_ftprintf(pFile, _T("%lf,%lf,%lf\r\n"), m_dfX, m_dfY, m_dfR);
	return true;
}

BOOL CHole::Draw(CDC* pDC, const CPoint& ptCenter, float fRatio) {
	CPoint pt;

	pt.x = ptCenter.x + (int)(fRatio * m_dfX);
	pt.y = ptCenter.y - (int)(fRatio * m_dfY);

	int nRadius = (int)(fRatio * m_dfR);

	return pDC->Ellipse(pt.x - nRadius, pt.y - nRadius, pt.x + nRadius, pt.y + nRadius);
}

BOOL CHole::DrawRect(CDC* pDC, const CPoint& ptCenter, float fRatio) {
	CPoint pt;

	pt.x = ptCenter.x + (int)(fRatio * m_dfX);
	pt.y = ptCenter.y - (int)(fRatio * m_dfY);

	int nRadius = (int)(fRatio * m_dfR);

	return pDC->Rectangle(pt.x - nRadius, pt.y - nRadius, pt.x + nRadius, pt.y + nRadius);
}

int CHole::GetHole(float* pfPoint) {
	if (pfPoint) {
		pfPoint[0] = (float)m_dfX;
		pfPoint[1] = (float)m_dfY;
	}
	return 1;
}

int CHole::GetRadius(float* pfPoint) {
	if (pfPoint) {
		pfPoint[0] = (float)m_dfR;
	}
	return 1;
}

bool CHole::GetRegion(CRgn& Region) {
	return  (Region.CreateEllipticRgn(int(m_dfX - m_dfR), int(m_dfY - m_dfR), int(m_dfX + m_dfR), int(m_dfY + m_dfR)) == TRUE);
}

bool CHole::GetRectRegion(CRgn& Region) {
	return  (Region.CreateRectRgn(int(m_dfX - m_dfR), int(m_dfY - m_dfR), int(m_dfX + m_dfR), int(m_dfY + m_dfR)) == TRUE);
}

bool CHole::HitTest(float& XCoor, float& YCoor) {
	float Dist = (float)sqrt(((m_dfX - XCoor) * (m_dfX - XCoor)) + ((m_dfY - YCoor) * (m_dfY - YCoor)));
	if (Dist > m_dfR) {
		return false;
	}
	XCoor = (float)m_dfX;
	YCoor = (float)m_dfY;
	return true;
}

class CComplex : public CHollow {
public:
	CComplex() {}
	virtual ~CComplex();

	virtual BOOL ProcessText(LPCTSTR psz, int& nStage);
	virtual BOOL Draw(CDC* pDC, const CPoint& ptCenter, float fRatio);
	virtual BOOL DrawRect(CDC* pDC, const CPoint& ptCenter, float fRatio);
	virtual int GetHole(float* pfPoint);
	virtual bool GetRegion(CRgn& Region);
	virtual bool GetRectRegion(CRgn& Region);
	virtual bool HitTest(float& XCoor, float& YCoor);
	virtual bool Write2File(FILE* pFile);
	virtual int GetRadius(float* pfPoint);
protected:
	class CVertex {
	public:
		CVertex() : m_dfX(NULL), m_dfY(NULL) {}
		virtual ~CVertex() {}
		virtual BOOL IsArc() { return FALSE; }
		virtual BOOL GetVertex(UINT nPoint, double& dfX, double& dfY);

	public:
		double m_dfX, m_dfY;
	};

	class CArc : public CVertex {
	public:
		CArc() {}
		virtual BOOL IsArc() { return TRUE; }
		virtual BOOL GetVertex(UINT nPoint, double& dfX, double& dfY);

	public:
		double m_dfR;
		double m_dfStartAngle, m_dfEndingAngle;
	};

	CTypedPtrList<CPtrList, CVertex*> m_lstVertex;
};

BOOL CComplex::CVertex::GetVertex(UINT nPoint, double& fX, double& fY) {
	if (nPoint == 0) {
		fX = m_dfX;
		fY = m_dfY;
	}
	return FALSE;	// no more vertex
}

BOOL CComplex::CArc::GetVertex(UINT nPoint, double& dfX, double& dfY) {
	double dfAngle = atan(1.) * 4 / 180;	// PI / 180
	switch (nPoint) {
	case 0:
		dfAngle *= m_dfStartAngle;
		dfX = m_dfX + m_dfR * cos(dfAngle);
		dfY = m_dfY + m_dfR * sin(dfAngle);
		break;
	case 1:
		dfX = m_dfX;
		dfY = m_dfY;
		break;
	case 2:
		dfAngle *= m_dfEndingAngle;
		dfX = m_dfX + m_dfR * cos(dfAngle);
		dfY = m_dfY + m_dfR * sin(dfAngle);
		break;
	case 3:
		dfX = m_dfX - m_dfR;
		dfY = m_dfY - m_dfR;
		break;
	case 4:
		dfX = m_dfX + m_dfR;
		dfY = m_dfY + m_dfR;	// fall thru
	default:
		return FALSE; // no more vertex
	}
	return TRUE;
}

CComplex::~CComplex() {
	POSITION pos = m_lstVertex.GetHeadPosition();
	while (pos) {
		CVertex* pVertex = m_lstVertex.GetNext(pos);
		if (pVertex) delete pVertex;
	}
}
bool CComplex::GetRegion(CRgn& Region) {
	return false;
}

bool CComplex::GetRectRegion(CRgn& Region) {
	return false;
}

bool CComplex::Write2File(FILE* pFile) {
	ASSERT(pFile);
	TCHAR szVertex[] = _T("point");
	TCHAR szArc[] = _T("arc");
	TCHAR szRgn[] = _T("Region");
	TCHAR szEnd[] = _T("end");

	_ftprintf(pFile, _T("%s\r\n"), szRgn);
	CVertex* pVertex = NULL;
	POSITION Pos = m_lstVertex.GetHeadPosition();
	while (Pos) {
		pVertex = m_lstVertex.GetNext(Pos);
		if (pVertex->IsArc()) {
			CArc* pArc = static_cast<CArc*>(pVertex);
			if (pArc) {
				_ftprintf(pFile, _T("%s\r\n"), szArc);
				_ftprintf(pFile, _T("%lf,%lf %lf %lf,%lf\r\n"), pArc->m_dfX, pArc->m_dfY,
					pArc->m_dfR, pArc->m_dfStartAngle, pArc->m_dfEndingAngle);
			}
		}
		else {
			_ftprintf(pFile, _T("%s\r\n"), szVertex);
			_ftprintf(pFile, _T("%lf,%lf"), pVertex->m_dfX, pVertex->m_dfY);
		}
	}
	_ftprintf(pFile, _T("%s\r\n"), szEnd);
	return true;
}

BOOL CComplex::ProcessText(LPCTSTR psz, int& nStage) {
	if (nStage != m_lstVertex.GetCount()) return FALSE;

	TCHAR szVertex[] = _T("point");
	TCHAR szArc[] = _T("arc");

	nStage++;	// increase it
	if (_tcsncmp(psz, szVertex, sizeof(szVertex) - 1) == 0) {
		CVertex* pVertex = new CVertex;
		if (!pVertex) return FALSE;
		// it expects like:	point_X,Y
		if (2 == _stscanf(psz + sizeof(szVertex), _T("%lf,%lf"), &pVertex->m_dfX, &pVertex->m_dfY)) {
			m_lstVertex.AddTail(pVertex);
		}
		else {
			delete pVertex;
			return FALSE;
		}
	}
	else if (_tcsncmp(psz, szArc, sizeof(szArc) - 1) == 0) {
		CArc* pArc = new CArc;
		if (!pArc) return FALSE;

		// it expects like:	arc_X,Y_R_ang1_ang2
		if (5 == _stscanf(psz + sizeof(szArc), _T("%lf,%lf %lf %lf,%lf"), &pArc->m_dfX, &pArc->m_dfY,
			&pArc->m_dfR, &pArc->m_dfStartAngle, &pArc->m_dfEndingAngle)) {
			m_lstVertex.AddTail(pArc);
		}
		else {
			delete pArc;
			return FALSE;
		}
	}
	else {
		nStage = 0;
	}
	return TRUE;
}

bool CComplex::HitTest(float& XCoor, float& YCoor) {
	//OSVERSIONINFO osvi;
	//osvi.dwOSVersionInfoSize = sizeof(osvi);
	//GetVersionEx(&osvi);
	CDC MemDc;
	MemDc.CreateDC(L"DISPLAY", NULL, NULL, NULL);
	CPoint PtTest(XCoor, YCoor);

	POSITION pos = m_lstVertex.GetHeadPosition();
	vector<CPoint> Lines;
	if (pos) {
		CVertex* pVertex = m_lstVertex.GetNext(pos);
		if (!pVertex) return FALSE;

		double dfX, dfY;
		if (!pVertex->GetVertex(0, dfX, dfY)) pVertex = NULL;	// for 1 vertex

		while (pos) {
			if (!pVertex) {
				pVertex = m_lstVertex.GetNext(pos);
				if (!pVertex) break;
			}
			if (pVertex->IsArc()) {
				CPoint ptStartPoint;
				pVertex->GetVertex(0, dfX, dfY);
				ptStartPoint.x = (dfX);
				ptStartPoint.y = (dfY);
				Lines.push_back(ptStartPoint);

				CPoint ptEndPoint;
				pVertex->GetVertex(2, dfX, dfY);
				ptEndPoint.x = (dfX);
				ptEndPoint.y = (dfY);
				Lines.push_back(ptEndPoint);

				CPoint ptLeftTop;
				pVertex->GetVertex(3, dfX, dfY);
				ptLeftTop.x = (dfX);
				ptLeftTop.y = (dfY);

				CPoint ptRightBottom;
				pVertex->GetVertex(4, dfX, dfY);
				ptRightBottom.x = (dfX);
				ptRightBottom.y = (dfY);
				MemDc.BeginPath();
				MemDc.Chord(CRect(ptLeftTop, ptRightBottom), ptEndPoint, ptStartPoint);
				MemDc.EndPath();
				CRgn Region;
				if (Region.CreateFromPath(&MemDc)) {
					if (Region.PtInRegion(PtTest)) {
						return true;
					}
				}
			}
			else {
				pVertex->GetVertex(0, dfX, dfY);

				int nX = (dfX);
				int nY = (dfY);
				CPoint CurrPt;
				CurrPt.x = dfX; CurrPt.y = dfY;
				Lines.push_back(CurrPt);
			}
			pVertex = NULL;
		}
		MemDc.BeginPath();
		for (unsigned int iPoint = 0; iPoint < Lines.size(); iPoint++) {
			if (!iPoint) {
				MemDc.MoveTo(Lines[iPoint]);
			}
			else {
				MemDc.LineTo(Lines[iPoint]);
			}
		}
		if (Lines.size()) {
			MemDc.MoveTo(Lines[0]);
		}
		MemDc.EndPath();
		CRgn Region;
		if (Region.CreateFromPath(&MemDc)) {
			if (Region.PtInRegion(PtTest)) {
				return true;
			}
		}
	}
	return false;
}

BOOL CComplex::Draw(CDC* pDC, const CPoint& ptCenter, float fRatio) {
	//OSVERSIONINFO osvi;
	//osvi.dwOSVersionInfoSize = sizeof(osvi);
	//GetVersionEx(&osvi);

	//BOOL bArcPathSupported = (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT);
	//if (bArcPathSupported) pDC->SetArcDirection(AD_COUNTERCLOCKWISE);

	BOOL bArcPathSupported = TRUE;

	POSITION pos = m_lstVertex.GetHeadPosition();
	if (pos) {
		CVertex* pVertex = m_lstVertex.GetNext(pos);
		if (!pVertex) return FALSE;

		double dfX, dfY;
		pDC->BeginPath();
		if (!pVertex->GetVertex(0, dfX, dfY)) pVertex = NULL;	// for 1 vertex

		int nStartX = ptCenter.x + (int)(dfX * fRatio);
		int nStartY = ptCenter.y - (int)(dfY * fRatio);

		pDC->MoveTo(nStartX, nStartY);

		while (pos) {
			if (!pVertex) {
				pVertex = m_lstVertex.GetNext(pos);
				if (!pVertex) break;
			}
			if (pVertex->IsArc()) {
				CPoint ptStartPoint;
				pVertex->GetVertex(0, dfX, dfY);
				ptStartPoint.x = ptCenter.x + (int)(dfX * fRatio);
				ptStartPoint.y = ptCenter.y - (int)(dfY * fRatio);

				CPoint ptEndPoint;
				pVertex->GetVertex(2, dfX, dfY);
				ptEndPoint.x = ptCenter.x + (int)(dfX * fRatio);
				ptEndPoint.y = ptCenter.y - (int)(dfY * fRatio);

				if (bArcPathSupported) {
					CPoint ptLeftTop;
					pVertex->GetVertex(3, dfX, dfY);
					ptLeftTop.x = ptCenter.x + (int)(dfX * fRatio);
					ptLeftTop.y = ptCenter.y - (int)(dfY * fRatio);

					CPoint ptRightBottom;
					pVertex->GetVertex(4, dfX, dfY);
					ptRightBottom.x = ptCenter.x + (int)(dfX * fRatio);
					ptRightBottom.y = ptCenter.y - (int)(dfY * fRatio);

					pDC->ArcTo(CRect(ptLeftTop, ptRightBottom), ptStartPoint, ptEndPoint);
				}
				else {
					CPoint ptCenterPoint;
					pVertex->GetVertex(1, dfX, dfY);
					ptCenterPoint.x = ptCenter.x + (int)(dfX * fRatio);
					ptCenterPoint.y = ptCenter.y - (int)(dfY * fRatio);

					pDC->LineTo(ptStartPoint);
					pDC->LineTo(ptCenterPoint);
					pDC->LineTo(ptEndPoint);
				}
			}
			else {
				pVertex->GetVertex(0, dfX, dfY);

				int nX = ptCenter.x + (int)(dfX * fRatio);
				int nY = ptCenter.y - (int)(dfY * fRatio);

				pDC->LineTo(nX, nY);
			}
			pVertex = NULL;
		}
		if (!bArcPathSupported) {
			pos = m_lstVertex.GetHeadPosition();
			while (pos) {
				CVertex* pVrtex = m_lstVertex.GetNext(pos);
				if (!pVrtex) break;
				if (pVrtex->IsArc()) {
					CPoint ptStartPoint;
					pVrtex->GetVertex(0, dfX, dfY);
					ptStartPoint.x = ptCenter.x + (int)(dfX * fRatio);
					ptStartPoint.y = ptCenter.y - (int)(dfY * fRatio);

					CPoint ptEndPoint;
					pVrtex->GetVertex(2, dfX, dfY);
					ptEndPoint.x = ptCenter.x + (int)(dfX * fRatio);
					ptEndPoint.y = ptCenter.y - (int)(dfY * fRatio);

					CPoint ptLeftTop;
					pVrtex->GetVertex(3, dfX, dfY);
					ptLeftTop.x = ptCenter.x + (int)(dfX * fRatio);
					ptLeftTop.y = ptCenter.y - (int)(dfY * fRatio);

					CPoint ptRightBottom;
					pVrtex->GetVertex(4, dfX, dfY);
					ptRightBottom.x = ptCenter.x + (int)(dfX * fRatio);
					ptRightBottom.y = ptCenter.y - (int)(dfY * fRatio);

					pDC->Chord(CRect(ptLeftTop, ptRightBottom), ptStartPoint, ptEndPoint);
				}
			}
		}
		pDC->LineTo(nStartX, nStartY);
		pDC->EndPath();

		CRgn rgnHollow;
		if (rgnHollow.CreateFromPath(pDC)) {
			CRect Rect;
			rgnHollow.GetRgnBox(Rect);
			pDC->PaintRgn(&rgnHollow);
		}
	}

	return TRUE;
}

BOOL CComplex::DrawRect(CDC* pDC, const CPoint& ptCenter, float fRatio) {
	//OSVERSIONINFO osvi;
	//osvi.dwOSVersionInfoSize = sizeof(osvi);
	//GetVersionEx(&osvi);

	//BOOL bArcPathSupported = (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT);
	//if (bArcPathSupported) pDC->SetArcDirection(AD_COUNTERCLOCKWISE);

	BOOL bArcPathSupported = TRUE;

	POSITION pos = m_lstVertex.GetHeadPosition();
	if (pos) {
		CVertex* pVertex = m_lstVertex.GetNext(pos);
		if (!pVertex) return FALSE;

		double dfX, dfY;
		pDC->BeginPath();
		if (!pVertex->GetVertex(0, dfX, dfY)) pVertex = NULL;	// for 1 vertex

		int nStartX = ptCenter.x + (int)(dfX * fRatio);
		int nStartY = ptCenter.y - (int)(dfY * fRatio);

		pDC->MoveTo(nStartX, nStartY);

		while (pos) {
			if (!pVertex) {
				pVertex = m_lstVertex.GetNext(pos);
				if (!pVertex) break;
			}
			if (pVertex->IsArc()) {
				CPoint ptStartPoint;
				pVertex->GetVertex(0, dfX, dfY);
				ptStartPoint.x = ptCenter.x + (int)(dfX * fRatio);
				ptStartPoint.y = ptCenter.y - (int)(dfY * fRatio);

				CPoint ptEndPoint;
				pVertex->GetVertex(2, dfX, dfY);
				ptEndPoint.x = ptCenter.x + (int)(dfX * fRatio);
				ptEndPoint.y = ptCenter.y - (int)(dfY * fRatio);

				if (bArcPathSupported) {
					CPoint ptLeftTop;
					pVertex->GetVertex(3, dfX, dfY);
					ptLeftTop.x = ptCenter.x + (int)(dfX * fRatio);
					ptLeftTop.y = ptCenter.y - (int)(dfY * fRatio);

					CPoint ptRightBottom;
					pVertex->GetVertex(4, dfX, dfY);
					ptRightBottom.x = ptCenter.x + (int)(dfX * fRatio);
					ptRightBottom.y = ptCenter.y - (int)(dfY * fRatio);

					pDC->ArcTo(CRect(ptLeftTop, ptRightBottom), ptStartPoint, ptEndPoint);
				}
				else {
					CPoint ptCenterPoint;
					pVertex->GetVertex(1, dfX, dfY);
					ptCenterPoint.x = ptCenter.x + (int)(dfX * fRatio);
					ptCenterPoint.y = ptCenter.y - (int)(dfY * fRatio);

					pDC->LineTo(ptStartPoint);
					pDC->LineTo(ptCenterPoint);
					pDC->LineTo(ptEndPoint);
				}
			}
			else {
				pVertex->GetVertex(0, dfX, dfY);

				int nX = ptCenter.x + (int)(dfX * fRatio);
				int nY = ptCenter.y - (int)(dfY * fRatio);

				pDC->LineTo(nX, nY);
			}
			pVertex = NULL;
		}
		if (!bArcPathSupported) {
			pos = m_lstVertex.GetHeadPosition();
			while (pos) {
				CVertex* pVrtex = m_lstVertex.GetNext(pos);
				if (!pVrtex) break;
				if (pVrtex->IsArc()) {
					CPoint ptStartPoint;
					pVrtex->GetVertex(0, dfX, dfY);
					ptStartPoint.x = ptCenter.x + (int)(dfX * fRatio);
					ptStartPoint.y = ptCenter.y - (int)(dfY * fRatio);

					CPoint ptEndPoint;
					pVrtex->GetVertex(2, dfX, dfY);
					ptEndPoint.x = ptCenter.x + (int)(dfX * fRatio);
					ptEndPoint.y = ptCenter.y - (int)(dfY * fRatio);

					CPoint ptLeftTop;
					pVrtex->GetVertex(3, dfX, dfY);
					ptLeftTop.x = ptCenter.x + (int)(dfX * fRatio);
					ptLeftTop.y = ptCenter.y - (int)(dfY * fRatio);

					CPoint ptRightBottom;
					pVrtex->GetVertex(4, dfX, dfY);
					ptRightBottom.x = ptCenter.x + (int)(dfX * fRatio);
					ptRightBottom.y = ptCenter.y - (int)(dfY * fRatio);

					pDC->Chord(CRect(ptLeftTop, ptRightBottom), ptStartPoint, ptEndPoint);
				}
			}
		}
		pDC->LineTo(nStartX, nStartY);
		pDC->EndPath();

		CRgn rgnHollow;
		if (rgnHollow.CreateFromPath(pDC)) {
			CRect Rect;
			rgnHollow.GetRgnBox(Rect);
			pDC->PaintRgn(&rgnHollow);
		}
	}

	return TRUE;
}

int CComplex::GetHole(float*) {
	return 0;
}

int CComplex::GetRadius(float*) {
	return 0;
}

class CHoleArray : public CHole {
public:
	CHoleArray() : m_dfXStep(NULL), m_dfYStep(NULL), m_nXCount(NULL), m_nYCount(NULL) {}
	virtual BOOL ProcessText(LPCTSTR psz, int& nStage);
	virtual BOOL Draw(CDC* pDC, const CPoint& ptCenter, float fRatio);
	virtual BOOL DrawRect(CDC* pDC, const CPoint& ptCenter, float fRatio);
	virtual int  GetHole(float* pfPoint);
	virtual bool GetRegion(CRgn& Region);
	virtual bool GetRectRegion(CRgn& Region);
	virtual bool HitTest(float& XCoor, float& YCoor);
	virtual bool Write2File(FILE* pFile);
	virtual int GetRadius(float* pfPoint);
protected:
	//double m_dfX, m_dfY, m_dfR;
	double m_dfXStep, m_dfYStep;
	int m_nXCount, m_nYCount;
};

BOOL CHoleArray::ProcessText(LPCTSTR psz, int& nStage) {
	int nResult;
	switch (nStage++) {
	case 0: nResult = 1 - _stscanf(psz, _T("%lf"), &m_dfR); break;
	case 1: nResult = 2 - _stscanf(psz, _T("%lf,%lf"), &m_dfX, &m_dfY); break;
	case 2: nResult = 2 - _stscanf(psz, _T("%lf,%lf"), &m_dfXStep, &m_dfYStep); break;
	case 3: nResult = 2 - _stscanf(psz, _T("%i,%i"), &m_nXCount, &m_nYCount); break;
	default: return FALSE;
	}
	return (nResult == 0);
}

BOOL CHoleArray::Draw(CDC* pDC, const CPoint& ptCenter, float fRatio) {
	int nRadius = (int)(fRatio * m_dfR);
	for (int i = 0; i < m_nXCount; i++) {
		int nX = ptCenter.x + (int)((i * m_dfXStep + m_dfX) * fRatio);

		for (int j = 0; j < m_nYCount; j++) {
			int nY = ptCenter.y - (int)((j * m_dfYStep + m_dfY) * fRatio);
			pDC->Ellipse(CRect(nX - nRadius, nY - nRadius, nX + nRadius, nY + nRadius));
		}
	}
	return TRUE;
}

BOOL CHoleArray::DrawRect(CDC* pDC, const CPoint& ptCenter, float fRatio) {
	int nRadius = (int)(fRatio * m_dfR);
	for (int i = 0; i < m_nXCount; i++) {
		int nX = ptCenter.x + (int)((i * m_dfXStep + m_dfX) * fRatio);

		for (int j = 0; j < m_nYCount; j++) {
			int nY = ptCenter.y - (int)((j * m_dfYStep + m_dfY) * fRatio);
			pDC->Rectangle(CRect(nX - nRadius, nY - nRadius, nX + nRadius, nY + nRadius));
		}
	}
	return TRUE;
}

bool CHoleArray::Write2File(FILE* pFile) {
	ASSERT(pFile);
	TCHAR szArray[] = _T("Array");

	_ftprintf(pFile, _T("%s\r\n"), szArray);
	_ftprintf(pFile, _T("%lf"), m_dfR);
	_ftprintf(pFile, _T("%lf,%lf"), m_dfX, m_dfY);
	_ftprintf(pFile, _T("%lf,%lf"), m_dfXStep, m_dfYStep);
	_ftprintf(pFile, _T("%i,%i"), m_nXCount, m_nYCount);
	return true;
}

int CHoleArray::GetHole(float* pfPoint) {
	if (pfPoint) {
		int n = 0;
		for (int i = 0; i < m_nXCount; i++) {
			for (int j = 0; j < m_nYCount; j++) {
				pfPoint[n++] = (float)(m_dfX + m_dfXStep * i);
				pfPoint[n++] = (float)(m_dfY + m_dfYStep * j);
			}
		}
	}
	return (m_nXCount * m_nYCount);
}

int CHoleArray::GetRadius(float* pfPoint) {
	if (pfPoint) {
		pfPoint[0] = (float)m_dfR;
	}
	return 1;
}

bool CHoleArray::GetRegion(CRgn& Region) {
	return false;
}

bool CHoleArray::GetRectRegion(CRgn& Region) {
	return false;
}

bool CHoleArray::HitTest(float& XCoor, float& YCoor) {
	int n = 0;
	float TmpX, TmpY;
	float Dist = 2000.f, TmpDist;  // Arbitrary high number
	for (int i = 0; i < m_nXCount; i++) {
		for (int j = 0; j < m_nYCount; j++) {
			TmpX = (float)(m_dfX + m_dfXStep * i);
			TmpY = (float)(m_dfY + m_dfYStep * j);
			TmpDist = (float)sqrt(((TmpX - XCoor) * (TmpX - XCoor)) + ((TmpY - YCoor) * (TmpY - YCoor)));
			if (TmpDist < Dist) {
				Dist = TmpDist;
				if (Dist < m_dfR) {
					return true;
				}
			}
		}
	}
	return false;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CChuckMask::CChuckMask() {
	m_fXPitch = 12.7f, m_fYPitch = 12.7f;
	nDesign = 0;

	wchar_t inifile[_MAX_PATH];
	wchar_t drive[_MAX_DRIVE];
	wchar_t dir[_MAX_DIR];
	wchar_t fname[_MAX_FNAME];
	wchar_t ext[_MAX_EXT];

	GetModuleFileName(NULL, inifile, _MAX_PATH);
	_wsplitpath(inifile, drive, dir, fname, ext);
	_wmakepath(inifile, drive, dir, L"FSMLocalCfg", L"ini");

	wchar_t str[255];
	nDesign = GetPrivateProfileInt(L"Stage", L"Design", 0, inifile);
	wsprintf(str, L"%.2f", m_fXPitch);
	GetPrivateProfileString(L"Stage", L"XPitch", str, str, 255, inifile);
	m_fXPitch = wcstof(str, nullptr);
	wsprintf(str, L"%.2f", m_fYPitch);
	GetPrivateProfileString(L"Stage", L"YPitch", str, str, 255, inifile);
	m_fYPitch = wcstof(str, nullptr);

	bOffset = false;
}

CChuckMask::~CChuckMask() {
	ClearList();
}

BOOL CChuckMask::ClearList() {
	POSITION pos = m_lstHollow.GetHeadPosition();
	while (pos) {
		CHollow* pHollow = m_lstHollow.GetNext(pos);
		if (pHollow) delete pHollow;
	}
	if (m_lstHollow.GetCount()) {
		m_lstHollow.RemoveAll();
	}
	return TRUE;
}

/*
	Superman: to handle the text file the C-type functions are more useful
*/
void Dump(Points& Pts);

BOOL CChuckMask::LoadFromFile(LPCTSTR lpszFile) {
	if (_tcslen(lpszFile) <= 0) return FALSE;
	FILE* fp = _tfopen(lpszFile, _T("rt"));
	if (!fp) return FALSE;

	ClearList();

	TCHAR szHole[] = _T("Hole");
	TCHAR szRegion[] = _T("Region");
	TCHAR szArray[] = _T("Array");
	TCHAR szPitch[] = _T("Pitch");

	CHollow* pHollow = NULL;
	int nLine = 0;
	while (!feof(fp)) {
		TCHAR chLine[256];
		TCHAR* psz = _fgetts(chLine, sizeof(chLine) / sizeof(chLine[0]), fp);
		if (!psz) break;

		TCHAR* p = _tcschr(psz, _T(';'));
		if (p) *p = NULL;	// delete the trailing comments
		if (_tcslen(psz) <= 0) continue;

		if (_tcsncmp(szPitch, psz, _tcslen(szPitch)) == 0) {
			_stscanf(psz + _tcslen(szPitch), _T(":%f,%f"), &m_fXPitch, &m_fYPitch);
			continue;
		}
		if (!pHollow) {
			if (_tcsncmp(szHole, psz, _tcslen(szHole)) == 0) {
				pHollow = new CHole;
			}
			else if (_tcsncmp(szRegion, psz, _tcslen(szRegion)) == 0) {
				pHollow = new CComplex;
			}
			else if (_tcsncmp(szArray, psz, _tcslen(szArray)) == 0) {
				pHollow = new CHoleArray;
			}
			else {
			}
			nLine = 0;
			if (!pHollow) break;
		}
		else {
			if (!pHollow->ProcessText(psz, nLine)) {
				break;
			}
			if (nLine == 0) {
				m_lstHollow.AddTail(pHollow);
				pHollow = NULL;
			}
		}
	}
	fclose(fp);
	if (!pHollow) return TRUE;
	delete pHollow;

	return FALSE;
}

BOOL CChuckMask::IsPointOnMask(float XCoor, float YCoor) {
	/*	BOOL bRes = TRUE;
		int iHole = 0,nPoints;
		float *Points =
		GetCenterPoints(nPoints);
		float ClosestX = 0.f;
		float ClosestY = 0.f;
		float Dist = 200.f,TempDist;
		if(Points&&nPoints)
		{
			for(int iPoint = 0;iPoint < nPoints;iPoint++)
			{
				float XCenter = *Points++;
				float YCenter = *Points++;
				TempDist = (float)sqrt((XCoor - XCenter)*(XCoor - XCenter)+(YCoor - YCenter)*(YCoor - YCenter));
				if(Dist > TempDist)
				{
					iHole  = iPoint;
					Dist = TempDist;
				}
			}
		}
		POSITION pos = m_lstHollow.GetHeadPosition();
		while ((pos)&&(iHole >=0))
		{
			CHollow *pHollow = m_lstHollow.GetNext(pos);
			if(!iHole)
			{
				CRgn Region;
				pHollow->GetRegion(Region);
				CPoint Pt(int(XCoor-1),int(YCoor-1));
				bRes = Region.PtInRegion(Pt);
			}
			iHole--;
		}
		return bRes;*/
		// Above algorithm does not wirk for all cases because Complex & Holearray does not have
		// .. one single Point & so cannot get a single region
	POSITION pos = m_lstHollow.GetHeadPosition();
	if (bOffset) {
		XCoor -= m_fXPitch / 2;
		YCoor -= m_fYPitch / 2;
	}
	while (pos) {
		CHollow* pHollow = m_lstHollow.GetNext(pos);
		if (pHollow->HitTest(XCoor, YCoor)) {
			return true;
		}
	}
	return false;
}

BOOL CChuckMask::Draw(CDC* pDC, const CPoint& ptCenter, float fRatio, float Radius) {
	if (0 == m_lstHollow.GetCount()) return FALSE;
	CPoint TPoint = ptCenter;
	if (bOffset) {
		TPoint.x += (m_fXPitch / 2) * fRatio;
		TPoint.y -= (m_fYPitch / 2) * fRatio;
	}

	POSITION pos = m_lstHollow.GetHeadPosition();
	while (pos) {
		CHollow* pHollow = m_lstHollow.GetNext(pos);
		if (pHollow) {
			int nData = pHollow->GetHole(NULL);
			if (nData) {
				float* pTemp = new float[nData * 2];
				if (pTemp && pHollow->GetHole(pTemp)) {
					float Dist = sqrt(pTemp[0] * pTemp[0] + pTemp[1] * pTemp[1]);
					if (Dist < Radius) {
						if (nDesign > 0) {
							pHollow->DrawRect(pDC, TPoint, fRatio);
						}
						else {
							pHollow->Draw(pDC, TPoint, fRatio);
						}
					}
				}
				delete[] pTemp;
			}
		}
	}
	return TRUE;
}

float* CChuckMask::GetCenterPoints(int& nCount) {
	if (0 == m_lstHollow.GetCount()) return NULL;

	int nHole = 0;
	POSITION pos = m_lstHollow.GetHeadPosition();
	while (pos) {
		CHollow* pHollow = m_lstHollow.GetNext(pos);
		if (pHollow) nHole += pHollow->GetHole(NULL);
	}

	if (nHole <= 0) return NULL;

	float* pfPoint = new float[2 * nHole];
	int i = 0;
	pos = m_lstHollow.GetHeadPosition();
	while (pos) {
		CHollow* pHollow = m_lstHollow.GetNext(pos);
		if (pHollow) {
			int Temp = pHollow->GetHole(pfPoint + i * 2);
			if (bOffset) {
				pfPoint[i * 2] += m_fXPitch / 2;
				pfPoint[(i * 2) + 1] += m_fYPitch / 2;
			}
			i += Temp;
			if (i >= nHole) break;
		}
	}
	nCount = i;
	return pfPoint;
}

bool CChuckMask::Write2File(CString& FileName) {
	CHollow* pHollow = NULL;
	FILE* pFile = _wfopen(FileName, L"w+");
	if (pFile) {
		POSITION Pos = m_lstHollow.GetHeadPosition();
		while (Pos) {
			pHollow = m_lstHollow.GetNext(Pos);
			if (pHollow) {
				pHollow->Write2File(pFile);
			}
		}
		fclose(pFile);
		return true;
	}
	return false;
}
bool CChuckMask::GetCenter(float& X, float& Y) {
	if (0 == m_lstHollow.GetCount()) return FALSE;
	float fTemp[2], Radius;
	if (bOffset) {
		X -= m_fXPitch / 2;
		Y -= m_fYPitch / 2;
	}

	POSITION pos = m_lstHollow.GetHeadPosition();
	while (pos) {
		CHollow* pHollow = m_lstHollow.GetNext(pos);
		if (pHollow) {
			int nData = pHollow->GetHole(NULL);
			if (nData) {
				if (pHollow->GetHole(fTemp)) {
					pHollow->GetRadius(&Radius);
					if ((fabs(X - fTemp[0]) < Radius) && (fabs(Y - fTemp[1]) < Radius)) {
						X = fTemp[0]; Y = fTemp[1];
						if (bOffset) {
							X += m_fXPitch / 2;
							Y += m_fYPitch / 2;
						}
						return true;
					}
				}
			}
		}
	}
	return false;
}

/***********************

	@@Vijay Following code is not used now but may be useful in situation
			when we use highly irregular chuck holes
			: Following code is incomplete

				  **************************************/

				  // Temporary data structures
				  // Bare Data for a hole
struct dfPoint {
	float X,
		Y,
		Radius,
		DeltaX; // XDistance to next point
};
typedef vector<dfPoint> _PtVec;
inline bool LT(const dfPoint& LHS, const dfPoint& RHS) {
	return LHS.X < RHS.X;
}
struct Line {
	_PtVec Holes;  // One Line
	bool IsNormalised;
	float YVal;     // Lines are sorted on this value
};
typedef map<float, Line > _Lines;
struct LineData {
	_Lines Lines;
	float DeltaY;
};
struct LogLines {
	LineData Map;
	float YMin, YMax;
}LogLines;

// End temporary data structures
bool CChuckMask::Compile() {
	CHollow* pHollow = NULL;
	CHole* pHole = NULL;
	float* pFloat = NULL, Float;

	dfPoint* pPoints = NULL;
	vector<dfPoint> CenterPoints;
	vector<float> Points;
	POSITION Pos = m_lstHollow.GetHeadPosition();
	while (Pos) {
		pHollow = m_lstHollow.GetNext(Pos);
		if (pHollow) {
			unsigned int nHole = pHollow->GetHole(NULL);
			if (nHole) {
				pFloat = new float[nHole * 2];
				pHollow->GetHole(pFloat);
				pHollow->GetRadius(&Float);
				for (unsigned int iFloat = 0; iFloat < nHole; iFloat++) {
					dfPoint Pt;
					Pt.X = pFloat[iFloat * 2];
					Pt.Y = pFloat[iFloat * 2 + 1];
					Pt.Radius = Float;
					CenterPoints.push_back(Pt);
				}
				delete[]pFloat;
			}
		}
	}

	_Lines::iterator mit;
	for (unsigned iPoint = 0; iPoint < CenterPoints.size(); iPoint++) {
		dfPoint TempPt = CenterPoints[iPoint];
		mit = LogLines.Map.Lines.find(TempPt.Y);
		if (LogLines.Map.Lines.end() == mit) {
			Line Temp;
			Temp.Holes.push_back(TempPt);
			LogLines.Map.Lines.insert(_Lines::value_type(TempPt.Y, Temp));
		}
		else {
			mit->second.Holes.push_back(TempPt);
		}
	}
	mit = LogLines.Map.Lines.begin();
	for (; mit != LogLines.Map.Lines.end(); ++mit) {
		Line& TmpCrd = (mit->second);
		for (unsigned int iVal = 0; iVal < TmpCrd.Holes.size(); iVal++) {
		}
	}
	mit = LogLines.Map.Lines.begin();
	for (; mit != LogLines.Map.Lines.end(); ++mit) {
		Line& TmpCrd = (mit->second);
		//		sort(mit->second.Holes.begin(),mit->second.Holes.end(),LT);
	}

	mit = LogLines.Map.Lines.begin();
	for (; mit != LogLines.Map.Lines.end(); ++mit) {
		Line& TmpCrd = (mit->second);
		for (unsigned int iVal = 0; iVal < TmpCrd.Holes.size(); iVal++) {
		}
	}
	return true;
}

unsigned int CChuckMask::Rows() {
	return LogLines.Map.Lines.size();
}

unsigned int CChuckMask::Cols() {
	unsigned int Cols = 0, TmpCols;
	_Lines::iterator mit;
	mit = LogLines.Map.Lines.begin();
	for (; mit != LogLines.Map.Lines.end(); ++mit) {
		TmpCols = (mit->second.Holes.size());
		if (TmpCols > Cols) {
			Cols = TmpCols;
		}
	}
	return Cols;
}

bool CChuckMask::AddHole(float XCoor, float YCoor, float Radius) {
	bool bRes = false;
	m_lstHollow.AddTail(new CHole(XCoor, YCoor, Radius));
	Compile();

	return bRes;
}

bool CChuckMask::RemoveHole(float XCoor, float YCoor) {
	bool bRes = false;
	CHollow* pHollow = NULL;
	POSITION Pos = m_lstHollow.GetHeadPosition();
	while (Pos) {
		pHollow = m_lstHollow.GetNext(Pos);
		if (pHollow) {
			unsigned int nHole = pHollow->GetHole(NULL);
			if (1 == nHole) {
				float* pFloat = new float[nHole * 2];
				pHollow->GetHole(pFloat);
				float DeltaX = fabs(pFloat[0] - XCoor);
				float DeltaY = fabs(pFloat[1] - YCoor);
				delete[]pFloat;
				if ((DeltaX < .1f) && (DeltaY < .1f)) {
					m_lstHollow.RemoveAt(Pos);
					delete pHollow;
					bRes = true;
					break;
				}
			}
		}
	}
	if (bRes) {
		Compile();
	}
	return bRes;
}

bool CChuckMask::GetRow(unsigned int Row, Points& Pts) {
	bool bRes = false;
	if (Row < LogLines.Map.Lines.size()) {
		_Lines::iterator lit = LogLines.Map.Lines.begin();
		for (UINT iRow = 0; iRow < Row; iRow++) {
			++lit;
		}
		Line& RefLine = lit->second;
		for (UINT iPt = 0; iPt < RefLine.Holes.size(); iPt++) {
			Point Pt;
			dfPoint& Ref = RefLine.Holes[iPt];
			Pt.X = Ref.X;
			Pt.Y = Ref.Y;
			Pts.push_back(Pt);
		}
		bRes = true;
	}
	return bRes;
}

bool CChuckMask::Cross(Points& Pts) {
	bool bRes = false;
	unsigned int nLines = LogLines.Map.Lines.size();
	if (nLines) {
		_Lines::iterator lit = LogLines.Map.Lines.begin();
		for (; lit != LogLines.Map.Lines.end(); ++lit) {
			float Temp = lit->first;
			if (fabs(lit->first - 0.0f) < .1) {
				for (unsigned int iCpt = 0; iCpt < lit->second.Holes.size(); iCpt++) {
					dfPoint& Pt = lit->second.Holes[iCpt];
					Point Cpt;
					Cpt.X = Pt.X;
					Cpt.Y = Pt.Y;
					Pts.push_back(Cpt);
				}
			}
			else {
				for (unsigned int iPt = 0; iPt < lit->second.Holes.size(); iPt++) {
					if (fabs(lit->second.Holes[iPt].X) < .1) {
						Point CPt;
						dfPoint& Pt = lit->second.Holes[iPt];
						CPt.X = Pt.X;
						CPt.Y = Pt.Y;
						break;
					}
				}
			}
		}
		bRes = true;
	}
	return bRes;
}

bool CChuckMask::Circumferance(Points& Pts) {
	bool bRes = false;
	unsigned int nLines = LogLines.Map.Lines.size();
	if (nLines) {
		_Lines::iterator lit = LogLines.Map.Lines.begin();
		for (; lit != LogLines.Map.Lines.end(); ++lit) {
			dfPoint& Pt1 = lit->second.Holes[0];
			Point Cpt;
			Cpt.X = Pt1.X;
			Cpt.Y = Pt1.Y;
			Pts.push_back(Cpt);

			dfPoint& Pt2 = lit->second.Holes[lit->second.Holes.size() - 1];
			Cpt.X = Pt2.X;
			Cpt.Y = Pt2.Y;
			Pts.push_back(Cpt);
		}
		bRes = true;
	}
	return bRes;
}
bool GetX(unsigned int XSkip, Points& Pts, _PtVec Holes);
/*
bool CChuckMask::Grid(unsigned int X,unsigned int Y,Points &Pts)
{
	bool bRes = false;
	unsigned int nLines = LogLines.Map.Lines.size();
	if(nLines)
	{
		if (!X) X =1;if(!Y)Y =1;
		_Lines::iterator lit = LogLines.Map.Lines.begin();
		for(; lit !=  LogLines.Map.Lines.end(); ++lit)
		{
			if(fabs(lit->first-0.f)< 0.1)
			{
				GetX(X,Pts,lit->second.Holes);
				_Lines::iterator fit ,rit ;
				int Temp;
				for(fit = lit;fit != LogLines.Map.Lines.end();)
				{
					GetX(X,Pts,fit->second.Holes);
					Temp = Y;
					while(1 )
					{
						Temp--;
						++fit;
						if(fit == LogLines.Map.Lines.end())
						{
							break;
						}
						if(!Temp)
						{
							break;
						}
					};
				}
				rit = lit;
				Temp = Y;
				while(1 )
				{
					Temp--;
					--rit;
					if(rit == LogLines.Map.Lines.begin())
					{
						break;
					}
					if(!Temp)
					{
						break;
					}
				};

				for( ;rit != LogLines.Map.Lines.begin();rit--)
				{
					GetX(X,Pts,rit->second.Holes);
					Temp = Y;
					while(1 )
					{
						Temp--;
						--rit;
						if(rit == LogLines.Map.Lines.begin())
						{
							break;
						}
						if(!Temp)
						{
							break;
						}
					};
				}
				bRes = true;
				break;
			}
		}

		bRes = true;
	}
	return bRes;
}*/

bool CChuckMask::Grid(unsigned int X, unsigned int Y, Points& Pts) {
	bool bRes = false;
	unsigned int nLines = LogLines.Map.Lines.size();
	// Has to be Odd number
	int SectX = (X - 1) / 2;
	int SectY = (Y - 1) / 2;

	if (nLines) {
		_Lines::iterator lit = LogLines.Map.Lines.begin();
		for (; lit != LogLines.Map.Lines.end(); ++lit) {
			if (fabs(lit->first - 0.f) < 0.1) {
				// Go in positive direction
				GetX(X, Pts, lit->second.Holes);

				_Lines::iterator fit, rit;
				int Temp;
				for (fit = lit; fit != LogLines.Map.Lines.end();) {
					GetX(X, Pts, fit->second.Holes);
					Temp = Y;
					while (1) {
						Temp--;
						++fit;
						if (fit == LogLines.Map.Lines.end()) {
							break;
						}
						if (!Temp) {
							break;
						}
					};
				}
				rit = lit;
				Temp = Y;
				while (1) {
					Temp--;
					--rit;
					if (rit == LogLines.Map.Lines.begin()) {
						break;
					}
					if (!Temp) {
						break;
					}
				};

				for (; rit != LogLines.Map.Lines.begin(); rit--) {
					GetX(X, Pts, rit->second.Holes);
					Temp = Y;
					while (1) {
						Temp--;
						--rit;
						if (rit == LogLines.Map.Lines.begin()) {
							break;
						}
						if (!Temp) {
							break;
						}
					};
				}
				bRes = true;
				break;
			}
		}

		bRes = true;
	}
	return bRes;
}

bool GetX(unsigned int XSkip, Points& Pts, _PtVec Holes) {
	bool bRes = false;
# if 0
	for (unsigned int iHole = 0; iHole < Holes.size(); iHole++) {
		if (fabs(Holes[iHole].X - 0.f) < .1) {
			// Get the center point
			Point Pt;
			Pt.X = Holes[iHole].X;
			Pt.Y = Holes[iHole].Y;
			Points.push_back(Pt);

			int Fwd = iHole, Rev = iHole;
			for (int iTemp = iHole; (iTemp < Holes.size()) || (iTemp < XSkip); iTemp++) {
				Point Pt; dfPoint Dpt = Holes[iTemp];
				Pt.X = Dpt.X;
				Pt.Y = Dpt.Y;
				Pts.push_back(Pt);
			}
			for (iTemp = iHole; (iTemp > 0); iTemp--) {
				Point Pt; dfPoint Dpt = Holes[iTemp];
				Pt.X = Dpt.X;
				Pt.Y = Dpt.Y;
				Pts.push_back(Pt);
			}
			bRes = true;
			break;
		}
	}
#endif
	return bRes;
}

void Dump(Points& Pts) {
	for (int iPt = 0; iPt < int(Pts.size()); iPt++) {
	}
}