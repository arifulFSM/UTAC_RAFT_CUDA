// MeasurementDlg.cpp : implementation file
//

#include "pch.h"
#include "WLI.h"
#include "MeasurementDlg.h"
#include "afxdialogex.h"
#include "RAFTApp.h"
#include "Dev.h"
#include "WLIView.h"
#include "PSI/Strip.h"
#include <thread>
#include <MotionControlDlg.h> //Morsalinn
#include <chrono> // Morsalinn
#include "MTH/LSF3D.h"//20250916

// MeasurementDlg dialog

IMPLEMENT_DYNAMIC(MeasurementDlg, CResizableDialog)

MeasurementDlg::MeasurementDlg(CWnd* pParent /*=nullptr*/)
	: CResizableDialog(IDD_MEASUREMENT_DLG, pParent) {
	if (progress == nullptr) {
		progress = new MeasProgressDlg();
		progress->Create(IDD_DLG_RCP_MEAS_PBAR);
	}
}

MeasurementDlg::~MeasurementDlg() {
	if (progress != nullptr) {
		delete progress;
		progress = nullptr;
	}
}

void MeasurementDlg::DoDataExchange(CDataExchange* pDX) {
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RESULTS, m_cResults);
	DDX_Control(pDX, IDC_WAFERMAP2, m_cWaferMap);
	DDX_Control(pDX, IDC_CAMERA_MEAS, cLiveVid);
	DDX_Control(pDX, IDC_PROGRESS_MSG, m_ProgressMsg);
	DDX_Control(pDX, IDC_PROGRESS1, m_MeasurementProgress);
	DDX_Control(pDX, IDC_PROGRESS_COUNT, m_ProgressCount);
}

BEGIN_MESSAGE_MAP(MeasurementDlg, CResizableDialog)
	/*ON_MESSAGE(IDC_ADDPOINT, OnAddPoint)
	ON_MESSAGE(IDC_ADDALGNPOINT, OnAddalgnpoint)
	ON_MESSAGE(IDC_DELETEPOINT, OnDeletepoint)*/
	ON_MESSAGE(WM_XTAB_SELECTED, OnTabSelected)
	ON_MESSAGE(WM_XTAB_DESELECTED, OnTabDeselected)
	ON_BN_CLICKED(IDC_ME_LOADRCP, &MeasurementDlg::OnBnClickedMeLoadrcp)
	ON_BN_CLICKED(IDC_Measure, &MeasurementDlg::OnBnClickedMeasure)
	ON_BN_CLICKED(IDC_MOT_SETUP_MD, &MeasurementDlg::OnBnClickedMotSetupMd)
	ON_BN_CLICKED(IDC_CAM_PROP_MD, &MeasurementDlg::OnBnClickedCamPropMd)
END_MESSAGE_MAP()

// MeasurementDlg message handlers
BOOL MeasurementDlg::OnInitDialog() {
	__super::OnInitDialog();

	pRcp = &pRAFTApp->RcpSetup;
	m_cWaferMap.pRcp = pRcp;
	m_cWaferMap.bSiteView = FALSE; // Show recipe points [6/25/2010 Yuen]
	m_cWaferMap.pParent = this;
	m_cWaferMap.Redraw();

	pResult = &pRAFTApp->Result;

	const wchar_t* dwCjName[] =
	{
		L"#", L"Position", L"Sq[um]", L"Sa[um]", L"Sy[um]"
	};
	int nSize[] = { 32, 160, 100, 100, 100 };
	LV_COLUMN nListColumn;
	for (int i = 0; i < sizeof(nSize) / sizeof(int); i++) {
		nListColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		nListColumn.fmt = LVCFMT_LEFT;
		nListColumn.cx = nSize[i];
		nListColumn.iSubItem = 0;
		nListColumn.pszText = (LPWSTR)dwCjName[i];
		m_cResults.InsertColumn(i, &nListColumn);
	}
	m_cResults.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	RecipeToLocal();

	return TRUE;
}

void MeasurementDlg::OnBnClickedMeLoadrcp() {
	// TODO: Add your control notification handler code here
	if (pRcp) {
		if (pRcp->LoadRecipe()) {
			RecipeToLocal();
		}
	}
}
#include <thread>

