#include "pch.h"
#include <direct.h>

#include "PicWnd.h"
#include "IM/IM.h"
#include "MTH/Point3.h"
#include "SRC/DOSUtil.h"

// Macros
#define NINT(f)   ((f >= 0) ? (int)(f + .5) : (int)(f - .5))
#define THRESH(d) ((d > 255) ? 255 : ((d < 0) ? 0 : d))
#define INRANGE() (m_image.minmax.min >= 0 && m_image.minmax.max <= 255)

IMPLEMENT_DYNAMIC(CPicWnd, CStatic)

void CPicWnd::Redraw(BOOL bClear) {
	if (BMP.m_hObject) BMP.DeleteObject();
	bErase = bClear;
	Invalidate(bClear);
}

void CPicWnd::RetainPsudo(IMGL::CIM::eCLRCHN type) {
	if (Img2.IsNull()) return;
	switch (type) {
	case IMGL::CIM::PSUDO1: CMG.GenColorMapJet(); break;
	case IMGL::CIM::PSUDO2: CMG.GenColorMapHot2Cold(); break;
	case IMGL::CIM::PSUDO3: CMG.GenColorMapConstBrightness(); break;
	case IMGL::CIM::PSUDO4: CMG.GenColorMapGrey(); break;
	case IMGL::CIM::PSUDO5: CMG.GenColorMapHeatMap(); break;
	case IMGL::CIM::PSUDO6: CMG.GenColorMapHSV(); break;
	}
	//if (UI.b2D) {
#pragma omp parallel for
	for (int y = 0; y < Img2.GetHeight(); y++) {
		PBYTE p = (PBYTE)Img2.GetPixelAddress(0, y);
		for (int x = 0; x < Img2.GetWidth(); x++) {
			int v = (*(p)+*(p + 1) * 2 + *(p + 2)) / 4;
			v = CMG.Map[v];
			*(p++) = GetBValue(v); *(p++) = GetGValue(v); *(p++) = GetRValue(v);
		}
	}
	//}
}

void CPicWnd::DoROI() {
	//CDC* pDC = GetDC();
	//GDIDraw(pDC);
	CRect rc; rc = ROI; rc.InflateRect(5, 5);
	ROI.SetRect(m_LeftDownPos.x, m_LeftDownPos.y, m_LeftUpPos.x, m_LeftUpPos.y);
	if (ROI.left > ROI.right) {
		LONG tmp = ROI.left; ROI.left = ROI.right; ROI.right = tmp;
	}
	if (ROI.top > ROI.bottom) {
		LONG tmp = ROI.top; ROI.top = ROI.bottom; ROI.bottom = tmp;
	}
	InvalidateRect(&rc, TRUE);
	//GDIDraw(pDC);
	//ReleaseDC(pDC);
	//OutputDebugString(L"DoROI\n");
}

void CPicWnd::DrawROI() {}

void CPicWnd::SaveROI() {}

void CPicWnd::ClearROI() { bROI = false; }

void CPicWnd::GDIDraw(CDC* pDC) {
	if (!bROI || (!ROI.left && !ROI.top && !ROI.right && !ROI.bottom)) return;
	CRect rc;
	int nDC = pDC->SaveDC();
	pDC->SelectObject(GetStockObject(NULL_BRUSH));
	CPen Pen(PS_SOLID, 3, RGB(255, 255, 0));
	CPen* pOPen = (CPen*)pDC->SelectObject(&Pen);
	//pDC->SetROP2(R2_COPYPEN);
	pDC->Rectangle(&ROI);
	pDC->SelectObject(pOPen);
	pDC->RestoreDC(nDC);
	//OutputDebugString(L"GDIDraw\n");
}

CPicWnd::CPicWnd() {
	pGrayPen = new CPen(PS_SOLID, 1, RGB(96, 96, 96));
}

CPicWnd::~CPicWnd() {
	if (pGrayPen) delete pGrayPen;
}

// void CPicWnd::Test() {
// 	float mat1[2][2] = { 7,21,21,68.25 };
// 	SMatInv a;
// 	a.Matrix2x2Inverse(mat1);
// }

