#pragma once

#pragma warning(disable:4786)

#include <vector>
class CHollow;
// Chuck mask is ALWAYS grid X& Y colinear but some holes may be absent
// .. Also there will always be a Vertical & Horizontal line
// .. going through Origin
struct Point { float X, Y; };
typedef std::vector<Point> Points;

class CChuckMask {
public:
	CChuckMask();
	virtual ~CChuckMask();
	bool bOffset;
	int nDesign;
public:
	BOOL LoadFromFile(LPCTSTR lpszFile);
	bool Write2File(CString& FileName);

	BOOL Draw(CDC* pDC, const CPoint& ptCenter, float fRatio, float Radius);

	float* GetCenterPoints(int& nCount);
	float GetXPitch() { return m_fXPitch; }
	float GetYPitch() { return m_fYPitch; }
	BOOL IsPointOnMask(float XCoor, float YCoor);
	unsigned int Rows();
	unsigned int Cols();
	bool AddHole(float X, float Y, float R);
	bool RemoveHole(float X, float Y);
	bool GetRow(unsigned int Row, Points& Pts);
	bool Cross(Points& Pts);
	bool Circumferance(Points& Pts);
	bool Grid(unsigned int X, unsigned int Y, Points& Pts);
	bool GetCenter(float& X, float& Y);
protected:
	CTypedPtrList<CPtrList, CHollow*> m_lstHollow;
	float m_fXPitch, m_fYPitch;
	BOOL ClearList();
private:
	bool Compile();
};