void MeasurementDlg::DataAcquisitionSimu() {
	// 1. READ FILE AND PROVIDE Z POSITIONS
	Strip.DeallocAll();
	CString folder = L"C:/WLIN_MS/BMP/DATA/";
	CString zFilePath = folder + L"DATA.csv";
	std::string line;
	std::ifstream zFile(zFilePath.GetString());
	if (!zFile.is_open()) {
		AfxMessageBox(L"Failed to open Z positions file", MB_ICONERROR);
		return;
	}
	std::vector<float> zPositions;
	std::getline(zFile, line); // Skip header line if exists
	while (std::getline(zFile, line)) {
		// Read z position from each line
		std::istringstream iss(line);
		std::string col1, col2;
		if (std::getline(iss, col1, ',') && std::getline(iss, col2, ',')) {
			try {
				float z = std::stof(col2);
				zPositions.push_back(z);
			}
			catch (const std::exception&) {
			}
		}
	}

	CString filePath;
	IMGL::CIM tmpImg;

	for (int i = 0; i < 222; ++i) {
		filePath.Format(L"DATA_%d.BMP", i + 1);
		filePath = folder + filePath; // Full path to the image file
		//if (!tmpImg.Load(filePath)) { // Assuming IMGL::CIM::Load returns bool
		if (tmpImg.Load(filePath) == E_FAIL) { // Assuming IMGL::CIM::Load returns bool
			TRACE("Failed to load image: %ws\n", filePath.GetString());
			continue;
		}
		WLI::SIms* pImN = Strip.NewImgs(zPositions[i]); // Use 0.0f or appropriate Z position
		if (pImN) {
			pImN->Im = tmpImg;
			tmpImg.Detach();
		}
	}
}

//20250916
void MeasurementDlg::DataAcquisitionSimuCV() {
	// 1. READ FILE AND PROVIDE Z POSITIONS
	Strip.DeallocAllCV();
	std::string folder = "C:/WLIN_MS/BMP/DATA/";
	std::string zFilePath = folder + "DATA.csv";
	std::string line;
	std::ifstream zFile(zFilePath);
	if (!zFile.is_open()) {
		AfxMessageBox(L"Failed to open Z positions file", MB_ICONERROR);
		return;
	}
	std::vector<float> zPositions;
	std::getline(zFile, line); // Skip header line if exists
	while (std::getline(zFile, line)) {
		// Read z position from each line
		std::istringstream iss(line);
		std::string col1, col2;
		if (std::getline(iss, col1, ',') && std::getline(iss, col2, ',')) {
			try {
				float z = std::stof(col2);
				zPositions.push_back(z);
			}
			catch (const std::exception&) {
			}
		}
	}

	std::string filePath;
	//IMGL::CIM tmpImg;
	cv::Mat tempImg;

	for (int i = 0; i < 222; ++i) {
		//filePath.Format(L"DATA_%d.BMP", i + 1);
		filePath = "Data_" + std::to_string(i) + ".BMP";
		filePath = folder + filePath; // Full path to the image file
		//if (!tmpImg.Load(filePath)) { // Assuming IMGL::CIM::Load returns bool
		tempImg = cv::imread(filePath);
		if (tempImg.empty()) { // Assuming IMGL::CIM::Load returns bool
			TRACE("Failed to load image: %ws\n", filePath);
			continue;
		}
		Strip.CVImgs.push_back({ tempImg, zPositions[i] });
		//WLI::SIms* pImN = Strip.NewImgs(zPositions[i]); // Use 0.0f or appropriate Z position
		//if (pImN) {
		//	pImN->Im = tmpImg;
		//	tmpImg.Detach();
		//}
	}
}

//20250916
void MeasurementDlg::LevelCV(cv::Mat& ImCV) {
	CLSF3D LSF3D;
	int skip = 8;
	for (int y = 0; y < ht; y += skip) {
		float* row = ImCV.ptr<float>(y);
		for (int x = 0; x < wd; x += skip) {
			if (row[x]!= BADDATA) {
				LSF3D.Input(x, y, row[x]);
			}
		}
	}

	// Normalize data: All points [3/10/2011 FSMT]
	float fMin = FLT_MAX; float fMax = -(FLT_MAX - 1);
	double dfA, dfB, dfC;
	if (LSF3D.GetResult(dfA, dfB, dfC)) {

		//// Normalize data by subtracting fitted plane
		//float fMin = std::numeric_limits<float>::max();
		//float fMax = std::numeric_limits<float>::lowest();

		for (int y = 0; y < ht; y++) {
			float* row = ImCV.ptr<float>(y);
			for (int x = 0; x < wd; x++) {
				if (row[x] != BADDATA) {
					float dfNorDist = row[x] - dfC - dfA * x - dfB * y;
					if (dfNorDist > fMax) fMax = dfNorDist;
					if (dfNorDist < fMin) fMin = dfNorDist;
					row[x] = dfNorDist;
				}
			}
		}
		// Add offset to make all values positive
		float off = std::abs(fMin);
		for (int y = 0; y < ht; y++) {
			float* row = ImCV.ptr<float>(y);
			for (int x = 0; x < wd; x++) {
				if (row[x]!=BADDATA) {
					row[x] += off;
				}
			}
		}
		fMax += off; fMin += off;
	}
}


