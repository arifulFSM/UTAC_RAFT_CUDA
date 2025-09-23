#pragma once
#include "afxdialogex.h"

// MeasProgressDlg dialog

class MeasProgressDlg : public CDialogEx {
	DECLARE_DYNAMIC(MeasProgressDlg)

public:
	MeasProgressDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~MeasProgressDlg();
	BOOL OnInitDialog();
	void ShowMessage(CString str);
	//CProgressCtrl* progress = nullptr;
	void SetUpProgressBarProp(int sz);
	void SetPosProBar(int idx, int sz, CString Message = L"");

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_RCP_MEAS_PBAR };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CProgressCtrl m_ProgressBar;
};
