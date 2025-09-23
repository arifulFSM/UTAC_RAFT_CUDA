#pragma once
#include "afxdialogex.h"


// C3DPlotDlg dialog

class C3DPlotDlg : public CDialogEx
{
	DECLARE_DYNAMIC(C3DPlotDlg)

	HWND m_hPE;
public:
	
	C3DPlotDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~C3DPlotDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG3D };
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
	static C3DPlotDlg* threeDDlgPointer;
	void copyData();
	std::vector<std::vector<float>>imgData3D;
	void Create3DContouredSurface();
};
