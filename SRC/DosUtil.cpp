#include "pch.h"

#include <format>
#include <direct.h>

#include "DOSUtil.h"

#pragma warning(disable: 4996)

CDosUtil DosUtil;

CDosUtil::CDosUtil() {
	ProgrmDir = _GetProgramDir();
	LocalCfg = _GetLocalCfgFile();
	LocalSys = _GetLocalSysFile();
}

CDosUtil::~CDosUtil() {}

BOOL CDosUtil::MakeFileNameFromModule(TCHAR* filename, TCHAR* name, TCHAR* extn) {
	TCHAR fullname[_MAX_PATH], dir[_MAX_DIR], drive[_MAX_DRIVE];
	TCHAR fname[_MAX_FNAME], ext[_MAX_EXT];

	GetModuleFileName(NULL, fullname, MAX_PATH);
	_wsplitpath(fullname, drive, dir, fname, ext);
	_wmakepath(fullname, drive, dir, name, extn);
	filename = fullname;
	return TRUE;
}

void CDosUtil::AddExtension(TCHAR* filename, TCHAR* ex) {
	_wsplitpath(filename, drive, dir, fname, ext);
	_wmakepath(dirname, drive, dir, fname, ex);
	filename = dirname;
}

std::wstring CDosUtil::MakeFullPathName(TCHAR* path, TCHAR* name, TCHAR* ext) {
	return std::wstring(path + std::wstring(_T("\\")) + name + ext);
}

std::wstring CDosUtil::ExtractFilename(const TCHAR* filename) {
	wcscpy(dirname, filename);
	_wsplitpath(dirname, drive, dir, fname, ext);
	return fname;
}

std::wstring CDosUtil::ExtractDrive(TCHAR* filename) {
	wcscpy(dirname, filename);
	_wsplitpath(dirname, drive, dir, fname, ext);
	return drive;
}

std::wstring CDosUtil::ExtractFilenameExt(TCHAR* filename) {
	wcscpy(dirname, filename);
	_wsplitpath(dirname, drive, dir, fname, ext);
	std::wstring rname(fname); rname += ext;
	return rname;
}

std::wstring CDosUtil::RemoveExtension(const TCHAR* filename) {
	wcscpy(dirname, filename);
	_wsplitpath(dirname, drive, dir, fname, ext);
	_wmakepath(dirname, drive, dir, fname, _T(""));
	return dirname;
}

std::wstring CDosUtil::GetProgramDir() {
	return ProgrmDir;
}

BOOL CDosUtil::GetProgramDir(CString& mdir) {
	wchar_t dirname[_MAX_PATH];
	wchar_t drive[_MAX_DRIVE];
	wchar_t dir[_MAX_DIR];
	wchar_t fname[_MAX_FNAME];
	wchar_t ext[_MAX_EXT];

	GetModuleFileName(NULL, dirname, _MAX_PATH);
	_wsplitpath(dirname, drive, dir, fname, ext);
	_wmakepath(dirname, drive, dir, L"", L"");
	mdir = dirname;
	return TRUE;
}

std::wstring CDosUtil::_GetProgramDir() {
	GetModuleFileName(NULL, dirname, _MAX_PATH);
	_wsplitpath(dirname, drive, dir, fname, ext);
	_wmakepath(dirname, drive, dir, _T(""), _T(""));
	return dirname;
}

std::wstring CDosUtil::GetLocalCfgFile() {
	return LocalCfg;
}

std::wstring CDosUtil::_GetLocalCfgFile() {
	return GetFilename(_T("FSMLocalCfg"), _T("INI"));
}

int CDosUtil::ReadCfgINI(const TCHAR* section, const TCHAR* name, int defvalue) {
	return GetPrivateProfileInt(section, name, defvalue, LocalCfg.c_str());
}

ULONG CDosUtil::ReadCfgINI(const TCHAR* section, const TCHAR* name, ULONG defvalue) {
	TCHAR str[256];
	swprintf(str, 256, _T("%ld"), defvalue);
	GetPrivateProfileString(section, name, str, str, sizeof(str), LocalCfg.c_str());
	return ULONG(_wtol(str));
}

