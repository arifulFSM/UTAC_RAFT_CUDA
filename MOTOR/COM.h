#pragma once
namespace MOT {
	class CCOM {
	protected:
		char nPa = 'n';
		BYTE nBit = 8, nStop = 1;

		void Yld(DWORD dur);

	public:
		//bool bSimu = true;

		CCOM();
		virtual ~CCOM();

		virtual int IsData();
		virtual bool Close();
		virtual bool Open(short nPort, int baud = 9600);
		virtual int Write(const char* buffer, int bufsz);
		virtual int Read1(char* buffer, int bufsz, char term, DWORD tout);

	public:
		bool IsOpened();

	protected:
		bool bOpened = false;
	};
}