void CPicWnd::SwitchToRedChannel(IMGL::CIM& Img) {
	ClorChn = IMGL::CIM::RED;
	Img2 = Img;
	Img2.RetainRed();
	Invalidate(FALSE);
}

void CPicWnd::SwitchToGreenChannel(IMGL::CIM& Img) {
	ClorChn = IMGL::CIM::GREEN;
	Img2 = Img;
	Img2.RetainGreen();
	Invalidate(FALSE);
}

void CPicWnd::SwitchToBlueChannel(IMGL::CIM& Img) {
	ClorChn = IMGL::CIM::BLUE;
	Img2 = Img;
	Img2.RetainBlue();
	Invalidate(FALSE);
}

void CPicWnd::SwitchToColorChannel(IMGL::CIM& Img) {
	ClorChn = IMGL::CIM::COLOR;
	Img2 = Img;
	Invalidate(FALSE);
}

void CPicWnd::SwitchToGreyChannel(IMGL::CIM& Img) {
	ClorChn = IMGL::CIM::GREY;
	Img2 = Img;
	Img2.RetainGrey();
	Invalidate(FALSE);
}

void CPicWnd::SwitchToPsudoChannel(IMGL::CIM& Img, IMGL::CIM::eCLRCHN type) {
	ClorChn = type;
	Img2 = Img;
	RetainPsudo(type);
	Invalidate(FALSE);
}

BOOL CPicWnd::StartOpenGL() {
	long style = GetWindowLong(GetSafeHwnd(), GWL_EXSTYLE);
	style |= WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	SetWindowLong(GetSafeHwnd(), GWL_EXSTYLE, style);

#ifdef _GLXDRAW
	CRect rc; GetClientRect(rc);
	GlX.SetViewPortSize(rc.Width(), rc.Height());
	return GlX.StartOpenGL(GetDC()->GetSafeHdc());
#endif
	return TRUE;
}

BOOL CPicWnd::StopOpenGL() {
#ifdef _GLXDRAW
	GlX.ShutDown();
#endif
	return TRUE;
}

IMGL::CIM* CPicWnd::GetImg2() {
	return &Img2;
}

void CPicWnd::SetImg(IMGL::CIM& Img) {
	//Img1 = *pImg;
	Img2 = Img;
}

void CPicWnd::RetainColor(IMGL::CIM& Im) {
	switch (ClorChn) {
	case IMGL::CIM::RED: Im.RetainRed(); break;
	case IMGL::CIM::GREEN: Im.RetainGreen(); break;
	case IMGL::CIM::BLUE: Im.RetainBlue(); break;
	case IMGL::CIM::GREY: Im.RetainGrey(); break;
	case IMGL::CIM::COLOR: break;
	case IMGL::CIM::PSUDO1:
	case IMGL::CIM::PSUDO2:
	case IMGL::CIM::PSUDO3:
	case IMGL::CIM::PSUDO4:
	case IMGL::CIM::PSUDO5:
	case IMGL::CIM::PSUDO6: RetainPsudo(ClorChn); break;
	default: break;
	}
}

void CPicWnd::SetImg2(IMGL::CIM& Img) {
	if (Img.IsNull()) return;
	Img2 = Img;
	RetainColor(Img2);
}

void CPicWnd::SetImg2(IMGL::CIM2& Img) {
	if (Img.IsNull()) return;
	Img2 = Img;
	RetainColor(Img2);
}

void CPicWnd::SetImg2Reduce(IMGL::CIM16* pImg, int cx, int cy) {
	if (!pImg) return; if (pImg->IsNull()) return;
	if (!pImg->Resampling(Img2, cx, cy)) return;
	RetainColor(Img2);
}

void CPicWnd::SetImg2(IMGL::CIM& Img, COLORREF cr) {
	if (!Img) return;
	Img2 = Img;
	if (cr) { Img2.ReplacePixel(0, cr); }
	RetainColor(Img2);
}

