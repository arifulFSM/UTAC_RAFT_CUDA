#pragma once
#include "StdAfx.h"

class fft_lib
{
public:
	fft_lib(void);
	void correl(double data1[], double data2[], unsigned long n, double ans[]);
	void RearrangeCorrelation(double c[], long n);
	void GetAllMax(double d[], int N, double t,int M, int* imax, double *mmax);
	void GetAllMaxInRange(double d[], int N, int R0, int R1, double thr, int M, int* imax, double *mmax);
	double average(double d[], int N);
	double std_deviation(double d[], int N);
	void ExFilter(double d[],int N, double t, double f[]);
	void distribution(double s[], int N, int L, double min, double max, double D[]);
	void SlidingAverage(double x[], int N, int m, double y[]);
	void BaseLine(int N, double S[], double *Base);
	void hilbert(double data[], unsigned long n, double amp[], double phase[]);
	void WaveToNum(int N, double *Lam, double *S, double *Wn, double *Sp);
    void remove_dc(double d[], int N);
	void memcof(double data[], int n, int m, double *xms, double d[]);//index from 1 to n
	void memcof2(double data[], int n, int m, double *xms, double d[]); //index from 0 to n-1 ggl
	double evlmem(double fdt, double d[], int m, double xms);//index from 1 to n
	double evlmem2(double fdt, double d[], int m, double xms);//index from 0 to n-1 ggl
	void GetMaxImax(double d[], int N, double *max, int *imax);
	void GetMaxImaxInRange(double d[], int N, int R1, int R2, double *max, int *imax);
	void SetInverseInterpolation(int N0, double *Lam,int N, double* WaveNum, 
		double* WLam, int &NumLam0, int &NumLam1, double &Resolution512, double &Resolution64K);
	void InverseInterpolation(int N0, double *Lam, double *Spec0,int N, double* WaveNum, double* SpectWN, 
		double &Delta512, double &Resolution16K, double &Resolution64K, int nXInterpolation );
	void realft(double data[], unsigned long n, int isign);
	void realft2(double data[], unsigned long n, int isign);
	void four1(double data[], unsigned long nn, int isign);
	void real_part(double data[], double real[], unsigned long nn);
	void imag_part(double data[], double imag[], unsigned long nn);
	void GetAllMax1(double d[], int N, int *M, int* imax, double *mmax);
	void GetMaxInRange(double d[], int L, int H, double *max, int *Nmax);
	void phase_rectifier(double phase[], unsigned long N);
	double *vector(long nl, long nh);
	void free_vector(double *v, long nl, long nh);
	void nrerror(char error_text[]);
	void twofft(double data1[], double data2[], double fft1[], double fft2[],	unsigned long n);
	void quickSort(double arr[], int ind[], int left, int right); 
	void gaussj(double **a, int n, double **b, int m);
	void GetMinMax(double d[], int N, double *min, double *max);
	void Weighted_distribution(double s[],double w[], int N, int L, double min, double max, double D[]);
	void mnbrak(float *ax, float *bx, float *cx, float *fa, float *fb, float *fc,
	float (*func)(float));
	float brent(float ax, float bx, float cx, float (*f)(float), float tol,float *xmin);
	void GetFirstMax(double d[], int N, int *M, int* imax, double *mmax);

	~fft_lib(void);
};
