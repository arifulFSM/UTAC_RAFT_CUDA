#pragma once

#include <gl/GL.h>

#include "IM/IM.h"
#include "IM/ColorMapGenerator.h"

#define WL_CTRLMOUSEMOVE  (WM_USER+7300)
#define WL_CTRLMOUSEMOVE1 (WM_USER+7301)
#define WL_CTRLMOUSEMOVE2 (WM_USER+7302)
#define WL_CTRLMOUSEMOVE3 (WM_USER+7303)
#define WL_CTRLMOUSEMOVE4 (WM_USER+7304)
#define WL_CTRLMOUSEMOVE5 (WM_USER+7305)

#define WL_LMOUSEDBLCLK   (WM_USER+7306)
#define WL_LMOUSEDBLCLK1  (WM_USER+7307)
#define WL_LMOUSEDBLCLK2  (WM_USER+7308)
#define WL_LMOUSEDBLCLK3  (WM_USER+7309)
#define WL_LMOUSEDBLCLK4  (WM_USER+7310)
#define WL_LMOUSEDBLCLK5  (WM_USER+7311)

#define WL_LMOUSEDOWN     (WM_USER+7312)
#define WL_LMOUSEDOWN1    (WM_USER+7313)
#define WL_LMOUSEDOWN2    (WM_USER+7314)
#define WL_LMOUSEDOWN3    (WM_USER+7315)
#define WL_LMOUSEDOWN4    (WM_USER+7316)
#define WL_LMOUSEDOWN5    (WM_USER+7317)

#define WL_LMOUSEUP       (WM_USER+7318)
#define WL_LMOUSEUP1      (WM_USER+7319)
#define WL_LMOUSEUP2      (WM_USER+7320)
#define WL_LMOUSEUP3      (WM_USER+7321)
#define WL_LMOUSEUP4      (WM_USER+7322)
#define WL_LMOUSEUP5      (WM_USER+7323)

#define WL_RMOUSEDBLCLK   (WM_USER+7324)
#define WL_RMOUSEDBLCLK1  (WM_USER+7325)
#define WL_RMOUSEDBLCLK2  (WM_USER+7326)
#define WL_RMOUSEDBLCLK3  (WM_USER+7327)
#define WL_RMOUSEDBLCLK4  (WM_USER+7328)
#define WL_RMOUSEDBLCLK5  (WM_USER+7329)

#define WL_RMOUSEDOWN     (WM_USER+7330)
#define WL_RMOUSEDOWN1    (WM_USER+7331)
#define WL_RMOUSEDOWN2    (WM_USER+7332)
#define WL_RMOUSEDOWN3    (WM_USER+7333)
#define WL_RMOUSEDOWN4    (WM_USER+7334)
#define WL_RMOUSEDOWN5    (WM_USER+7335)

#define WL_RMOUSEUP       (WM_USER+7336)
#define WL_RMOUSEUP1      (WM_USER+7337)
#define WL_RMOUSEUP2      (WM_USER+7338)
#define WL_RMOUSEUP3      (WM_USER+7339)
#define WL_RMOUSEUP4      (WM_USER+7340)
#define WL_RMOUSEUP5      (WM_USER+7341)

#define WL_MOUSEMOVE      (WM_USER+7342)
#define WL_MOUSEMOVE1     (WM_USER+7343)
#define WL_MOUSEMOVE2     (WM_USER+7344)
#define WL_MOUSEMOVE3     (WM_USER+7345)
#define WL_MOUSEMOVE4     (WM_USER+7346)
#define WL_MOUSEMOVE5     (WM_USER+7347)

struct SStat;

class CPicWnd : public CStatic {
	DECLARE_DYNAMIC(CPicWnd)

	CBitmap BMP;
	IMGL::CIM::eCLRCHN ClorChn = IMGL::CIM::COLOR;

