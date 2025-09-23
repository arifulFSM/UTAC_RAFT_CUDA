#include "pch.h"
#include "InPEdit.h"
#include "EdListCtrl.h"

IMPLEMENT_DYNAMIC(CEdListCtrl, CListCtrl)

int CEdListCtrl::GetRowFromPoint(CPoint& point, int* col) const {
	int column = 0;
	int row = HitTest(point, NULL);

	if (col) *col = 0;

	// Make sure that the ListView is in LVS_REPORT
	if ((GetWindowLong(m_hWnd, GWL_STYLE) & LVS_TYPEMASK) != LVS_REPORT) {
		return row;
	}

	// Get the top and bottom row visible
	row = GetTopIndex();
	int bottom = row + GetCountPerPage();

	if (bottom > GetItemCount()) {
		bottom = GetItemCount();
	}

	// Get the number of columns
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();

	// Loop through the visible rows
	for (; row <= bottom; row++) {
		// Get bounding rectangle of item and check whether point falls in it.
		CRect rect;
		GetItemRect(row, &rect, LVIR_BOUNDS);

		if (rect.PtInRect(point)) {
			// Find the column
			for (column = 0; column < nColumnCount; column++) {
				int colwidth = GetColumnWidth(column);

				if (point.x >= rect.left && point.x <= (rect.left + colwidth)) {
					if (col) *col = column;
					return row;
				}
				rect.left += colwidth;
			}
		}
	}
	return -1;
}

CEdit* CEdListCtrl::EditSubLabel(int nItem, int nCol) {
	// The returned pointer should not be saved, make sure item visible
	if (!EnsureVisible(nItem, TRUE)) return NULL;

	// Make sure that column number is valid
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();
	if (nCol >= nColumnCount || GetColumnWidth(nCol) < 5) return NULL;

	// Get the column offset
	int offset = 0;
	for (int i = 0; i < nCol; i++) {
		offset += GetColumnWidth(i);
	}

	CRect rect;
	GetItemRect(nItem, &rect, LVIR_BOUNDS);

	// Scroll horizontally if we need to expose the column
	CRect rcClient;
	GetClientRect(&rcClient);

	if (offset + rect.left < 0 || offset + rect.left > rcClient.right) {
		CSize size;
		size.cx = offset + rect.left;
		size.cy = 0;
		Scroll(size);
		rect.left -= size.cx;
	}

	// Get Column alignment
	LV_COLUMN lvcol;
	lvcol.mask = LVCF_FMT;
	GetColumn(nCol, &lvcol);
	DWORD dwStyle;

	if ((lvcol.fmt & LVCFMT_JUSTIFYMASK) == LVCFMT_LEFT) {
		dwStyle = ES_LEFT;
	}
	else if ((lvcol.fmt & LVCFMT_JUSTIFYMASK) == LVCFMT_RIGHT) {
		dwStyle = ES_RIGHT;
	}
	else {
		dwStyle = ES_CENTER;
	}

	rect.left += offset + 4;
	rect.right = rect.left + GetColumnWidth(nCol) - 3;

	if (rect.right > rcClient.right) {
		rect.right = rcClient.right;
	}

	dwStyle |= WS_BORDER | WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL;

	CEdit* pEdit = new CInPEdit(nItem, nCol, GetItemText(nItem, nCol));
	pEdit->Create(dwStyle, rect, this, rcID);

	return pEdit;
}

WCHAR* CEdListCtrl::VToS(int value) {
	strValue.Format(_T("%d"), value);
	return strValue.GetBuffer();
}

WCHAR* CEdListCtrl::VToS(float value, int deci) {
	CString xfmt; xfmt.Format(_T("%%.%df"), deci);
	strValue.Format(xfmt, value);
	return strValue.GetBuffer();
}

int CEdListCtrl::InsertHeader(int col, CString name, int cx, BOOL bEdit, int fmt /*= LVCFMT_LEFT*/) {
	// helper [2/1/2021 yuenl]
	vVis.push_back(bEdit);
	LVCOLUMN lvColumn;
	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = fmt;
	lvColumn.cx = cx;
	lvColumn.pszText = name.GetBuffer();
	return InsertColumn(col, &lvColumn);
}

int CEdListCtrl::InsertRow(int n, CString name) {
	// helper [2/1/2021 yuenl]
	return InsertItem(LVIF_TEXT | LVIF_STATE, n, name, 0, LVIS_SELECTED, 0, 0);
}

void CEdListCtrl::ClearAll() {
	if (vVis.size() > 0) vVis.clear();
}

CEdListCtrl::CEdListCtrl() {}

void CEdListCtrl::Initialize(int ID) {
	rcID = ID;
	ClearAll();
	SetExtendedStyle(GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EDITLABELS);
}

CEdListCtrl::~CEdListCtrl() {}

BEGIN_MESSAGE_MAP(CEdListCtrl, CListCtrl)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

void CEdListCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) {
	// TODO: Add your message handler code here and/or call default

	CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CEdListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) {
	// TODO: Add your message handler code here and/or call default

	CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CEdListCtrl::OnLButtonDown(UINT nFlags, CPoint point) {
	int index;
	CListCtrl::OnLButtonDown(nFlags, point);

	ModifyStyle(0, LVS_EDITLABELS);

	int colnum;

	if ((index = GetRowFromPoint(point, &colnum)) != -1) {
		UINT flag = LVIS_FOCUSED;

		if ((GetItemState(index, flag) & flag) == flag /*&& colnum == 2*/) {
			// Add check for LVS_EDITLABELS
			if (GetWindowLong(m_hWnd, GWL_STYLE) & LVS_EDITLABELS) {
				if (vVis[colnum]) EditSubLabel(index, colnum);
			}
		}
		else {
			SetItemState(index, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
		}
	}
}