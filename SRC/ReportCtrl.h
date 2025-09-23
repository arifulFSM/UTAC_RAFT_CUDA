#pragma once

class CReportCtrl : public CListCtrl {
	DECLARE_DYNAMIC(CReportCtrl)

	CCriticalSection CS;

	HWND hParent = NULL;
public:
	CReportCtrl();
	virtual ~CReportCtrl();

	//////////////////////////////////////////////////////////////////////////
	BOOL SetHeader(const CString& strHeader);
	int AddItem(LPCTSTR pszText, ...);
	int GetNumColumn();
	int InsertItem0(LPCTSTR pszText, ...);
	//////////////////////////////////////////////////////////////////////////

	void SetParentWindow(HWND parent) {
		hParent = parent;
	}
protected:
	DECLARE_MESSAGE_MAP()
};

class CRecipe;
struct SMPoint;

class CRcpRptCtrl : public CReportCtrl {
	//////////////////////////////////////////////////////////////////////////
	void FormatStr(CString& str, SMPoint& MP, short index);
	void SetupReportHeader(CRecipe& Rcp);
	void UpdateReport(CRecipe& Rcp, SMPoint& MP);
	void UpdateStats(CRecipe& Rcp);
	//////////////////////////////////////////////////////////////////////////

public:
	CRcpRptCtrl();
	virtual ~CRcpRptCtrl();

	//////////////////////////////////////////////////////////////////////////
	void ReloadGrid(CRecipe& Rcp);
	void ReloadGrid2(CRecipe& Rcp);
	//////////////////////////////////////////////////////////////////////////
	DECLARE_MESSAGE_MAP()
	afx_msg void OnNMRClick(NMHDR* pNMHDR, LRESULT* pResult);
};