void CPicWnd::SetImgC(IMGL::CIM* pImg) {
	if (!pImg) return;
	ImgC = pImg;
}

bool CPicWnd::CropSave() {
	// CString srcDir = _T("F:\\OneDrive\\Desktop\\WLI\\IMG1\\");
	// CString desDir = _T("C:\\WLI2\\IMG\\");
	// CString srcName = _T(""), desName = _T("");
	// CIM* pIm = GetImg();
	// for (UINT i = 1; i <= 300; i++) {
	// 	srcName.Format(_T("%s%d.BMP"), srcDir.GetBuffer(), i);
	// 	desName.Format(_T("%s%d.BMP"), desDir.GetBuffer(), i);
	// 	if (!pIm->LoadPicF(srcName)) continue;
	// 	pIm->Crop(115, 49, 790, 583);
	// 	pIm->SavePic(desName);
	// }
	return true;
}

bool CPicWnd::Contrast() {
	//Img1.Contrast(256);
	//Img1.CalcHisto();
	return true;
}

bool CPicWnd::SavePic() {
	if (Img2.SavePic()) return true;
	return false;
}

bool CPicWnd::Save3D(CString fname) {
	bSave3D = true;
	CDC DC; DC.GetSafeHdc();
	//OnDraw3D(&DC);
	bSave3D = false;
	return false;
}

void CPicWnd::Reset() {
	//if (!Img1.IsNull()) Img1.Destroy();
	if (!Img2.IsNull()) Img2.Destroy();
	//if (!ImgBG.IsNull()) ImgBG.Destroy();
	Redraw(TRUE);
}

void CPicWnd::Dump(int n, char* filename) {
	FILE* fp = fopen(filename, "wb"); if (!fp) return;
	int wd = Img2.GetWidth();
	PBYTE p = (PBYTE)Img2.GetPixelAddress(0, n);
	for (int i = 0; i < wd; i++) {
		fprintf(fp, "%d,%d\n", i + 1, *(p++));
	}
	fclose(fp);
}

BEGIN_MESSAGE_MAP(CPicWnd, CStatic)
	ON_WM_PAINT()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_SIZE()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_MOUSEHWHEEL()
	ON_WM_RBUTTONDBLCLK()
END_MESSAGE_MAP()

void CPicWnd::OnPaint() {
	CPaintDC dc(this);
	if (Img2.IsNull()) return;
	OnDraw2D(&dc);
	//if (UI.b2D) OnDraw2D(&dc);
	//else {
	//#ifdef _GLXDRAW
	//	GlX.OnDraw();
	//#else
	//	OnDraw3D(&dc);
	//#endif
	//}
}

void CPicWnd::OnDraw2D(CDC* pDC) {
	if (bErase) {
		CBrush brBackground;
		brBackground.CreateSolidBrush(GetSysColor(COLOR_3DFACE));
		pDC->FillRect(&bestRC, &brBackground);
		bErase = false;
	}
	if (Img2.IsNull()) return;
	GetClientRect(&bestRC);
	Img2.BestRect(bestRC);
	if (((bestRC.Width() > 16)) && (bestRC.Height() > 16)) {
		Img2.StretchBlt(pDC->m_hDC,
			bestRC.left, bestRC.top, bestRC.Width(), bestRC.Height(),
			SRCCOPY);
	}
	if (bROI) GDIDraw(pDC);
}

void CPicWnd::OnLButtonDown(UINT nFlags, CPoint point) {
	SetCapture();
	m_LeftButtonDown = true;
	m_LeftDownPos = m_LeftUpPos = point;
	static MTH::SPointf Pt;
	Pt.x = (point.x - bestRC.left) / float(bestRC.Width());
	Pt.y = (point.y - bestRC.top) / float(bestRC.Height());
	if (hWNd) {
		if ((Pt.x >= 0) && (Pt.x <= 1) && (Pt.y >= 0) && (Pt.y <= 1)) {
			::PostMessage(hWNd, LMouseDown, 0, LPARAM(&Pt));
		}
	}
}

