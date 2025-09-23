#pragma once

#include <string>
#include <vector>

#define IRPT        32

#define BGR(b,g,r) (((b) << 16) + ((g) << 8) + (r))

namespace IMGL {
	enum eCH { REDC, GRNC, BLUC, WHTC, LST };
	enum eDIR { LR = 1, RL = 2, TB = 4, BT = 8 };

	struct SHis5 {
		const int nbins = 256;
		int His[256];
		float mx = 0, mn = 0, sf = 0;
	};

	struct SRGB {
		BYTE B, G, R;
		SRGB() {}
		SRGB(BYTE r, BYTE g, BYTE b) {
			R = r; G = g; B = b;
		}
		~SRGB() {}
		COLORREF Get() {
			return COLORREF(B + (G << 8) + (R << 16));
		}
		bool IsZero() {
			if (!R && !G && !B) return true; return false;
		}
		BYTE White() {
			return (R + 2 * G + B) / 4;
		}
		void SetGray(BYTE cr) {
			B = G = R = cr;
		}
	};

	struct SRGBA {
		BYTE A, B, G, R;

		SRGBA() {}
		SRGBA(BYTE r, BYTE g, BYTE b) {
			R = r; G = g; B = b;
		}
		~SRGBA() {}

		COLORREF Get() {
			return COLORREF(B + (G << 8) + (R << 16));
		}
		bool IsZero() {
			if (!R && !G && !B) return true; return false;
		}
		BYTE White() {
			return (R + 2 * G + B) / 4;
		}
		void SetGray(BYTE cr) {
			B = G = R = cr;
		}
	};

	struct SRGA {
		USHORT A;
		BYTE G, R;

		SRGA() {}
		~SRGA() {}
	};

	struct CRGB {
		COLORREF cr = 0;
		BYTE R = 0, G = 0, B = 0, cR = 0, cG = 0, cB = 0;
		CRGB() {}
		CRGB(COLORREF c) {
			cr = c;
			R = (c >> 16) & 0xFF; cR = 255 - R;
			G = (c >> 8) & 0xFF; cG = 255 - G;
			B = c & 0xFF; cB = 255 - B;
		}
	};

	struct SMat {
		USHORT W = 0; // Width of matrix [6/5/2021 FSM]
		USHORT D = 0; // Dimension of matrix = W * W [6/5/2021 FSM]
		PBYTE K = nullptr;

		USHORT Det();
		SMat();
		SMat(USHORT n);
		~SMat();
	};

	struct SPixel {
		int x = 0, y = 0;
		float z = 0;
		SPixel(int xx, int yy, float zz) : x(xx), y(xx), z(zz) {}
	};

	struct SKernal {
		std::vector<float>Ker;

		float denom();
		void Hat(short N);
		void Sine(short N, short pole);
		void Wedge(short N, short inc);

		SKernal();
		~SKernal();
	};

	struct SCgI {
		int Count = 0;
		int cx = 0, cy = 0; // cg
		float Dst = 0, dx = 0, dy = 0;
		float A = 0; // line angle in radian
		float M = 0, C = 0; // line equation: y = mx + c

		// temporary variable, to be removed or replaced
		int Gray = 0; // original gray intensity
		USHORT from = 0; // from
		COLORREF cr = 0; // to
		int R = 0, G = 0, B = 0;
		bool bRev = false;
		int xmx = 0, xmn = 0, ymx = 0, ymn = 0;

		float Ang();
		float Distance(SCgI& co);
		CString Report(int nType);
		void Clear();
		void SetColor(COLORREF cr);
	};

	struct SPtI { //! one points [6/16/2021 FSM]
		int x = 0, y = 0, t = 0;
		SPtI() {}
		SPtI(int xx, int yy, int tt) : x(xx), y(yy), t(tt) {}
		virtual ~SPtI() {}
	};

	struct SPtS { //! one points [6/16/2021 FSM]
		float x = 0, y = 0, t = 0;
	};

	struct SBin {
		short nBin = 0;
		USHORT* psRed = nullptr, * psGrn = nullptr, * psBlu = nullptr;

