#include "pch.h"
#include "fft_lib.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>

#define NR_END 1
#define FREE_ARG char*
#define MAX_LOADSTRING 100
#define PATH_LENGTH    512
#define PI 3.1415926535897932384626433832795
#define MAX_KL         256
#define PSIZE_7        127

double swap;
#define SWAP(a,b) {swap=(a);(a)=(b);(b)=swap;}
#define SQR(x) (x)*(x)
#define SecondD(a,b,c) ((c)-2*(b)+(a))
#define FirstD(a,b) ((b)-(a))
#define SIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))
static double dmaxarg1, dmaxarg2;
static float maxarg1, maxarg2;
#define FMAX(a,b) (maxarg1=(a),maxarg2=(b),(maxarg1) > (maxarg2) ?\
        (maxarg1) : (maxarg2))
//extern double Resolution512;
//extern double Resolution16K;
//extern double Resolution64K;
//extern double *WaveNum;
//extern double *WaveL;
//extern double Delta512;
//extern int NumLam0;
//extern int NumLam1;
//extern double *WLam;

fft_lib::fft_lib(void) {}
double* fft_lib::vector(long nl, long nh) {
	double* v;
	v = (double*)calloc((size_t)(nh - nl + 1 + NR_END), sizeof(double));
	char str[] = "allocation failure in vector()";
	if (!v) nrerror(str);
	return v - nl + NR_END;
}
void fft_lib::nrerror(char error_text[]) {
	fprintf(stderr, "Numerical Recipes run-time error...\n");
	fprintf(stderr, "%s\n", error_text);
	fprintf(stderr, "...now exiting to system...\n");
	exit(1);
}

void fft_lib::free_vector(double* v, long nl, long nh) {
	free((FREE_ARG)(v + nl - NR_END));
}
void fft_lib::twofft(double data1[], double data2[], double fft1[], double fft2[], unsigned long n) {
	unsigned long nn3, nn2, jj, j;
	double rep, rem, aip, aim;

	nn3 = 1 + (nn2 = 2 + n + n);
	for (j = 1, jj = 2; j <= n; j++, jj += 2) {
		fft1[jj - 1] = data1[j];
		fft1[jj] = data2[j];
	}
	four1(fft1, n, 1);
	fft2[1] = fft1[2];
	fft1[2] = fft2[2] = 0.0;
	for (j = 3; j <= n + 1; j += 2) {
		rep = 0.5 * (fft1[j] + fft1[nn2 - j]);
		rem = 0.5 * (fft1[j] - fft1[nn2 - j]);
		aip = 0.5 * (fft1[j + 1] + fft1[nn3 - j]);
		aim = 0.5 * (fft1[j + 1] - fft1[nn3 - j]);
		fft1[j] = rep;
		fft1[j + 1] = aim;
		fft1[nn2 - j] = rep;
		fft1[nn3 - j] = -aim;
		fft2[j] = aip;
		fft2[j + 1] = -rem;
		fft2[nn2 - j] = aip;
		fft2[nn3 - j] = rem;
	}
}

void fft_lib::correl(double data1[], double data2[], unsigned long n, double ans[]) {
	unsigned long no2, i;
	double dum, * fft;

	fft = vector(1, n << 1);
	twofft(data1, data2, fft, ans, n);
	no2 = n >> 1;
	for (i = 2; i <= n + 2; i += 2) {
		ans[i - 1] = (fft[i - 1] * (dum = ans[i - 1]) + fft[i] * ans[i]) / no2;
		ans[i] = (fft[i] * dum - fft[i - 1] * ans[i]) / no2;
	}
	ans[2] = ans[n + 1];
	realft(ans, n, -1);
	free_vector(fft, 1, n << 1);
}
void fft_lib::RearrangeCorrelation(double c[], long n) {
	int k;
	double* pos, * neg;
	pos = vector(1, n >> 1);
	neg = vector(1, n >> 1);
	for (k = 1; k <= n / 2; k++) {
		pos[k] = c[k];
		neg[k] = c[k + n / 2];
	}
	for (k = 1; k <= n / 2; k++) {
		c[k] = neg[k];
		c[k + n / 2] = pos[k];
	}
	free_vector(pos, 1, n >> 1);
	free_vector(neg, 1, n >> 1);
}
void fft_lib::GetAllMax(double d[], int N, double thr, int M, int* imax, double* mmax) {
	int n = 0, m = 0, k = 0, imx;
	int timax[64];
	double tmmax[64];
	double smax;
	for (n = 0; n < 64; n++) {
		timax[n] = 0;
		tmmax[n] = 0.0;
	}
	GetMaxImax(d, N, &smax, &imx); //find peak
	for (n = 1; n < N - 1; n++) //find all peaks
	{
		if (FirstD(d[n - 1], d[n]) > 0 && SecondD(d[n + 1], d[n], d[n - 1]) < 0.0 && FirstD(d[n + 1], d[n]) > 0) {
			if (d[n] > thr * smax && k < M && k < 64) {
				tmmax[k] = d[n];
				timax[k] = n;
				k++;
			}
		}
	}

	quickSort(tmmax, timax, 0, k);
	for (m = 0; m < k; m++) {
		mmax[m] = tmmax[k - m];
		imax[m] = timax[k - m];
	}
	////////////////////////////////////////////
	imax[0] = imx;
	mmax[0] = smax;
	///////////////////////////////////////////
}
void fft_lib::GetAllMaxInRange(double d[], int N, int R0, int R1, double thr, int M, int* imax, double* mmax) {
	int n = 0, m = 0, k = 0, imx;
	int timax[64];
	double tmmax[64];
	double smax;
	for (n = 0; n < 64; n++) {
		timax[n] = 0;
		tmmax[n] = 0.0;
	}
	GetMaxImaxInRange(d, N, R0, R1, &smax, &imx);
	for (n = R0; n < R1 - 1; n++) {
		if (FirstD(d[n - 1], d[n]) > 0 && SecondD(d[n + 1], d[n], d[n - 1]) < 0.0 && FirstD(d[n + 1], d[n]) > 0) {
			if (d[n] > thr * smax && k < M && k < 64) {
				tmmax[k] = d[n] / smax;
				timax[k] = n;
				k++;
			}
		}
	}

	quickSort(tmmax, timax, 0, k);
	for (m = 0; m < k; m++) {
		mmax[m] = tmmax[k - m];
		imax[m] = timax[k - m];
	}

	imax[0] = imx;
	mmax[0] = smax;
}

