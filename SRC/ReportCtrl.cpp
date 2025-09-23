#include "pch.h"
#include "ReportCtrl.h"

#ifdef _DEBUG
#define ASSERT_VALID_STRING( str ) ASSERT( !IsBadStringPtr( str, 0xfffff ) )
#else	//	_DEBUG
#define ASSERT_VALID_STRING( str ) ( (void)0 )
#endif	//	_DEBUG

IMPLEMENT_DYNAMIC(CReportCtrl, CListCtrl)

CReportCtrl::CReportCtrl() {}

CReportCtrl::~CReportCtrl() {}

BOOL CReportCtrl::SetHeader(const CString& strHeader) {
	int iStart = 0;
	int N = 0;
	for (;;) {
		const int iComma = strHeader.Find(_T(','), iStart);
		if (iComma == -1) break;
		const CString strHeading = strHeader.Mid(iStart, iComma - iStart);
		iStart = iComma + 1;
		int iSemiColon = strHeader.Find(_T(';'), iStart);
		if (iSemiColon == -1) iSemiColon = strHeader.GetLength();
		const int iWidth = atoi(strHeader.Mid(iStart, iSemiColon - iStart));
		iStart = iSemiColon + 1;
		if (InsertColumn(N++, strHeading, LVCFMT_LEFT, iWidth) == -1)
			return FALSE;
	}
	return TRUE;
}

int CReportCtrl::AddItem(LPCTSTR pszText, ...) {
	int N = GetNumColumn();
	const int iIndex = InsertItem(GetItemCount(), pszText);
	va_list list; va_start(list, pszText);
	for (int iColumn = 1; iColumn < N; iColumn++) {
		pszText = va_arg(list, LPCTSTR);
		ASSERT_VALID_STRING(pszText);
		VERIFY(CListCtrl::SetItem(iIndex, iColumn, LVIF_TEXT, pszText, 0, 0, 0, 0));
	}
	va_end(list);
	return iIndex;
}

int CReportCtrl::GetNumColumn() {
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	if (pHeader) return pHeader->GetItemCount();
	return 0;
}

int CReportCtrl::InsertItem0(LPCTSTR pszText, ...) {
	int N = GetNumColumn();
	int iIndex = InsertItem(0, pszText);
	va_list list;
	va_start(list, pszText);
	for (int iColumn = 1; iColumn < N; iColumn++) {
		pszText = va_arg(list, LPCTSTR);
		ASSERT_VALID_STRING(pszText);
		VERIFY(CListCtrl::SetItem(iIndex, iColumn, LVIF_TEXT, pszText, 0, 0, 0, 0));
	}
	va_end(list);
	return iIndex;
}

BEGIN_MESSAGE_MAP(CReportCtrl, CListCtrl)
END_MESSAGE_MAP()

void CRcpRptCtrl::FormatStr(CString& str, SMPoint& MP, short index) {
	if (index == 0)
		str.Format("%d", MP.n + 1);
	else if (index == 1)
		str.Format("%.2f,%.2f", MP.x, MP.y);
	else
		str.Format("%.3f", MP.VaL[index - 2]);
}

void CRcpRptCtrl::SetupReportHeader(CRecipe& Rcp) {
	int n = 0;
	//////////////////////////////////////////////////////////////////////////
	// Delete all of the columns.
	int nColumnCount = GetHeaderCtrl()->GetItemCount();
	for (int i = 0; i < nColumnCount; i++) DeleteColumn(0);
	//////////////////////////////////////////////////////////////////////////
	SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	LV_COLUMN nListColumn;
	nListColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	nListColumn.fmt = LVCFMT_LEFT;
	nListColumn.iSubItem = 0;
	int col = 0;
	for (UINT i = 0; i < MAXDATAVAL; i++) {
		SDesc* pDe = &Rcp.WPar.Desc[i];
		if (pDe->bVis && pDe->ID.GetLength()) {
			nListColumn.cx = int(pDe->ID.GetLength() * 12);
			nListColumn.pszText = pDe->ID.GetBuffer();
			InsertColumn(col++, &nListColumn);
		}
	}
}