		SBin();
		SBin(int sz);
		virtual ~SBin();
		bool Alloc(int n);
		void Dealloc();
		void Clear();
		void Add(BYTE By, eCH Ch);
		USHORT GetMax(IMGL::eCH Ch, int st, int ed, bool bErase);
	};

	struct SBinU {
		short nBin = 0;
		USHORT* pU = nullptr;

		SBinU();
		SBinU(int sz);
		virtual ~SBinU();
		bool Alloc(int n);
		void Dealloc();
		void Clear();
		void Add(BYTE By);
		USHORT GetMax(int st, int ed, bool bErase);
	};

	struct Sln {
		std::vector<SPtS> Pts;

		void Clear();
		std::vector<SPtS>& Get();
		SPtS* GetPts(int n);
		float Aver();
		Sln();
		Sln(int sz);
		~Sln();
	};

	struct Slin {
		enum EDType {
			//! draw type
			LINEC, BARC, PIEC, POINTC, DASH, DDASH
		} type = LINEC;

		short nMkr = 0; // marker
		float xmin = 0, xmax = 0, ymin = 0, ymax = 0;
		COLORREF cr = RGB(0, 0, 0);

		Sln ln;
		std::string legnd = "";

		SPtS* getPeak(int i);

		bool dump(char* fname);
		int size();
		float aver();
		void add(IMGL::SPtS& p);
		void clear();
		void legend(std::string& legd);
		void maxmin();
		void resize(int sz);
		void setprop(COLORREF color, EDType ty = EDType::LINEC, short marker = 0);
		void smooth(int n);
	};

	struct SLines { //! group of lines that shares the same x & y dimension
		//! global size
		int sz = 0; // questionable var
		float xmin = 0, xmax = 0;
		float ymin = 0, ymax = 0;

		std::string name = "";
		std::vector<Slin> lins;

		SLines();
		virtual ~SLines();

		int size();
		void clear();
		void maxmin();
		void dump(FILE* fp);
		void dump1(FILE* fp);
		void label(const std::string& nam);
	};

	struct SAVal {
		float PCount = 0; // percentage count [10/9/2020 yuenl]
		float Bright = 0; // Brightness [10/9/2020 yuenl]
	};

	struct SLoc {
		int x = -1, y = -1; // location of edge on image [1/7/2020 yuenl]
		CString str();
		SLoc() {}
		SLoc(int xx, int yy) { x = xx; y = yy; }
	};

	struct SEdge {
		std::vector<float> width; // Width calculate from Inner and Outer [6/15/2020 yuenl]
		float px = 0, py = 0; // location on stage [1/7/2020 yuenl]
		std::vector<SLoc> Inner, Outer;

		void Clear();
		BOOL IsValid(int i);
		void DumpConst(FILE* fp);
		void DumpHeader(FILE* fp);
		void Dump(FILE* fp);
		float AveWidth();
	};

	class CIM2 {
		// 2 bytes image buffer
		int PixSz = 0;
		USHORT* pPix = nullptr;

		bool Alloc(int sz);
		void Dealloc();

	public:
		int wd = 0, ht = 0, ave = 0;

	public: // operations
		bool GetCG(SCgI& CG, USHORT from);
		bool GetStripsStats(SCgI& Cg, USHORT from);

	public:
		bool Create(int w, int h);
		bool IsNull() const;
		int BufferSize();
		int ReplacePixel(USHORT from, USHORT to);
		USHORT* GetPixelAddress(int x, int y) const;
		void Clear();
		void GetDim(int& w, int& h) const;
		void ReplacePixelA(USHORT from, USHORT to/*, int wd, int ht*/);

	public:
		bool MaxMinAve(USHORT& Mn, USHORT& Mx, int& ave) const;
		bool Histo(SBinU& Bin);

	public:
		CIM2() {}
		CIM2(int w, int h);
		CIM2(CIM2& Image);
		virtual ~CIM2();
		CIM2& operator= (CIM2& Im) {
			if (this == &Im) return *this;
			if (Im.IsNull()) return *this;
			int w, h; Im.GetDim(w, h);
			if (!Create(w, h)) return Im;
			memcpy(pPix, Im.pPix, BufferSize());
			return *this;
		}
		void Serialize(CArchive& ar);
	};

