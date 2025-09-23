#pragma once

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#ifndef __AFXTEMPL_H
#include <afxtempl.h>
// class CXTabCtrl;
#endif

#if !defined(__SIZE_ANCHORS_)
#define __SIZE_ANCHORS_
const CSize
NOANCHOR(-1, -1),
TOP_LEFT(0, 0), TOP_CENTER(50, 0), TOP_RIGHT(100, 0),
MIDDLE_LEFT(0, 50), MIDDLE_CENTER(50, 50), MIDDLE_RIGHT(100, 50),
BOTTOM_LEFT(0, 100), BOTTOM_CENTER(50, 100), BOTTOM_RIGHT(100, 100);
#endif // !defined(__SIZE_ANCHORS_)

// jms - 11/28/00 - new code starts
struct FORMCONTROL {
	UINT  nID = 0;
	CRect rect;
	CSize tlType;
	CSize brType;
	HWND  hWnd = 0;
};
// jms - 11/28/00 - new code stops

class CResizableFormView : public CFormView {
protected:
	CResizableFormView();           // protected constructor used by dynamic creation
	CResizableFormView(UINT nIDTemplate);
	DECLARE_DYNCREATE(CResizableFormView)

private:
	// flags
	BOOL m_bShowGrip;
	BOOL m_bUseMaxTrack;
	BOOL m_bUseMinTrack;
	BOOL m_bUseMaxRect;
	BOOL m_bEnableSaveRestore;

	// internal status
	CString m_sSection;			// section name and
	CString m_sEntry;			// entry for save/restore

	BOOL m_bInitDone = false;			// if all internal vars initialized

	SIZE m_szGripSize;			// set at construction time

	CRect m_rcGripRect;			// current pos of grip

	POINT m_ptMinTrackSize;		// min tracking size
	POINT m_ptMaxTrackSize;		// max tracking size
	POINT m_ptMaxPos;			// maximized position
	POINT m_ptMaxSize;			// maximized size

	CPtrList m_plLayoutList;	// list of repositionable controls

	class Layout {
	public:
		HWND hwnd;

		BOOL adj_hscroll;
		BOOL need_refresh;

		// upper-left corner
		SIZE tl_type;
		SIZE tl_margin;

		// bottom-right corner
		SIZE br_type;
		SIZE br_margin;

	public:
		Layout(HWND hw, SIZE tl_t, SIZE tl_m,
			SIZE br_t, SIZE br_m, BOOL hscroll, BOOL refresh) {
			hwnd = hw;

			adj_hscroll = hscroll;
			need_refresh = refresh;

			tl_type = tl_t;
			tl_margin = tl_m;

			br_type = br_t;
			br_margin = br_m;
		};
	};

public:
	enum { IDD = 0 };

public:
	virtual void OnInitialUpdate();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
	virtual ~CResizableFormView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// used internally
private:
	void Construct();

protected:
	void ArrangeLayout();

	// callable from derived classes
	//protected:
public:
	CRect iniRC, finRC;
	BOOL bWnd;
	void SetiniRC();

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()

protected:
	void AddAnchor(HWND wnd, CSize tl_type, CSize br_type = NOANCHOR);	// add anchors to a control
	void AddAnchor(UINT ctrl_ID, CSize tl_type, CSize br_type = NOANCHOR);

	// functions added by jms
protected:
	CTypedPtrArray<CPtrArray, FORMCONTROL*> m_tpaControls;

	void AddResizedControl(UINT nID, CSize tl_type, CSize br_type = NOANCHOR);
	void AnchorControls();
	FORMCONTROL* GetFormControl(HWND hWnd);
};
