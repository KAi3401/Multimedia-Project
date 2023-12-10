#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <memory.h>

#define FS 48000.0f
#define FH 3500.0f
#define FL 1500.0f
#define M 4096
#define PI 3.141592653589793f

typedef struct _wav
{
	int fs;
	char header[44];
	size_t length;
	short *LChannel;
	short *RChannel;
} wav;

int wav_read_fn(char *fn, wav *p_wav)
{
	// char header[44];
	short temp = 0;
	size_t i = 0;

	FILE *fp = fopen(fn, "rb");
	if (fp == NULL)
	{
		fprintf(stderr, "cannot read %s\n", fn);
		return 0;
	}
	fread(p_wav->header, sizeof(char), 44, fp);
	while (!feof(fp))
	{
		fread(&temp, sizeof(short), 1, fp);
		i++;
	}
	p_wav->length = i / 2;
	p_wav->LChannel = (short *)calloc(p_wav->length, sizeof(short));
	if (p_wav->LChannel == NULL)
	{
		fprintf(stderr, "cannot allocate memory for LChannel in wav_read_fn\n");
		fclose(fp);
		return 0;
	}
	p_wav->RChannel = (short *)calloc(p_wav->length, sizeof(short));
	if (p_wav->RChannel == NULL)
	{
		fprintf(stderr, "cannot allocate memory for RChannel in wav_read_fn\n");
		fclose(fp);
		return 0;
	}
	fseek(fp, 44, SEEK_SET);
	for (i = 0; i < p_wav->length; i++)
	{
		fread(p_wav->LChannel + i, sizeof(short), 1, fp);
		fread(p_wav->RChannel + i, sizeof(short), 1, fp);
	}
	fclose(fp);
	return 1;
}

int wav_save_fn(char *fn, wav *p_wav)
{
	FILE *fp = fopen(fn, "wb");
	size_t i;
	if (fp == NULL)
	{
		fprintf(stderr, "cannot save %s\n", fn);
		return 0;
	}
	fwrite(p_wav->header, sizeof(char), 44, fp);
	for (i = 0; i < p_wav->length; i++)
	{
		fwrite(p_wav->LChannel + i, sizeof(short), 1, fp);
		fwrite(p_wav->RChannel + i, sizeof(short), 1, fp);
	}
	fclose(fp);
	return 1;
}

int wav_init(size_t length, wav *p_wav)
{
	p_wav->length = length;
	p_wav->LChannel = (short *)calloc(p_wav->length, sizeof(short));
	if (p_wav->LChannel == NULL)
	{
		fprintf(stderr, "cannot allocate memory for LChannel in wav_read_fn\n");
		return 0;
	}
	p_wav->RChannel = (short *)calloc(p_wav->length, sizeof(short));
	if (p_wav->RChannel == NULL)
	{
		fprintf(stderr, "cannot allocate memory for RChannel in wav_read_fn\n");
		return 0;
	}
	return 1;
}

void wav_free(wav *p_wav)
{
	free(p_wav->LChannel);
	free(p_wav->RChannel);
}

/* hamming: for n=0,1,2,...N, length of N+1 */
float hamming(int N, int n)
{
	return 0.54 - 0.46 * cosf(2 * PI * ((float)(n)) / ((float)N));
}

