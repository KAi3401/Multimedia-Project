#include <stdio.h>
#include <string.h>
#include <stdint.h>

void writeWavHeader(FILE *file, int num_channels, int sample_rate, int bits_per_sample, size_t data_size) {
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

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s <scp_file> <output_file_8K> <output_file_16K>\n", argv[0]);
        return 1;
    }

    char *in = argv[1];
    char *out1 = argv[2];
    char *out2 = argv[3];

    FILE *scpFile = fopen(in, "r");
    FILE *outputFile1 = fopen(out1, "wb");
    FILE *outputFile2 = fopen(out2, "wb");

    if (outputFile1 == NULL || outputFile2 == NULL || scpFile == NULL) {
        printf("Error opening files\n");
        return 1;
    }

    int num_channels8 = 1;
    int sample_rate8 = 8000;
    int bits_per_sample8 = 16;
    size_t data_size8 = 4 * sample_rate8 * sizeof(int16_t);  // Adjust as needed
    writeWavHeader(outputFile1, num_channels8, sample_rate8, bits_per_sample8, data_size8);

    int num_channels16 = 1;
    int sample_rate16 = 16000;
    int bits_per_sample16 = 16;
    size_t data_size16 = 4 * sample_rate16 * sizeof(int16_t);  // Adjust as needed
    writeWavHeader(outputFile2, num_channels16, sample_rate16, bits_per_sample16, data_size16);

    for (int i = 0; i < 80; i++) {
        char inputFileName[20];
        fscanf(scpFile, "%s", inputFileName);
        FILE *inputFile = fopen(inputFileName, "rb");

        if (inputFile == NULL) {
            printf("Error opening input file %s\n", inputFileName);
            return 1;
        }

        fseek(inputFile, 44, SEEK_SET);  // Assuming the header size is 44 bytes
        int c;

        if (i < 40) {
            while ((c = fgetc(inputFile)) != EOF) {
                fputc(c, outputFile1);
            }
        } else {
            while ((c = fgetc(inputFile)) != EOF) {
                fputc(c, outputFile2);
            }
        }

        fclose(inputFile);
        remove(inputFileName);
    }

    fclose(scpFile);
    fclose(outputFile1);
    fclose(outputFile2);

    return 0;
}
