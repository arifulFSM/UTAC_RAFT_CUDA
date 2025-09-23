#pragma once

class CLSF3D {
public:
	CLSF3D();
	virtual ~CLSF3D();

	void Init();

public:
	virtual void Input(double dfX, double dfY, double dfZ);
	virtual BOOL GetResult(double& dfA, double& dfB, double& dfC);

protected:
	double m_dfSxx, m_dfSyy, m_dfSxy;
	double m_dfSx, m_dfSy;
	double m_dfSo;

	double m_dfSxz, m_dfSyz;
	double m_dfSz;
};
