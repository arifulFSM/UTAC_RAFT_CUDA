#pragma once
#define MaXPA				16
#define MAXMEPASET			9
#define BADDATANEG		-9999990

class CData
{
public:
	short magic;
	float DVal[MAXMEPASET * MaXPA];
	float Get(short idx);
	void Serialize(CArchive& ar);
};