void fft_lib::GetFirstMax(double d[], int N, int* M, int* imax, double* mmax) {
	int n = 0, m = 0, k = 0;
	double mx = 0.0;
	int* timax;
	double* tmmax;
	timax = new int[N];
	tmmax = new double[N];
	for (n = 1; n < N - 1; n++) {
		if (FirstD(d[n - 1], d[n]) > 0 && SecondD(d[n + 1], d[n], d[n - 1]) < 0.0 && FirstD(d[n + 1], d[n]) > 0) {
			tmmax[k] = d[n];
			timax[k] = n;
			k++;
		}
	}
	mx = 0.0;
	//find first
	for (n = 0; n < k; n++) {
		if (tmmax[n] > mx) {
			mx = tmmax[n];
			imax[0] = n;
			mmax[0] = tmmax[n];
		}
	}
	//find second
	mx = 0.0;
	for (n = 0; n < k; n++) {
		if (n != imax[0]) {
			if (tmmax[n] > mx) {
				mx = tmmax[n];
				imax[1] = n;
				mmax[1] = tmmax[n];
			}
		}
	}
	//find third
	mx = 0.0;
	for (n = 0; n < k; n++) {
		if (n != imax[0] && n != imax[1]) {
			if (tmmax[n] > mx) {
				mx = tmmax[n];
				imax[2] = n;
				mmax[2] = tmmax[n];
			}
		}
	}
	//find fourth
	mx = 0.0;
	for (n = 0; n < k; n++) {
		if (n != imax[0] && n != imax[1] && n != imax[2]) {
			if (tmmax[n] > mx) {
				mx = tmmax[n];
				imax[3] = n;
				mmax[3] = tmmax[n];
			}
		}
	}
	//find fifthh
	mx = 0.0;
	for (n = 0; n < k; n++) {
		if (n != imax[0] && n != imax[1] && n != imax[2] && n != imax[3]) {
			if (tmmax[n] > mx) {
				mx = tmmax[n];
				imax[4] = n;
				mmax[4] = tmmax[n];
			}
		}
	}
	//find sixth
	mx = 0.0;
	for (n = 0; n < k; n++) {
		if (n != imax[0] && n != imax[1] && n != imax[2] && n != imax[3] && n != imax[4]) {
			if (tmmax[n] > mx) {
				mx = tmmax[n];
				imax[5] = n;
				mmax[5] = tmmax[n];
			}
		}
	}
	//find seventh
	mx = 0.0;
	for (n = 0; n < k; n++) {
		if (n != imax[0] && n != imax[1] && n != imax[2] && n != imax[3] && n != imax[4] && n != imax[5]) {
			if (tmmax[n] > mx) {
				mx = tmmax[n];
				imax[6] = n;
				mmax[6] = tmmax[n];
			}
		}
	}
	//find eighth
	mx = 0.0;
	for (n = 0; n < k; n++) {
		if (n != imax[0] && n != imax[1] && n != imax[2] && n != imax[3] && n != imax[4] && n != imax[5] && n != imax[6]) {
			if (tmmax[n] > mx) {
				mx = tmmax[n];
				imax[7] = n;
				mmax[7] = tmmax[n];
			}
		}
	}
	*M = 8;
	delete[] tmmax; //ggl
	delete[] timax;
}
void fft_lib::quickSort(double arr[], int ind[], int left, int right) {
	int i = left, j = right;
	int itmp;
	double dtmp;
	double pivot = arr[(left + right) / 2];
	/* partition */
	while (i <= j) {
		while (arr[i] < pivot)
			i++;
		while (arr[j] > pivot)
			j--;
		if (i <= j) {
			dtmp = arr[i];
			itmp = ind[i];
			arr[i] = arr[j];
			ind[i] = ind[j];
			arr[j] = dtmp;
			ind[j] = itmp;
			i++;
			j--;
		}
	};
	/* recursion */
	if (left < j)
		quickSort(arr, ind, left, j);
	if (i < right)
		quickSort(arr, ind, i, right);
}

