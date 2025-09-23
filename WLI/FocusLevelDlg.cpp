// FocusLevelDlg.cpp : implementation file
//

#include "pch.h"
#include "WLI.h"
#include "SRC/DOSUtil.h"
#include "MOTOR/Motor.h"
#include "afxdialogex.h"
#include "FocusLevelDlg.h"

// CFocusLevelDlg dialog

IMPLEMENT_DYNAMIC(CFocusLevelDlg, CDialogEx)

CFocusLevelDlg::CFocusLevelDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG12, pParent) {}

CFocusLevelDlg::~CFocusLevelDlg() {}

void CFocusLevelDlg::DoDataExchange(CDataExchange* pDX) {
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFocusLevelDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CFocusLevelDlg::OnBnClickedButton1)
END_MESSAGE_MAP()

// CFocusLevelDlg message handlers

void CFocusLevelDlg::OnBnClickedButton1() {
	CString fname = DosUtil.GetProgramDir().c_str(); fname += _T("IMG");
	int a = _wmkdir(fname);
	CString str = CTime::GetCurrentTime().Format("\\Meta_%d%H%M%S.CSV");
	fname += str;
	FILE* fp = fopen(CT2A(fname), "wb"); if (!fp) return;
	MOT::SMotPar* M = &Dev.MC->tilt.Mpar[0];
	for (int i = 0; i < 32; i++) {
		IMGL::CIM Im;
		if (Dev.Cam.Grab(Im, CAM::SECCAM, 4)) {
			//Im.SavePicTime(); // temporary
			fprintf(fp, "%d,%.4f,%f\n", i + 1, M[int(MOT::MAXIS::Z)].now, Im.Intensity());
		}
		MoveR(MOT::MAXIS::Z, -incZ / 10.f, 20000, bCheck);
	}
	fclose(fp);
}