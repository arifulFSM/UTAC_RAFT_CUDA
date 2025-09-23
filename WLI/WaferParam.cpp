#include "pch.h"
#include "WaferParam.h"

CWaferParam& CWaferParam::operator =(CWaferParam& co) {
	magic = co.magic;
	size = co.size;
	mapsize = co.mapsize;
	return *this;
}

CWaferParam::CWaferParam() {
	Clear();
}

CWaferParam::~CWaferParam() {}

void CWaferParam::Clear()
{
	magic = 0;
	size = 300;
	mapsize = 285;
}

void CWaferParam::Serialize(CArchive& ar) {
	if (ar.IsStoring()) {
		ar << magic;
		ar << size;
		ar << mapsize;
	}
	else {
		ar >> magic;
		ar >> size;
		ar >> mapsize;
	}
}