double fft_lib::average(double d[], int N) {
	int n;
	double a = 0.0;
	for (n = 0; n < N; n++)
		a += d[n];
	a /= (double)N;
	return a;;
}
double fft_lib::std_deviation(double d[], int N) {
	int n;
	double a = 0.0, b = 0.0;
	for (n = 0; n < N; n++)
		a += d[n];
	a /= (double)N;
	for (n = 0; n < N; n++)
		b += (a - d[n]) * (a - d[n]);
	b = sqrt(b / N);
	return b;
}
void fft_lib::ExFilter(double d[], int N, double t, double f[]) {
	int n;
	double a = d[0];
	f[0] = d[0];
	for (n = 1; n < N; n++) {
		if (fabs(d[n] - a) > t)
			f[n] = a;
		else
			f[n] = d[n];
		a = (n * a + d[n]) / (n + 1);
	}
}
void fft_lib::distribution(double s[], int N, int L, double min, double max, double D[]) {
	int l, n;
	double* lev, h;
	lev = new double[L];
	h = (max - min) / L;
	for (l = 0; l < L; l++) {
		lev[l] = min + l * h;
		D[l] = 0;
	}
	for (n = 0; n < N; n++) {
		for (l = 0; l < L - 1; l++) {
			if (s[n] >= lev[l] && s[n] < lev[l + 1])
				D[l]++;
		}
		if (s[n] >= lev[L - 1])
			D[L - 1]++;
	}

	delete[] lev;
}
/*void fft_lib::SlidingAverage(double x[], int N, int m, double y[])
{
	int n, k, l;
	for(n=0; n< N;n++)
	{
		y[n]=0.0;
		for(k=-m;k<m;k++)
		{
			l=n+k;
			if(l<0)
				l=-l;
			if(l>N-1)
				l=l-N;
			y[n]+=x[l];
		}
		y[n]=y[n]/(2*m+1);
	}
}*/

void fft_lib::SlidingAverage(double x[], int N, int m, double y[]) { //ggl modified the original one, fix the bugs at both ends
	int n, k, l;
	double temp;
	for (n = m; n < N - m; n++) {
		temp = 0.0;
		for (k = -m; k <= m; k++) {
			l = n + k;
			temp += x[l];
		}
		y[n] = temp / (2 * m + 1); //average
	}
	//linear fit on both ends
	double a11 = 0.0, a12 = 0.0, a22 = 0.0, c1 = 0.0, c2 = 0.0;

	//beginin
	for (n = 0; n < m; ++n) {
		a11 += n * n;
		a12 += n;
		c1 += n * x[n];
		c2 += x[n];
	}
	a22 = m;
	temp = a11 * a22 - a12 * a12;
	double slope = (c1 * a22 - c2 * a12) / temp;
	double intercept = (a11 * c2 - a12 * c1) / temp;
	for (n = 0; n < m; ++n)
		y[n] = slope * n + intercept;

	//end
	a11 = 0.0, a12 = 0.0, a22 = 0.0, c1 = 0.0, c2 = 0.0;
	for (n = N - m; n < N; ++n) {
		a11 += n * n;
		a12 += n;
		c1 += n * x[n];
		c2 += x[n];
	}
	a22 = m;
	temp = a11 * a22 - a12 * a12;
	slope = (c1 * a22 - c2 * a12) / temp;
	intercept = (a11 * c2 - a12 * c1) / temp;
	for (n = N - m; n < N; ++n)
		y[n] = slope * n + intercept;
}
void fft_lib::BaseLine(int N, double S[], double* Base) {
	int n;
	FILE* fp;
	double nu, ** a, ** b, * s, * f, mn, mx;
	a = new double* [6];
	b = new double* [6];
	s = new double[6];
	f = new double[6];
	for (n = 0; n < 6; n++) {
		s[n] = 0.0;
		f[n] = 0.0;
		a[n] = new double[6];
		b[n] = new double[2];
	}
	for (n = 0; n < N; n++) {
		nu = (double)n / (double)N;
		s[0] += 1;
		s[1] += nu;
		s[2] += nu * nu;
		s[3] += nu * nu * nu;
		s[4] += nu * nu * nu * nu;
		s[5] += nu * nu * nu * nu * nu;
		s[6] += nu * nu * nu * nu * nu * nu;
		f[0] += S[n];
		f[1] += S[n] * nu;
		f[2] += S[n] * nu * nu;
		f[3] += S[n] * nu * nu * nu;
	}
	a[1][1] = s[0];
	a[1][2] = a[2][1] = s[1];
	a[1][3] = a[2][2] = a[3][1] = s[2];
	a[1][4] = a[2][3] = a[3][2] = a[4][1] = s[3];
	a[2][4] = a[3][3] = a[4][2] = s[4];
	a[3][4] = a[4][3] = s[5];
	a[4][4] = s[6];
	b[1][1] = f[0];
	b[2][1] = f[1];
	b[3][1] = f[2];
	b[4][1] = f[3];
	gaussj(a, 4, b, 1);
	for (n = 0; n < N; n++) {
		nu = (double)n / (double)N;
		Base[n] = b[1][1] + nu * (b[2][1] + nu * (b[3][1] + nu * b[4][1]));
	}
	GetMinMax(Base, N, &mn, &mx);
	fp = fopen("Base00.txt", "w");
	for (n = 0; n < N; n++) {
		Base[n] = mx / Base[n];
		fprintf(fp, "%g %c", Base[n], ',');
	}
	fclose(fp);

	for (n = 0; n < 6; n++) //ggl
	{
		delete[] a[n];
		delete[] b[n];
	}
	delete[] s;
	delete[] f;
	delete[] a;
	delete[] b;
}
void fft_lib::gaussj(double** a, int n, double** b, int m) {
	int indxc[6], indxr[6], ipiv[6];
	int i, icol, irow, j, k, l, ll;
	double big, dum, pivinv;

	for (j = 1; j <= n; j++) ipiv[j] = 0;
	for (i = 1; i <= n; i++) {
		big = 0.0;
		for (j = 1; j <= n; j++)
			if (ipiv[j] != 1)
				for (k = 1; k <= n; k++) {
					if (ipiv[k] == 0) {
						if (fabs(a[j][k]) >= big) {
							big = fabs(a[j][k]);
							irow = j;
							icol = k;
						}
					}
					else if (ipiv[k] > 1)
						exit(1);//nrerror("gaussj: Singular Matrix-1");
				}
		++(ipiv[icol]);
		if (irow != icol) {
			for (l = 1; l <= n; l++) SWAP(a[irow][l], a[icol][l]);
			for (l = 1; l <= m; l++) SWAP(b[irow][l], b[icol][l]);
		}
		indxr[i] = irow;
		indxc[i] = icol;
		if (a[icol][icol] == 0.0)
			exit(1);//nrerror("gaussj: Singular Matrix-2");
		pivinv = 1.0 / a[icol][icol];
		a[icol][icol] = 1.0;
		for (l = 1; l <= n; l++) a[icol][l] *= pivinv;
		for (l = 1; l <= m; l++) b[icol][l] *= pivinv;
		for (ll = 1; ll <= n; ll++)
			if (ll != icol) {
				dum = a[ll][icol];
				a[ll][icol] = 0.0;
				for (l = 1; l <= n; l++) a[ll][l] -= a[icol][l] * dum;

				for (l = 1; l <= m; l++) b[ll][l] -= b[icol][l] * dum;
			}
	}
	for (l = n; l >= 1; l--) {
		if (indxr[l] != indxc[l])
			for (k = 1; k <= n; k++)
				SWAP(a[k][indxr[l]], a[k][indxc[l]]);
	}
}