void CPicWnd::OnLButtonUp(UINT nFlags, CPoint point) {
	m_LeftButtonDown = false;
	m_LeftUpPos = point;
	static MTH::SPointf Pt;
	Pt.x = (point.x - bestRC.left) / float(bestRC.Width());
	Pt.y = (point.y - bestRC.top) / float(bestRC.Height());
	if (hWNd) {
		if ((Pt.x >= 0) && (Pt.x <= 1) && (Pt.y >= 0) && (Pt.y <= 1)) {
			::PostMessage(hWNd, LMouseUp, 0, LPARAM(&Pt));
		}
	}
	ReleaseCapture();
}

void CPicWnd::OnMouseMove(UINT nFlags, CPoint point) {
	if (GetAsyncKeyState(VK_SHIFT) & 0xF0000) return;

	static MTH::SPointf Pt;
	if ((bestRC.Width() < 100) || (bestRC.Height() < 100)) return;

	Pt.x = (point.x - bestRC.left) / float(bestRC.Width());
	Pt.y = (point.y - bestRC.top) / float(bestRC.Height());

	if (hWNd && ((Pt.x >= 0) && (Pt.x <= 1) && (Pt.y >= 0) && (Pt.y <= 1))) {
		bVKeyCtrl = false;
		if (GetAsyncKeyState(VK_CONTROL) & 0xF0000) {
			bVKeyCtrl = false;
			::PostMessage(hWNd, CtrlMouseMove, 0, LPARAM(&Pt));
		}
		else {
			::PostMessage(hWNd, MouseMove, 0, LPARAM(&Pt));
		}
	}
	if (bROI && m_LeftButtonDown) {
		m_LeftUpPos = point;
		DoROI();
	}
}

void CPicWnd::OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt) {
#ifdef _GLXDRAW
	if (zDelta > 0) GlX.UpdateMouse(0, -5, 255);
	if (zDelta < 0) GlX.UpdateMouse(0, 5, 255);
#endif
}

void CPicWnd::OnRButtonDown(UINT nFlags, CPoint point) {
	m_RightButtonDown = true;
	m_RightDownPos = m_RightUpPos = point;
	static MTH::SPointf Pt;
	Pt.x = (point.x - bestRC.left) / float(bestRC.Width());
	Pt.y = (point.y - bestRC.top) / float(bestRC.Height());

	if (hWNd) {
		if ((Pt.x >= 0) && (Pt.x <= 1) && (Pt.y >= 0) && (Pt.y <= 1)) {
			::PostMessage(hWNd, RMouseDown, 0, LPARAM(&Pt));
		}
	}
	SetCapture();
}

void CPicWnd::OnRButtonUp(UINT nFlags, CPoint point) {
	m_RightButtonDown = false;
	m_RightUpPos = point;
	static MTH::SPointf Pt;
	Pt.x = (point.x - bestRC.left) / float(bestRC.Width());
	Pt.y = (point.y - bestRC.top) / float(bestRC.Height());
	if (hWNd) {
		if ((Pt.x >= 0) && (Pt.x <= 1) && (Pt.y >= 0) && (Pt.y <= 1)) {
			::PostMessage(hWNd, RMouseUp, 0, LPARAM(&Pt));
		}
	}
	ReleaseCapture();
}

void CPicWnd::OnLButtonDblClk(UINT nFlags, CPoint point) {
	if ((bestRC.Width() < 100) || (bestRC.Height() < 100)) return;

	static MTH::SPointf Pt;
	Pt.x = (point.x - bestRC.left) / float(bestRC.Width());
	Pt.y = (point.y - bestRC.top) / float(bestRC.Height());

	if (hWNd) {
		bVKeyCtrl = false;
		if (GetAsyncKeyState(VK_CONTROL) & 0xF0000) { bVKeyCtrl = false; }
		if ((Pt.x >= 0) && (Pt.x < 1) && (Pt.y >= 0) && (Pt.y < 1)) {
			::PostMessage(hWNd, LMouseDblClick, 0, LPARAM(&Pt));
		}
	}
	//CStatic::OnLButtonDblClk(nFlags, point);
}

