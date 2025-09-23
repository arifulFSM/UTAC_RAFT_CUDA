#include "pch.h"
#include "XTabCtrl.h"

CXTabCtrl::CXTabCtrl() {
	m_ptTabs.x = 4;
	m_ptTabs.y = 28;
}

BOOL CXTabCtrl::IsTabEnabled(int iIndex) {
	ASSERT(iIndex < m_arrayStatusTab.GetSize());
	return m_arrayStatusTab[iIndex];
}

BOOL CXTabCtrl::ReselectTab(int iIndex) {
	if (bUIBlocked) return FALSE;
	ASSERT(iIndex < m_arrayStatusTab.GetSize());
	//** check if the tab is enabled --------
	if (m_arrayStatusTab[iIndex]) {
		TCITEM item;
		CWnd* pWnd;
		item.mask = TCIF_PARAM;

		if (GetItem(GetCurSel(), &item)) {
			pWnd = reinterpret_cast<CWnd*> (item.lParam);
			ASSERT_VALID(pWnd);
			pWnd->ShowWindow(SW_HIDE);

			SetCurSel(iIndex);
			if (GetItem(iIndex, &item)) {
				pWnd = reinterpret_cast<CWnd*> (item.lParam);
				ASSERT_VALID(pWnd);
				pWnd->ShowWindow(SW_SHOW);
				pWnd->BringWindowToTop();
				PostXTabSelected(pWnd, iIndex);
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL CXTabCtrl::SelectNextTab(BOOL bForward) {
	if (bUIBlocked) return FALSE;
	int iSum = bForward ? 1 : -1;
	int iCurSel = GetCurSel();
	int iCurSelSave(iCurSel);

	do {
		iCurSel += iSum;
		if (iCurSel < 0 || iCurSel == GetItemCount()) return FALSE;
		if (IsTabEnabled(iCurSel)) {
			if (iCurSel < GetItemCount()) {
				SelectTab(iCurSel);
				return TRUE;
			}
			return FALSE;
		}
	} while (iCurSel != iCurSelSave);
	return FALSE;
}

BOOL CXTabCtrl::SelectTab(int iIndex) {
	ASSERT(iIndex < m_arrayStatusTab.GetSize());
	if (GetCurSel() == iIndex) return TRUE;
	return ReselectTab(iIndex);
}

BOOL CXTabCtrl::SelectTabByName(LPCTSTR lpszTabCaption) {
	if (bUIBlocked) return FALSE;

	TCHAR szName[128];
	TCITEM item;

	memset(&item, 0, sizeof(item));

	item.mask = TCIF_TEXT;
	item.cchTextMax = sizeof(szName);
	item.pszText = szName;

	for (int i = 0; i < GetItemCount(); i++) {
		if (GetItem(i, &item)) {
#ifdef UNICODE
			if (wcscmp(lpszTabCaption, item.pszText) == 0) return SelectTab(i);
#else
			if (strcmp(lpszTabCaption, item.pszText) == 0) return SelectTab(i);
#endif
		}
	}
	return FALSE;
}

int CXTabCtrl::FindTabIndex(CWnd* pWnd) {
	CWnd* p;
	TCITEM item;
	for (int i = 0; i < GetItemCount(); i++) {
		if (GetItem(i, &item)) {
			p = reinterpret_cast<CWnd*> (item.lParam);
			if (p == pWnd) return i;
		}
	}
	return -1;
}

short CXTabCtrl::GetTabIndexByName(LPCTSTR lpszTabCaption) {
	TCHAR szName[128];
	TCITEM item;

	memset(&item, 0, sizeof(item));

	item.mask = TCIF_TEXT;
	item.cchTextMax = sizeof(szName);
	item.pszText = szName;

	for (int i = 0; i < GetItemCount(); i++) {
		if (GetItem(i, &item)) {
#ifdef UNICODE
			if (wcscmp(lpszTabCaption, item.pszText) == 0) return i;
#else
			if (strcmp(lpszTabCaption, item.pszText) == 0) return i;
#endif
		}
	}
	return -1;
}

CXTabCtrl::~CXTabCtrl() {
	m_arrayStatusTab.RemoveAll();
}

void CXTabCtrl::ActivateCurrentTab() {
	int i;
	TCITEM item;
	CWnd* pWnd;
	item.mask = TCIF_PARAM;

	curTab = i = GetCurSel();
	if (!GetItem(curTab, &item)) return;
	pWnd = reinterpret_cast<CWnd*> (item.lParam);
	if (pWnd) {
		ASSERT_VALID(pWnd);
		pWnd->ShowWindow(SW_SHOW);
		pWnd->BringWindowToTop();

		CWnd* pWnd1;
		for (int j = 0; j < m_arrayStatusTab.GetSize(); j++) {
			if (j != curTab) {
				if (!GetItem(j, &item)) continue;
				pWnd1 = reinterpret_cast<CWnd*> (item.lParam);
				if (pWnd1) {
					ASSERT_VALID(pWnd1);
					PostXTabDeselected(pWnd1, j);
				}
			}
		}
		PostXTabSelected(pWnd, i);
	}
}

BOOL CXTabCtrl::SelectTabForce(int iIndex) {
	ASSERT(iIndex < m_arrayStatusTab.GetSize());
	//** check if the tab is enabled --------
	if (m_arrayStatusTab[iIndex]) {
		TCITEM item;
		CWnd* pWnd;
		item.mask = TCIF_PARAM;

		if (GetItem(GetCurSel(), &item)) {
			pWnd = reinterpret_cast<CWnd*> (item.lParam);
			ASSERT_VALID(pWnd);
			pWnd->ShowWindow(SW_HIDE);

			SetCurSel(iIndex);
			if (GetItem(iIndex, &item)) {
				pWnd = reinterpret_cast<CWnd*> (item.lParam);
				ASSERT_VALID(pWnd);
				pWnd->ShowWindow(SW_SHOW);
				pWnd->BringWindowToTop();
				PostXTabSelected(pWnd, iIndex);
				return TRUE;
			}
		}
	}
	return FALSE;
}

CWnd* CXTabCtrl::GetSelectTab(int iIndex) {
	ASSERT(iIndex < m_arrayStatusTab.GetSize());
	//** check if the tab is enabled --------
	if (m_arrayStatusTab[iIndex]) {
		TCITEM item;
		CWnd* pWnd;
		item.mask = TCIF_PARAM;

		GetItem(GetCurSel(), &item);
		pWnd = reinterpret_cast<CWnd*> (item.lParam);
		if (pWnd) {
			ASSERT_VALID(pWnd);
			pWnd->ShowWindow(SW_HIDE);
		}

		SetCurSel(iIndex);
		GetItem(iIndex, &item);
		pWnd = reinterpret_cast<CWnd*> (item.lParam);
		if (pWnd) {
			ASSERT_VALID(pWnd);
			pWnd->ShowWindow(SW_SHOW);
		}
		return pWnd;
	}
	return NULL;
}

void CXTabCtrl::AddTab(CWnd* pWnd, LPTSTR lpszCaption, int iImage) {
	AddTab(pWnd, lpszCaption, iImage, 0, 0);
}

short CXTabCtrl::AddSelectTab(CWnd* pWnd, LPTSTR lpszCaption, int iImage) {
	short idx = AddTab(pWnd, lpszCaption, iImage, 0, 0);
	SelectTab(idx);
	return idx;
}

short CXTabCtrl::AddTab(CWnd* pWnd, LPTSTR lpszCaption, int iImage, int cx, int cy) {
	ASSERT_VALID(pWnd);

	TCITEM item;
	item.mask = TCIF_TEXT | TCIF_PARAM | TCIF_IMAGE;
	item.lParam = (LPARAM)pWnd;
	item.pszText = lpszCaption;
	item.iImage = iImage;

	int iIndex = int(m_arrayStatusTab.GetSize());
	InsertItem(iIndex, &item);

	if ((cx == 0) && (cy == 0)) {
		pWnd->SetWindowPos(NULL, m_ptTabs.x, m_ptTabs.y, cx, cy,
			SWP_FRAMECHANGED | SWP_NOSIZE | SWP_NOZORDER);
	}
	else {
		cx -= 2 * m_ptTabs.x;
		if (cx < 10) cx = 10;
		cy -= (m_ptTabs.y + 8);
		if (cy < 10) cy = 10;
		pWnd->SetWindowPos(NULL, m_ptTabs.x, m_ptTabs.y, cx, cy, SWP_FRAMECHANGED | SWP_NOZORDER);
	}

	pWnd->ShowWindow(iIndex ? SW_HIDE : SW_SHOW);
	if (iIndex > 0) pWnd->BringWindowToTop();
	//** the initial status is enabled
	m_arrayStatusTab.Add(TRUE);
	return iIndex;
}

void CXTabCtrl::ChangeCaption(CString name) {
	int curTab = GetCurSel();

	ChangeCaption(curTab, name);
}

void CXTabCtrl::ChangeCaption(int idx, CString name) {
	TCITEM item;
	item.mask = TCIF_PARAM;

	//	SelectTab(curTab);
	if (GetItem(idx, &item)) {
		item.mask = TCIF_PARAM | TCIF_TEXT;
		item.pszText = name.GetBuffer();
		//** update item
		VERIFY(SetItem(idx, &item));
	}
}

CWnd* CXTabCtrl::ChangeTab(int iIndex, CWnd* pNewTab, LPTSTR lpszCaption, int iImage) {
	return ChangeTab(iIndex, pNewTab, lpszCaption, iImage, 0, 0);
}

CWnd* CXTabCtrl::ChangeTab(int iIndex, CWnd* pNewTab, LPTSTR lpszCaption, int iImage, int cx, int cy) {
	if (!pNewTab) { ASSERT(0); return NULL; }

	TCITEM itemO;
	CWnd* pOldTab = 0;
	// LYF  [4/23/2006]
	if (GetItem(iIndex, &itemO)) {
		if (itemO.lParam) {
			pOldTab = ((CWnd*)itemO.lParam);
			if (pOldTab) {
				pOldTab->ShowWindow(SW_HIDE); //  [12/24/2007 YUEN]
				pOldTab->DestroyWindow();
			}
		}
	}

	//** update item
	TCITEM item;
	item.mask = TCIF_TEXT | TCIF_PARAM | TCIF_IMAGE;
	item.lParam = (LPARAM)pNewTab;
	item.pszText = lpszCaption;
	item.iImage = iImage;
	VERIFY(SetItem(iIndex, &item));
	pNewTab->ShowWindow(SW_HIDE);

	if ((cx == 0) && (cy == 0)) {
		pNewTab->SetWindowPos(NULL, m_ptTabs.x, m_ptTabs.y, 0, 0, SWP_FRAMECHANGED | SWP_NOSIZE | SWP_NOZORDER);
	}
	else {
		cx -= 2 * m_ptTabs.x; if (cx < 10) cx = 10;
		cy -= (m_ptTabs.y + 8); if (cy < 10) cy = 10;
		pNewTab->SetWindowPos(NULL, m_ptTabs.x, m_ptTabs.y, cx, cy, SWP_FRAMECHANGED | SWP_NOZORDER);
	}
	//** the initial status is enabled
	m_arrayStatusTab[iIndex] = TRUE;
	//** updates the screen
	ReselectTab(GetCurSel());
	return pOldTab;
}

BEGIN_MESSAGE_MAP(CXTabCtrl, CTabCtrl)
	ON_NOTIFY_REFLECT(TCN_SELCHANGE, OnSelchange)
	ON_NOTIFY_REFLECT(TCN_SELCHANGING, OnSelchanging)
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_SIZE()
END_MESSAGE_MAP()

void CXTabCtrl::PreSubclassWindow() {
	CTabCtrl::PreSubclassWindow();
	ModifyStyle(0, TCS_OWNERDRAWFIXED);
}

void CXTabCtrl::OnSelchange(NMHDR* pNMHDR, LRESULT* pResult) {
	if (bUIBlocked) return;

	int iNewTab = GetCurSel();

	if (!IsTabEnabled(iNewTab)) {
		SetCurSel(m_iSelectedTab);
	}
	else {
		TCITEM item;
		CWnd* pWnd;

		item.mask = TCIF_PARAM;

		//** hide the current tab ---------
		if (!GetItem(m_iSelectedTab, &item)) return;
		pWnd = reinterpret_cast<CWnd*> (item.lParam);
		if (!pWnd) return;
		HWND p = pWnd->GetSafeHwnd(); if (!p) return;

		ASSERT_VALID(pWnd);
		pWnd->ShowWindow(SW_HIDE);
		PostXTabDeselected(pWnd, m_iSelectedTab);

		//** show the selected tab --------
		if (GetItem(iNewTab, &item)) {
			pWnd = reinterpret_cast<CWnd*> (item.lParam);
			ASSERT_VALID(pWnd);
			pWnd->ShowWindow(SW_SHOW);
			pWnd->BringWindowToTop();
			CWnd* pWnd1;
			for (int j = 0; j < m_arrayStatusTab.GetSize(); j++) {
				if (j != iNewTab) {
					if (GetItem(j, &item)) {
						pWnd1 = reinterpret_cast<CWnd*> (item.lParam);
						if (pWnd1) {
							ASSERT_VALID(pWnd1);
							PostXTabDeselected(pWnd1, j);
						}
					}
				}
			}
			curTab = iNewTab;
			PostXTabSelected(pWnd, iNewTab);
		}
	}
	*pResult = 0;
}

void CXTabCtrl::OnSelchanging(NMHDR* pNMHDR, LRESULT* pResult) {
	m_iSelectedTab = GetCurSel();
	*pResult = 0;
}

void CXTabCtrl::OnMouseMove(UINT nFlags, CPoint point) {
	//** if we should change the color of the tab ctrl ---
	if (m_bColorMouseOver) {
		SetTimer(1, 10, NULL);

		if (m_iIndexMouseOver != -1) {
			CRect rectItem;
			GetItemRect(m_iIndexMouseOver, rectItem);
			if (!rectItem.PtInRect(point)) {
				CRect rectOldItem;
				GetItemRect(m_iIndexMouseOver, rectOldItem);
				m_iIndexMouseOver = -1;
				InvalidateRect(rectOldItem);
				return;
			}
		}

		if (!m_bMouseOver) {
			TCHITTESTINFO hitTest;

			m_bMouseOver = true;
			hitTest.pt = point;

			int iItem = HitTest(&hitTest);
			if (iItem != -1 && m_arrayStatusTab[iItem]) {
				RECT rectItem;
				GetItemRect(iItem, &rectItem);
				InvalidateRect(&rectItem);
			}
		}
	}
	CTabCtrl::OnMouseMove(nFlags, point);
}

void CXTabCtrl::OnTimer(UINT_PTR nIDEvent) {
	POINT pt;
	GetCursorPos(&pt);
	CRect rectItem, rectScreen;

	GetItemRect(m_iIndexMouseOver, rectItem);
	rectScreen = rectItem;
	ClientToScreen(rectScreen);

	// If mouse leaves, show normal
	if (!rectScreen.PtInRect(pt)) {
		KillTimer(1);
		m_bMouseOver = false;
		m_iIndexMouseOver = -1;
		InvalidateRect(rectItem);
	}

	CTabCtrl::OnTimer(nIDEvent);
}

void CXTabCtrl::OnSize(UINT nType, int cx, int cy) {
	CTabCtrl::OnSize(nType, cx, cy);

	cx -= 2 * m_ptTabs.x;
	if (cx < 10) cx = 10;
	cy -= (m_ptTabs.y + 8);
	if (cy < 10) cy = 10;

	int count = GetItemCount();
	TCITEM item;
	item.mask = TCIF_PARAM;
	for (int i = 0; i < count; i++) {
		if (GetItem(i, &item)) {
			CWnd* pWnd = reinterpret_cast<CWnd*> (item.lParam);
			if (pWnd)
				pWnd->SetWindowPos(NULL, m_ptTabs.x, m_ptTabs.y, cx, cy, SWP_FRAMECHANGED | SWP_NOZORDER);
		}
	}
}

void CXTabCtrl::DeactivateCurrentTab() {
	if (curTab == -1) return;

	TCITEM item;
	CWnd* pWnd;
	item.mask = TCIF_PARAM;

	if (!GetItem(curTab, &item)) return;
	pWnd = reinterpret_cast<CWnd*> (item.lParam);
	if (pWnd) {
		ASSERT_VALID(pWnd);
		PostXTabDeselected(pWnd, curTab);
	}
}

void CXTabCtrl::DeleteAllTabs() {
	m_arrayStatusTab.RemoveAll();
	DeleteAllItems();
}

void CXTabCtrl::DeleteTab(int iIndex) {
	ASSERT(iIndex < m_arrayStatusTab.GetSize());
	SelectTab(iIndex - 1);
	CWnd* pWnd;
	TCITEM item;
	item.mask = TCIF_PARAM;
	if (GetItem(iIndex, &item)) {
		pWnd = reinterpret_cast<CWnd*> (item.lParam);
		ASSERT_VALID(pWnd);
		pWnd->ShowWindow(SW_HIDE);
	}
	DeleteItem(iIndex);
	m_arrayStatusTab.RemoveAt(iIndex);
}

void CXTabCtrl::EnableAllTabs(BOOL bEnable) {
	for (int iIndex = 0; iIndex < m_arrayStatusTab.GetSize(); iIndex++) {
		//** if it should change the status ----
		if (m_arrayStatusTab[iIndex] != bEnable) {
			m_arrayStatusTab[iIndex] = bEnable;
			//** redraw the item -------
			CRect rect;
			GetItemRect(iIndex, &rect);
			InvalidateRect(rect);
		}
	}
}

void CXTabCtrl::EnableTab(int iIndex, BOOL bEnable) {
	ASSERT(iIndex < m_arrayStatusTab.GetSize());

	//** if it should change the status ----
	if (m_arrayStatusTab[iIndex] != bEnable) {
		m_arrayStatusTab[iIndex] = bEnable;
		//** redraw the item -------
		CRect rect;
		GetItemRect(iIndex, &rect);
		InvalidateRect(rect);
	}
}

void CXTabCtrl::PostXTabDeselected(CWnd* pWnd, int iTab) {
	TCITEM item;
	static TCHAR szName[128];
	memset(&item, 0, sizeof(item));
	item.mask = TCIF_TEXT;
	item.cchTextMax = sizeof(szName);
	item.pszText = szName;

	memset(szName, 0, 128);
	if (GetItem(iTab, &item)) {
#ifdef UNICODE
		wcscpy(szName, item.pszText);
#else
		strcpy(szName, item.pszText);
#endif
	}
	pWnd->PostMessage(WM_XTAB_DESELECTED, 0, (LPARAM)szName);
}

void CXTabCtrl::PostXTabSelected(CWnd* pWnd, int iTab) {
	TCITEM item;
	static TCHAR szName[128];
	memset(&item, 0, sizeof(item));
	item.mask = TCIF_TEXT;
	item.cchTextMax = sizeof(szName);
	item.pszText = szName;
	memset(szName, 0, 128);
	if (GetItem(iTab, &item)) {
#ifdef UNICODE
		wcscpy(szName, item.pszText);
#else
		strcpy(szName, item.pszText);
#endif
	}
	pWnd->PostMessage(WM_XTAB_SELECTED, 0, (LPARAM)szName);
}

void CXTabCtrl::SetDisabledColor(COLORREF cr) {
	m_bColorDisabled = true;
	m_crDisabled = cr;
}

void CXTabCtrl::SetMouseOverColor(COLORREF cr) {
	m_bColorMouseOver = true;
	m_crMouseOver = cr;
}

void CXTabCtrl::SetNormalColor(COLORREF cr) {
	m_bColorNormal = true;
	m_crNormal = cr;
}

void CXTabCtrl::SetSelectedColor(COLORREF cr) {
	m_bColorSelected = true;
	m_crSelected = cr;
}

void CXTabCtrl::SetTopLeftCorner(CPoint pt) {
	m_ptTabs.x = pt.x;
	m_ptTabs.y = pt.y;
}

void CXTabCtrl::SetUIState(BOOL bSt) {
	bUIBlocked = bSt;
	EnableAllTabs(!bUIBlocked);
}

void CXTabCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) {
	CRect rect = lpDrawItemStruct->rcItem;
	rect.top += ::GetSystemMetrics(SM_CYEDGE);

	int nTabIndex = lpDrawItemStruct->itemID;
	if (nTabIndex < 0) return;

	BOOL bSelected = (nTabIndex == GetCurSel());

	COLORREF crSelected = m_bColorSelected ? m_crSelected : GetSysColor(COLOR_BTNTEXT);
	COLORREF crNormal = m_bColorNormal ? m_crNormal : GetSysColor(COLOR_BTNTEXT);
	COLORREF crDisabled = m_bColorDisabled ? m_crDisabled : GetSysColor(COLOR_GRAYTEXT);

	TCHAR label[64];
	label[0] = 0;
	TC_ITEM item;
	item.mask = TCIF_TEXT | TCIF_IMAGE;
	item.pszText = label;
	item.cchTextMax = 63;
	if (!GetItem(nTabIndex, &item)) return;

	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC); if (!pDC) return;

	int nSavedDC = pDC->SaveDC();

	CRect rectItem;
	POINT pt;

	GetItemRect(nTabIndex, &rectItem);
	GetCursorPos(&pt);
	ScreenToClient(&pt);

	if (rectItem.PtInRect(pt)) m_iIndexMouseOver = nTabIndex;

	pDC->SetBkMode(TRANSPARENT);
	pDC->FillSolidRect(rect, ::GetSysColor(COLOR_BTNFACE));

	//**  Draw the image
	CImageList* pImageList = GetImageList();
	if (pImageList && item.iImage >= 0) {
		rect.left += pDC->GetTextExtent(_T(" ")).cx;

		IMAGEINFO info;
		pImageList->GetImageInfo(item.iImage, &info);
		CRect ImageRect(info.rcImage);
		int nYpos = rect.top;

		pImageList->Draw(pDC, item.iImage, CPoint(rect.left, nYpos), ILD_TRANSPARENT);
		rect.left += ImageRect.Width();
	}

	if (!IsTabEnabled(nTabIndex)) {
		pDC->SetTextColor(crDisabled);
		rect.top -= ::GetSystemMetrics(SM_CYEDGE);
#ifdef UNICODE
		if (wcslen(label) > 0) {
#else
		if (strlen(label) > 0) {
#endif
			pDC->DrawText(label, rect, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
		}
		}
	else {
		//** selected item -----
		if (bSelected) pDC->SetTextColor(crSelected);
		else //** other item ---
		{
			if (m_bColorMouseOver && nTabIndex == m_iIndexMouseOver)
				pDC->SetTextColor(m_crMouseOver);
			else
				pDC->SetTextColor(crNormal);
		}

		rect.top -= ::GetSystemMetrics(SM_CYEDGE);
#ifdef UNICODE
		if (wcslen(label) > 0) {
#else
		if (strlen(label) > 0) {
#endif
			pDC->DrawText(label, rect, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
		}
		}
	pDC->RestoreDC(nSavedDC);
	}