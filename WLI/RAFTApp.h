#pragma once
#include "ChuckMask.h"
#include "RecipeRAFT.h"
#include "ResultRAFT.h"

class CRAFTApp
{
private:
	CChuckMask* m_pChuckMask;

public:
	CRecipeRAFT RcpSetup;
	CResultRAFT Result;

	CRAFTApp();
	~CRAFTApp();
	BOOL LoadChuckMask();
	CChuckMask* GetChuckMask() { return m_pChuckMask; }
	BOOL ValidCheckRcp(CRecipeRAFT& Rcp);
	bool isReset; //Morsalinn
	long long start_sec; // Morsalinn
};

extern CRAFTApp* pRAFTApp;

