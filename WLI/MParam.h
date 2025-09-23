#pragma once
#ifndef __MPARAM_H__
#define __MPARAM_H__

// Note: changing MaXPA or MAXMEPASET value will void CRecipe413 downward compatibility[3/24/2012 Administrator]
#define MaXPA				16
#define MAXMEPASET			9
#define MAXMATCHPATT		9
#define MAXTGTSET			4

struct MParam
{
	short D;

	MParam() {
		Clear();
	}

	void Clear() 
	{
		D = 2;
	}
};

#endif

