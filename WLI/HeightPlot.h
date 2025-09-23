#pragma once
#include "afxdialogex.h"
#include "FringeWnd.h"
#include "SRC/ResizableDialog.h"
// HeightPlot dialog

class HeightPlot : public CResizableDialog {
	DECLARE_DYNAMIC(HeightPlot)

public:
	HeightPlot(CWnd* pParent = nullptr);   // standard constructor
	virtual ~HeightPlot();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_HEIGHT_PLOT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	double stepHeight;	//07182023
	CFringeWnd cPlot;
	CEdit cHeightData;
	void ShowPlot(int x, int y);
	afx_msg LRESULT OnUmHeightCalc(WPARAM wParam, LPARAM lParam);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	void OnMouseMove(UINT nFlags, CPoint point);
	//20230801================================
	void DetectCD();
	double CDWidth, CDLength, depth;
	//==========================================
};

extern HeightPlot hPlot;