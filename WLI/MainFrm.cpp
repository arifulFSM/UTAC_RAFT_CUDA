#include "pch.h"
#include "framework.h"
#include "WLI.h"

#include "Dev.h"
#include "MainFrm.h"
#include "SRC/sinstance.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_CLOSE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

CMainFrame::CMainFrame() noexcept {}

CMainFrame::~CMainFrame() {}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1) return -1;
	Dev.Initialize();
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs) {
	if (!CFrameWnd::PreCreateWindow(cs)) return FALSE;
	cs.style &= ~(FWS_ADDTOTITLE);

	// Set the window title here!
	this->SetTitle(L"FSM RAFT 2.1.2402.5");

	// Remove Menu
	if (cs.hMenu != NULL) {
		::DestroyMenu(cs.hMenu);      // delete menu if loaded
		cs.hMenu = NULL;              // no menu for this window
	}

	return TRUE;
}

#ifdef _DEBUG
void CMainFrame::AssertValid() const {
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const {
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG

void CMainFrame::OnClose() {
	Dev.Deinitialize();
	InstanceChecker.SaveState(this->m_hWnd);
	CFrameWnd::OnClose();
}