	class CIM : public CImage {
		//////////////////////////////////////////////////////////////////////////
		// RGB to gray: 0.2126 R + 0.7152 G + 0.0722 B
		//////////////////////////////////////////////////////////////////////////
		// Least square line info [2/1/2020 yuenl]
		BOOL bVertEdge = FALSE;
		//////////////////////////////////////////////////////////////////////////

	public:
		enum EDIR { LEFT, RIGHT, LASTD };
		enum EMOR { MISS, FIT, HIT, ERODE, DILATE, EDGE, OPEN, CLOSE };

	public:
		enum class WRGB { RED, GREEN, BLUE, WHITE, LAST };
		enum eCLRCHN {
			RED, GREEN, BLUE, COLOR, GREY,
			PSUDO1, PSUDO2, PSUDO3, PSUDO4, PSUDO5, PSUDO6
		};
		BYTE bmax = 0, bmin = 0;

	public:
		//////////////////////////////////////////////////////////////////////////
		// buffer access [11/3/2019 FSM]
		int GetBYPP();
		int GetBYPR();
		ULONG ImageSize();
		ULONG BufferSize() const;
		PVOID BufferStart();
		BOOL IsSameSize(const CIM& DiB);
		BOOL Histo(SBin& Bin, eCH Ch);
		double Intensity();
		short GetIntensity(int x, int y, eCH nChan);
		inline BYTE GetPixelG(int x, int y);
		inline COLORREF GetPixel(int x, int y);
		inline BYTE GetGray(int x, int y, int bpp);
		void RGBAddTo(int x, int y, int& R, int& G, int& B);
		//////////////////////////////////////////////////////////////////////////
		// Buffer modification [11/3/2019 FSM]
		bool Crop(int left, int top, int right, int bottom);
		bool Crop(IMGL::CIM& INew, int left, int top, int right, int bottom);
		bool SetBuffer(PBYTE pBuffer, int wd, int ht, int bpp);
		bool SetPixel(int x, int y, COLORREF& cr);
		bool SetPixel(int x, int y, BYTE r, BYTE g, BYTE b);
		bool SetLine(int y, PBYTE pBuffer, int len, int bpp);
		bool ReplacePixel(USHORT from, COLORREF to);
		int ReplacePixel(IMGL::CIM2& Im2, USHORT from, COLORREF to);
		COLORREF* GetLine(CPoint& st, CPoint& ed, int& sz, eCH Ch);
		BOOL GetLine(std::vector<SPixel>& line, CPoint& st, CPoint& ed);
		//////////////////////////////////////////////////////////////////////////
		// Buffer modification [11/3/2019 FSM]
		void AddDiv(CIM& Img);
		bool Sub(CIM& Img);
		void Copy(CIM& Img);
		void Morpho(IMGL::CIM& Im, IMGL::CIM::EMOR Op, IMGL::SMat& K, USHORT N, BYTE Thre);
		BOOL Convert32Bit();
		BOOL Convert24Bit();
		short GetI(int x, int y, eCH nChan);
		void Select(bool bRed, bool bGrn, bool bBlu);
		// [ 04192023 ZHIMING
		void Avg(std::vector<CIM>& vImg) {
			int vsize = int(vImg.size());
			if (vsize == 0) return;
			int wd, ht, bpp; GetDim(wd, ht, bpp);
			std::vector<PBYTE> vImgBuf; vImgBuf.resize(vsize);
#pragma omp parallel for
			for (int y = 0; y < ht; y++) {
				for (int i = 0; i < vsize; i++) {
					vImgBuf[i] = (PBYTE)vImg[i].GetPixelAddress(0, y);
				}
				for (int x = 0; x < wd; x++) {
					int r = *(vImgBuf[0]), g = *(vImgBuf[0] + 1), b = *(vImgBuf[0] + 2);
					switch (bpp) {
					case 24:
						for (int i = 1; i < vsize; i++) {
							r += *(vImgBuf[i]++);
							g += *(vImgBuf[i]++);
							b += *(vImgBuf[i]++);
						}
						*(vImgBuf[0]++) = r / vsize;
						*(vImgBuf[0]++) = g / vsize;
						*(vImgBuf[0]++) = b / vsize;
						break;
					}
				}
			}
		}
		// ]
		//////////////////////////////////////////////////////////////////////////
		// queries [11/5/2019 FSM]
		//////////////////////////////////////////////////////////////////////////
		// Draw operations [6/4/2021 FSM]
	public:
		PBYTE GetBufferStart() const;
		void RangeLEdge(std::vector<SLoc>& Inner, int& dx, int& dy, int* pX, int* pY);
		void DrawCross(int x1, int y1, int len, BYTE r, BYTE g, BYTE b);
		void DrawLine(int x1, int y1, int x2, int y2, BYTE r, BYTE g, BYTE b);
		void DrawCircle(int cx, int cy, int cr, BYTE r, BYTE g, BYTE b);
		void DrawSinuGridPattern(float px, float py, float shift, eCH ch, BOOL bBin);
		void DrawSinuHorzPattern(float py, float shift, eCH ch);
		void DrawSinuVertPattern(float px, float shift, eCH ch);
		void DrawBinHorzPattern(int n, float shift, eCH ch);
		void DrawBinVertPattern(int n, float shift, eCH ch);
		void DrawBinCheckerPattern(int n, float shift, eCH ch);
		void DrawFringePattern(float lamda, float shift, short A, short B, bool vert, eCH ch);
		//////////////////////////////////////////////////////////////////////////
	public:
		BYTE Set();