void fft_lib::hilbert(double data[], unsigned long n, double amp[], double phase[]) {
	unsigned long i;
	double* h;
	h = vector(0, n << 1);

	for (i = 0; i < n; i++) {
		h[2 * i] = data[i];
		h[2 * i + 1] = 0.0;
	}
	four1(h - 1, n, 1);
	for (i = 2; i < 2 * n; i++) {
		if (i < n)
			h[i] *= 2.0;
		else
			h[i] = 0.0;
	}
	four1(h - 1, n, -1);
	for (i = 0; i < n; i++) {
		amp[i] = sqrt(h[2 * i] * h[2 * i] + h[2 * i + 1] * h[2 * i + 1]);
		phase[i] = atan2(h[2 * i + 1], h[2 * i]);
	}
	phase_rectifier(phase, n);
	free_vector(h, 0, 2 * n);
}
void fft_lib::WaveToNum(int N, double* Lam, double* S, double* Wn, double* Sp) {
	double lam0, lam1, wn0, wn1, dwn, dlam;
	double* ww;
	int n, k;
	ww = new double[N];
	for (n = 0; n < N; n++)
		ww[n] = 2.0 * PI / Lam[n];
	lam0 = Lam[0];
	wn1 = 2.0 * PI / lam0;
	lam1 = Lam[N - 1];
	wn0 = 2.0 * PI / lam1;
	dlam = (lam1 - lam0) / (N - 1);
	dwn = 2.0 * PI * dlam / lam1 / lam1;
	for (n = 0; n < N; n++) {
		Wn[n] = wn0 + n * dwn;
		Sp[n] = 0.0;
	}
	for (n = 0; n < N; n++) {
		for (k = 1; k < N - 1; k++) {
			if ((Wn[n] - ww[k]) * (ww[k + 1] - Wn[n]) >= 0) {
				if (S[k] != S[k - 1])
					Sp[n] = S[k] + (2 * PI / Wn[n] - Lam[k]) / dlam * (S[k] - S[k - 1]);
				else
					Sp[n] = S[k];
			}
		}
	}

	delete[] ww; //ggl
}
void fft_lib::remove_dc(double d[], int N) {
	int n;
	double a = 0.0;
	a = average(d, N);
	for (n = 0; n < N; n++)
		d[n] -= a;
}
void fft_lib::memcof(double data[], int n, int m, double* xms, double d[]) {
	int k, j, i;
	double p = 0.0, * wk1, * wk2, * wkm;

	wk1 = new double[n + 1];
	wk2 = new double[n + 1];
	wkm = new double[m + 1];
	for (j = 1; j <= n; j++)
		p += SQR(data[j]);
	*xms = p / n;
	wk1[1] = data[1];
	wk2[n - 1] = data[n];
	for (j = 2; j <= n - 1; j++) {
		wk1[j] = data[j];
		wk2[j - 1] = data[j];
	}
	for (k = 1; k <= m; k++) {
		double num = 0.0, denom = 0.0;
		for (j = 1; j <= (n - k); j++) {
			num += wk1[j] * wk2[j];
			denom += SQR(wk1[j]) + SQR(wk2[j]);
		}
		d[k] = 2.0 * num / denom;
		*xms = *xms * (1.0 - SQR(d[k]));

		for (i = 1; i <= (k - 1); i++)
			d[i] = wkm[i] - d[k] * wkm[k - i];
		if (k == m) {
			delete[] wkm;//free_vector(wkm,1,m); ggl added []
			delete[] wk2;//free_vector(wk2,1,n);
			delete[] wk1;//free_vector(wk1,1,n);
			return;
		}
		for (i = 1; i <= k; i++) wkm[i] = d[i];
		for (j = 1; j <= (n - k - 1); j++) {
			wk1[j] -= wkm[k] * wk2[j];
			wk2[j] = wk2[j + 1] - wkm[k] * wk1[j + 1];
		}
	}
}

