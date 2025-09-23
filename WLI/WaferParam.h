#pragma once
class CWaferParam
{
private:
	/**
	 * Version description: Should be used while serializing and deserilization
	 */
	short magic;

public:
	/**
	 * Wafer diameter
	 */
	short size;

	/**
	 * Daimeter of the wafer that is actually measure, specified in term of percentage.
	 */
	short mapsize;

	CWaferParam();
	~CWaferParam();
	CWaferParam& operator=(CWaferParam& co);
	void Clear();
	void Serialize(CArchive& ar);
};

