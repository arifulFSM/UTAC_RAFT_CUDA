#pragma once

namespace MOT {
//#define PZHOMEPOS 125.00f
#define PZHOMEPOS 50.00f

	//#define _SIMUPIEZO
	enum class EPZAXIS { A, B, C, LAST };

	class CPiezoB {
	public:
		float Mx_um = 50.0f; 
		//float Mx_um = 100.0f; 
		float Mn_um = 0;
		float NowPos_um = PZHOMEPOS;

	protected:
		EPZAXIS ID = EPZAXIS::A;  // ID < 0 is failed [10/22/2020 yuenl]
		int iError = 0;
		char szAxes[17] = "";
		BOOL bFlags[3] = { TRUE };
		BOOL bIsMoving[3] = { FALSE };
		double dPos_um[3] = { 0 }; // unit is um

	public:
		void Yld();

	public:
		virtual bool Goto(float pos_um, bool bWait);
		virtual bool IsMoving(DWORD tout);
		virtual bool Step(float dist_um);
		virtual bool Wait(DWORD tout);
		virtual float GetPos_um();
		virtual int GetError(int ID);

		//////////////////////////////////////////////////////////////////////////
		virtual bool Reset();
		virtual bool AutoZero();
		//////////////////////////////////////////////////////////////////////////
		virtual bool Initialize();
		virtual bool Deinitialize();

	public:
		CPiezoB();
		~CPiezoB();
	};

#ifndef _SIMUPIEZO
	class CPiezoPI : public CPiezoB {
		int iID = -1;

	protected:
		int GetError(int ID);

	public:
		bool AutoZero();
		bool Deinitialize();
		bool Initialize();
		bool IsMoving(DWORD tout);
		bool Reset();
		bool Wait(DWORD tout);

	public:
		float GetPos_um();
		bool Goto(float pos_um, bool bWait);
		bool Step(float dist_um);

	public:
		CPiezoPI();
		~CPiezoPI();
	};
#endif

	class CPiezo {
	protected:
		std::unique_ptr<CPiezoB> Pz;

	public:
		bool Initialize();
		bool Deinitialize();

	public:
		CPiezoB* Get();
		float GetPos_um();
		bool WaitStop(DWORD tout);
		bool Goto(float pos_um, bool bWait);
		void Yld(DWORD tout);

	public:
		CPiezo();
		virtual ~CPiezo();
	};
}
