#include <stdio.h>
#include <stdlib.h>

void printBitsFromFile(const char *filename) {
    FILE *file = fopen(filename, "rb");
    unsigned char buffer;

    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    while (fread(&buffer, 1, 1, file)) {
        for (int i = 7; i >= 0; --i) {
            printf("%d", (buffer >> i) & 1);
        }
        printf(" ");
    }

    printf("\n");
    fclose(file);
}

int main() {
    const char *filename = "output.bin"; // 讀取的檔案名稱
    printBitsFromFile(filename);

    return 0;
}