//20250916
bool MeasurementDlg::Histo256(const cv::Mat& image) {
	if (image.empty() || image.type() != CV_32F) return false;

	// Clear histogram
	for (int i = 0; i < 256; ++i) {
		His5.His.push_back(0);
	}
	int ht = image.rows;
	int wd = image.cols;

	// Find min/max values
	float mx = image.at<float>(0,0);
	float mn = image.at<float>(0,0);

	for (int y = 0; y < ht; y++) {
		const float* row = image.ptr<float>(y);
		for (int x = 0; x < wd; x++) {
			if (row[x]!=BADDATA) {
				if (row[x] > mx) mx = row[x];
				if (row[x] < mn) mn = row[x];
			}
		}
	}

	if (mx == mn) return false;

	His5.mx = mx;
	His5.mn = mn;
	His5.sf = His5.nbins / (mx - mn);

	// Build histogram
	for (int y = 0; y < ht; y++) {
		const float* row = image.ptr<float>(y);
		for (int x = 0; x < wd; x++) {
			if (row[x] != BADDATA) {
				int bin = static_cast<int>(His5.sf * (row[x] - mn));
				bin = max(0, min(His5.nbins - 1, bin));
				His5.His[bin]++;
			}
		}
	}

	return true;
}

//20250916 
void MeasurementDlg::Make24HStretchCV(cv::Mat& ImCV) {
	//CV_Assert(ImCV.type() == CV_32F || ImCV.type() == CV_64F);

	//// 1. Find min/max, ignoring outliers if needed
	//double minVal, maxVal;
	//cv::minMaxLoc(ImCV, &minVal, &maxVal);

	//// Optionally ignore outliers using percentiles (see below for advanced)
	//// For most cases, minMaxLoc is sufficient

	//// 2. Normalize to [0, 255]
	//cv::Mat normImg;
	//ImCV.convertTo(normImg, CV_8U, 255.0 / (maxVal - minVal), -minVal * 255.0 / (maxVal - minVal));

	//// 3. Convert to 3-channel grayscale
	//cv::Mat gray3ch;
	//cv::cvtColor(normImg, gray3ch, cv::COLOR_GRAY2BGR);

	//// 4. Optionally, set invalid pixels (e.g., BADDATA) to blue
	//// If you have a mask of invalid pixels, set them:
	//// for (int y = 0; y < ImCV.rows; ++y)
	////     for (int x = 0; x < ImCV.cols; ++x)
	////         if (isBad(ImCV.at<float>(y, x))) gray3ch.at<cv::Vec3b>(y, x) = cv::Vec3b(255, 0, 0);

	//ImCV = gray3ch;


	if (ImCV.empty() || ImCV.type() != CV_32F) return;

	// Create a working copy
	cv::Mat workingImage = ImCV.clone();

	// Level the image (remove tilt)
	LevelCV(workingImage);

	// Build histogram
	if (!Histo256(workingImage)) return;

	int ht = workingImage.rows;
	int wd = workingImage.cols;

	int lim = int(0.0000f * wd * ht);
	int sum = 0, imx = 0, imn = 0;
	for (int i = 0; i < 255; i++) {
		sum += His5.His[i];
		if (sum < lim) His5.His[i] = 0;
		else { imn = i; break; }
	}
	sum = 0;
	for (int i = 255; i >= 0; i--) {
		sum += His5.His[i];
		if (sum < lim) His5.His[i] = 0;
		else { imx = i; break; }
	}

	cv::Mat outImg;
	outImg.create(ht, wd, CV_8UC3);

	//scaling factor
	float sf = His5.sf;
	float min = His5.mn;
	float sf2 = 255.0f / static_cast<float>(imx - imn);

	//convert to 24-bit RGB
	for (int y = 0; y < ht; y++) {
		const float* srcRow = workingImage.ptr<float>(y);
		cv::Vec3b* dstRow = outImg.ptr<cv::Vec3b>(y);
		for (int x = 0; x < wd; ++x) {
			if (srcRow[x] != BADDATA) {
				int scaledValue = static_cast<int>(sf2 * static_cast<int>(sf * (srcRow[x] - min)));
				scaledValue = std::max(0, std::min(255, scaledValue));

				dstRow[x] = cv::Vec3b(scaledValue, scaledValue, scaledValue);
			}
			else {
				dstRow[x] = cv::Vec3b(255, 0, 0); // Mark bad data in blue
			}
		}
	}

	ImCV = outImg;
}