/* low-pass filter coef: n=0,1,2...,2M */
float low_pass(int m, int n)
{
	float wc = 2 * PI * FL / FS;
	if (n == m)
	{ // L'Hopital's Rule
		return wc / PI;
	}
	else
	{
		return sinf(wc * ((float)(n - m))) / PI / ((float)(n - m)) * hamming(2 * m + 1, n);
	}
}
float high_pass(int m, int n)
{
	float wc = 2 * PI * FH / FS;
	if (n == m)
	{ // L'Hopital's Rule
		return 1 - wc / PI;
	}
	else
	{
		return -sinf(wc * ((float)(n - m))) / PI / ((float)(n - m)) * hamming(2 * m + 1, n);
	}
}
float band_pass(int m, int n)
{
	float wh = 2 * PI * FH / FS;
	float wl = 2 * PI * FL / FS;
	if (n == m)
	{ // L'Hopital's Rule
		return 2.0 * (wh / PI - wl / PI);
	}
	else
	{
		return 2.0 * (sinf(wh * ((float)(n - m))) - sinf(wl * ((float)(n - m)))) / PI / ((float)(n - m)) * hamming(2 * m + 1, n);
	}
}
double cos_values[1200] = {0};
double sin_values[1200] = {0};
void DFT(wav wavout, char *fn_YL, char *fn_YR)
{

	// 1200 samples = 0.025s
	// 20.06s = 1200 * (20.06/0.025) = 962880
	double sum1;
	double sum2;
	FILE *YL = fopen(fn_YL, "w");
	FILE *YR = fopen(fn_YR, "w");
	for (int k = 0; k < 600; k++)
	{
		double real1 = 0.0;
		double imag1 = 0.0;
		double real2 = 0.0;
		double imag2 = 0.0;

		for (int n = 0; n < 1200; n++)
		{
			int freq = (n * k) % 1200;
			// 使用查表，避免重複計算
			real1 += wavout.LChannel[962880 + n] * cos_values[freq] * hamming(1199,n);
			imag1 -= wavout.LChannel[962880 + n] * sin_values[freq] * hamming(1199,n);
			real2 += wavout.RChannel[962880 + n] * cos_values[freq] * hamming(1199,n);
			imag2 -= wavout.RChannel[962880 + n] * sin_values[freq] * hamming(1199,n);
		}

		sum1 = sqrt(real1 * real1 + imag1 * imag1);
		sum2 = sqrt(real2 * real2 + imag2 * imag2);
		if (sum1 < 1)
		{
			fprintf(YL, "0");
		}
		else
		{
			fprintf(YL, "%e", 20 * fabs(log10(sum1)));
		}
		if (sum2 < 1)
		{
			fprintf(YR, "0");
		}
		else
		{
			fprintf(YR, "%e", 20 * fabs(log10(sum2)));
		}
		if (k != 599)
		{
			fprintf(YL, "\n");
			fprintf(YR, "\n");
		}
	}
}
int main(int argc, char **argv)
{
	// simple_filter M hL.txt hR.txt YL.txt YR.txt input.wav output.wav
	wav wavin;
	wav wavout;
	int m = atoi(argv[1]);
	char *fn_hL = argv[2];
	char *fn_hR = argv[3];
	char *fn_YL = argv[4];
	char *fn_YR = argv[5];
	char *fn_in = argv[6];
	char *fn_out = argv[7];
	float h_L[2 * M + 1] = {0};
	float h_R[2 * M + 1] = {0};
	int n = 0;
	float y = 0;
	int k;

	// read wav
	if (wav_read_fn(fn_in, &wavin) == 0)
	{
		fprintf(stderr, "cannot read wav file %s\n", fn_in);
		exit(1);
	}

	for (n = 0; n < (2 * m + 1); n++)
	{
		h_L[n] = band_pass(m, n);
		h_R[n] = low_pass(m, n) + high_pass(m, n); //bandstop
	}
	FILE *hL = fopen(fn_hL, "w");
	FILE *hR = fopen(fn_hR, "w");

	for (n = 0; n < (2 * m + 1); n++)
	{
		fprintf(hL, "%e", h_L[n]);
		if(n!=2*m)	fprintf(hL,"\n");
		
		fprintf(hR, "%e", h_R[n]);
		if(n!=2*m)	fprintf(hR,"\n");

	}
	fclose(hL);
	fclose(hR);

	// filtering (convolution)
	if (wav_init(wavin.length, &wavout) == 0)
	{
		exit(1);
	}
	for (n = 0; n < wavin.length; n++)
	{
		y = 0;
		for (k = 0; k < (2 * m + 1); k++)
		{
			if ((n - k) >= 0)
				y = y + h_L[k] * ((float)(wavin.LChannel[n - k]));
		}
		wavout.LChannel[n] = (short)(roundf(y));

		y = 0;
		for (k = 0; k < (2 * m + 1); k++)
		{
			if ((n - k) >= 0)
				y = y + h_R[k] * ((float)(wavin.RChannel[n - k]));
		}
		wavout.RChannel[n] = (short)(roundf(y));
	}
	//calculate sine and cosine values
	for (int i = 0; i < 1200; i++)
	{
		cos_values[i] = cosf(2 * PI * i / 1199);
		sin_values[i] = sinf(2 * PI * i / 1199);
	}

	// compute DFT (YL、YR)
	// and save log spectrum .txt
	DFT(wavout, fn_YL, fn_YR);

	memcpy(wavout.header, wavin.header, 44);
	// save wav
	if (wav_save_fn(fn_out, &wavout) == 0)
	{
		fprintf(stderr, "cannot save %s\n", fn_out);
		exit(1);
	}
	wav_free(&wavin);
	wav_free(&wavout);
}