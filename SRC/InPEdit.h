#pragma once

#define WM_NOTIFY_KILL_FOCUS WM_USER + 8366
#define WM_NOTIFY_DESCRIPTION_EDITED WM_USER + 8365

class CInPEdit : public CEdit {
	DECLARE_DYNAMIC(CInPEdit)

	int m_iItem;
	int m_iSubItem;
	CString m_sInitText;
	BOOL    m_bESC;
	LV_DISPINFO dispinfo;

public:
	CInPEdit(int iItem, int iSubItem, CString sInitText);
	virtual ~CInPEdit();

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnNcDestroy();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnEnKillfocus();
};