void fft_lib::memcof2(double data[], int n, int m, double* xms, double d[]) { //index 0 to n-1 ggl
	int k, j, i;
	double p = 0.0, * wk1, * wk2, * wkm;

	wk1 = new double[n];
	wk2 = new double[n];
	wkm = new double[m];
	for (j = 0; j < n; j++)
		p += SQR(data[j]);
	*xms = p / n;
	wk1[0] = data[0];
	wk2[n - 1] = data[n - 1];
	for (j = 1; j < n; j++) {
		wk1[j] = data[j];
		wk2[j - 1] = data[j];
	}
	for (k = 0; k < m; k++) {
		float num = 0.0, denom = 0.0;
		for (j = 0; j < (n - k); j++) {
			num += wk1[j] * wk2[j];
			denom += SQR(wk1[j]) + SQR(wk2[j]);
		}
		d[k] = 2.0 * num / denom;
		*xms = *xms * (1.0 - SQR(d[k]));

		for (i = 0; i < (k - 1); i++)
			d[i] = wkm[i] - d[k] * wkm[k - i];

		if (k == m) {
			delete[] wkm;//free_vector(wkm,1,m);
			delete[] wk2;//free_vector(wk2,1,n);
			delete[] wk1;//free_vector(wk1,1,n);
			return;
		}
		for (i = 0; i <= k; i++)
			wkm[i] = d[i];
		for (j = 0; j <= (n - k - 1); j++) {
			wk1[j] -= wkm[k] * wk2[j];
			wk2[j] = wk2[j + 1] - wkm[k] * wk1[j + 1];
		}
	}

	delete[] wk1;
	delete[] wk2;
	delete[] wkm;
	//	nrerror("never get here in memcof.");
}
double fft_lib::evlmem(double fdt, double d[], int m, double xms) {
	int i;
	double sumr = 1.0, sumi = 0.0;
	double wr = 1.0, wi = 0.0, wpr, wpi, wtemp, theta;

	theta = 6.28318530717959 * fdt;
	wpr = cos(theta);
	wpi = sin(theta);
	for (i = 1; i <= m; i++) {
		wr = (wtemp = wr) * wpr - wi * wpi;
		wi = wi * wpr + wtemp * wpi;
		sumr -= d[i] * wr;
		sumi -= d[i] * wi;
	}
	return xms / (sumr * sumr + sumi * sumi);
}

