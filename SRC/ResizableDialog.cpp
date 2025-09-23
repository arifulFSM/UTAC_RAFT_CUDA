// ResizableDialog.cpp : implementation file
//
/////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2000-2001 by Paolo Messina
// (http://www.geocities.com/ppescher - ppescher@yahoo.com)
//
// The contents of this file are subject to the Artistic License (the "License").
// You may not use this file except in compliance with the License.
// You may obtain a copy of the License at:
// http://www.opensource.org/licenses/artistic-license.html
//
// If you find this code useful, credits would be nice!
//
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "ResizableDialog.h"

inline void CResizableDialog::Construct() {
	bTabSelected = FALSE;
	m_bEnableSaveRestore = FALSE;
	m_bInitDone = FALSE;
}

CResizableDialog::CResizableDialog() {
	Construct();
}

CResizableDialog::CResizableDialog(UINT nIDTemplate, CWnd* pParentWnd)
	: CDialogEx(nIDTemplate, pParentWnd) {
	Construct();
}

CResizableDialog::CResizableDialog(LPCTSTR lpszTemplateName, CWnd* pParentWnd)
	: CDialogEx(lpszTemplateName, pParentWnd) {
	Construct();
}

void CResizableDialog::SetID(short idx, HWND hPW, HWND hWMP) {
	TabIdx = idx;
	hPWnd = hPW; hWWnd = hWMP;
}

short CResizableDialog::GetID() { return TabIdx; }

HWND CResizableDialog::GetPWnd() { return hPWnd; }

CResizableDialog::~CResizableDialog() {}

BEGIN_MESSAGE_MAP(CResizableDialog, CDialogEx)
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

int CResizableDialog::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// keep client area
	CRect rect;
	GetClientRect(&rect);
	// set resizeable style
	// ModifyStyle(DS_MODALFRAME, WS_POPUP | WS_THICKFRAME); // LYF  [5/3/2006]
	// ModifyStyle(0,  WS_THICKFRAME); // LYF  [5/3/2006]
	// adjust size to reflect new style
	::AdjustWindowRectEx(&rect, GetStyle(),
		::IsMenu(GetMenu()->GetSafeHmenu()), GetExStyle());
	SetWindowPos(NULL, 0, 0, rect.Width(), rect.Height(), SWP_FRAMECHANGED |
		/*SWP_NOMOVE |*/ SWP_NOZORDER | SWP_NOACTIVATE /*| SWP_NOREPOSITION*/);

	if (!InitGrip()) return -1;

	return 0;
}

BOOL CResizableDialog::OnInitDialog() {
	CDialogEx::OnInitDialog();

	// gets the template size as the min track size
	CRect rc; GetWindowRect(&rc);
	SetMinTrackSize(rc.Size());
	UpdateGripPos();
	m_bInitDone = TRUE;
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CResizableDialog::OnDestroy() {
	if (m_bEnableSaveRestore) SaveWindowRect(m_sSection, m_bRectOnly);
	// remove child windows
	RemoveAllAnchors();
	CDialogEx::OnDestroy();
}

void CResizableDialog::UpdateDialiogElements() {
	if (m_bInitDone) {
		UpdateGripPos(); // update size-grip
		ArrangeLayout();
	}
}

void CResizableDialog::OnSize(UINT nType, int cx, int cy) {
	CWnd::OnSize(nType, cx, cy);
	if (nType == SIZE_MAXHIDE || nType == SIZE_MAXSHOW) return;		// arrangement not needed
	UpdateDialiogElements();
}

void CResizableDialog::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) {
	if (!m_bInitDone) return;
	MinMaxInfo(lpMMI);
}

// NOTE: this must be called after setting the layout
//       to have the dialog and its controls displayed properly
void CResizableDialog::EnableSaveRestore(LPCTSTR pszSection, BOOL bRectOnly) {
	m_sSection = pszSection;
	m_bEnableSaveRestore = TRUE;
	m_bRectOnly = bRectOnly;
	// restore immediately
	LoadWindowRect(pszSection, bRectOnly);
}

CWnd* CResizableDialog::GetResizableWnd() {
	// make the layout know its parent window
	return this;
}

BOOL CResizableDialog::OnEraseBkgnd(CDC* pDC) {
	ClipChildren(pDC);
	return CDialogEx::OnEraseBkgnd(pDC);
}