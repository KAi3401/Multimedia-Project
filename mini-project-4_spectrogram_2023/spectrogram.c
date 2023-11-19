#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#define PI 3.1415926535897932384626433832795028841971693993

typedef struct
{
    char chunkId[4];
    uint32_t chunkSize;
    char format[4];
    char subchunk1Id[4];
    uint32_t subchunk1Size;
    uint16_t audioFormat;
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
    char subchunk2Id[4];
    uint32_t subchunk2Size;
} WavHeader;

// Apply Hamming window to the audio data
void HammingWindow(int16_t *data, int loc, int window_Size, int16_t *DFT_data)
{
    for (int i = 0; i < window_Size; i++)
    {
        double window = 0.54 - 0.46 * cos(2 * PI * i / (window_Size - 1));
        DFT_data[i] = (int16_t)(data[loc + i] * window);
    }
}

void RectangularWindow(int16_t *data, int loc, int window_Size, int16_t *DFT_data)
{
    for (int i = 0; i < window_Size; i++)
    {
        DFT_data[i] = (int16_t)(data[loc + i]);
    }
}

// Global variable for spectrogram data
double specData[512] = {0};
double cos_values[512] = {0};
double sin_values[512] = {0};

// Compute Discrete Fourier Transform (DFT) for audio data
void computeDFT(int16_t *DFT_data, int DFT_window)
{
    for (int k = 0; k < DFT_window / 2; k++)
    {
        double real = 0.0;
        double imag = 0.0;

        for (int n = 0; n < DFT_window; n++)
        {
            int freq = (n * k) % DFT_window;
            // 使用查表，避免重複計算
            real += DFT_data[n] * cos_values[freq];
            imag -= DFT_data[n] * sin_values[freq];
        }

        double sum = sqrt(real * real + imag * imag);

        if (sum < 1)
        {
            specData[k] = 0;
        }
        else
        {
            specData[k] = 20 * log10(sum);
        }
    }
}
// //for debug
// void printWavHeader(WavHeader *header) {
//     printf("chunkId: %.4s\n", header->chunkId);
//     printf("chunkSize: %u\n", header->chunkSize);
//     printf("format: %.4s\n", header->format);
//     printf("subchunk1Id: %.4s\n", header->subchunk1Id);
//     printf("subchunk1Size: %u\n", header->subchunk1Size);
//     printf("audioFormat: %u\n", header->audioFormat);
//     printf("numChannels: %u\n", header->numChannels);
//     printf("sampleRate: %u\n", header->sampleRate);
//     printf("byteRate: %u\n", header->byteRate);
//     printf("blockAlign: %u\n", header->blockAlign);
//     printf("bitsPerSample: %u\n", header->bitsPerSample);
//     printf("subchunk2Id: %.4s\n", header->subchunk2Id);
//     printf("subchunk2Size: %u\n", header->subchunk2Size);
//     printf("\n");
// }
// Perform Discrete Fourier Transform (DFT) on audio file
void DFT(const char *inputFile, const char *outputFile, int w_Size, const char *w_Type, int DFT_Size, int f_itv)
{
    FILE *file = fopen(inputFile, "rb");
    if (file == NULL)
    {
        perror("Error opening file");
        return;
    }

    WavHeader header;

    // Read header information
    fread(&header, sizeof(WavHeader), 1, file);
    //printWavHeader(&header);
    // Check if audio format is 16 bits
    if (header.bitsPerSample != 16)
    {
        fprintf(stderr, "Unsupported bit depth. Only 16 bits per sample is supported.\n");
        fclose(file);
        return;
    }

    int fs = header.sampleRate;
    int frameInterval = fs / 1000 * f_itv;
    int windowSize = fs / 1000 * w_Size;
    int DFT_window = fs / 1000 * DFT_Size;

    // Calculate the number of samples (dataSize)
    int numSamples = header.subchunk2Size / (header.bitsPerSample / 8);

    // Allocate an array to store the audio data
    int16_t *data = (int16_t *)malloc((numSamples + DFT_window) * sizeof(int16_t));
    // Read audio data
    fread(data, sizeof(int16_t), numSamples, file);
    fclose(file);

    int cur = 0;
    int frameIndex = 0;
    FILE *specFile = fopen(outputFile, "w");
    if (specFile == NULL)
    {
        perror("Error creating spec.txt file");
        free(data);
        return;
    }

    int range = DFT_window / 2;

    // 建立cos_value和sin_value array
    double angle;
    for (int i = 0; i < DFT_window; i++)
    {
        angle = 2 * PI * i / DFT_window;
        cos_values[i] = cos(angle);
        sin_values[i] = sin(angle);
    }

    // 分割 window 執行DFT
    while (cur < numSamples)
    {
        int16_t DFT_data[512] = {0};

        if (strcmp(w_Type, "Hamming") == 0)
            HammingWindow(data, cur, windowSize, DFT_data);
        else if (strcmp(w_Type, "Rectangular") == 0)
        {
            RectangularWindow(data, cur, windowSize, DFT_data);
        }
        else
        {
            printf("Invalid window type");
        }
        // Perform DFT
        computeDFT(DFT_data, DFT_window);

        for (int j = 0; j < range; j++)
        {
            fprintf(specFile, "%.2f", specData[j]);
            if (j != range - 1)
                fprintf(specFile, ",");
        }
        fprintf(specFile, "\n");

        cur += frameInterval;
        frameIndex++;
    }

    // Output the specData array or perform further processing
    fclose(specFile);

    // Free allocated memory
    free(data);
}

int main(int argc, char *argv[])
{
    if (argc != 7)
    {
        fprintf(stderr, "Usage: %s w_size w_type dft_size f_itv wav_in spec_out\n", argv[0]);
        return 1;
    }

    int w_size = atoi(argv[1]);
    const char *w_type = argv[2];
    int dft_size = atoi(argv[3]);
    int f_itv = atoi(argv[4]);
    const char *wav_in = argv[5];
    const char *spec_out = argv[6];

    DFT(wav_in, spec_out, w_size, w_type, dft_size, f_itv);
    printf("calculation. Done\n\n");

    return 0;
}
