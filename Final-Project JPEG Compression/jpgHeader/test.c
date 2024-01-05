#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

void printHuffmanTable(FILE *file) {
    uint16_t length;
    fread(&length, 2, 1, file);
    length = (length >> 8) | (length << 8); // Convert to big-endian format

    printf("Huffman Table Length: %d\n", length - 2);

    // Allocate memory for the Huffman table
    uint8_t *buffer = (uint8_t *)malloc(length - 2);
    if (buffer == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return;
    }

    fread(buffer, 1, length - 2, file);

    // Print each byte of the Huffman table
    for (int i = 0; i < length - 2; ++i) {
        printf("%02X ", buffer[i]);
        if ((i + 1) % 16 == 0) printf("\n");
    }
    printf("\n");

    free(buffer); // Free the allocated memory
}

void printJpegHeader(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    uint16_t marker;
    while (fread(&marker, 2, 1, file)) {
        marker = (marker >> 8) | (marker << 8); // Convert to big-endian format

        if (marker == 0xFFC4) { // DHT marker
            printf("Found Huffman Table\n");
            printHuffmanTable(file);
        } else if (marker == 0xFFD9) { // EOI marker
            break;
        } else if (marker >= 0xFFC0 && marker <= 0xFFCF && marker != 0xFFC4 && marker != 0xFFC8) {
            // Skip other segments that are not DHT
            uint16_t length;
            fread(&length, 2, 1, file);
            length = (length >> 8) | (length << 8);
            fseek(file, length - 2, SEEK_CUR);
        }
    }

    fclose(file);
}

int main() {
    printJpegHeader("input.jpg");
    return 0;
}
