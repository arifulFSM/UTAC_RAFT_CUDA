#include "pch.h"
#include "ResultRAFT.h"
#include "SRC\DOSUtil.h"
#include <stdio.h>

CResultRAFT::CResultRAFT() {}

CResultRAFT::~CResultRAFT() {
	ResultList.clear();
}

void CResultRAFT::Clear() {
	ResultList.clear();
}

void CResultRAFT::InsertResult(float x, float y, float Ra, float Rrms, float Rmax) {
	DATA tData;
	tData.x = x;
	tData.y = y;
	tData.ResultData[0] = Ra;
	tData.ResultData[1] = Rrms;
	tData.ResultData[2] = Rmax;

	ResultList.push_back(tData);
}

float CResultRAFT::GetRa(int index) {
	if (index >= ResultList.size()) {
		return BADDATA;
	}
	return ResultList[index].ResultData[0];
}

float CResultRAFT::GetRrms(int index) {
	if (index >= ResultList.size()) {
		return BADDATA;
	}
	return ResultList[index].ResultData[1];
}

float CResultRAFT::GetRmax(int index) {
	if (index >= ResultList.size()) {
		return BADDATA;
	}
	return ResultList[index].ResultData[2];
}

BOOL CResultRAFT::SaveResult() {
	CString ResultPath = DosUtil.GetResultDir().c_str();

	SYSTEMTIME st;
	GetSystemTime(&st);
	CString time;
	time.Format(L"%02d_%02d_%02d_%02d_%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute);

	ResultPath = ResultPath + RecipeName + "_" + time + L".DAT";

	CFile theFile;
	if (theFile.Open(ResultPath, CFile::modeCreate | CFile::modeWrite)) {
		CArchive archive(&theFile, CArchive::store);
		Serialize(archive);
		archive.Close();
		theFile.Close();
		return TRUE;
	}
	return FALSE;
}

BOOL CResultRAFT::LoadResult() {
	CFileDialog dlg(TRUE,
		L"DAT",
		NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_ALLOWMULTISELECT,
		L"Data file|*.DAT|");

	CString filename = L"";
	dlg.m_ofn.lpstrInitialDir = DosUtil.GetResultDir().c_str();

	if (dlg.DoModal() == IDOK) {
		filename = dlg.GetPathName();
		CFile theFile;
		if (theFile.Open(filename, CFile::modeRead)) {
			Clear();
			CArchive archive(&theFile, CArchive::load);
			Serialize(archive);
			archive.Close();
			theFile.Close();
			return TRUE;
		}
	}
	return FALSE;
}

void CResultRAFT::Serialize(CArchive& ar) {
	if (ar.IsStoring()) {
		ar << RecipeName;
		ar << ResultList.size();
		for (int i = 0; i < ResultList.size(); i++) {
			ar << ResultList[i].x;
			ar << ResultList[i].y;
			for (int j = 0; j < MeData; j++) {
				ar << ResultList[i].ResultData[j];
			}
		}
	}
	else {
		Clear();
		DATA temp;
		int n;
		ar >> RecipeName;
		ar >> n;
		for (int i = 0; i < n; i++) {
			ar >> temp.x;
			ar >> temp.y;
			for (int j = 0; j < MeData; j++) {
				ar >> temp.ResultData[j];
			}
			ResultList.push_back(temp);
		}
	}
}

void CResultRAFT::UpdateControl(CListCtrl& Ctrl) {
	CString str;

	Ctrl.DeleteAllItems();

	for (int i = 0; i < ResultList.size(); i++) {
		short n = 1;
		str.Format(L"%d", i + 1);
		int idx = Ctrl.InsertItem(i + 1, str);
		str.Format(L"%0.2f    %0.2f", ResultList[i].x, ResultList[i].y);
		Ctrl.SetItemText(idx, n++, str);
		str.Format(L"%0.6f", GetRa(i));
		Ctrl.SetItemText(idx, n++, str);
		str.Format(L"%0.6f", GetRrms(i));
		Ctrl.SetItemText(idx, n++, str);
		str.Format(L"%0.6f", GetRmax(i));
		Ctrl.SetItemText(idx, n++, str);
	}
}