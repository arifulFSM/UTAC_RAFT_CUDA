#pragma once

#include <afxtempl.h>
#include "XTabDefine.h"

class CXTabCtrl : public CTabCtrl {
public:
	CXTabCtrl();
	virtual ~CXTabCtrl();

public:
	short curTab = -1;
	BOOL bUIBlocked = FALSE;

public:
	BOOL IsTabEnabled(int iIndex);
	BOOL ReselectTab(int iIndex);
	BOOL SelectNextTab(BOOL bForward);
	BOOL SelectTab(int iIndex);
	BOOL SelectTabByName(LPCTSTR lpszTabCaption);
	BOOL SelectTabForce(int iIndex);
	CWnd* GetSelectTab(int iIndex);
	int FindTabIndex(CWnd* pWnd);
	short GetTabIndexByName(LPCTSTR lpszTabCaption);
	void ActivateCurrentTab();
	void AddTab(CWnd* pWnd, LPTSTR lpszCaption, int iImage = 0);
	short AddSelectTab(CWnd* pWnd, LPTSTR lpszCaption, int iImage);
	short AddTab(CWnd* pWnd, LPTSTR lpszCaption, int iImage, int cx, int cy);
	void ChangeCaption(CString name/*, int curTab*/);
	void ChangeCaption(int idx, CString name);
	CWnd* ChangeTab(int iIndex, CWnd* pNewTab, LPTSTR lpszCaption, int iImage);
	CWnd* ChangeTab(int iIndex, CWnd* pNewTab, LPTSTR lpszCaption, int iImage, int cx, int cy);
	void DeactivateCurrentTab();
	void DeleteAllTabs();
	void DeleteTab(int iIndex);
	void EnableAllTabs(BOOL bEnable = TRUE);
	void EnableTab(int iIndex, BOOL bEnable = TRUE);
	void PostXTabDeselected(CWnd* pWnd, int iTab);
	void PostXTabSelected(CWnd* pWnd, int iTab);
	void SetDisabledColor(COLORREF cr);
	void SetMouseOverColor(COLORREF cr);
	void SetNormalColor(COLORREF cr);
	void SetSelectedColor(COLORREF cr);
	void SetTopLeftCorner(CPoint pt);
	void SetUIState(BOOL bSt);

protected:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void PreSubclassWindow();

protected:
	CArray<BOOL, BOOL> m_arrayStatusTab; //** enabled Y\N
	POINT m_ptTabs;
	COLORREF m_crSelected = 0;
	COLORREF m_crDisabled = 0;
	COLORREF m_crNormal = 0;
	COLORREF m_crMouseOver = 0;

	int m_iSelectedTab = 0;
	int m_iIndexMouseOver = -1;

	bool m_bMouseOver = false;
	bool m_bColorMouseOver = false;
	bool m_bColorNormal = false;
	bool m_bColorDisabled = false;
	bool m_bColorSelected = false;

public:
	afx_msg void OnSelchange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchanging(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
};