void MeasurementDlg::OnBnClickedMeasure() {
	// TODO: Add your control notification handler code here
	//DataAcquisitionSimu();
	DataAcquisitionSimuCV();
	getHeightData();
	return;

	pRcp->AFCalibZ;  // Uncomment by Morsalin

	IMGL::CIM ImA;
	CAM::ECAM eID = CAM::PRICAM;
	int tryCap = 1;
	while (tryCap++ <= 5 && !Dev.Cam.Grab(ImA, eID, Dev.Cam.pCm[eID]->subSampling)) {
	}
	ImA.Detach();
	if (tryCap == 6) {
		AfxMessageBox(L"Camera Not Running", MB_ICONINFORMATION);
		return;
	}

	//DataAcquisition();
	//return;

	CString str = L"";
	progress->ShowWindow(SW_SHOW);  // Uncomment by Morsalin
	str.Format(L"System Resetting...");
	progress->SetPosProBar(0, 0, str);  // Uncomment by Morsalin
	ShowMessage(str);
	pRcp->UpdateControl(m_cResults);
	pResult->Clear();
	pResult->RecipeName = pRcp->RcpeName;

	int numOfPoint = 0;
	POSITION pos = pRcp->Patt.MP.MPLst.GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)pRcp->Patt.MP.MPLst.GetNext(pos);
		if (p) {
			numOfPoint++;
			p->Co.status = CCoor::MEASURE;
			m_cWaferMap.Redraw();
		}
	}

	int stepNum = 1;
	int measureStep = (numOfPoint * 4) + 4;
	m_MeasurementProgress.SetRange(0, measureStep);
	CString progressCount;
	progressCount.Format(L"%d / %d", stepNum, measureStep);
	m_ProgressCount.SetWindowText(progressCount);
	m_MeasurementProgress.SetPos(stepNum++);

	// Morsalinn [
	using namespace std::chrono;
	auto end = time_point_cast<seconds>(system_clock::now());
	long long end_sec = end.time_since_epoch().count();
	long long diff_sec = end_sec - pRAFTApp->start_sec;

	const long long MS_IN_24_HOURS = 24LL * 60 * 60 * 1000; // 86400000

	if(!pRAFTApp->isReset || diff_sec > MS_IN_24_HOURS)
	{
		//pWLIView->pMSet->Reset();  // Uncomment by Morsalin //12345
		auto start = time_point_cast<seconds>(system_clock::now());
		pRAFTApp->start_sec = start.time_since_epoch().count();
		pRAFTApp->isReset = true;
	}
	// ]
	progressCount.Format(L"%d / %d", stepNum, measureStep);
	m_ProgressCount.SetWindowText(progressCount);
	m_MeasurementProgress.SetPos(stepNum++);

	pos = pRcp->Patt.MP.MPLst.GetHeadPosition();

	CString temp;
	int i = 0, posPt = 0;
	int nItem;

	str.Format(L"Measurement Is Starting...");
	progress->SetPosProBar(0, 0, str);  // Uncomment by Morsalin
	ShowMessage(str);
	bMeasured = TRUE;

	float x, y;
	while (pos) {
		CMPoint* p = (CMPoint*)pRcp->Patt.MP.MPLst.GetNext(pos);
		if (p) {
			//02282024/HAQUE/ADD MEASURING COLOR==================
			p->Co.status = CCoor::MEASURING;
			m_cWaferMap.Redraw();
			//====================================================

			//nItem = 2;
			str.Format(L"Moving Measurement Position-%d...", i + 1);
			progress->SetPosProBar(0, 0, str);  // Uncomment by Morsalin
			ShowMessage(str);
			//if (i != 0) pWLIView->pMSet->LiftZMot(); // 12345
			Dev.MC.get()->stage.GotoXY(p->Co.x, p->Co.y, 10000, true);
			str.Format(L"Adjusting Auto Focus Position-%d...", i + 1);
			progress->SetPosProBar(0, 0, str);  // Uncomment by Morsalin
			ShowMessage(str);
			//pWLIView->pMSet->FringAdjustAF(pRcp->AFCalibZ, pRcp->AFTiltZ, pRcp->AFRange, pRcp->AFStepSize);  // Uncomment by Morsalin //12345
			progressCount.Format(L"%d / %d", stepNum, measureStep);
			m_ProgressCount.SetWindowText(progressCount);
			m_MeasurementProgress.SetPos(stepNum++);

			str.Format(L"Adjusting Centering Position-%d...", i + 1);
			ShowMessage(str);
			//pWLIView->pMSet->CenteringFringe(30, 0.1);  // Uncomment by Morsalin  12345
			progressCount.Format(L"%d / %d", stepNum, measureStep);
			m_ProgressCount.SetWindowText(progressCount);
			m_MeasurementProgress.SetPos(stepNum++);

			str.Format(L"Adjusting Auto Tilt Position-%d...", i + 1);
			ShowMessage(str);

			 //Uncomment by Morsalin
			//if (i == 0) pWLIView->pMSet->ATWithGeo();	//07142025/HAQUE/COMMENT //12345
			//bool ATStatus = pWLIView->pMSet->FringAdjustAT();	//07142025/HAQUE/COMMENT //12345
			if (i == 0 /* || ATStatus */) {
				str.Format(L"Adjusting Auto Focus Position-%d...", i + 1);
				ShowMessage(str);
				//pWLIView->pMSet->FringAdjustAF(pRcp->AFCalibZ, pRcp->AFTiltZ, pRcp->AFRange, pRcp->AFStepSize); //12345
				progressCount.Format(L"%d / %d", stepNum, measureStep);
				m_ProgressCount.SetWindowText(progressCount);
				m_MeasurementProgress.SetPos(stepNum++);
				str.Format(L"Adjusting Centering Position-%d...", i + 1);
				ShowMessage(str);
				//pWLIView->pMSet->CenteringFringe(30, 0.1); //12345
				progressCount.Format(L"%d / %d", stepNum, measureStep);
				m_ProgressCount.SetWindowText(progressCount);
				m_MeasurementProgress.SetPos(stepNum++);
			}

			if (i != 0) {
				str.Format(L"Height Calculation Ongoing Position-%d...", i);
				ShowMessage(str);
			}
			while (true) {
				if (bMeasured) {
					if (i != 0) {
						nItem = 2;
						temp.Format(L"%f", m_fRa1);
						m_cResults.SetItemText(posPt, nItem++, temp);
						temp.Format(L"%f", m_fRrms1);
						m_cResults.SetItemText(posPt, nItem++, temp);
						temp.Format(L"%f", m_fRmax1);
						m_cResults.SetItemText(posPt++, nItem++, temp);
						progressCount.Format(L"%d / %d", stepNum, measureStep);
						m_ProgressCount.SetWindowText(progressCount);
						m_MeasurementProgress.SetPos(stepNum++);
					}
					bMeasured = FALSE;
					break;
				}
				Sleep(10);
			}
			
			str.Format(L"Data Acquisition Ongoing Position-%d...", i + 1);
			ShowMessage(str);
			//return; //12345
			DataAcquisition();
			progressCount.Format(L"%d / %d", stepNum, measureStep);
			m_ProgressCount.SetWindowText(progressCount);
			m_MeasurementProgress.SetPos(stepNum++);
			x = p->Co.x;
			y = p->Co.y;
			std::thread t([&] {
				TRACE("Thread 1");

				getHeightData(posPt + 1);
				TRACE("Height done\n");
				calcRoughness();
				TRACE("Roughness done\n");
				//CString temp;
				/*temp.Format(L"%f", m_fRa1);
				m_cResults.SetItemText(posPt, nItem++, temp);
				temp.Format(L"%f", m_fRrms1);
				m_cResults.SetItemText(posPt, nItem++, temp);
				temp.Format(L"%f", m_fRmax1);
				m_cResults.SetItemText(posPt++, nItem++, temp);
				TRACE("Print List Control\n");*/
				//02282024/HAQUE/ADD MEASURING COLOR==================
				pResult->InsertResult(x, y, m_fRa1, m_fRrms1, m_fRmax1);
				bMeasured = TRUE;
				//TRACE("ppp%d\n", bMeasured);
				});
			t.detach();
			/*str.Format(L"Height Calc Ongoing Position-%d...", i + 1);
			ShowMessage(str);
			getHeightData(i + 1);
			str.Format(L"Roughness Calc Ongoing Position-%d...", i + 1);
			ShowMessage(str);
			calcRoughness();
			str.Format(L"Measurement Done Position-%d", i + 1);
			ShowMessage(str);*/

			/*temp.Format(L"%f", m_fRa1);
			m_cResults.SetItemText(i, nItem++, temp);
			temp.Format(L"%f", m_fRrms1);
			m_cResults.SetItemText(i, nItem++, temp);
			temp.Format(L"%f", m_fRmax1);
			m_cResults.SetItemText(i++, nItem++, temp);*/

			//02282024/HAQUE/ADD MEASURING COLOR==================
			//pResult->InsertResult(p->Co.x, p->Co.y, m_fRa1, m_fRrms1, m_fRmax1);

			i++;
			p->Co.status = CCoor::MEASURED;
			m_cWaferMap.Redraw();
			//====================================================
		}
	}

	str.Format(L"Height Calculation Ongoing Position-%d...", i);
	ShowMessage(str);
	while (true) {
		if (bMeasured) {
			if (i != 0) {
				nItem = 2;
				temp.Format(L"%f", m_fRa1);
				m_cResults.SetItemText(posPt, nItem++, temp);
				temp.Format(L"%f", m_fRrms1);
				m_cResults.SetItemText(posPt, nItem++, temp);
				temp.Format(L"%f", m_fRmax1);
				m_cResults.SetItemText(posPt++, nItem++, temp);
				progressCount.Format(L"%d / %d", stepNum, measureStep);
				m_ProgressCount.SetWindowText(progressCount);
				m_MeasurementProgress.SetPos(stepNum++);
			}
			bMeasured = FALSE;
			break;
		}
		Sleep(10);
	}
	//pResult->SaveResult(pRcp->RcpeName);
	//progress->ShowWindow(SW_HIDE);
	ShowMessage(L"Measurement Not Yet Start");
	::PostMessageW(hWndParent, UM_RESULT_DLG, 0, 0);
}