float CDosUtil::ReadCfgINI(const TCHAR* section, const TCHAR* name, float defvalue) {
	TCHAR str[256];
	swprintf(str, 256, _T("%f"), defvalue);
	GetPrivateProfileString(section, name, str, str, sizeof(str), LocalCfg.c_str());
	return float(_wtof(str));
}

double CDosUtil::ReadCfgINI(const TCHAR* section, const TCHAR* name, double defvalue) {
	TCHAR str[256];
	swprintf(str, 256, _T("%f"), defvalue);
	GetPrivateProfileString(section, name, str, str, sizeof(str), LocalCfg.c_str());
	return _wtof(str);
}

std::wstring CDosUtil::ReadCfgINI(const TCHAR* section, const TCHAR* name, TCHAR* str) {
	TCHAR cstr[256];
	GetPrivateProfileString(section, name, str, cstr, sizeof(cstr), LocalCfg.c_str());
	return cstr;
}

void CDosUtil::WriteCfgINI(const TCHAR* section, const TCHAR* key, int val) {
	std::wstring str = std::to_wstring(val);
	WritePrivateProfileString(section, key, str.c_str(), LocalCfg.c_str());
}

void CDosUtil::WriteCfgINI(const TCHAR* section, const  TCHAR* key, ULONG val) {
	std::wstring str = std::to_wstring(val);
	WritePrivateProfileString(section, key, str.c_str(), LocalCfg.c_str());
}

void CDosUtil::WriteCfgINI(const TCHAR* section, const  TCHAR* key, UINT val) {
	std::wstring str = std::to_wstring(val);
	WritePrivateProfileString(section, key, str.c_str(), LocalCfg.c_str());
}

void CDosUtil::WriteCfgINI(const TCHAR* section, const TCHAR* key, const TCHAR* str) {
	WritePrivateProfileString(section, key, str, LocalCfg.c_str());
}

void CDosUtil::WriteCfgINI(const TCHAR* section, const TCHAR* key, double val) {
	std::wstring str = std::to_wstring(val);
	WritePrivateProfileString(section, key, str.c_str(), LocalCfg.c_str());
}

void CDosUtil::WriteCfgINI(const TCHAR* section, const TCHAR* key, float val) {
	std::wstring str = std::to_wstring(val);
	WritePrivateProfileString(section, key, str.c_str(), LocalCfg.c_str());
}

std::wstring CDosUtil::GetLocalSysFile() {
	return LocalSys;
}

std::wstring CDosUtil::_GetLocalSysFile() {
	return GetFilename(_T("FSMLocalSys"), _T("INI"));
}

std::wstring CDosUtil::GetFSMAppFile() {
	return GetFilename(_T("FSMAPP"), _T("INI"));
}

std::wstring CDosUtil::GetMotorFile() {
	return GetFilename(_T("Motor"), _T("TXT"));
}

std::wstring CDosUtil::GetRecipeDir() {
	std::wstring str;
	str = DosUtil.GetProgramDir() + _T("RECIPE\\");
	if (GetFileAttributes(str.c_str()) == -1) {
		if (!CreateDirectory(str.c_str(), NULL)) return _T("");
	}
	return str;
}

std::wstring CDosUtil::GetResultDir() {
	std::wstring str;
	str = DosUtil.GetProgramDir() + _T("DATA\\");
	if (GetFileAttributes(str.c_str()) == -1) {
		if (!CreateDirectory(str.c_str(), NULL)) return _T("");
	}
	return str;
}

std::wstring CDosUtil::GetFilename(const TCHAR* name, const TCHAR* ex) {
	TCHAR filename[_MAX_PATH];
	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	TCHAR ext[_MAX_EXT];

	GetModuleFileName(NULL, filename, _MAX_PATH);
	_wsplitpath(filename, drive, dir, fname, ext);
	_wmakepath(filename, drive, dir, name, ex);
	return filename;
}