void CPicWnd::OnRButtonDblClk(UINT nFlags, CPoint point) {
	if ((bestRC.Width() < 100) || (bestRC.Height() < 100)) return;

	static MTH::SPointf Pt;
	Pt.x = (point.x - bestRC.left) / float(bestRC.Width());
	Pt.y = (point.y - bestRC.top) / float(bestRC.Height());

	if (hWNd) {
		if ((Pt.x >= 0) && (Pt.x < 1) && (Pt.y >= 0) && (Pt.y < 1)) {
			::PostMessage(hWNd, RMouseDblClick, 0, LPARAM(&Pt));
		}
	}
}

void CPicWnd::OnSize(UINT nType, int cx, int cy) {
	CStatic::OnSize(nType, cx, cy);
	//if (!UI.b2D) {
	//#ifdef _GLXDRAW
	//	GlX.iVPortWd = cx; GlX.iVPortHt = cy;
	//	GlX.OnSize();
	//#else
	//HWND hWnd = GetSafeHwnd();
	//HDC hDC = ::GetDC(hWnd);
	//wglMakeCurrent(hDC, m_hGLContext);
	//glViewport(0, 0, cx, cy);
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//glMatrixMode(GL_MODELVIEW);
	//::ReleaseDC(hWnd, hDC);
	//Redraw(TRUE);
	//#endif
	//}
}

void CPicWnd::MapColor(BYTE r, BYTE g, BYTE b, PBYTE bgr) {
	// if (m_TrueColor) {
	// 	short v;
	// 	switch (ClorChn) {
	// 	case IMGL::CIM::COLOR:
	// 		if (bgr) {
	// 			glColor3f(*(bgr + 2) / (GLfloat)255., *(bgr + 1) / (GLfloat)255., *bgr / (GLfloat)255.);
	// 		}
	// 		else {
	// 			glColor3f(r / (GLfloat)255., g / (GLfloat)255., b / (GLfloat)255.);
	// 		}
	// 		break;
	// 	case IMGL::CIM::PSUDO1:
	// 	case IMGL::CIM::PSUDO2:
	// 	case IMGL::CIM::PSUDO3:
	// 	case IMGL::CIM::PSUDO4:
	// 	case IMGL::CIM::PSUDO5:
	// 	case IMGL::CIM::PSUDO6:
	// 		v = (r + 2 * g + b) / 4;
	// 		r = BLU(CMG.Map[v]); g = GRN(CMG.Map[v]); b = RED(CMG.Map[v]);
	// 		glColor3f(r / (GLfloat)255., g / (GLfloat)255., b / (GLfloat)255.);
	// 		break;
	// 	default:
	// 		glColor3f(r / (GLfloat)255., g / (GLfloat)255., b / (GLfloat)255.);
	// 		break;
	// 	}
	// }
	// else {	// 5 level color map
	// 	if (g > r) r = g;
	// 	if (b > r) r = b;
	// 	if (r < (float)255 * 0.25)		// < 1/4
	// 		glColor3f(0.2f, 0.2f, 0.2f);
	// 	else if (r < (float)255 * 0.33)	// 1/3
	// 		glColor3f(0.0f, 0.2f, 0.7f);
	// 	else if (r < (float)255 * 0.5)	// 1/2
	// 		glColor3f(0.0f, 0.6f, 0.5f);
	// 	else if (r < (float)255 * 0.75)	// 3/4
	// 		glColor3f(1.0f, 0.0f, 0.0f);
	// 	else							// > 3/4
	// 		glColor3f(1.0f, 1.0f, 0.0f);
	// }
}

