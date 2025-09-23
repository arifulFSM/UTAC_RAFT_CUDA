#include "pch.h"
// #include "WLI.h"
#include "sinstance.h"

CInstanceChecker InstanceChecker;

/* Typical use instructions:

	CMyApp::InitInstance()
	{
	  instanceChecker.SetRegKey("MyCoName");
	  ....
	  if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	  instantChecker.LoadState();
	  ....
	  CMainFrame::OnClose()
	  instantChecker.SaveState();
	  .....
	}
*/

///////////////////////////////// Implementation //////////////////////////////

CInstanceChecker::CInstanceChecker() {}

CInstanceChecker::~CInstanceChecker() {}

void CInstanceChecker::SetRegKey(CString key) {
	Regkey = key;
}

LONG CInstanceChecker::LoadState(WINDOWPLACEMENT* sWP) {
	LONG Ret;
	HKEY RegistryKey;
	DWORD type = REG_BINARY;
	DWORD sizewp = sizeof(WINDOWPLACEMENT);

	CString str;
	str.Format(_T("Software\\FrontierSemi\\%s\\PreservedWindowPos"), Regkey.GetBuffer());
	Ret = RegOpenKeyEx(
		HKEY_CURRENT_USER,
		str.GetBuffer(),
		0,
		KEY_READ,
		&RegistryKey);
	//Sample 9.3: Read the Window Placement Structure
	if (Ret == ERROR_SUCCESS)
		Ret = ::RegQueryValueEx(RegistryKey,
			_T("PosAndSize"),
			0,
			&type,
			(LPBYTE)sWP,
			&sizewp);
	return Ret;
}

void CInstanceChecker::SaveState(HWND hWnd) {
	LONG Ret;
	HKEY Registry_Key;
	DWORD disposition;
	WINDOWPLACEMENT sWindow_Position;

	CString str;
	str.Format(_T("Software\\FrontierSemi\\%s\\PreservedWindowPos"), Regkey.GetBuffer());
	Ret = RegOpenKeyEx(
		HKEY_CURRENT_USER,
		str.GetBuffer(),
		NULL,
		KEY_WRITE,
		&Registry_Key);
	//Sample 05: The key will not exists for the very
	//first time and hence create
	if (Ret != ERROR_SUCCESS) {
		RegCreateKeyEx(
			HKEY_CURRENT_USER,
			str,
			NULL,
			NULL,
			REG_OPTION_NON_VOLATILE,
			KEY_ALL_ACCESS,
			NULL,
			&Registry_Key,
			&disposition);
	}
	GetWindowPlacement(hWnd, &sWindow_Position);
	//Sample 07: Write this Structure to Registry
	RegSetValueEx(
		Registry_Key,
		_T("PosAndSize"),
		NULL,
		REG_BINARY,
		(BYTE*)&sWindow_Position,
		sizeof(WINDOWPLACEMENT));
	RegCloseKey(Registry_Key);
}