void MeasurementDlg::StartCalculation(int pt, float x, float y) {
	getHeightData(pt + 1);
	calcRoughness();
	CString temp;
	short nItem = 2;
	temp.Format(L"%f", m_fRa1);
	m_cResults.SetItemText(pt, nItem++, temp);
	temp.Format(L"%f", m_fRrms1);
	m_cResults.SetItemText(pt, nItem++, temp);
	temp.Format(L"%f", m_fRmax1);
	m_cResults.SetItemText(pt, nItem++, temp);

	//02282024/HAQUE/ADD MEASURING COLOR==================
	pResult->InsertResult(x, y, m_fRa1, m_fRrms1, m_fRmax1);
	bMeasured = TRUE;
}

void MeasurementDlg::ShowMessage(CString str) {
	m_ProgressMsg.SetWindowTextW(str);
}

void MeasurementDlg::DataAcquisition() {
	CWaitCursor wc;

	MOT::CPiezo& Piezo = Dev.MC->Piezo;
	const float wlen = ICC.SPar.Cwlen / 2.f; // wavelength unit: um [4/14/2022 yuenl]
	const float range = pRcp->MERange / 2.f;
	float iniPos = Piezo.GetPos_um();
	float stPos = iniPos - range / 2;
	float shift = wlen / float(pRcp->MEFrames);
	int nTotal = int(range / shift + 0.5f);

	int settleTime = 5;
	int waitTime = 5;
	Strip.DeallocAll();
	Strip.ResetTime();
	Strip.AddTime(CHighTime::GetPresentTime());
	Dev.MC->stage.devGetCurPos(MOT::MAXIS::X, &Strip.xPos);
	Dev.MC->stage.devGetCurPos(MOT::MAXIS::Y, &Strip.yPos);
	Dev.MC->stage.devGetCurPos(MOT::MAXIS::Z, &Strip.zPos);
	if (!Piezo.Goto(0, false)) { return; }
	if (!Piezo.WaitStop(waitTime + 5000)) {}
	while (!Dev.Cam.Grab(Strip.ImBG, CAM::PRICAM, Dev.Cam.pCm[CAM::PRICAM]->subSampling));
	if (Strip.ImBG) {
		if (ICC.bShowProgress) {
			//cPicWnd.SetImg2(Strip.ImBG); cPicWnd.Invalidate(FALSE); Piezo.Yld(5);
		}
		Strip.ImBG.Detach();
	}

	float now = stPos;
	if (!Piezo.Goto(now, false)) { return; }
	if (!Piezo.WaitStop(waitTime)) {}
	Piezo.Yld(settleTime); // setteling time

	TCHAR status[256];

	DWORD tick = GetTickCount();
	IMGL::CIM tmp;

	Dev.Cam.SetTriggerMode(CAM::PRICAM, true);

	for (int i = 0; i <= nTotal; i++) {
		WLI::SIms* pImN = Strip.NewImgs(now);
		/*while (!Dev.Cam.Grab(tmp, CAM::PRICAM, Dev.Cam.pCm[CAM::PRICAM]->subSampling));
		pImN->Im = tmp;
		tmp.Detach();*/
		Dev.Cam.ExecuteTrigger(CAM::PRICAM);
		Dev.Cam.GetBitmapImage(tmp, CAM::PRICAM);
		pImN->Im = tmp;
		tmp.Detach();
		now = stPos + (i + 1) * shift;
		if (!Piezo.Goto(now, false)) { AfxMessageBox(L"Piezo Move Error!", MB_ICONERROR); return; }
		float dur = (GetTickCount() - tick) / 1000.f;
		swprintf(status, 256, L"Processing %d of %d  Duration: %.2f sec [Working FPS: %.2lf] XYZ:(%.4f, %.4f, %.4f) um  P:%.4f um",
			i + 1, nTotal + 1, dur, (i + 1) / dur,
			Strip.xPos, Strip.yPos, Strip.zPos,
			now);
		ShowMessage(status);
	}

	if (!Piezo.Goto(iniPos, false)) {}
	Strip.AddTime(CHighTime::GetPresentTime());
	LgS.Log(L"Success: Scan operation completed");
	ICC.outfile = L"Acquired";

	Dev.Cam.SetTriggerMode(CAM::PRICAM, false);
	Strip.ExportBMP(L"C:\\WLIN\\BMP\\DATA");
}

