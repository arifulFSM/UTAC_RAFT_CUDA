#include "pch.h"
#include "MeasurementProperties.h"

CMeasurementProperties::CMeasurementProperties()
{
	Clear();
}

CMeasurementProperties::~CMeasurementProperties() {}

CMeasurementProperties& CMeasurementProperties::operator = (CMeasurementProperties& co) {
	matrixspacing = co.matrixspacing;
	xdicesize = co.xdicesize;
	ydicesize = co.ydicesize;
	return *this;
}

void CMeasurementProperties::Clear()
{
	magic = 0;
	matrixspacing = 1;
	xdicesize = 1.f;
	ydicesize = 1.f;
}

void CMeasurementProperties::Serialize(CArchive& ar) {
	if (ar.IsStoring()) {
		magic = 0;
		ar << magic;
		ar << matrixspacing;
		ar << xdicesize;
		ar << ydicesize;
	}
	else {
		ar >> magic;
		ar >> matrixspacing;
		ar >> xdicesize;
		ar >> ydicesize;
	}
}

