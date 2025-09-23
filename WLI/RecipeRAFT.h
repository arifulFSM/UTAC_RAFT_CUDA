#pragma once
#include "WaferParam.h"
#include "Pattern.h"
#include "MeasurementProperties.h"
#include "MeParam.h"

#define RCPNAMELEN			64
#define CURRENTRECIPEVERSION	97


class CRecipeRAFT
{
private:
	CWaferParam Wp;

public:
	short Version;
	float AFCalibZ;
	float AFTiltZ;
	float AFRange;
	float AFStepSize;
	float MERange;
	float MEFrames;

	wchar_t RcpeName[RCPNAMELEN + 1];
	CPattern Patt;
	CMeasurementProperties Me;
	CMeParam MeParam[MAXMEPASET];
	CRecipeRAFT();
	CRecipeRAFT& operator=(CRecipeRAFT& co);
	void Clear();
	CWaferParam* GetWp();
	wchar_t* GetRecipeName();
	void Renumber();
	void UpdateControl(CListCtrl& Ctrl);
	BOOL SaveRecipe(BOOL bSortPrompt = TRUE);
	BOOL IsMarkers();
	BOOL SaveRecipe(CString filename, BOOL bSort);
	BOOL strscpy(wchar_t* dest, short szdest, const wchar_t* sorc);
	void Serialize(CArchive& ar);
	BOOL LoadRecipe();
	BOOL Load(CString RcpName);

};