	public:
		bool ReduceSize(CIM& Image, short factor);
		CString Analyse();
		void GetDim(int& wd, int& ht, int& bpp) const;
		//////////////////////////////////////////////////////////////////////////
		// helpers [11/6/2019 FSM]
		void DumpEdge(PBYTE pSrc, PBYTE pDes, short wd, short bpp, char* filename);
		void DumpEdge(char* filename, int* pX, int* pY, short n);
		void DumpPoints(std::vector<SLoc>& ledge, std::vector<SLoc>& redge, char* filename);
		//////////////////////////////////////////////////////////////////////////
		// helpers [11/6/2019 FSM]
		//////////////////////////////////////////////////////////////////////////
		// Measure [11/3/2019 FSM]
		bool GetInnerEdge(std::vector<SLoc>& Outer, float& x, float& y);
		bool GetOuterEdge(std::vector<SLoc>& Inner, float& x, float& y);
		//////////////////////////////////////////////////////////////////////////
		int AverW();
		void AverStdev(size_t sz, int* pD, float& Ave, float& Stdev);
		void Dump(UINT sz, int* pX, int* pY, char* Filename);
		//////////////////////////////////////////////////////////////////////////
		bool IsInRect(int x, int y);
		int GetPx(float angle, int dy);
		int GetPy(float angle, int dx);
		float IScore();
		short Set(PBYTE src, short wdw, short bpp);
		short CountObject(short u, BYTE Obj, int x, int y);
		//short CountBlackWin(short u, CIM& Isrc, int x, int y);
		//short CountWhiteWin(short u, const CIM& Isrc, short x, short y);
		//////////////////////////////////////////////////////////////////////////
		// Maths [2/1/2020 yuenl]
		//////////////////////////////////////////////////////////////////////////
		// Convolution operation [1/5/2020 yuenl]
		void ConvlHSobel();
		void ConvlVSobel();
		bool Sobel();
		bool Magnitude(CIM& I1, CIM& I2);
		bool Contour(short nlvl);
		void Convl(float k1, float k2, float k3, float k4, float k5, float k6, float k7, float k8, float k9);
		//////////////////////////////////////////////////////////////////////////
		void ConvlLowPass();
		bool Smooth(USHORT N);
		//////////////////////////////////////////////////////////////////////////
		void SmoothLeft(std::vector<SLoc>& Inner, short wdw, short ntime, short bpp);
		//////////////////////////////////////////////////////////////////////////
		// Process [11/3/2019 FSM]
		bool Compare(CIM& Img);
		void ContrastStretch(int Max, int Min);
		bool FillEdge(BYTE Thres);
		//bool DeSpeckleWhite(short wdw, short lmt);
		//bool DeSpeckleBlack(short wdw, short lmt);
		UINT Thining(short wdw, short lmt, BYTE Obj);
		//////////////////////////////////////////////////////////////////////////
		// Input output [11/2/2019 FSM]
		bool SavePic();
		CString SavePicTime();
		bool SavePic(CString name);
		bool SaveCSV(CString name);
		CString GetProgramDir();
		void Serialize(CArchive& ar);
		//////////////////////////////////////////////////////////////////////////
		// LUT related [11/3/2019 FSM]
		void SetGrayScale();
		void MakeGrayLUT();
		void CopyLUT(CIM Image);
		//////////////////////////////////////////////////////////////////////////
		// Color related [11/3/2019 FSM]
		COLORREF GetColorRef(PBYTE p);
		static BYTE GetRed(COLORREF cr);
		static BYTE GetGreen(COLORREF cr);
		static BYTE GetBlue(COLORREF cr);
		static BYTE GetWhite(COLORREF cr);
		void RetainRed();
		void RetainGreen();
		void RetainBlue();
		void RetainGrey();
		//////////////////////////////////////////////////////////////////////////
		void BestRect(CRect& rc);
		//////////////////////////////////////////////////////////////////////////
		bool ConvTo8Bit(CIM& Im24) {
			// convert 24 bits to 8 bits
			return true;
		}
		bool ConvTo24Bit(CIM& Im8) {
			// convert 8 bits to 24 bits
			return true;
		}
		bool MaxMin();
		bool Color2Gray();
		bool Normalize();
		//bool BThres(short lvl);
		bool BThres(short lvl, BYTE low, BYTE hi);
		bool BThres32();
		SAVal ImgProperties(USHORT thres);
		std::vector<SAVal> ImgRegionProperties(USHORT thres);