	//////////////////////////////////////////////////////////////////////////
	CRect bestRC;
	IMGL::CIM /*Img1,*/ Img2, /*ImgUndo, ImgBG,*/* ImgC = NULL;
	CPen* pGrayPen = NULL, * oPen = NULL;
	//////////////////////////////////////////////////////////////////////////
	// 3D drawing related [10/30/2020 yuenl]
	HGLRC m_hGLContext = 0;
	bool bSave3D = false;
	bool bErase = false;
	bool m_TrueColor = true;
	float m_Bkcolor = 0.50f;	// Background color
	int m_DrawMode = GL_TRIANGLES; // GL_LINE_STRIP, GL_QUADS or GL_TRIANGLES
	int m_Fill = GL_FILL/*GL_LINE*/;
	//////////////////////////////////////////////////////////////////////////
	// Mouse [10/30/2020 yuenl]
	bool bROI = false;
	bool m_LeftButtonDown = false;
	bool m_RightButtonDown = false;
	CRect ROI;
	CPoint m_LeftDownPos, m_LeftUpPos;
	CPoint m_RightDownPos, m_RightUpPos;
	//////////////////////////////////////////////////////////////////////////
	UCHAR vKey = 0;
	//////////////////////////////////////////////////////////////////////////
	IMGL::CColorMapGenerator CMG;
	void RetainPsudo(IMGL::CIM::eCLRCHN type);
	//////////////////////////////////////////////////////////////////////////
	// XOR draw [6/6/2021 FSM]
	void DoROI();
	void DrawROI();
	void SaveROI();
	void ClearROI();
	void GDIDraw(CDC* pDC);
	//////////////////////////////////////////////////////////////////////////

public:
	HWND hWNd = 0;
	bool bVKeyCtrl = false;
	CString title = _T("");

public:
	UINT MouseMove = WL_MOUSEMOVE;
	UINT CtrlMouseMove = WL_CTRLMOUSEMOVE;
	UINT LMouseUp = WL_LMOUSEUP;
	UINT LMouseDown = WL_LMOUSEDOWN;
	UINT RMouseUp = WL_RMOUSEUP;
	UINT RMouseDown = WL_RMOUSEDOWN;
	UINT LMouseDblClick = WL_LMOUSEDBLCLK;
	UINT RMouseDblClick = WL_RMOUSEDBLCLK;

#ifndef _GLXDRAW
	float m_Scale = 1.3f;		// Plot scale
	float m_xRotation = 10.0f;
	float m_yRotation = -44.0f;
#endif

public:
	void SetMouseMove(UINT Ums) { MouseMove = Ums; }
	void SetCtrlMouseMove(UINT Ums) { CtrlMouseMove = Ums; }
	void SetLMouseUp(UINT Ums) { LMouseUp = Ums; }
	void SetLMouseDown(UINT Ums) { LMouseDown = Ums; }
	void SetRMouseDown(UINT Ums) { RMouseDown = Ums; }
	void SetLMouseDblClick(UINT Ums) { LMouseDblClick = Ums; }
	void SetRMouseDblClick(UINT Ums) { RMouseDblClick = Ums; }

public:
	CPicWnd();
	virtual ~CPicWnd();

	IMGL::CIM* GetImg2();
	void SetImg(IMGL::CIM& Img);
	void RetainColor(IMGL::CIM& Im);
	void SetImg2(IMGL::CIM& Img);
	void SetImg2(IMGL::CIM2& Img);
	void SetImg2Reduce(IMGL::CIM16* pImg, int cx, int cy);
	void SetImg2(IMGL::CIM& Img, COLORREF cr);
	void SetImgC(IMGL::CIM* pImg);

public:
	BOOL SetWindowPixelFormat(HDC hDC);
	bool Contrast();
	bool CropSave();
	bool SavePic();
	bool Save3D(CString fname);
	void Reset();
	void Dump(int n, char* filename);
	void Redraw(BOOL bClear);
	void OnDraw2D(CDC* pDC);
	void MapColor(BYTE r, BYTE g, BYTE b, PBYTE bgr);
	void PlotLine();
	void PlotQuads();
	void PlotTriangles();
	void BlkBgrd();
	void WhtBgrd();
	void TrueColor();
	void PolygonFill();
	void TextOut(unsigned x, unsigned y, int col, char* Format, ...);
	void SwitchToRedChannel(IMGL::CIM& Img);
	void SwitchToGreenChannel(IMGL::CIM& Img);
	void SwitchToBlueChannel(IMGL::CIM& Img);
	void SwitchToColorChannel(IMGL::CIM& Img);
	void SwitchToGreyChannel(IMGL::CIM& Img);
	void SwitchToPsudoChannel(IMGL::CIM& Img, IMGL::CIM::eCLRCHN type);

public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL StartOpenGL();
	virtual BOOL StopOpenGL();

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg virtual void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

class CHeightWnd : public CPicWnd {
public:
	CHeightWnd() {}
	virtual ~CHeightWnd() {}

public:
	afx_msg void OnSize(UINT nType, int cx, int cy) {
		CStatic::OnSize(nType, cx, cy);
		Redraw(TRUE);
	}
};