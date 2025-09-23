// C2DPlotDlg.cpp : implementation file
//

#include "pch.h"
#include "WLI.h"
#include "afxdialogex.h"
#include "C2DPlotDlg.h"
#include "AnalysisDlg.h"
#include "Pegrpapi.h"
#include "Cfilters.h"

#pragma comment(lib, "PEGRP64G.lib")
// C2DPlotDlg dialog

IMPLEMENT_DYNAMIC(C2DPlotDlg, CDialogEx)

C2DPlotDlg::C2DPlotDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG2D, pParent) {}

C2DPlotDlg::~C2DPlotDlg() {}

C2DPlotDlg* C2DPlotDlg::tdDlgPointer = NULL;
BOOL C2DPlotDlg::OnInitDialog() {
	CDialogEx::OnInitDialog();
	__super::OnInitDialog();
	tdDlgPointer = this;

	return true;
}

void C2DPlotDlg::DoDataExchange(CDataExchange* pDX) {
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(C2DPlotDlg, CDialogEx)
	ON_WM_DESTROY()
	ON_MESSAGE(IDC_RENDER_3DX, &C2DPlotDlg::OnRENDER_GDILEGACYTypeChanged)
END_MESSAGE_MAP()

// C2DPlotDlg message handlers

void C2DPlotDlg::copyData() {
	AnalysisDlg* analysisdlgobject = AnalysisDlg::analysisDlgPointer;
	imgData2D.clear();
	imgData2D = analysisdlgobject->data;
	Cfilters filterobject;
	//filterobject.ApplySmoothing(imgData2D);
	//filterobject.removeOutliers(imgData2D);

	if (imgData2D.size() > 0) {
		if (m_hPE)PEdestroy(m_hPE);
		Create2DContouredSurface();
	}
}

void C2DPlotDlg::OnDestroy() {
	CDialogEx::OnDestroy();
}

LRESULT C2DPlotDlg::OnRENDER_GDILEGACYTypeChanged(WPARAM nMetaFileType, LPARAM lparam) {
	if (nMetaFileType != m_nRENDER_GDILEGACYType) {
		m_nRENDER_GDILEGACYType = nMetaFileType;
		PEnset(m_hPE, PEP_nRENDERENGINE, m_nRENDER_GDILEGACYType);

		PEreinitialize(m_hPE);
		PEresetimage(m_hPE, 0, 0);
		Invalidate();
	}
	return 1;
}

BOOL C2DPlotDlg::OnCommand(WPARAM wParam, LPARAM lParam) {
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
	if (hsd.nHotSpotType == PEHS_DATAPOINT) {
		swprintf_s(buffer, TEXT("X=%.2f\t  Y=%.2f"), dX, dY);
		PEszset(m_hPE, PEP_szTRACKINGTEXT, buffer);
		return true;
	}

	//return CDialogEx::OnCommand(wParam, lParam);
}

void C2DPlotDlg::Create2DContouredSurface() {
	RECT rect;
	GetClientRect(&rect);
	//UpdateWindow();
	// Construct Object //
	m_hPE = PEcreate(PECONTROL_SGRAPH, WS_VISIBLE, &rect, m_hWnd, 1001);//PECONTROL_SGRAPH

	// Enable middle mouse dragging //
	PEnset(m_hPE, PEP_bMOUSEDRAGGINGX, TRUE);
	PEnset(m_hPE, PEP_bMOUSEDRAGGINGY, TRUE);

	// Axis label
	TCHAR szBufX[] = TEXT("X um");
	PEszset(m_hPE, PEP_szXAXISLABEL, szBufX);
	TCHAR szBufZ[] = TEXT("Y um");
	PEszset(m_hPE, PEP_szYAXISLABEL, szBufZ);
	TCHAR szBufY[] = TEXT("Z um");
	PEszset(m_hPE, PEP_szZAXISLABEL, szBufY);

	long Row, Col, s;
	int rowNumber = imgData2D.size();
	int colNumber = imgData2D[0].size();
	int dataLength = rowNumber * colNumber;

	float* pMyXData = new float[dataLength];
	float* pMyYData = new float[dataLength];
	float* pMyZData = new float[dataLength];

	int pos = 0;
	for (Row = 0; Row < rowNumber; Row++) {
		for (Col = 0; Col < colNumber; Col++) {
			pMyXData[pos] = Col; //X axis
			pMyYData[pos] = Row; //Z axis
			pMyZData[pos] = imgData2D[Row][Col]; //Data for plot
			pos++;
		}
	}

	PEnset(m_hPE, PEP_nSUBSETS, rowNumber);
	PEnset(m_hPE, PEP_nPOINTS, colNumber);

	//Perform the actual transfer of data //
	PEvset(m_hPE, PEP_faXDATA, pMyXData, dataLength);
	PEvset(m_hPE, PEP_faYDATA, pMyYData, dataLength);//height data
	PEvset(m_hPE, PEP_faZDATA, pMyZData, dataLength);

	if (pMyXData)
		delete pMyXData;
	if (pMyYData)
		delete pMyYData;
	if (pMyZData)
		delete pMyZData;

	// Set Contour Color regions //
	PEnset(m_hPE, PEP_nCONTOURCOLORBLENDS, 8);
	PEnset(m_hPE, PEP_nCONTOURCOLORSET, PECCS_BLUE_GREEN_RED);//PECCS_BLUE_GREEN_YELLOW_RED
	PEnset(m_hPE, PEP_nCONTOURCOLORALPHA, 255);

	//PEnset(m_hPE, PEP_nAUTOMINMAXPADDING, 0);

	PEnset(m_hPE, PEP_bSHOWLEGEND, FALSE);
	PEnset(m_hPE, PEP_bCONTOURSTYLELEGEND, FALSE);
	PEnset(m_hPE, PEP_bGRAPHDATALABELS, TRUE);
	//PEnset(m_hPE, PEP_nLEGENDLOCATION, PELL_LEFT);

	// Optional, instead of showing default numeric labels, replace with custom strings
	//PEvsetcell( m_hPE, PEP_szaCONTOURLABELS, 0, TEXT("0"));
	//PEvsetcell( m_hPE, PEP_szaCONTOURLABELS, 19, TEXT("20" ));
	//PEvsetcell( m_hPE, PEP_szaCONTOURLABELS, 39, TEXT("40" ));
	//PEvsetcell( m_hPE, PEP_szaCONTOURLABELS, 59, TEXT("60" ));
	//PEnset(m_hPE, PEP_nCONTOURCOLORSET, PECCS_BLUE_GREEN_YELLOW_RED);

	// Set Various Other Properties ///
	PEnset(m_hPE, PEP_bBITMAPGRADIENTMODE, TRUE);
	PEnset(m_hPE, PEP_nQUICKSTYLE, PEQS_DARK_NO_BORDER);
	PEnset(m_hPE, PEP_nBORDERTYPES, PETAB_NO_BORDER);

	// Set the plotting method //
	PEnset(m_hPE, PEP_bALLOWCONTOURCOLORS, TRUE);
	PEnset(m_hPE, PEP_bALLOWCONTOURLINES, TRUE);
	PEnset(m_hPE, PEP_bALLOWCONTOURCOLORSSHADOWS, TRUE);

	PEnset(m_hPE, PEP_bCONTOURLINESCOLORED, TRUE);
	PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_CONTOURCOLORS);
	PEnset(m_hPE, PEP_nDATASHADOWMENU, PEMC_HIDE);

	PEnset(m_hPE, PEP_bGRIDINFRONT, TRUE);
	PEnset(m_hPE, PEP_nGRIDLINECONTROL, PEGLC_BOTH);
	PEnset(m_hPE, PEP_nGRIDSTYLE, PEGS_DOT);
	PEnset(m_hPE, PEP_bPREPAREIMAGES, TRUE);
	PEnset(m_hPE, PEP_bCACHEBMP, TRUE);
	PEnset(m_hPE, PEP_nZOOMSTYLE, PEZS_RO2_NOT);

	// Disable other non contour plotting method //
	PEnset(m_hPE, PEP_bALLOWLINE, FALSE);
	PEnset(m_hPE, PEP_bALLOWPOINT, FALSE);
	PEnset(m_hPE, PEP_bALLOWBAR, FALSE);
	PEnset(m_hPE, PEP_bALLOWAREA, FALSE);
	PEnset(m_hPE, PEP_bALLOWSPLINE, FALSE);
	PEnset(m_hPE, PEP_bALLOWSPLINEAREA, FALSE);
	PEnset(m_hPE, PEP_bALLOWPOINTSPLUSLINE, FALSE);
	PEnset(m_hPE, PEP_bALLOWPOINTSPLUSSPLINE, FALSE);
	PEnset(m_hPE, PEP_bALLOWBESTFITCURVE, FALSE);
	PEnset(m_hPE, PEP_bALLOWBESTFITLINE, FALSE);
	PEnset(m_hPE, PEP_bALLOWSTICK, FALSE);

	// Main title and sub title
	TCHAR mainTitle[] = TEXT("||");
	TCHAR subTitle[] = TEXT("||");
	TCHAR subSubTitle[] = TEXT("||");//\n Height[um]
	PEszset(m_hPE, PEP_szMAINTITLE, mainTitle);
	PEszset(m_hPE, PEP_szSUBTITLE, subTitle);
	PEvsetcell(m_hPE, PEP_szaMULTISUBTITLES, 0, subSubTitle);

	// Enable Zooming //
	PEnset(m_hPE, PEP_nALLOWZOOMING, PEAZ_HORZANDVERT);
	PEnset(m_hPE, PEP_nMOUSEWHEELFUNCTION, PEMWF_HORZPLUSVERT_ZOOM);
	PEnset(m_hPE, PEP_nMOUSEWHEELZOOMSMOOTHNESS, 4);
	PEnset(m_hPE, PEP_bGRIDBANDS, FALSE);
	float fZ = 2.00F; PEvset(m_hPE, PEP_fMOUSEWHEELZOOMFACTOR, &fZ, 1);

	// Set small font size //
	PEnset(m_hPE, PEP_nFONTSIZE, PEFS_SMALL);
	PEnset(m_hPE, PEP_bFIXEDFONTS, TRUE);

	// Disable appropriate tabs //
	PEnset(m_hPE, PEP_bALLOWAXISPAGE, FALSE);
	PEnset(m_hPE, PEP_bALLOWSTYLEPAGE, TRUE);
	PEnset(m_hPE, PEP_bALLOWSUBSETSPAGE, FALSE);

	PEnset(m_hPE, PEP_bSCROLLINGVERTZOOM, TRUE);
	PEnset(m_hPE, PEP_bSCROLLINGHORZZOOM, TRUE);

	PEnset(m_hPE, PEP_nTEXTSHADOWS, PETS_BOLD_TEXT);
	PEnset(m_hPE, PEP_bMAINTITLEBOLD, TRUE);
	PEnset(m_hPE, PEP_bSUBTITLEBOLD, TRUE);
	PEnset(m_hPE, PEP_bLABELBOLD, TRUE);

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

	PEnset(m_hPE, PEP_bCURSORPROMPTTRACKING, TRUE);
	PEnset(m_hPE, PEP_nCURSORPROMPTSTYLE, PECPS_ZVALUE);

	// v9 features
	//PEszset(m_hPE, PEP_szTRACKINGTOOLTIPTITLE, TEXT("Value at point: "));
	PEnset(m_hPE, PEP_nCURSORPROMPTLOCATION, 2);
	PEnset(m_hPE, PEP_bTRACKINGCUSTOMDATATEXT, TRUE);

	// v9 features
	PEnset(m_hPE, PEP_bGRAPHBMPALWAYS, TRUE);
	PEnset(m_hPE, PEP_dwGRAPHBACKCOLOR, 5);
	//PEszset(m_hPE, PEP_szGRAPHBMPFILENAME, TEXT("usmid.jpg"));
	PEnset(m_hPE, PEP_nGRAPHBMPSTYLE, PEBS_BITBLT_ZOOMING);

	// v9 features
	// optionally control the relationship between the zoomable background bitmap and the data units.
	//double dX;
	//dX = 200.0F;
	//PEvsetW(m_hPE, PEP_fGRAPHBMPMINX, &dX, 1);
	//dX = 260.0F;
	//PEvsetW(m_hPE, PEP_fGRAPHBMPMAXX, &dX, 1);
	//dX = 100.0F;
	//PEvsetW(m_hPE, PEP_fGRAPHBMPMINY, &dX, 1);
	//dX = 160.0F;
	//PEvsetW(m_hPE, PEP_fGRAPHBMPMAXY, &dX, 1);

	PEnset(m_hPE, PEP_nRENDERENGINE, PERE_DIRECT3D);
	PEnset(m_hPE, PEP_nCOMPOSITE2D3D, PEC2D_FOREGROUND);

	PEnset(m_hPE, PEP_bFORCE3DXVERTICEREBUILD, TRUE);
	PEnset(m_hPE, PEP_bFORCE3DXNEWCOLORS, TRUE);

	PEnset(m_hPE, PEP_bALLOWDATAHOTSPOTS, TRUE);
}