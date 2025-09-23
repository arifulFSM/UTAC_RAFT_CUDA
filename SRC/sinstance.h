#pragma once

// https://owlcation.com/stem/VC-MFC-Example-Preserve-Placement-of-the-Window

class CInstanceChecker {
public:
	CString Regkey;
	void SetRegKey(CString key);
	LONG LoadState(WINDOWPLACEMENT* sWP);
	void SaveState(HWND hWnd);

public:
	CInstanceChecker();
	~CInstanceChecker();
};

extern CInstanceChecker InstanceChecker;