double fft_lib::evlmem2(double fdt, double d[], int m, double xms) { //index 0 to m-1 ggl
	int i;
	double sumr = 1.0, sumi = 0.0;
	double wr = 1.0, wi = 0.0, wpr, wpi, wtemp, theta;

	theta = 6.28318530717959 * fdt;
	wpr = cos(theta);
	wpi = sin(theta);
	for (i = 0; i < m; i++) {
		wr = (wtemp = wr) * wpr - wi * wpi;
		wi = wi * wpr + wtemp * wpi;
		sumr -= d[i] * wr;
		sumi -= d[i] * wi;
	}
	return xms / (sumr * sumr + sumi * sumi);
}
void fft_lib::GetMaxImax(double d[], int N, double* max, int* imax) {
	int n;
	*max = d[0];
	*imax = 0;
	for (n = 1; n < N; n++) {
		if (d[n] > *max) {
			*max = d[n];
			*imax = n;
		}
	}
}
void fft_lib::GetMaxImaxInRange(double d[], int N, int R1, int R2, double* max, int* imax) {
	int n;
	*max = d[R1];
	*imax = R1;
	for (n = R1 + 1; n < R2; n++) {
		if (d[n] > *max) {
			*max = d[n];
			*imax = n;
		}
	}
}
void fft_lib::SetInverseInterpolation(int N0, double* Lam, int N, double* WaveNum,
	double* WLam, int& NumLam0, int& NumLam1, double& Resolution512, double& Resolution64K) {
	int n;
	double lam0, lam1, wn0, wn1, dwn, dlam;
	for (n = 0; n < N0; n++)
		WLam[n] = 2.0 * PI / Lam[n];
	lam0 = Lam[0];
	wn1 = 2.0 * PI / lam0;
	lam1 = Lam[N0 - 1];
	wn0 = 2.0 * PI / lam1;
	dlam = (lam1 - lam0) / (N0 - 1);
	dwn = 2.0 * PI * dlam / lam1 / lam1;
	NumLam0 = (int)(wn0 / dwn);
	NumLam1 = (int)(wn1 / dwn);
	for (n = 0; n < N; n++) {
		WaveNum[n] = n * dwn;
	}
	Resolution512 = 0.001 * PI / WaveNum[N - 1];
	Resolution64K = 0.25 * Resolution512;
}
void fft_lib::realft(double data[], unsigned long n, int isign) {
	unsigned long i, i1, i2, i3, i4, np3;
	double c1 = 0.5, c2, h1r, h1i, h2r, h2i;
	double wr, wi, wpr, wpi, wtemp, theta;

	theta = PI / (double)(n >> 1);
	if (isign == 1) {
		c2 = -0.5;
		four1(data, n >> 1, 1);
	}
	else {
		c2 = 0.5;
		theta = -theta;
	}
	wtemp = sin(0.5 * theta);
	wpr = -2.0 * wtemp * wtemp;
	wpi = sin(theta);
	wr = 1.0 + wpr;
	wi = wpi;
	np3 = n + 3;
	for (i = 2; i <= (n >> 2); i++) {
		i4 = 1 + (i3 = np3 - (i2 = 1 + (i1 = i + i - 1)));
		h1r = c1 * (data[i1] + data[i3]);
		h1i = c1 * (data[i2] - data[i4]);
		h2r = -c2 * (data[i2] + data[i4]);
		h2i = c2 * (data[i1] - data[i3]);
		data[i1] = h1r + wr * h2r - wi * h2i;
		data[i2] = h1i + wr * h2i + wi * h2r;
		data[i3] = h1r - wr * h2r + wi * h2i;
		data[i4] = -h1i + wr * h2i + wi * h2r;
		wr = (wtemp = wr) * wpr - wi * wpi + wr;
		wi = wi * wpr + wtemp * wpi + wi;
	}
	if (isign == 1) {
		data[1] = (h1r = data[1]) + data[2];
		data[2] = h1r - data[2];
	}
	else {
		data[1] = c1 * ((h1r = data[1]) + data[2]);
		data[2] = c1 * (h1r - data[2]);
		four1(data, n >> 1, -1);
	}
}
void fft_lib::realft2(double data_0[], unsigned long n, int isign) {//This is the same as realft, except that the index is from 0 to n-1, added by ggl
	unsigned long i, i1, i2, i3, i4, np3;
	double c1 = 0.5, c2, h1r, h1i, h2r, h2i;
	double wr, wi, wpr, wpi, wtemp, theta;

	double* data = new double[n + 1];
	memcpy(data + 1, data_0, n * sizeof(double));

	theta = PI / (double)(n >> 1);
	if (isign == 1) {
		c2 = -0.5;
		four1(data, n >> 1, 1);
	}
	else {
		c2 = 0.5;
		theta = -theta;
	}
	wtemp = sin(0.5 * theta);
	wpr = -2.0 * wtemp * wtemp;
	wpi = sin(theta);
	wr = 1.0 + wpr;
	wi = wpi;
	np3 = n + 3;
	for (i = 2; i <= (n >> 2); i++) {
		i4 = 1 + (i3 = np3 - (i2 = 1 + (i1 = i + i - 1)));
		h1r = c1 * (data[i1] + data[i3]);
		h1i = c1 * (data[i2] - data[i4]);
		h2r = -c2 * (data[i2] + data[i4]);
		h2i = c2 * (data[i1] - data[i3]);
		data[i1] = h1r + wr * h2r - wi * h2i;
		data[i2] = h1i + wr * h2i + wi * h2r;
		data[i3] = h1r - wr * h2r + wi * h2i;
		data[i4] = -h1i + wr * h2i + wi * h2r;
		wr = (wtemp = wr) * wpr - wi * wpi + wr;
		wi = wi * wpr + wtemp * wpi + wi;
	}
	if (isign == 1) {
		data[1] = (h1r = data[1]) + data[2];
		data[2] = h1r - data[2];
	}
	else {
		data[1] = c1 * ((h1r = data[1]) + data[2]);
		data[2] = c1 * (h1r - data[2]);
		four1(data, n >> 1, -1);
	}

	memcpy(data_0, data + 1, n * sizeof(double));
	delete[] data;
}
void fft_lib::four1(double data[], unsigned long nn, int isign) {
	unsigned long n, mmax, m, j, istep, i;
	double wtemp, wr, wpr, wpi, wi, theta;
	double tempr, tempi;

	n = nn << 1;
	j = 1;
	for (i = 1; i < n; i += 2) {
		if (j > i) {
			SWAP(data[j], data[i]);
			SWAP(data[j + 1], data[i + 1]);
		}
		m = n >> 1;
		while (m >= 2 && j > m) {
			j -= m;
			m >>= 1;
		}
		j += m;
	}
	mmax = 2;
	while (n > mmax) {
		istep = mmax << 1;
		theta = isign * (2.0 * PI / mmax);
		wtemp = sin(0.5 * theta);
		wpr = -2.0 * wtemp * wtemp;
		wpi = sin(theta);
		wr = 1.0;
		wi = 0.0;
		for (m = 1; m < mmax; m += 2) {
			for (i = m; i <= n; i += istep) {
				j = i + mmax;
				tempr = wr * data[j] - wi * data[j + 1];
				tempi = wr * data[j + 1] + wi * data[j];
				data[j] = data[i] - tempr;
				data[j + 1] = data[i + 1] - tempi;
				data[i] += tempr;
				data[i + 1] += tempi;
			}
			wr = (wtemp = wr) * wpr - wi * wpi + wr;
			wi = wi * wpr + wtemp * wpi + wi;
		}
		mmax = istep;
	}
	for (i = 1; i <= n; i++)
		data[i] /= (double)sqrt((double)nn);
}
void fft_lib::real_part(double data[], double real[], unsigned long nn) {
	int n, m;
	for (n = 1; n < 2 * (long)nn; n += 2) {
		m = (n + 1) / 2;
		real[m] = data[n];
	}
}
void fft_lib::imag_part(double data[], double imag[], unsigned long nn) {
	int n, m;
	for (n = 2; n <= 2 * (long)nn; n += 2) {
		m = n / 2;
		imag[m] = data[n];
	}
}
void fft_lib::GetAllMax1(double d[], int N, int* M, int* imax, double* mmax) {
	int n = 0, m = 0, k = 0;
	double min, max;
	GetMinMax(d, N, &min, &max);
	for (n = 1; n < N - 1; n++) {
		if (FirstD(d[n - 1], d[n]) > 0 &&
			SecondD(d[n + 1], d[n], d[n - 1]) < 0.0 &&
			FirstD(d[n + 1], d[n]) > 0 &&
			d[n] > max / 8.0) {
			mmax[k] = d[n];
			imax[k] = n;
			k++;
		}
	}
	*M = k;
}
void fft_lib::phase_rectifier(double phase[], unsigned long N) {
	int n, m;
	for (n = 0; n < (long)N - 1; n++) {
		if (fabs(phase[n + 1] - phase[n]) > PI) {
			if (phase[n + 1] > phase[n]) {
				for (m = n + 1; m < (long)N; m++)
					phase[m] -= 2.0 * PI;
			}
			else {
				for (m = n + 1; m < (long)N; m++)
					phase[m] += 2.0 * PI;
			}
		}
	}
}
void fft_lib::GetMinMax(double d[], int N, double* min, double* max) {
	int n;
	*min = *max = d[0];
	for (n = 1; n < N; n++) {
		if (d[n] < *min)
			*min = d[n];
		if (d[n] > *max) {
			*max = d[n];
		}
	}
}

