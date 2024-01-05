#include <stdio.h>
#include <string.h>

void writeBitsToFile(const char *bitString, FILE *file) {
    unsigned char buffer = 0;
    int bitLength = strlen(bitString);

    for (int i = 0; i < bitLength; ++i) {
        buffer = buffer | (bitString[i] - '0') << (7 - (i % 8));

        if (i % 8 == 7) {
            fwrite(&buffer, 1, 1, file);
            buffer = 0;
        }
    }

    // 處理剩餘的比特（如果有）
    if (bitLength % 8 != 0) {
        fwrite(&buffer, 1, 1, file);
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <fileName>\n", argv[0]);
        return 1;
    }
    char bitString[1024];
    char *in = argv[1];
    FILE *fn_in = fopen(in, "r");
    FILE *file = fopen("output.bin", "wb"); // 打開文件用於寫入
    //read string and store in bitString
    if (fgets(bitString, sizeof(bitString), fn_in) == NULL) {
        perror("Error reading from input file");
        fclose(fn_in);
        return 1;
    }
    fclose(fn_in);
    fclose(fn_in);
    if (file) {
        writeBitsToFile(bitString, file);
        fclose(file);
    }

    return 0;
}