void MeasurementDlg::getHeightData(int idx) {
	CString ResultPath = DosUtil.GetResultDir().c_str();
	ResultPath.Format(L"%sHeightData\\%s\\", ResultPath, pRcp->RcpeName);

	SYSTEMTIME st;
	GetSystemTime(&st);
	CString time;
	time.Format(L"P%d_%02d_%02d_%02d_%02d_%02d", idx, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute);

	if (GetFileAttributes(ResultPath) == -1) {
		if (!CreateDirectory(ResultPath, NULL));
	}

	ResultPath = ResultPath + time + L".csv";
	std::ofstream myfile(ResultPath);

	// ARIF COMMENTED OUT FOR TESTING 20250916
	/*Strip.InitCalc();
	Strip.GenHMapV5(Rcp);
	IMGL::CIM16& Im = Strip.Im16um;
	Im.Make24H();
	Im.GetDim(wd, ht);*/
	
	//20250916 
	Strip.InitCalcCV();
	Strip.GenHMapV5CV(Rcp);
	cv::Mat& ImCV = Strip.CVIm16um;
	Make24HStretchCV(ImCV); // Need to modify this to convert height map into 8-bit/3 channel image
	wd = ImCV.cols;
	ht = ImCV.rows;
	

	//HeightData.clear();
	HeightDataCV.clear();
	for (int y = 0; y < ht - 1; y++) {
		for (int x = 0; x < wd - 1; x++) {
			//ARIF COMMENTED OUT FOR TESTING 20250916
			/*float* p = Im.GetPixelAddress(x, y);
			HeightData.push_back(*p);*/
			float grayScale = (ImCV.at<cv::Vec3b>(y, x)[0]+ImCV.at<cv::Vec3b>(y,x)[1]+ImCV.at<cv::Vec3b>(y,x)[2])/3.0; // Assuming grayscale image, take the first channel
			HeightDataCV.push_back(grayScale);
		}
	}

	//filter.removeOutliers(HeightData, wd, ht);
	filter.removeOutliers(HeightDataCV, wd, ht);

	for (int y = 0; y < ht - 1; y++) {
		for (int x = 0; x < wd - 1; x++) {
			//myfile << HeightData[y * (wd - 1) + x] << ',';
			myfile << HeightDataCV[y * (wd - 1) + x] << ',';
		}
		myfile << std::endl;
	}
	myfile.close();
}