void fft_lib::InverseInterpolation(int N0, double* Lam, double* Spec0, int N, double* WaveNum, double* SpectWN,
	double& Delta512, double& Resolution16K, double& Resolution64K, int nX) {
	int n, k;
	double lam0, lam1, wn0, wn1;
	double* ww;
	//	static int nX = 1; //interpolated by nX
	int n16K = N;
	N = N0 * nX;
	if (N > n16K)
		AfxMessageBox(L"error in fft_lib::InverseInterpolation: nX larger than 16K");

	ww = new double[N0];
	for (n = 0; n < N0; n++)
		ww[n] = 2.0 * PI / Lam[n];

	wn1 = ww[0];
	wn0 = ww[N0 - 1];
	double WaveNum_rage = wn1 - wn0;
	double WaveNum_step = (ww[N0 - 2] - ww[N0 - 1]) / nX;
	double ww_midle = ww[N0 / 2];
	bool bFound;

	lam0 = Lam[0];
	lam1 = Lam[N0 - 1];
	double dlam = (lam1 - lam0) / (N0 - 1);
	double dwn = 2.0 * PI * dlam / lam1 / lam1;
	WaveNum_step = dwn / nX;
	for (n = 0; n < n16K; n++) {
		WaveNum[n] = wn0 + WaveNum_step * n;//ww_midle + (n-N0)*WaveNum_step; // n * dwn;
		SpectWN[n] = 0.0;
	}
	k = N0 - 2;
	for (n = 0; n < n16K; n++) {
		bFound = false;
		if (WaveNum[n] <= wn0 || WaveNum[n] >= wn1)
			continue;

		while (bFound == false && k >= 0) //improve speed
		{
			if ((WaveNum[n] <= ww[k]) && (WaveNum[n] >= ww[k + 1])) {
				bFound = true;
				SpectWN[n] = Spec0[k + 1] + (WaveNum[n] - ww[k + 1]) / (ww[k] - ww[k + 1]) * (Spec0[k] - Spec0[k + 1]); //ggl
				//				SpectWN[n]=Spec0[k]+(WaveNum[n]-ww[k+1])/(ww[k]-ww[k+1])*(Spec0[k]-Spec0[k+1]); //wrong, but higher peak
			}
			else
				--k;
		}

		if (bFound == false)
			AfxMessageBox(L"Error in fft_lib::InverseInterpolation");
	}
	delete[] ww; //ggl added  []
	Resolution16K = 0.001 * PI / (WaveNum[n16K - 1] - WaveNum[0]);
	Delta512 = Resolution16K * 32;
	Resolution64K = 0.25 * Resolution16K;
}
/*
void fft_lib::InverseInterpolation(int N0, double *Lam, double *Spec0,int N, double* WaveNum, double* SpectWN,
								   double &Delta512, double &Resolution16K, double &Resolution64K  )
{
	int n, k;
	double lam0, lam1, wn0, wn1;
	double *ww;

	ww = new double[N0];
	for(n=N0-1;n>=0;n--)
		ww[n]=2.0*PI/Lam[n];
	lam0 = Lam[0];
	wn1 = 2.0 * PI / lam0;
	lam1 = Lam[N0-1];
	wn0 = 2.0 * PI / lam1;
	double WaveNum_rage = wn1 - wn0;
	double WaveNum_step = WaveNum_rage/N;
	bool bFound;

	double dlam = (lam1 - lam0) / (N0 - 1);
	double dwn = 2.0 * PI * dlam / lam1 / lam1;
	WaveNum_step = dwn;
	for(n=0;n<N;n++)
	{
		WaveNum[n] = wn0 + n*WaveNum_step; // n * dwn; //
		SpectWN[n]=0.0;
	}
	for(n=0;n<N;n++)
	{
		if(WaveNum[n] < wn0 || WaveNum[n] > wn1)
			continue;

		bFound = false;
		for(k = 0; k < N0-1; k++)
		{
			if((WaveNum[n]<=ww[k]) && (WaveNum[n] >=ww[k+1]))
			{
				bFound = true;
					SpectWN[n]=Spec0[k]+(WaveNum[n]-ww[k+1])/(ww[k]-ww[k+1])*(Spec0[k]-Spec0[k+1]); //ggl, give higher peak

//					SpectWN[n]=Spec0[k+1]+(WaveNum[n]-ww[k+1])/(ww[k]-ww[k+1])*(Spec0[k]-Spec0[k+1]); //ggl, correct
			}
		}

		if(bFound == false)
			AfxMessageBox("Error in fft_lib::InverseInterpolation");
	}
	delete [] ww; //ggl added  []
	Resolution16K = 0.001 * PI / (WaveNum[N-1]-WaveNum[0]);
	Delta512=Resolution16K * 32;
	Resolution64K = 0.25 * Resolution16K;
}

void fft_lib::InverseInterpolation(int N0, double *Lam, double *Spec0,int N, double* WaveNum, double* SpectWN,
								   double &Delta512, double &Resolution16K, double &Resolution64K  )
{
	int n, k, n0, n1;
	double lam0, lam1, wn0, wn1, dwn, dlam;
	double *ww;

	ww = new double[N0];
	for(n=0;n<N0;n++)
		ww[n]=2.0*PI/Lam[n];
	lam0 = Lam[0];
	wn1 = 2.0 * PI / lam0;
	lam1 = Lam[N0-1];
	wn0 = 2.0 * PI / lam1;
	dlam = (lam1 - lam0) / (N0 - 1);
	dwn = 2.0 * PI * dlam / lam1 / lam1;
	n0 = (int)(wn0/dwn);
	n1 = (int)(wn1/dwn);
	for(n=0;n<N;n++)
	{
		WaveNum[n] = n* dwn;
		SpectWN[n]=0.0;
	}
	for(n=0;n<N;n++)
	{
		if(n <= n0 || n >= n1)
		{
			SpectWN[n]=0.0;
		}
		else
		{
			for(k=1;k<N0-1;k++)
			{
				if((WaveNum[n]-ww[k])*(ww[k+1]-WaveNum[n])>=0)
				{
					if(Spec0[k]!=Spec0[k-1])
	//					SpectWN[n]=Spec0[k]+(2*PI/WaveNum[n]-Lam[k])/dlam*(Spec0[k]-Spec0[k-1]);
						SpectWN[n]=Spec0[k]+(WaveNum[n]-ww[k])/(ww[k]-ww[k-1])*(Spec0[k]-Spec0[k-1]); //ggl
					else
						SpectWN[n]=Spec0[k];
				}
			}
		}
	}
	delete [] ww; //ggl added  []
	Resolution16K = 0.001 * PI / WaveNum[N-1];
	Delta512=Resolution16K * 32;
	Resolution64K = 0.25 * Resolution16K;
}
*/

