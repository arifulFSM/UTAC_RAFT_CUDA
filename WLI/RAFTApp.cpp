#include "pch.h"
#include "RAFTApp.h"
#include "../SRC/DOSUtil.h"
#include <chrono> //Morsalinn

CRAFTApp* pRAFTApp = NULL;

CRAFTApp::CRAFTApp()
{
	m_pChuckMask = NULL;
	// Morsalin [
	using namespace std::chrono;
	auto start = time_point_cast<seconds>(system_clock::now());
	start_sec = start.time_since_epoch().count();
	isReset = 0;
	//]

	LoadChuckMask();
}

CRAFTApp::~CRAFTApp()
{
	if (m_pChuckMask)
	{
		delete m_pChuckMask;
	}
}

BOOL CRAFTApp::LoadChuckMask() {
	if (m_pChuckMask) {
		delete m_pChuckMask;
	}
	m_pChuckMask = new CChuckMask;

	CString MaskFile;
	DosUtil.GetProgramDir(MaskFile);
	MaskFile += "300mmChuck.msk";
	if (m_pChuckMask->LoadFromFile(MaskFile)) {
		return TRUE;
	}
	AfxMessageBox(L"No Chuck Mask file found");
	return FALSE;
}

BOOL CRAFTApp::ValidCheckRcp(CRecipeRAFT& Rcp) {
	return TRUE;
}