#include "pch.h"

#include "Point3.h"

MTH::SPoint3::SPoint3() {}

MTH::SPoint3::SPoint3(float a, float b, float c) :x(a), y(b), z(c) {}

void MTH::SPoint3::Zero() {
	x = 0; y = 0; z = 0;
}

void MTH::SPoint3::Set(float a, float b, float c) {
	x = a; y = b; z = z;
}

MTH::SPoint3 MTH::SPoint3::Normalize() {
	float mag = sqrtf(x * x + y * y + z * z);
	if (mag) {
		x /= mag; y /= mag; z /= mag;
	}
	else Set(0, 0, 0);
	return *this;
}

MTH::SPoint3 MTH::SPoint3::Normalize(SPoint3 p) {
	float mag = sqrtf(p.x * p.x + p.y * p.y + p.z * p.z);
	if (mag > 0) {
		p.x /= mag; p.y /= mag; p.z /= mag;
	}
	return p;
}

void MTH::SAbcD::PlN(MTH::SPoint3& pn, MTH::SPoint3& N) {
	A = N.x; B = N.y; C = N.z;
	D = A * pn.x + B * pn.y + C * pn.z;
}

MTH::SGeo::SGeo() {
	T = MTH::SPoint3(-rx, -ry, 0);
	U = MTH::SPoint3(rx, -ry, 0);
	V = MTH::SPoint3(rx, ry, 0);
	Pln(T, U, V);
}

MTH::SGeo::SGeo(float x, float y) {
	rx = x / 2.f; ry = y / 2.f;
	T = MTH::SPoint3(-rx, -ry, 0);
	U = MTH::SPoint3(rx, -ry, 0);
	V = MTH::SPoint3(rx, ry, 0);
	Pln(T, U, V);
}

float MTH::SGeo::Pz(MTH::SPoint3& p) {
	return p.z = (AB.D - (AB.A * p.x + AB.B * p.y)) / AB.C;
}

void MTH::SGeo::SetVz(MTH::SPoint3& p, SAbcD& A) {
	p.z = (A.D - (A.A * p.x + A.B * p.y)) / A.C;
}

void MTH::SGeo::SetVz(MTH::SPoint3& p, MTH::SPoint3& pn, MTH::SPoint3& N) {
	SAbcD A; A.PlN(pn, N);
	p.z = (A.D - (A.A * p.x + A.B * p.y)) / A.C;
}

void MTH::SGeo::SetVp(MTH::SPoint3& p) {
	Pz(T); Pz(U); Pz(V);
}

void MTH::SGeo::Pln(MTH::SPoint3& P, MTH::SPoint3& Q, MTH::SPoint3& R) {
	MTH::SPoint3 u; u = R - P;
	MTH::SPoint3 v; v = Q - P;
	N = Xss(u, v);
	AB.A = N.x; AB.B = N.y; AB.C = N.z;
	AB.D = AB.A * P.x + AB.B * P.y + AB.C * P.z;
}

void MTH::SGeo::Pln(MTH::SPoint3& P, MTH::SPoint3& N) {
	AB.A = N.x; AB.B = N.y; AB.C = N.z;
	AB.D = AB.A * P.x + AB.B * P.y + AB.C * P.z;
}

MTH::SPoint3 MTH::SGeo::Xss(const MTH::SPoint3& P, const MTH::SPoint3& Q) {
	MTH::SPoint3 R;
	R.x = P.y * Q.z - P.z * Q.y;
	R.y = -(P.x * Q.z - P.z * Q.x);
	R.z = P.x * Q.y - P.y * Q.x;
	return R;
}