void MeasurementDlg::calcRoughness() {
	RSTATS Stats;
	if (HeightData.size() > 0) {
		ApplyFFT();
		CalculateRoughnessStats(&Stats);
		m_fRrms1 = Stats.fStDev;
		m_fRa1 = Stats.fRa;
		m_fRmax1 = Stats.fMax - Stats.fMin;
	}
}

void MeasurementDlg::ApplyFFT() {
	fft_lib fft;
	double* pfSignal = new double[m_nFFT + 5];
	memset(pfSignal, 0, m_nFFT * sizeof(double));
	int i, j;
	int row = ht - 1;
	int col = wd - 1;
	for (i = 0; i < row; i++) {
		for (j = 0; j < col; j++)
			pfSignal[j] = HeightData[i * col + j];
		SpreadArray(pfSignal, col, m_nFFT);

		fft.realft2(pfSignal, m_nFFT, 1);
		for (j = 0; j < m_nFFT; j++) {
			if (j <= m_nFFTcutoff)
				pfSignal[j] = 0;
		}
		fft.realft2(pfSignal, m_nFFT, -1);
		SpreadArray(pfSignal, m_nFFT, col);
		for (j = 0; j < col; j++)
			HeightData[i * col + j] = pfSignal[j];
	}
	for (i = 0; i < col; i++) {
		for (j = 0; j < row; j++)
			pfSignal[j] = HeightData[j * col + i];
		SpreadArray(pfSignal, row, m_nFFT);
		fft.realft2(pfSignal, m_nFFT, 1);
		for (j = 0; j < m_nFFT; j++) {
			if (j <= m_nFFTcutoff)
				pfSignal[j] = 0;
		}
		fft.realft2(pfSignal, m_nFFT, -1);
		SpreadArray(pfSignal, m_nFFT, row);
		for (j = 0; j < row; j++)
			HeightData[j * col + i] = pfSignal[j];
	}
	if (pfSignal)
		delete[] pfSignal;
}

