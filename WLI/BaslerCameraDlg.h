#pragma once
enum EUpdateHint {
	UpdateHint_All = 0
	, UpdateHint_Image = 1
	, UpdateHint_Feature = 2
	, UpdateHint_DeviceList = 4
};

#include "afxdialogex.h"

#include "SRC/ResizableDialog.h"
#include<pylon/PylonIncludes.h>
#ifdef PYLON_WIN_BUILD
#include<pylon/PylonGUI.h>
#endif
#include <pylon/BaslerUniversalInstantCamera.h>
#include <vector>
#include "PicWnd.h"

// BaslerCameraDlg dialog

class BaslerCameraDlg : public CResizableDialog
	, public Pylon::CImageEventHandler             // Allows you to get informed about received images and grab errors.
	, public Pylon::CConfigurationEventHandler     // Allows you to get informed about device removal.
	, public Pylon::CCameraEventHandler            // Allows you to get notified about camera events and GenICam node
{
	DECLARE_DYNAMIC(BaslerCameraDlg)

		CStatic cDisplayImage;

public:
	BaslerCameraDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~BaslerCameraDlg();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG15 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	struct SCtx {
		HWND hWnd = 0;
		CRect rc;
	};

	afx_msg void OnClose();
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	void OnStart();
	void OnStop();
	afx_msg void OnViewRefresh();
	int EnumerateDevices();
	void FillDeviceListCtrl(Pylon::DeviceInfoList_t devices);
	void DeleteContents();
	BOOL OpenCamera(LPCTSTR lpszPathName);
	virtual void OnImageGrabbed(Pylon::CInstantCamera& camera, const Pylon::CGrabResultPtr& grabResult);
	Pylon::CIntegerParameter GetIntegerParameter(GenApi::INode* pNode) const;
	void OnNodeChanged(GenApi::INode*);
	void UpdateSlider(CSliderCtrl* ctrl, Pylon::CIntegerParameter& integerParameter);
	void UpdateSliderText(CStatic* ctrl, Pylon::CIntegerParameter& integerParameter);
	UINT OnScroll(CScrollBar* pScrollBar, CSliderCtrl* pCtrl, Pylon::CIntegerParameter& integerParameter);
	void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);

public:
	//Pylon::CInstantCamera m_camera;
	static Pylon::CBaslerUniversalInstantCamera m_camera;
	Pylon::DeviceInfoList_t m_devices;
	// The camera
	BOOL m_updatingList;
	// The grab result retrieved from the camera
	Pylon::CGrabResultPtr m_ptrGrabResult;
	// The grab result as a windows DIB to be displayed on the screen
	Pylon::CPylonBitmapImage m_bitmapImage;
	Pylon::CAcquireSingleFrameConfiguration m_singleConfiguration;
	Pylon::CAcquireContinuousConfiguration m_continuousConfiguration;

	Pylon::String_t m_fullNameSelected;
	// Smart pointer to camera features
	Pylon::CIntegerParameter m_exposureTime;
	Pylon::CIntegerParameter m_gain;
	Pylon::CEnumParameter m_pixelFormat;
	// Callback handles
	GenApi::CallbackHandleType m_hExposureTime;
	GenApi::CallbackHandleType m_hGain;
	GenApi::CallbackHandleType m_hPixelFormat;
	GenApi::CallbackHandleType m_hTestImage;
	// Protects members.
	mutable CCriticalSection m_MemberLock;
	// Protects the converted bitmap.
	mutable CCriticalSection m_bmpLock;

public:
	int cntFrame;
	CListCtrl m_CameraDeviceListCtrl;
	// Exposure time slider
	CSliderCtrl m_ctrlExposureSlider;
	// Exposure time text control
	CStatic m_ctrlExposureText;
	// Gain slider
	CSliderCtrl m_ctrlGainSlider;
	// Gain text control
	CStatic m_ctrlGainText;
	afx_msg void OnBnClickedSingleShot();
	afx_msg void OnLvnItemchangedCameraList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnGrabOne();
	afx_msg void OnStartGrabbing();
	afx_msg void OnStopGrab();
	afx_msg void OnUpdateGrabOne(CCmdUI* pCmdUI);
	afx_msg void OnUpdateStartGrabbing(CCmdUI* pCmdUI);
	afx_msg void OnUpdateStopGrab(CCmdUI* pCmdUI);
	afx_msg void OnEnChangeFrameCnt();
	afx_msg void OnStnClickedExposure();
	afx_msg void OnCbnSelchangeTrigSelector();
	afx_msg void OnCbnSelchangeTrigMode();
	afx_msg void OnCbnSelchangeTrigSource();
	afx_msg void OnBnClickedExecBtn();
	CComboBox m_triggerSelector;
	CComboBox m_triggerMode;
	CComboBox m_triggerSource;
	std::vector<Pylon::CBaslerUniversalInstantCamera> GetBasCamInstance();
	CString camName;
	CPicWnd cPicWnd;
	IMGL::CIM tmpBitmapImage;
	afx_msg void OnCbnSelchangeCombBat();
	afx_msg void OnCbnSelchangeCombExpoauto();
	CComboBox m_AutoWB;
	CComboBox m_AutoExpos;
	CEdit m_ImWidth;
	CEdit m_ImHeight;
	CEdit m_ImFrame;
};
