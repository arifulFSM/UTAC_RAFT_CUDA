#pragma once
class CMeasurementProperties
{
public:
	short magic;
	short matrixspacing;
	float xdicesize;
	float ydicesize;

	CMeasurementProperties();
	virtual ~CMeasurementProperties();
	CMeasurementProperties& operator=(CMeasurementProperties& co);
	void Clear();
	void Serialize(CArchive& ar);
};

