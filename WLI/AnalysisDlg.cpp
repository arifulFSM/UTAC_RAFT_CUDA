// AnalysisDlg.cpp : implementation file
//

#include "pch.h"
#include "WLI.h"
#include "AnalysisDlg.h"
#include "afxdialogex.h"
#include "C2DPlotDlg.h"
#include "C3DPlotDlg.h"
#include <fstream>
#include <sstream>
#include <vector>

// AnalysisDlg dialog

IMPLEMENT_DYNAMIC(AnalysisDlg, CResizableDialog)

AnalysisDlg::AnalysisDlg(CWnd* pParent /*=nullptr*/)
	: CResizableDialog(IDD_ANALYSIS_DLG, pParent) {
}

AnalysisDlg::~AnalysisDlg() {}

void AnalysisDlg::DoDataExchange(CDataExchange* pDX) {
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB2D3D, m_Tab);
}

BEGIN_MESSAGE_MAP(AnalysisDlg, CResizableDialog)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_DATA, &AnalysisDlg::OnBnClickedButtonLoadData)
END_MESSAGE_MAP()

// AnalysisDlg message handlers

AnalysisDlg* AnalysisDlg::analysisDlgPointer = NULL;
BOOL AnalysisDlg::OnInitDialog() {
	__super::OnInitDialog();
	analysisDlgPointer = this;

	short nTab = 0;
	tdPlotDlg = new C2DPlotDlg;
	if (tdPlotDlg) {
		tdPlotDlg->Create(IDD_DLG2D, &m_Tab);
		m_Tab.AddTab(tdPlotDlg, CString("2D Plot").GetBuffer(), nTab++);
	}

	threeDPlotDlg = new C3DPlotDlg;
	if (threeDPlotDlg) {
		threeDPlotDlg->Create(IDD_DLG3D, &m_Tab);
		m_Tab.AddTab(threeDPlotDlg, CString("3D Plot").GetBuffer(), nTab++);
	}

	return true;
}

void AnalysisDlg::OnBnClickedButtonLoadData() {
	// TODO: Add your control notification handler code here

	data.clear();
	UpdateData(FALSE);
	Invalidate();
	CFileDialog dlg(TRUE, _T(".csv"), NULL, OFN_FILEMUSTEXIST, _T("CSV Files (*.csv)|*.csv||"), this);
	if (dlg.DoModal() == IDOK) {
		CString filePath = dlg.GetPathName();
		std::ifstream file(filePath);
		std::string line;
		while (std::getline(file, line)) {
			std::istringstream lineStream(line);
			std::string value;
			std::vector<float>vec;
			while (std::getline(lineStream, value, ',')) {
				vec.push_back(stod(value));
			}
			data.push_back(vec);
		}
		size_t row = data.size();
		size_t col = data[0].size();
	}
	float mxVal = -1000, mnVal = 1000, mxX, mxY, mnX, mnY;
	for (int i = 0; i < data.size(); i++) {
		for (int j = 0; j < data[0].size(); ++j) {
			if (mxVal <= data[i][j]) {
				mxVal = data[i][j];
				mxX = i, mxY = j;
			}
			if (mnVal > data[i][j]) {
				mnVal = data[i][j];
				mnX = i, mnY = j;
			}
		}
	}

	mxVal, mnVal;

	if (data.size() > 0) {
		C3DPlotDlg* threeDPlotObject = C3DPlotDlg::threeDDlgPointer;
		threeDPlotObject->copyData();

		LPARAM lParam = 0;
		HWND hWnd = threeDPlotObject->GetSafeHwnd();
		::SendMessage(hWnd, IDC_RENDER_3DX, 5, lParam);

		C2DPlotDlg* tdPlotObject = C2DPlotDlg::tdDlgPointer;
		tdPlotObject->copyData();
		lParam = 0;
		hWnd = tdPlotObject->GetSafeHwnd();
		::SendMessage(hWnd, IDC_RENDER_3DX, 5, lParam);
		Invalidate(true);
	}
}