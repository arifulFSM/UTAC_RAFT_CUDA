// C3DPlotDlg.cpp : implementation file
//

#include "pch.h"
#include "WLI.h"
#include "afxdialogex.h"
#include "C3DPlotDlg.h"
#include "AnalysisDlg.h"
#include "Pegrpapi.h"
#include "Cfilters.h"

#pragma comment(lib, "PEGRP64G.lib")
// C3DPlotDlg dialog

IMPLEMENT_DYNAMIC(C3DPlotDlg, CDialogEx)

C3DPlotDlg::C3DPlotDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG3D, pParent) {}

C3DPlotDlg::~C3DPlotDlg() {}

C3DPlotDlg* C3DPlotDlg::threeDDlgPointer = NULL;
BOOL C3DPlotDlg::OnInitDialog() {
	CDialogEx::OnInitDialog();
	__super::OnInitDialog();
	threeDDlgPointer = this;

	return true;
}

void C3DPlotDlg::DoDataExchange(CDataExchange* pDX) {
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(C3DPlotDlg, CDialogEx)
	//ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_MESSAGE(IDC_RENDER_3DX, &C3DPlotDlg::OnRENDER_GDILEGACYTypeChanged)
END_MESSAGE_MAP()

// C3DPlotDlg message handlers

void C3DPlotDlg::copyData() {
	AnalysisDlg* analysisdlgobject = AnalysisDlg::analysisDlgPointer;
	imgData3D.clear();
	imgData3D = analysisdlgobject->data;

	/*Cfilters filterobject;
	filterobject.ApplySmoothing(imgData3D);
	filterobject.removeOutliers(imgData3D);*/

	if (imgData3D.size() > 0) {
		if (m_hPE)PEdestroy(m_hPE);
		Create3DContouredSurface();
	}
	//::InvalidateRect(m_hPE, 0, FALSE);
}

void C3DPlotDlg::OnDestroy() {
	CDialogEx::OnDestroy();
}

LRESULT C3DPlotDlg::OnRENDER_GDILEGACYTypeChanged(WPARAM nMetaFileType, LPARAM lparam) {
	if (nMetaFileType != m_nRENDER_GDILEGACYType) {
		m_nRENDER_GDILEGACYType = nMetaFileType;
		PEnset(m_hPE, PEP_nRENDERENGINE, m_nRENDER_GDILEGACYType);

		PEreinitialize(m_hPE);
		PEresetimage(m_hPE, 0, 0);
		Invalidate();
	}
	return 1;
}

BOOL C3DPlotDlg::OnCommand(WPARAM wParam, LPARAM lParam) {
	// TODO: Add your specialized code here and/or call the base class
	if (lParam != (LPARAM)m_hPE)
		return CDialogEx::OnCommand(wParam, lParam);
	TCHAR  buffer[128];
	double dX;
	PEvget(m_hPE, PEP_fCURSORVALUEX, &dX);

	double dY;
	PEvget(m_hPE, PEP_fCURSORVALUEY, &dY);

	double dZ;
	PEvget(m_hPE, PEP_fCURSORVALUEZ, &dZ);

	POINT pt;
	HOTSPOTDATA hsd;

	// get last mouse location within control //
	PEvget(m_hPE, PEP_ptLASTMOUSEMOVE, &pt);

	// call PEgethotspot //
	PEgethotspot(m_hPE, pt.x, pt.y);

	// now look at HotSpotData structure //
	PEvget(m_hPE, PEP_structHOTSPOTDATA, &hsd);
	PEnset(m_hPE, PEP_dwTRACKINGTOOLTIPBKCOLOR, PERGB(0, 50, 50, 50));
	PEnset(m_hPE, PEP_dwTRACKINGTOOLTIPTEXTCOLOR, PERGB(0, 100, 255, 50));

	swprintf_s(buffer, TEXT(" X=%.2f\t Y=%.2f\t Height=%.2f"), dZ, dX, dY);
	PEszset(m_hPE, PEP_szTRACKINGTEXT, buffer);
	return true;
	/*if (hsd.nHotSpotType == PEHS_DATAPOINT)
	{
	}*/

	//return CDialogEx::OnCommand(wParam, lParam);
}

void C3DPlotDlg::Create3DContouredSurface() {
	RECT rect;
	GetClientRect(&rect);
	//PEreset(m_hWnd);
	m_hPE = PEcreate(PECONTROL_3D, WS_VISIBLE, &rect, m_hWnd, 1001);
	/*PEreinitialize(m_hPE);
	PEresetimage(m_hPE, 0, 0);*/
	//PEreset(m_hPE);
	::InvalidateRect(m_hPE, 0, FALSE);
	// Enable smoother rotation and zooming //
	PEnset(m_hPE, PEP_nSCROLLSMOOTHNESS, 4);
	PEnset(m_hPE, PEP_nMOUSEWHEELZOOMSMOOTHNESS, 4);
	PEnset(m_hPE, PEP_nPINCHZOOMSMOOTHNESS, 2);

	PEnset(m_hPE, PEP_bDEGREEPROMPTING, TRUE);
	PEnset(m_hPE, PEP_nROTATIONSPEED, 500);
	PEnset(m_hPE, PEP_nROTATIONINCREMENT, PERI_INCBY1);
	PEnset(m_hPE, PEP_nROTATIONDETAIL, PERD_FULLDETAIL);
	PEnset(m_hPE, PEP_nROTATIONMENU, PEMC_GRAYED);
	PEnset(m_hPE, PEP_nDEGREEOFROTATION, 180);
	// Hide Axes //
   /* PEnset(m_hPE, PEP_nSHOWXAXIS, PESA_EMPTY);
	PEnset(m_hPE, PEP_nSHOWYAXIS, PESA_EMPTY);
	PEnset(m_hPE, PEP_nSHOWZAXIS, PESA_EMPTY);*/

	// Set eye/camera distance, or Zoom amount //
   /* PEnset(m_hPE, PEP_nVIEWINGHEIGHT, 23);
	PEnset(m_hPE, PEP_nDEGREEOFROTATION, 44);*/

	//Set PlottingMethod //
	PEnset(m_hPE, PEP_nPLOTTINGMETHOD, 4);  // Surface with contours
	PEnset(m_hPE, PEP_nCPLOTTINGMETHOD, 4);

	//LOG //

	//Axis scale
	PEnset(m_hPE, PEP_nXAXISSCALECONTROL, PEAC_NORMAL);//axis type
	PEnset(m_hPE, PEP_nYAXISSCALECONTROL, PEAC_NORMAL);//axis type
	PEnset(m_hPE, PEP_nZAXISSCALECONTROL, PEAC_NORMAL);//axis type

	// Axis label
	TCHAR szBufX[] = TEXT("X um");
	PEszset(m_hPE, PEP_szXAXISLABEL, szBufX);
	TCHAR szBufZ[] = TEXT("Z um");
	PEszset(m_hPE, PEP_szYAXISLABEL, szBufZ);
	TCHAR szBufY[] = TEXT("Y um");
	PEszset(m_hPE, PEP_szZAXISLABEL, szBufY);

	// Set a light rotation location //
	Point3D p3d;
	p3d.X = 50.50F; p3d.Y = 50.5F; p3d.Z = 50.25F;
	PEvset(m_hPE, PEP_struct3DXLIGHT0, &p3d, 1);

	// Set eye/camera distance, or Zoom amount //
	float fMyZoom = -25.0F;
	PEvset(m_hPE, PEP_f3DXZOOM, &fMyZoom, 1);

	// Set a chart translation //
	/*float fTweakX = -7.8F; PEvset(m_hPE, PEP_f3DXVIEWPORTX, &fTweakX, 1);
	float fTweakY = .8F;   PEvset(m_hPE, PEP_f3DXVIEWPORTY, &fTweakY, 1);*/

	// Enable mouse dragging //
	PEnset(m_hPE, PEP_bMOUSEDRAGGINGX, TRUE);
	PEnset(m_hPE, PEP_bMOUSEDRAGGINGY, TRUE);

	// Enable DegreePrompting, to view rotation, zoom, light location to aid
	// in determining different default values for such properties //
	PEnset(m_hPE, PEP_bDEGREEPROMPTING, TRUE);

	// Reduce the terrain height with respect to surface area
	float fX = 10.0F;  PEvset(m_hPE, PEP_fGRIDASPECTX, &fX, 1);
	float fZ = 10.0F;  PEvset(m_hPE, PEP_fGRIDASPECTZ, &fZ, 1);
	PEnset(m_hPE, PEP_n3DXFITCONTROLSHAPE, FALSE);

	// Pass Data //
	long Row, Col, o = 0, e, s, nQ;
	long nStartRow, nEndRow, nStartCol, nEndCol, nTargetRows, nTargetCols;
	e = 0.0F;

	nTargetRows = imgData3D.size();
	nTargetCols = imgData3D[0].size();
	nQ = (nTargetRows - 1) * (nTargetCols - 1);
	float* pMyXData = NULL;
	float* pMyYData = NULL;
	float* pMyZData = NULL;

	pMyXData = new float[nQ]; // v9.5
	pMyYData = new float[nQ];
	pMyZData = new float[nQ]; // v9.5

	long rowcounter, colcounter;
	BOOL foundtargetindex = FALSE;
	rowcounter = 0;
	colcounter = 0;
	int cnt = 0;

	for (Row = 0; Row < nTargetRows - 1; Row++) {
		for (Col = 0; Col < nTargetCols - 1; Col++) {
			pMyXData[o] = Col;
			pMyZData[o] = Row;
			pMyYData[o] = imgData3D[Row][Col];//(pElevData[(Row * 1500) + Col] * 0.1)
			o++;
		}
	}

	PEnset(m_hPE, PEP_nSUBSETS, nTargetRows - 1);
	PEnset(m_hPE, PEP_nPOINTS, nTargetCols - 1);
	// Perform the actual transfer of data //
	PEvset(m_hPE, PEP_faXDATA, pMyXData, nQ);
	PEvset(m_hPE, PEP_faYDATA, pMyYData, nQ);
	PEvset(m_hPE, PEP_faZDATA, pMyZData, nQ);
	PEnset(m_hPE, PEP_nDUPLICATEXDATA, PEDD_POINTINCREMENT);  // v9.5
	PEnset(m_hPE, PEP_nDUPLICATEZDATA, PEDD_SUBSETINCREMENT);  // v9.5

	if (pMyXData)
		delete pMyXData;
	if (pMyYData)
		delete pMyYData;
	if (pMyZData)
		delete pMyZData;

	//Color Bar [Legend]
	int nTypes[2];
	TCHAR szText[] = TEXT("Upper Boundary\tLower Boundary\t");
	nTypes[0] = PEGAT_PLUS;
	nTypes[1] = PEGAT_CROSS;
	PEnset(m_hPE, PEP_nLEGENDLOCATION, PELL_RIGHT);
	PEnset(m_hPE, PEP_bSHOWLEGEND, TRUE);
	PEvset(m_hPE, PEP_naLEGENDANNOTATIONTYPE, nTypes, 0);
	PEvset(m_hPE, PEP_szaLEGENDANNOTATIONTEXT, szText, 1);
	PEnset(m_hPE, PEP_bSHOWANNOTATIONS, TRUE);
	PEnset(m_hPE, PEP_nLEGENDLOCATIONMENU, PEMC_SHOW);

	//color bar and contour
	PEnset(m_hPE, PEP_nCONTOURCOLORBLENDS, 8);  // this must be set before COLORSET, COLORSET ALWAYS LAST
	PEnset(m_hPE, PEP_nCONTOURCOLORSET, PECCS_BLUE_GREEN_RED);//PECCS_BLUE_GREEN_YELLOW_RED
	PEnset(m_hPE, PEP_nSHOWCONTOUR, PESC_NONE);//PESC_BOTTOMLINES
	//PEnset(m_hPE, PEP_nCONTOURLEGENDPRECISION, 3);

	PEnset(m_hPE, PEP_nAUTOMINMAXPADDING, 0);
	PEnset(m_hPE, PEP_bSURFACENULLDATAGAPS, TRUE);
	PEnset(m_hPE, PEP_bCONTOURSTYLELEGEND, TRUE);//TRUE

	//Set various other properties //
	//font
	PEnset(m_hPE, PEP_bFIXEDFONTS, TRUE);
	PEnset(m_hPE, PEP_szLABELFONT, 10);

	//background color

	PEnset(m_hPE, PEP_dwGRAPHFORECOLOR, PERGB(255, 0, 0, 0));
	PEnset(m_hPE, PEP_dwTABACKCOLOR, PERGB(255, 0, 0, 0));

	// Main title and sub title
	TCHAR mainTitle[] = TEXT("||");
	TCHAR subTitle[] = TEXT("||");
	TCHAR subSubTitle[] = TEXT("||\n Height[um]");
	PEszset(m_hPE, PEP_szMAINTITLE, mainTitle);
	PEszset(m_hPE, PEP_szSUBTITLE, subTitle);
	PEvsetcell(m_hPE, PEP_szaMULTISUBTITLES, 0, subSubTitle);

	PEnset(m_hPE, PEP_nRENDERENGINE, PERE_DIRECT3D);
	PEnset(m_hPE, PEP_bBITMAPGRADIENTMODE, TRUE);//TRUE
	PEnset(m_hPE, PEP_nQUICKSTYLE, PEQS_DARK_NO_BORDER);

	PEnset(m_hPE, PEP_bFIXEDFONTS, TRUE);
	PEnset(m_hPE, PEP_nFONTSIZE, PEFS_MEDIUM);
	PEnset(m_hPE, PEP_bPREPAREIMAGES, TRUE);
	PEnset(m_hPE, PEP_bCACHEBMP, TRUE);
	PEnset(m_hPE, PEP_bFOCALRECT, FALSE);
	PEnset(m_hPE, PEP_nSHOWBOUNDINGBOX, PESBB_NEVER);
	PEnset(m_hPE, PEP_nSHADINGSTYLE, PESS_WHITESHADING);
	PEnset(m_hPE, PEP_nROTATIONDETAIL, PERD_FULLDETAIL);

	PEnset(m_hPE, PEP_nIMAGEADJUSTLEFT, 0);
	PEnset(m_hPE, PEP_nIMAGEADJUSTRIGHT, 0);
	PEnset(m_hPE, PEP_nIMAGEADJUSTBOTTOM, 0);

	PEnset(m_hPE, PEP_nTEXTSHADOWS, PETS_BOLD_TEXT);
	PEnset(m_hPE, PEP_bLABELBOLD, TRUE);

	//! There are different plotting method values for each case of PolyMode  //
	PEnset(m_hPE, PEP_bALLOWWIREFRAME, FALSE);
	PEnset(m_hPE, PEP_nCONTOURMENU, PEMC_HIDE);

	// Set export defaults //
	PEnset(m_hPE, PEP_nDPIX, 600);
	PEnset(m_hPE, PEP_nDPIY, 600);
	PEnset(m_hPE, PEP_nEXPORTSIZEDEF, PEESD_NO_SIZE_OR_PIXEL);
	PEnset(m_hPE, PEP_nEXPORTTYPEDEF, PEETD_PNG);
	PEnset(m_hPE, PEP_nEXPORTDESTDEF, PEEDD_CLIPBOARD);
	TCHAR exportX[] = TEXT("1280");
	TCHAR exportY[] = TEXT("768");
	PEszset(m_hPE, PEP_szEXPORTUNITXDEF, exportX);
	PEszset(m_hPE, PEP_szEXPORTUNITYDEF, exportY);
	PEnset(m_hPE, PEP_nEXPORTIMAGEDPI, 300);
	PEnset(m_hPE, PEP_bALLOWTEXTEXPORT, FALSE);
	PEnset(m_hPE, PEP_bALLOWEMFEXPORT, FALSE);
	PEnset(m_hPE, PEP_bALLOWWMFEXPORT, FALSE);
	PEnset(m_hPE, PEP_bALLOWSVGEXPORT, FALSE);

	PEnset(m_hPE, PEP_bALLOWDATAHOTSPOTS, TRUE);
	PEnset(m_hPE, PEP_bCURSORPROMPTTRACKING, TRUE);
	PEnset(m_hPE, PEP_bTRACKINGCUSTOMDATATEXT, TRUE);
	PEnset(m_hPE, PEP_nCURSORPROMPTSTYLE, 2);
	PEnset(m_hPE, PEP_nDATAPRECISION, 2);
	PEnset(m_hPE, PEP_dw3DHIGHLIGHTCOLOR, PERGB(255, 255, 0, 0));

	PEnset(m_hPE, PEP_nDATASHADOWMENU, PEMC_SHOW);

	PEnset(m_hPE, PEP_bALLOWANNOTATIONCONTROL, TRUE);
	PEnset(m_hPE, PEP_nSHOWANNOTATIONTEXTMENU, PEMC_SHOW);
	PEnset(m_hPE, PEP_nANNOTATIONTEXTFIXEDSIZEMENU, PEMC_SHOW);

	float f;  double d;  	int t;
	int aCnt = 0;

	PEnset(m_hPE, PEP_bSHOWANNOTATIONS, TRUE);
	PEnset(m_hPE, PEP_bSHOWGRAPHANNOTATIONS, TRUE);
	PEnset(m_hPE, PEP_bLEFTJUSTIFICATIONOUTSIDE, TRUE);
	PEnset(m_hPE, PEP_bGRAPHANNOTATIONSYMBOLOBSTACLES, TRUE);
	PEnset(m_hPE, PEP_nGRAPHANNOTATIONTEXTSIZE, 110);
	PEnset(m_hPE, PEP_bALLOWGRAPHANNOTHOTSPOTS, FALSE);

	// create a custom menu to toggle PEP_nVIEWINGMODE
	TCHAR customText[] = TEXT("");
	PEvsetcell(m_hPE, PEP_szaCUSTOMMENUTEXT, 0, customText);
	t = PECMS_CHECKED; PEvsetcellEx(m_hPE, PEP_naCUSTOMMENUSTATE, 0, 0, &t);
	t = PECML_ABOVE_SEPARATOR; PEvsetcell(m_hPE, PEP_naCUSTOMMENULOCATION, 0, &t);
	PEvsetcell(m_hPE, PEP_szaMULTISUBTITLES, 1, customText);

	Point3D p3;  // Set a default ViewingAt location to first graph annotation, index 0
	PEvgetcell(m_hPE, PEP_faGRAPHANNOTATIONX, 0, &d);  p3.X = (float)d;
	PEvgetcell(m_hPE, PEP_faGRAPHANNOTATIONY, 0, &d);  p3.Y = (float)d;
	PEvgetcell(m_hPE, PEP_faGRAPHANNOTATIONZ, 0, &d);  p3.Z = (float)d;
	PEvset(m_hPE, PEP_structVIEWINGAT, &p3, 1);

	PEnset(m_hPE, PEP_nRENDERENGINE, PERE_DIRECT3D);

	PEnset(m_hPE, PEP_bFORCE3DXVERTICEREBUILD, TRUE);
	PEnset(m_hPE, PEP_bFORCE3DXANNOTVERTICEREBUILD, TRUE);
}