#define NRANSI
#define GOLD 1.618034
#define GLIMIT 100.0
#define TINY 1.0e-20
#define SHFT(a,b,c,d) (a)=(b);(b)=(c);(c)=(d);

void fft_lib::mnbrak(float* ax, float* bx, float* cx, float* fa, float* fb, float* fc,
	float (*func)(float)) {
	float ulim, u, r, q, fu, dum;

	*fa = (*func)(*ax);
	*fb = (*func)(*bx);
	if (*fb > *fa) {
		SHFT(dum, *ax, *bx, dum)
			SHFT(dum, *fb, *fa, dum)
	}
	*cx = (*bx) + GOLD * (*bx - *ax);
	*fc = (*func)(*cx);
	while (*fb > *fc) {
		r = (*bx - *ax) * (*fb - *fc);
		q = (*bx - *cx) * (*fb - *fa);
		u = (*bx) - ((*bx - *cx) * q - (*bx - *ax) * r) /
			(2.0 * SIGN(FMAX(fabs(q - r), TINY), q - r));
		ulim = (*bx) + GLIMIT * (*cx - *bx);
		if ((*bx - u) * (u - *cx) > 0.0) {
			fu = (*func)(u);
			if (fu < *fc) {
				*ax = (*bx);
				*bx = u;
				*fa = (*fb);
				*fb = fu;
				return;
			}
			else if (fu > *fb) {
				*cx = u;
				*fc = fu;
				return;
			}
			u = (*cx) + GOLD * (*cx - *bx);
			fu = (*func)(u);
		}
		else if ((*cx - u) * (u - ulim) > 0.0) {
			fu = (*func)(u);
			if (fu < *fc) {
				SHFT(*bx, *cx, u, *cx + GOLD * (*cx - *bx))
					SHFT(*fb, *fc, fu, (*func)(u))
			}
		}
		else if ((u - ulim) * (ulim - *cx) >= 0.0) {
			u = ulim;
			fu = (*func)(u);
		}
		else {
			u = (*cx) + GOLD * (*cx - *bx);
			fu = (*func)(u);
		}
		SHFT(*ax, *bx, *cx, u)
			SHFT(*fa, *fb, *fc, fu)
	}
}
#undef GOLD
#undef GLIMIT
#undef TINY
#undef SHFT

#define ITMAX 100
#define CGOLD 0.3819660
#define ZEPS 1.0e-10
#define SHFT(a,b,c,d) (a)=(b);(b)=(c);(c)=(d);

float fft_lib::brent(float ax, float bx, float cx, float (*f)(float), float tol,
	float* xmin) {
	int iter;
	float a, b, d, etemp, fu, fv, fw, fx, p, q, r, tol1, tol2, u, v, w, x, xm;
	float e = 0.0;

	a = (ax < cx ? ax : cx);
	b = (ax > cx ? ax : cx);
	x = w = v = bx;
	fw = fv = fx = (*f)(x);
	for (iter = 1; iter <= ITMAX; iter++) {
		xm = 0.5 * (a + b);
		tol2 = 2.0 * (tol1 = tol * fabs(x) + ZEPS);
		if (fabs(x - xm) <= (tol2 - 0.5 * (b - a))) {
			*xmin = x;
			return fx;
		}
		if (fabs(e) > tol1) {
			r = (x - w) * (fx - fv);
			q = (x - v) * (fx - fw);
			p = (x - v) * q - (x - w) * r;
			q = 2.0 * (q - r);
			if (q > 0.0) p = -p;
			q = fabs(q);
			etemp = e;
			e = d;
			if (fabs(p) >= fabs(0.5 * q * etemp) || p <= q * (a - x) || p >= q * (b - x))
				d = CGOLD * (e = (x >= xm ? a - x : b - x));
			else {
				d = p / q;
				u = x + d;
				if (u - a < tol2 || b - u < tol2)
					d = SIGN(tol1, xm - x);
			}
		}
		else {
			d = CGOLD * (e = (x >= xm ? a - x : b - x));
		}
		u = (fabs(d) >= tol1 ? x + d : x + SIGN(tol1, d));
		fu = (*f)(u);
		if (fu <= fx) {
			if (u >= x) a = x; else b = x;
			SHFT(v, w, x, u)
				SHFT(fv, fw, fx, fu)
		}
		else {
			if (u < x) a = u; else b = u;
			if (fu <= fw || w == x) {
				v = w;
				w = u;
				fv = fw;
				fw = fu;
			}
			else if (fu <= fv || v == x || v == w) {
				v = u;
				fv = fu;
			}
		}
	}
	*xmin = x;
	return fx;
}
#undef ITMAX
#undef CGOLD
#undef ZEPS
#undef SHFT

fft_lib::~fft_lib(void) {}