#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#define PI 3.1415926535897932384626433832795028841971693993

// declare function
void generateSineWave(double f, double A, size_t N, double p, double fs, char *fn1);
void generateSawtoothWave(double f, double A, size_t N, double p, double fs, char *fn1);
void generateSquareWave(double f, double A, size_t N, double p, double fs, char *fn1);
void generateTriangleWave(double f, double A, size_t N, double p, double fs, char *fn1);
void writeWavHeader(FILE *file, size_t data_size, int16_t num_channels, int32_t sample_rate, int16_t bits_per_sample);

int main(int argc, char *argv[])
{
    // check whether the command line arguments are correct
    if (argc != 8)
    {
        printf("Usage: %s SampleRate BitsPerSample wavetype Frequency Amplitude Duration file.wav \n", argv[0]);
        return 1;
    }

    double fs = atof(argv[1]); // Sampling frequency 
    char *m = argv[2];         // Sample size 16
    char *wavetype = argv[3];  // Wavetype sine/sawtooth/square/triangle
    double f = atof(argv[4]);  // Frequency
    double A = atof(argv[5]);
    double T = atof(argv[6]);  // Length (seconds)
    char *fn1 = argv[7];       // Filename for output wav

    double p = 1 / fs;

    size_t N = (size_t)(T * fs);

    // for debugging
    // for (int i = 0; i < 10; i++)
    // {
    //     A[i] = 32767 / 2;
    // }

    if (strcmp(wavetype, "sine") == 0)
    {
        generateSineWave(f, A, N, p, fs, fn1);
    }
    else if (strcmp(wavetype, "sawtooth") == 0)
    {
        generateSawtoothWave(f, A, N, p, fs, fn1);
    }
    else if (strcmp(wavetype, "square") == 0)
    {
        generateSquareWave(f, A, N, p, fs, fn1);
    }
    else if (strcmp(wavetype, "triangle") == 0)
    {
        generateTriangleWave(f, A, N, p, fs, fn1);
    }
    else
    {
        fprintf(stderr, "Unsupported wavetype: %s\n", wavetype);
        return 1;
    }

    return 0;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ write header ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void writeWavHeader(FILE *file, size_t data_size, int16_t num_channels, int32_t sample_rate, int16_t bits_per_sample)
{
    char header[44];

    // RIFF header
    strncpy(header, "RIFF", 4);
    *(int32_t *)(header + 4) = data_size + 36; // File size - 8
    strncpy(header + 8, "WAVE", 4);

    // Format chunk
    strncpy(header + 12, "fmt ", 4);
    *(int32_t *)(header + 16) = 16; // Sub-chunk size
    *(int16_t *)(header + 20) = 1;  // Audio format (1 for PCM)
    *(int16_t *)(header + 22) = num_channels;
    *(int32_t *)(header + 24) = sample_rate;
    *(int32_t *)(header + 28) = sample_rate * num_channels * bits_per_sample / 8; // Byte rate
    *(int16_t *)(header + 32) = num_channels * bits_per_sample / 8;               // Block align
    *(int16_t *)(header + 34) = bits_per_sample;

    // Data chunk
    strncpy(header + 36, "data", 4);
    *(int32_t *)(header + 40) = data_size;

    fwrite(header, 1, 44, file);
}

void generateSineWave(double f, double A, size_t N, double p, double fs, char *fn1)
{
    double tmp = 0.0;
    FILE *fp1;
    fp1 = fopen(fn1, "wb");

    int16_t *x = (int16_t *)malloc(N * sizeof(int16_t));
    for (size_t n = 0; n < N; n++)
    {
        tmp = A * sin(2 * PI * f * n * p);
        x[n] = (int16_t)floor(tmp + 0.5);
    }

    if (!fp1)
    {
        fprintf(stderr, "Cannot save %s\n", fn1);
    }

    int16_t bits_per_sample = 16;
    writeWavHeader(fp1, N * sizeof(int16_t), 1, (int32_t)fs, bits_per_sample);
    fwrite(x, sizeof(int16_t), N, fp1);
    fclose(fp1);
    free(x);
}
// sawtooth wave function
void generateSawtoothWave(double f, double A, size_t N, double p, double fs, char *fn1)
{
    double tmp;
    FILE *fp1;
    fp1 = fopen(fn1, "wb");
    int16_t *x = (int16_t *)malloc(N * sizeof(int16_t));
    for (size_t n = 0; n < N; n++)
    {
        tmp = 2 * ((f * n * p - floor(f * n * p)) * A - A / 2);
        x[n] = (int16_t)floor(tmp + 0.5);
    }
    int16_t bits_per_sample = 16;
    writeWavHeader(fp1, N * sizeof(int16_t), 1, (int32_t)fs, bits_per_sample);
    fwrite(x, sizeof(int16_t), N, fp1);
    fclose(fp1);
}

// square wave function
void generateSquareWave(double f, double A, size_t N, double p, double fs, char *fn1)
{
    FILE *fp1;
    fp1 = fopen(fn1, "wb");
    double half_period = fs / (2 * f); // Half period in samples
    double tmp;

    // ( 16 bits ) square wave
    int16_t *x = (int16_t *)malloc(N * sizeof(int16_t));
    for (size_t n = 0; n < N; n++)
    {
        tmp = A * sin(2 * PI * f * n * p);

        if (tmp > 0)
        {
            tmp = A;
        }
        else if (tmp < 0)
        {
            tmp = -A;
        }
        else
        {
            tmp = 0;
        }

        x[n] = (int16_t)floor(tmp + 0.5);
    }

    if (!fp1)
    {
        fprintf(stderr, "Cannot save %s\n", fn1);
    }

    int16_t bits_per_sample = 16;
    writeWavHeader(fp1, N * sizeof(int16_t), 1, (int32_t)fs, bits_per_sample);
    fwrite(x, sizeof(int16_t), N, fp1);
    free(x);

    fclose(fp1);
}

// triangle wave function
void generateTriangleWave(double f, double A, size_t N, double p, double fs, char *fn1)
{
    double tmp;
    FILE *fp1;
    fp1 = fopen(fn1, "wb");

    int16_t *x = (int16_t *)malloc(N * sizeof(int16_t));
    double P = fs / f;
    for (size_t n = 0; n < N; n++)
    {
        tmp = 2 * (fabs(2 * (f * n * p - floor(f * n * p + 0.5))) - 1) * A + A;
        x[n] = (int16_t)floor(tmp + 0.5);
    }
    int16_t bits_per_sample = 16;
    writeWavHeader(fp1, N * sizeof(int16_t), 1, (int32_t)fs, bits_per_sample);
    fwrite(x, sizeof(int16_t), N, fp1);
    fclose(fp1);
}