void CRcpRptCtrl::UpdateReport(CRecipe& Rcp, SMPoint& MP) {
	CString str;

	str.Format("%d", MP.n);
	int cnt = GetItemCount();
	int idx = InsertItem(cnt, str);
	int m = 1;
	str.Format("%.3f,%.3f", MP.x, MP.y);
	SetItemText(idx, m++, str);

	for (int i = 2; i < MAXDATAVAL; i++) {
		SDesc* pDe = &Rcp.WPar.Desc[i];
		if (!pDe->bVis) continue;
		if (MP.VaL[i - 2] != BADDATAVAL) {
			FormatStr(str, MP, i);
			SetItemText(idx, m++, str);
		}
		else {
			SetItemText(idx, m++, _T(""));
		}
	}
	EnsureVisible(idx, FALSE);
}

void CRcpRptCtrl::UpdateStats(CRecipe& Rcp) {
	int m = 1;
	int cnt = GetItemCount();
	int idx[8];

	//CPattern* pPat = pRcp->GetPattern(pRcp->type);
	for (int i = 0; i < 8; i++) {
		idx[i] = InsertItem(cnt++, " ");
	}

	SetItemText(idx[1], m, "Minimum");
	SetItemText(idx[2], m, "Maximum");
	SetItemText(idx[3], m, "Average");
	SetItemText(idx[4], m, "Stdev");
	SetItemText(idx[5], m, "TTV");
	SetItemText(idx[6], m, "Warp");
	SetItemText(idx[7], m, "Bow");
	m++;

	CString str;
	for (int i = 2; i < MAXDATAVAL; i++) {
		SDesc* pDe = &Rcp.WPar.Desc[i];
		if (!pDe->bVis) continue;
		SStat* pS = &Rcp.WPar.Stat[i - 2];
		if (pS->N) {
			str.Format("%.3f", pS->fMin);
			SetItemText(idx[1], m, str);
			str.Format("%.3f", pS->fMax);
			SetItemText(idx[2], m, str);
			str.Format("%.3f", pS->fAve);
			SetItemText(idx[3], m, str);
			str.Format("%.3f", pS->fStdev);
			SetItemText(idx[4], m, str);
			str.Format("%.3f", pS->fMax - pS->fMin);
			SetItemText(idx[5], m, str);
			str.Format("%.3f", pS->fWarp);
			SetItemText(idx[6], m, str);
			str.Format("%.3f", pS->fBow);
			SetItemText(idx[7], m, str);
		}
		else {
			SetItemText(idx[1], m, "");
			SetItemText(idx[2], m, "");
			SetItemText(idx[3], m, "");
			SetItemText(idx[4], m, "");
			SetItemText(idx[5], m, "");
			SetItemText(idx[6], m, "");
			SetItemText(idx[7], m, "");
		}
		m++;
	}
	EnsureVisible(idx[1], FALSE);
}

CRcpRptCtrl::CRcpRptCtrl() {}

CRcpRptCtrl::~CRcpRptCtrl() {}

void CRcpRptCtrl::ReloadGrid(CRecipe& Rcp) {
	DeleteAllItems();
	SetupReportHeader(Rcp);
	std::vector<SMPoint>& MPs = Rcp.GetMPs();
	int sz = int(MPs.size());
	for (int i = 0; i < sz; i++) {
		MPs[i].n = i + 1;
		UpdateReport(Rcp, MPs[i]);
	}
	UpdateStats(Rcp);
}

void CRcpRptCtrl::ReloadGrid2(CRecipe& Rcp) {
	SetupReportHeader(Rcp);
}BEGIN_MESSAGE_MAP(CRcpRptCtrl, CReportCtrl)
//ON_WM_RBUTTONUP()
ON_NOTIFY_REFLECT(NM_RCLICK, &CRcpRptCtrl::OnNMRClick)
END_MESSAGE_MAP()

void CRcpRptCtrl::OnNMRClick(NMHDR* pNMHDR, LRESULT* pResult) {
	CSingleLock lock(&CS);
	lock.Lock();
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	if (pNMItemActivate->iItem + 1 > RcpSetup.GetMPs().size()) {
		lock.Unlock();
		*pResult = 0; return;
	}

	SMPoint* pPoint = &RcpSetup.GetMPs()[pNMItemActivate->iItem];
	Dev.Mtr.GotoXY(pPoint->x, pPoint->y, TRUE);

	*pResult = 0;
	lock.Unlock();
}