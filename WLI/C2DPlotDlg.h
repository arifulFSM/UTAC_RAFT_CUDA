#pragma once
#include "afxdialogex.h"


// C2DPlotDlg dialog

class C2DPlotDlg : public CDialogEx
{
	DECLARE_DYNAMIC(C2DPlotDlg)
	HWND m_hPE;
public:
	C2DPlotDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~C2DPlotDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG2D };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	int m_nRENDER_GDILEGACYType;
	afx_msg LRESULT OnRENDER_GDILEGACYTypeChanged(WPARAM wparam, LPARAM lparam);
	afx_msg void OnDestroy();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	static C2DPlotDlg* tdDlgPointer;
	void copyData();
	std::vector<std::vector<float>>imgData2D;
	void Create2DContouredSurface();
};