void MeasurementDlg::SpreadArray(double* p, int N1, int N2) {
	int i, j;
	double fXMin = 0;
	double fXMax = N1;
	double fXTemp = fXMin;
	double fXStep = (fXMax - fXMin) / (N2 - 1);

	double* p2 = new double[N2];

	p2[0] = p[0];
	p2[N2 - 1] = p[N1 - 1];

	j = 0;
	for (i = 1; i < N2 - 1; i++) {
		fXTemp += fXStep;
		while (!(fXTemp >= j && fXTemp <= j + 1)) {
			j++;
			if (j >= N1 - 1)
				break;
		}
		p2[i] = p[j] + (fXTemp - j) * (p[j + 1] - p[j]);
	}
	for (i = 0; i < N2; i++)
		p[i] = p2[i];
	if (p2)
		delete[] p2;
}

void MeasurementDlg::CalculateRoughnessStats(RSTATS* pStats) {
	int i, N = HeightData.size();
	pStats->fMin = 1e20;
	pStats->fMax = -1e20;
	pStats->fAver = 0;
	pStats->fRa = 0;
	pStats->fStDev = 0;
	for (i = 0; i < N; i++) {
		pStats->fAver += HeightData[i];
		if (HeightData[i] < pStats->fMin)
			pStats->fMin = HeightData[i];
		if (HeightData[i] > pStats->fMax)
			pStats->fMax = HeightData[i];
	}
	pStats->fAver /= N;
	for (i = 0; i < N; i++) {
		pStats->fStDev += (HeightData[i] - pStats->fAver) * (HeightData[i] - pStats->fAver);
		pStats->fRa += fabs(HeightData[i] - pStats->fAver);
	}
	pStats->fStDev = sqrt(pStats->fStDev / N);
	pStats->fRa /= N;
}

void MeasurementDlg::RecipeToLocal() {
	pRcp = &pRAFTApp->RcpSetup;
	pRcp->UpdateControl(m_cResults);
	m_cWaferMap.Redraw();
}

LRESULT MeasurementDlg::OnTabSelected(WPARAM wP, LPARAM lP) {
	if (!bTabSelected) {
		RecipeToLocal();
		m_cWaferMap.pRcp = pRcp;
		pRcp->UpdateControl(m_cResults);
		bTabSelected = TRUE;
	}

	camRun();

	return 0;
}

LRESULT MeasurementDlg::OnTabDeselected(WPARAM wP, LPARAM lP) {
	if (bTabSelected) {
		bTabSelected = FALSE;
	}
	return 0;
}

void MeasurementDlg::camRun() {
	CAM::SCtx Ctx;
	CAM::CCamera* pCam = Dev.Cam.GetCamera(CAM::ECAM::PRICAM);
	if (pCam != NULL) {
		Ctx.hWnd = cLiveVid.GetSafeHwnd();
		cLiveVid.GetClientRect(Ctx.rc);
		pCam->StopStream(Ctx, pCam->SCaM.ID);
		pCam->StartStream(Ctx, pCam->SCaM.ID);
	}
}

void MeasurementDlg::OnBnClickedMotSetupMd() {
	// TODO: Add your control notification handler code here
	pWLIView->ShowMoSetup(0);
}

void MeasurementDlg::OnBnClickedCamPropMd() {
	// TODO: Add your control notification handler code here
	CAM::CCamera* pCam = Dev.Cam.GetCamera(CAM::PRICAM); if (!pCam) return;
	pCam->propertyPageDisplay();
}