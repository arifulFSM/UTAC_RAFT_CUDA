#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "WLI.h"
#endif

#include "wdefine.h"
#include "WLIDoc.h"
#include "PSI/ICC.h"
#include "PSI/Strip.h"
#include "SRC/DOSUtil.h"
#include "WLIView.h"
#include <propkey.h>
#include <filesystem>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CWLIDoc, CDocument)

BEGIN_MESSAGE_MAP(CWLIDoc, CDocument)
	ON_COMMAND(ID_FILE_OPEN, &CWLIDoc::OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE_AS, &CWLIDoc::OnFileSaveAs)
	ON_COMMAND(ID_FILE_EXPORTSTRIPINFORMATION, &CWLIDoc::OnFileExportstripinformation)
	ON_COMMAND(ID_FILE_EXPORTSTRIPIMAGES, &CWLIDoc::OnFileExportstripimages)
	ON_COMMAND(ID_FILE_BATCHIMSEXPORT, &CWLIDoc::OnFileBatchimsexport)
END_MESSAGE_MAP()

CWLIDoc::CWLIDoc() noexcept {}

CWLIDoc::~CWLIDoc() {}

BOOL CWLIDoc::OnNewDocument() {
	if (!CDocument::OnNewDocument()) return FALSE;

	return TRUE;
}

void CWLIDoc::Serialize(CArchive& ar) {
	if (ar.IsStoring()) {
	}
	else {
	}
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CWLIDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds) {
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CWLIDoc::InitializeSearchContent() {
	CString strSearchContent;
	// Set search contents from document's data.
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CWLIDoc::SetSearchContent(const CString& value) {
	if (value.IsEmpty()) {
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else {
		CMFCFilterChunkValueImpl* pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr) {
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

#ifdef _DEBUG
void CWLIDoc::AssertValid() const {
	CDocument::AssertValid();
}

void CWLIDoc::Dump(CDumpContext& dc) const {
	CDocument::Dump(dc);
}
#endif //_DEBUG

void CWLIDoc::OnFileOpen() {
	CFileDialog dlg(
		TRUE, _T("IMS"), NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		_T("IMS|*.IMS|IMG|*.IMG|"));
	std::wstring fname = DosUtil.GetProgramDir();
	fname += _T("IMS");
#ifdef UNICODE
	_wmkdir(fname.c_str());  // do not delete, create directory if not present [3/22/2022 yuenl]
#else
	_mkdir(fname.c_str());  // do not delete, create directory if not present [3/22/2022 yuenl]
#endif
	dlg.m_ofn.lpstrInitialDir = fname.c_str();
	dlg.m_ofn.lpstrTitle = _T("Load image");
	if (dlg.DoModal() == IDOK) {
		CWaitCursor wc;
		std::wstring str = dlg.GetPathName().GetBuffer();
		if (!Strip.Load(str)) {
			ICC.outfile = _T("");
			AfxMessageBox(_T("Fail to load"));
			return;
		}
		str = str.substr(0, str.rfind('.')) + _T(".I16");
		ICC.outfile = str;
		//ICC.x1 = 10, ICC.y1 = 10, ICC.x2 = 10, ICC.y2 = 10; // 07252023
		Strip.InitCalc();

		CWLIView::GetView()->PostMessage(UM_STRIP_LOADED, 0, 0);
	}
}

void CWLIDoc::OnFileSaveAs() {
	CFileDialog dlg(
		FALSE, _T("IMS"), NULL,
		OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_NOCHANGEDIR,
		_T("IMS|*.IMS|"));
	std::wstring fname = DosUtil.GetProgramDir();
	fname += _T("IMS"); int a = _wmkdir(fname.c_str());
	dlg.m_ofn.lpstrInitialDir = fname.c_str();
	dlg.m_ofn.lpstrTitle = _T("Save image strip");
	if (dlg.DoModal() == IDOK) {
		CWaitCursor wc;
		Strip.Save(dlg.GetPathName().GetBuffer());
	}
}

void CWLIDoc::OnFileExportstripinformation() {
	int sz = Strip.size(); if (sz < 1) return;
	CFileDialog dlg(
		FALSE, _T("CSV"), NULL,
		OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_NOCHANGEDIR,
		_T("CSV|*.CSV|"));
	std::wstring fname = DosUtil.GetProgramDir();
	fname += _T("CSV"); int a = _wmkdir(fname.c_str());
	dlg.m_ofn.lpstrInitialDir = fname.c_str();
	dlg.m_ofn.lpstrTitle = _T("Save image strip");
	if (dlg.DoModal() == IDOK) {
		//std::wstring wide = dlg.GetPathName().GetBuffer();
		//std::string str(wide.begin(), wide.end());
		Strip.Dump(dlg.GetPathName().GetBuffer());
	}
}

void CWLIDoc::OnFileExportstripimages() {
	CFileDialog dlg(
		FALSE, _T("BMP"), NULL,
		OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_NOCHANGEDIR,
		_T("BMP|*.BMP|"));
	std::wstring fname = DosUtil.GetProgramDir();
	fname += _T("BMP"); int a = _wmkdir(fname.c_str());
	dlg.m_ofn.lpstrInitialDir = fname.c_str();
	dlg.m_ofn.lpstrTitle = _T("Save image strip as BMPs");
	if (dlg.DoModal() == IDOK) {
		CWaitCursor wc;
		Strip.ExportBMP(dlg.GetPathName().GetBuffer());
	}
}

void CWLIDoc::OnFileBatchimsexport() {
	// TODO: Add your command handler code here
	std::string path = "C:\\WLI\\IMSEXPORT";
	for (const auto& entry : std::filesystem::directory_iterator(path)) {
		Strip.Load(entry.path());
		std::wstring fileName = std::filesystem::path(entry.path()).filename();
		std::string Path = "C:\\WLI\\BMPEXPORT\\" + std::string(fileName.begin(), fileName.end());
		Strip.ExportBMP(CString(Path.c_str()));
	}
	AfxMessageBox(L"Data Export Is Finished!", MB_ICONINFORMATION);
}