void CPicWnd::TextOut(unsigned x, unsigned y, int col, char* Format, ...) {
	char* str;
	int n;
	va_list argptr;

	va_start(argptr, Format);				// Access argument list
	n = _vscprintf(Format, argptr) + 1;		// Length of formatted str
	str = (char*)malloc(n * sizeof(char));
	vsprintf_s(str, n, Format, argptr);		// Format string using argument list
	va_end(argptr);							// End access

	CDC* dc = GetDC();
	CFont pFont;
	VERIFY(pFont.CreateFont(10,	                    // nHeight
		0,                        // nWidth
		0, 	                      // nEscapement (angle in 1/10th deg)
		0,                        // nOrientation
		FW_NORMAL,                // nWeight
		false,					  // bItalic
		FALSE,                    // bUnderline
		0,                        // cStrikeOut
		ANSI_CHARSET,             // nCharSet
		OUT_DEFAULT_PRECIS,       // nOutPrecision
		CLIP_DEFAULT_PRECIS,      // nClipPrecision
		DEFAULT_QUALITY,          // nQuality
		DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily
		_T("MS Sans Serif"/*"Tahoma"*/)));

	dc->SelectObject(&pFont);
	dc->SetTextColor(col);
	dc->TextOut(x, y, CString(str));
	free(str);
	pFont.DeleteObject();
	ReleaseDC(dc);
}

void CPicWnd::PlotLine() {
	m_DrawMode = GL_LINE_STRIP;
	m_Fill = GL_LINE;
	Invalidate();
}

void CPicWnd::PlotQuads() {
	m_DrawMode = GL_QUADS;
	Invalidate();
}

void CPicWnd::PlotTriangles() {
	m_DrawMode = GL_TRIANGLES;
	Invalidate();
}

void CPicWnd::BlkBgrd() {
	m_Bkcolor = 0.0;
	//glClearColor(m_Bkcolor, m_Bkcolor, m_Bkcolor, 0.0);
	Invalidate();
}

void CPicWnd::WhtBgrd() {
	m_Bkcolor = 1.0;
	//glClearColor(m_Bkcolor, m_Bkcolor, m_Bkcolor, 0.0);
	Invalidate();
}

void CPicWnd::TrueColor() {
	m_TrueColor = !m_TrueColor;
	Invalidate();
}

void CPicWnd::PolygonFill() {
	if (m_Fill == GL_LINE) {
		m_Fill = GL_FILL;
	}
	else {
		m_Fill = GL_LINE;
	}
	Invalidate();
}

BOOL CPicWnd::SetWindowPixelFormat(HDC hDC) {
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),   // size
		1,						// version number
		PFD_DRAW_TO_WINDOW |	// support window
		PFD_SUPPORT_OPENGL |	// support OpenGL
		PFD_DOUBLEBUFFER, 		// double buffered
		PFD_TYPE_RGBA,			// RGBA type
		24,						// 24-bit color depth
		0, 0, 0, 0, 0, 0,		// color bits ignored
		0,						// no alpha buffer
		0,						// shift bit ignored
		0,						// no accumulation buffer
		0, 0, 0, 0,				// accum bits ignored
		32,						// 32-bit z-buffer
		0,						// no stencil buffer
		0,						// no auxiliary buffer
		PFD_MAIN_PLANE,			// main layer
		0,						// reserved
		0, 0, 0					// layer masks ignored
	};

	int GLPixelIndex = ChoosePixelFormat(hDC, &pfd);
	if (GLPixelIndex == 0) {	// Choose default
		GLPixelIndex = 1;
		if (DescribePixelFormat(hDC, GLPixelIndex,
			sizeof(PIXELFORMATDESCRIPTOR), &pfd) == 0)
			return(FALSE);
	}

	if (!SetPixelFormat(hDC, GLPixelIndex, &pfd)) return(FALSE);
	return(TRUE);
}

BOOL CPicWnd::PreCreateWindow(CREATESTRUCT& cs) {
	// An OpenGL window must be created with the following flags and must not
	// include CS_PARENTDC for the class style.
	cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	return CStatic::PreCreateWindow(cs);
}

void CPicWnd::OnDestroy() {
	StopOpenGL();
	CStatic::OnDestroy();
}

void CPicWnd::OnTimer(UINT_PTR nIDEvent) {
	CStatic::OnTimer(nIDEvent);
}

void CPicWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
	vKey = nChar;
}

void CPicWnd::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) {
	vKey = 0;
}