	public:
		CIM();
		CIM(int wd, int ht, int bpp) { Create(wd, ht, bpp); }
		virtual ~CIM();
		const CIM& operator= (const CIM& Im);
		const CIM& operator= (const CIM2& Im);
		CIM& operator-= (CIM& Rhs);
		const CIM operator-(CIM& rhs) const {
			return CIM(*this) -= rhs;
		};
		BOOL CreateA(int wd, int ht, int bpp);
		BOOL DivideGrey(CIM& Im1, CIM& Im2);
		BOOL DivideColor(CIM& Im1, CIM& Im2);
		BOOL SubAbs(CIM& Im1, CIM& Im2);
	};

	class CIM16x3 {
		// support 3 float per pixel [12/24/2020 FSM]
		float* pPixels = NULL;
		int wd = 0, ht = 0;

	public:
		BOOL Create(int w, int h);
		BOOL GetMaxMin(float& max, float& min);
		BOOL IsNull();
		BOOL IsSameSize(CIM& DiB);
		BOOL Make24BPPStretch(CIM& Im);
		float* GetPixelAddress(int x, int y);
		int size();
		void Destroy();

	public:
		CIM16x3();
		virtual ~CIM16x3();
	};

	class CIM16 {
	public:
		// support 1 float (4 bytes, 32 bits) per pixel [12/24/2020 FSM]
		float* pPix = nullptr;
		float zHtFac_um = 0; // Z Height factor [7/8/2021 FSM]
		int wd = 0, ht = 0;

	private:
		void Convl(float k1, float k2, float k3, float k4, float k5, float k6, float k7, float k8, float k9);
		void Convl5x5(int* kernel);

	public:
		SHis5 His5;
		IMGL::CIM Im8; // regular BMP image of CIM16 [12/10/2020 FSM]
		float mx = 0, mn = 0;

