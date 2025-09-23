#include "pch.h"
#include "RecipeRAFT.h"
#include "SRC\DOSUtil.h"
#include <cstring>

CRecipeRAFT::CRecipeRAFT() {
	AFTiltZ = 0;
	Clear();
}

CRecipeRAFT& CRecipeRAFT::operator =(CRecipeRAFT& co) {
	Wp = co.Wp;
	return *this;
}
void CRecipeRAFT::Clear() {
	Version = 0;

	AFCalibZ = 0;
	AFRange = 0;
	AFStepSize = 0;
	MERange = 0;
	MEFrames = 0;

	Wp.Clear();
	Me.Clear();
	wcscpy(RcpeName, L"");
	Patt.MP.Clear();
	for (int j = 0; j < MAXMEPASET; j++) {
		for (int i = 0; i < MaXPA; i++) {
			MeParam[j].MPa[i].Clear();
		}
	}
}

CWaferParam* CRecipeRAFT::GetWp() {
	return &Wp;
}

wchar_t* CRecipeRAFT::GetRecipeName() {
	return RcpeName;
}

void CRecipeRAFT::Renumber() {
	Patt.Renumber();
}

void CRecipeRAFT::UpdateControl(CListCtrl& Ctrl) {
	Patt.UpdateControl(Ctrl);
}

BOOL CRecipeRAFT::IsMarkers() {
	short cnt = 0;
	CPtrList* pMPList = &Patt.MP.MPLst;
	POSITION pos = pMPList->GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
		if (p) {
			if (p->Co.p > 0) {
				cnt++;
			}
		}
	}
	if (cnt >= 3) {
		return TRUE;
	}
	return FALSE;
}

BOOL CRecipeRAFT::SaveRecipe(BOOL bSortPrompt) {
	BOOL bSort = FALSE;
	if (bSortPrompt && (!IsMarkers())) {
		if (AfxMessageBox(L"Sort data?", MB_YESNO) == IDYES) {
			bSort = TRUE;
		}
	}
	CFileDialog dlg(FALSE,
		L"RCP",
		NULL,
		OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_NOCHANGEDIR,
		L"Recipe file|*.RCP|");

	CString filename = L"";
	dlg.m_ofn.lpstrInitialDir = DosUtil.GetRecipeDir().c_str();
	if (dlg.DoModal() == IDOK) {
		filename = dlg.GetPathName();
		BOOL ret = SaveRecipe(filename, bSort);
		return ret;
		/*
		CString str;
		str.Format("FSM 413C2C Control Module : [%s]", RcpeName);
		AfxGetMainWnd()->SetWindowText(str);*/
		//return ret;
	}
	return FALSE;
}

BOOL CRecipeRAFT::strscpy(wchar_t* dest, short szdest, const wchar_t* sorc) {
	wcscpy(dest, sorc);
	return TRUE;

	//memset(dest,0,szdest);
	short len = short(wcslen(sorc));
	if (len > 0) {
		if (len <= szdest) {
			wcscpy(dest, sorc);
			*(dest + szdest - 1) = 0;
			return TRUE;
		}
		else {
			memcpy(dest, sorc, szdest - 2);
			*(dest + szdest - 1) = 0;
			return TRUE;
		}
	}
	*dest = 0;
	return FALSE;
}

BOOL CRecipeRAFT::SaveRecipe(CString filename, BOOL bSort) {
	CFile theFile;
	if (theFile.Open(filename, CFile::modeCreate | CFile::modeWrite)) {
		if (bSort) {
			Patt.MP.Sort();
			Renumber();
		}
		Patt.MP.EnsureData();
		//}
		CString name;
		name = filename;
		std::wstring file = DosUtil.ExtractFilename(name);
		strscpy(RcpeName, RCPNAMELEN, file.c_str());
		CArchive archive(&theFile, CArchive::store);
		Serialize(archive);
		archive.Close();
		theFile.Close();
		return TRUE;
	}
	return FALSE;
}

void CRecipeRAFT::Serialize(CArchive& ar) {
	if (ar.IsStoring()) {
		Version = CURRENTRECIPEVERSION;
		ar << Version;
		ar << CString(RcpeName);
		ar << AFCalibZ;
		ar << AFTiltZ;
		ar << AFRange;
		ar << AFStepSize;
		ar << MERange;
		ar << MEFrames;
		Wp.Serialize(ar);
		Me.Serialize(ar);
		Patt.MP.Serialize(ar);
	}
	else {
		ar >> Version;
		CString rp;
		ar >> rp;
		strscpy(RcpeName, RCPNAMELEN, rp.GetBuffer(0));
		ar >> AFCalibZ;
		ar >> AFTiltZ;
		ar >> AFRange;
		ar >> AFStepSize;
		ar >> MERange;
		ar >> MEFrames;
		Wp.Serialize(ar);
		Me.Serialize(ar);
		Patt.MP.Serialize(ar);
	}
}

BOOL CRecipeRAFT::LoadRecipe() {
	CFileDialog dlg(TRUE,
		L"RCP",
		NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_ALLOWMULTISELECT,
		L"Recipe file|*.RCP|");

	CString filename = L"";
	dlg.m_ofn.lpstrInitialDir = DosUtil.GetRecipeDir().c_str();

	if (dlg.DoModal() == IDOK) {
		filename = dlg.GetPathName();
		BOOL ret = Load(filename);
		/*CString str;
		str.Format("FSM 413C2C Control Module : [%s]", RcpeName);
		AfxGetMainWnd()->SetWindowText(str);*/
		return ret;
	}
	return FALSE;
}

BOOL CRecipeRAFT::Load(CString RcpName) {
	CFile theFile;
	if (theFile.Open(RcpName, CFile::modeRead)) {
		Clear();
		CArchive archive(&theFile, CArchive::load);
		Serialize(archive);
		archive.Close();
		theFile.Close();
		std::wstring FileName = DosUtil.ExtractFilename(RcpName);
		RcpName = FileName.c_str();
		//strscpy(RcpeName, RCPNAMELEN, temp.c_str());
		//LoadBaseline();  // Load data from baseline file and save in CMPoint::baseline [10/31/2011 Administrator]
		return TRUE;
	}
	return FALSE;
}