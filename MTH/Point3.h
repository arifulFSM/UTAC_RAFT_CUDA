#pragma once

namespace MTH {
	struct SPointf {
		float x = 0, y = 0;
		float p = 0, m = 0;
	};

	struct SPoint2 {
		float x = 0, y = 0;
	};

	struct SPoint3 {
	public:
		float x = 0, y = 0, z = 0;

	public:
		SPoint3();
		SPoint3(float a, float b, float c);
		void Zero();
		void Set(float a, float b, float c);
		SPoint3 Normalize();
		static SPoint3 Normalize(SPoint3 p);
		SPoint3 operator-(const SPoint3& b) {
			SPoint3 a;
			a.x = this->x - b.x;
			a.y = this->y - b.y;
			a.z = this->z - b.z;
			return a;
		}
	};

	struct SAbcD {
		float A = 0, B = 0, C = 0, D = 0; // plane coeff.

		void PlN(MTH::SPoint3& pn, MTH::SPoint3& N);
	};
	struct SGeo { // geometry of tilt stage
		float rx = float(279.4f / 2.f);  // range x
		float ry = float(279.4f / 2.f);  // range y

		SAbcD AB;
		float Zh = 0;
		MTH::SPoint3 T, U, V, N;  // vertices's

	public:
		SGeo();
		SGeo(float x, float y);
		float Pz(MTH::SPoint3& p);
		void SetVz(MTH::SPoint3& p, SAbcD& A);
		void SetVz(MTH::SPoint3& p, MTH::SPoint3& pn, MTH::SPoint3& N);
		void SetVp(MTH::SPoint3& p);
		void Pln(MTH::SPoint3& P, MTH::SPoint3& Q, MTH::SPoint3& R);
		void Pln(MTH::SPoint3& P, MTH::SPoint3& N);
		MTH::SPoint3 Xss(const MTH::SPoint3& P, const MTH::SPoint3& Q);
	};
}
