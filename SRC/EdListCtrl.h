#pragma once

#include <vector>
#include "ReportCtrl.h"

class CEdListCtrl : public CListCtrl {
	DECLARE_DYNAMIC(CEdListCtrl)

public:
	int rcID = 0;
	std::vector<BOOL> vVis;
	int GetRowFromPoint(CPoint& point, int* col) const;
	CEdit* EditSubLabel(int nItem, int nCol);

	CString strValue = _T("");
	WCHAR* VToS(int value);
	WCHAR* VToS(float value, int deci);

public:
	void ClearAll();
	void Initialize(int ID);
	int InsertRow(int n, CString name);
	int InsertHeader(int col, CString name, int cx, BOOL bEdit, int fmt = LVCFMT_LEFT);

public:
	CEdListCtrl();
	virtual ~CEdListCtrl();

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};