	public:
		BOOL Add(IMGL::CIM16* Im);
		bool BThres(float lvl);
		BOOL ConvertPhase2Height_um(float fac_um, CIM16& Im_rad);
		BOOL Convolve(SKernal& K, int y, float* buf, int wd);
		BOOL Create(int w, int h);
		BOOL Devide(int n);
		BOOL DumpHLine(char* fname, int y);
		BOOL DumpVLine(char* fname, int x);
		BOOL GetHLn(Sln& ln, int y);
		BOOL GetVLn(Sln& ln, int x);
		BOOL GetAreaAvgHLn(Sln& ln, int y); // 07122023
		void MaxMin();
		BOOL GetMaxMin(float& max, float& min);
		BOOL GetMaxMin2(float& max, float& min);
		BOOL Histo(int* Bin, int nBin);
		BOOL Histo256();
		BOOL IsNull();
		BOOL IsSameSize(CIM& Im);
		BOOL IsSameSize(CIM16& Image);
		bool Load(const std::wstring& PathName);
		BOOL Make24BPP();
		BOOL Mult(float fac);
		BOOL Make24BPPStretch(CIM& Im);
		BOOL Make24H();
		BOOL Make24HStretch(CIM& Im);
		BOOL Make8BPP();
		BOOL Resampling(IMGL::CIM& Img, int cx, int cy);
		BOOL ResizeIM(IMGL::CIM16& newIM, float Percent);
		bool Save(const std::wstring& PathName);
		BOOL Save_CSV(std::wstring& PathName);
		BOOL SaveBMP(std::wstring& PathName, float Percent);
		BOOL SaveCSV(std::wstring& PathName, float Percent);
		BOOL SaveIM16(std::wstring& PathName, float Percent);
		BOOL SetBuffer(CIM16& Im1, CIM16& Im2);
		BOOL SetBuffer(float* pBuf, int len);
		BOOL WavinessH(CIM16& Im);
		BOOL WavinessV(CIM16& Im);
		float* GetBuffer();
		float* GetBufferStart();
		float* GetPixelAddress(int x, int y);
		inline float GetPixel(int x, int y);
		int GetHeight();
		int GetWidth();
		int size();
		TCHAR* Info();
		ULONG GetBufferSize();
		void Add(CIM16& Lhs, CIM16& Rhs);
		void AddPixel(int x, int y, float val);
		void ConvlLowPass();
		void Dealloc();
		void Destroy();
		void FlipX();
		void FlipY();
		void GetDim(int& w, int& h);
		void Level();
		void Normalize();
		void Offset(float off);
		void OffsetPixel(int x, int y, float val);
		void SetPixel(int x, int y, float val);
		void Smooth(USHORT N);
		void Substract(CIM16& Lhs, CIM16& Rhs);
		void SubtractNoise(CIM16& ImSmth);
		bool Unwrap(IMGL::eDIR Dir);
		bool UnwrapBT();
		bool UnwrapLR();
		bool UnwrapRL();
		bool UnwrapTB();
		void ZeroMean();
		void ZeroShift();

	public:
		CIM16();
		CIM16(int w, int h);
		CIM16(CIM16& Image);
		virtual ~CIM16();
		void Serialize(CArchive& ar);
		CIM16 operator- (CIM16& Rhs); // image subtraction [10/7/2020 FSM]
		CIM16 operator+ (CIM16& Rhs); // image addition [10/7/2020 FSM]
		CIM16& operator= (CIM16& Im);
		CIM16& operator/ (CIM16& Rhs); // remove background [10/7/2020 FSM]
	};

	struct SNxtItm {
		bool nxt = false;
		int count = 0;
	};

	struct SNxt {
#define NBINS 512
		enum ETYPE { BLOB, STRIPE };

		const int nBin = NBINS;

		int nNext = 1;
		SNxtItm nxt[NBINS];

	public:
		int Gets();
		int Count();
		int Max();
		void Reset();
		bool Clear(int n);

	public:
		void Identify(IMGL::CIM& Des, IMGL::CIM2& Ref, int lmt, int clmt, std::vector<IMGL::SCgI>& CG, ETYPE nType);
		bool Replace(USHORT from, IMGL::CIM& Des, IMGL::CIM2& Ref, IMGL::SCgI& Cg, COLORREF cr, ETYPE nType);
		bool Segment(IMGL::CIM& Src, IMGL::CIM2& Des, BYTE Obj);
	};
}
