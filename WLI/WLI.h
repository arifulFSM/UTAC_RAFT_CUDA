// WLI.h : main header file for the WLI application
//
#pragma once

#ifndef __AFXWIN_H__
#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols

// CWLIApp:
// See WLI.cpp for the implementation of this class
//

class CWLIApp : public CWinApp {
public:
	CWLIApp() noexcept;

	// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CWLIApp theApp;
