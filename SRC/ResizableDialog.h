#pragma once

// ResizableDialog.h : header file
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

#include "ResizableLayout.h"
#include "ResizableGrip.h"
#include "ResizableMinMax.h"
#include "ResizableState.h"

class CResizableDialog : public CDialogEx, public CResizableLayout,
	public CResizableGrip, public CResizableMinMax,
	public CResizableState {
public:
	CResizableDialog();
	CResizableDialog(UINT nIDTemplate, CWnd* pParentWnd = NULL);
	CResizableDialog(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL);

private:
	// flags
	BOOL m_bEnableSaveRestore = FALSE;
	BOOL m_bRectOnly = FALSE;

	// internal status
	CString m_sSection = _T("");			// section name (identifies a parent window)
	BOOL m_bInitDone = FALSE;			// if all internal vars initialized

	//////////////////////////////////////////////////////////////////////////
	// Process specific control variables [1/26/2020 yuenl]
protected:
	short TabIdx = -1;
	HWND hPWnd = 0; // parent window [1/25/2020 yuenl]
	HWND hWWnd = 0; // wafer map window [1/25/2020 yuenl]

public:
	void SetID(short idx, HWND hPW, HWND hWMP);
	short GetID();
	HWND GetPWnd();
	//////////////////////////////////////////////////////////////////////////

public:
	BOOL bTabSelected = FALSE;

public:
	virtual ~CResizableDialog();

	// used internally
private:
	void Construct();

	// callable from derived classes
protected:
	// section to use in app's profile
	void UpdateDialiogElements();
	void EnableSaveRestore(LPCTSTR pszSection, BOOL bRectOnly = FALSE);

	virtual CWnd* GetResizableWnd();

protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	DECLARE_MESSAGE_MAP()
};
