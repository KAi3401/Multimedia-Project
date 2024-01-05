#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
float PI = 3.14159265359;

/* Construct a structure of BMP header */
typedef struct Bmpheader
{
    char identifier[2]; // 0x0000
    uint32_t filesize;  // 0x0002
    uint16_t reserved;  // 0x0006
    uint16_t reserved2;
    uint32_t bitmap_dataoffset; // 0x000A
    uint32_t bitmap_headersize; // 0x000E
    uint32_t width;             // 0x0012
    uint32_t height;            // 0x0016
    uint16_t planes;            // 0x001A
    uint16_t bits_perpixel;     // 0x001C
    uint32_t compression;       // 0x001E
    uint32_t bitmap_datasize;   // 0x0022
    uint32_t hresolution;       // 0x0026
    uint32_t vresolution;       // 0x002A
    uint32_t usedcolors;        // 0x002E
    uint32_t importantcolors;   // 0x0032
    uint32_t palette;           // 0x0036
} Bitmap;

/* Construct a structure of RGB */
typedef struct RGB
{
    uint8_t R;
    uint8_t G;
    uint8_t B;
} ImgRGB;

typedef struct YCbCr
{
    float Y;
    float Cb;
    float Cr;
} ImgYCbCr;

void printHeader(Bitmap header)
{
    printf("Identifier: %c%c\n", header.identifier[0], header.identifier[1]);
    printf("File Size: %u bytes\n", header.filesize);
    printf("Reserved: %u\n", header.reserved);
    printf("Reserved2: %u\n", header.reserved2);
    printf("Bitmap Data Offset: %u bytes\n", header.bitmap_dataoffset);
    printf("Bitmap Header Size: %u bytes\n", header.bitmap_headersize);
    printf("Width: %u pixels\n", header.width);
    printf("Height: %u pixels\n", header.height);
    printf("Planes: %u\n", header.planes);
    printf("Bits Per Pixel: %u\n", header.bits_perpixel);
    printf("Compression: %u\n", header.compression);
    printf("Bitmap Data Size: %u bytes\n", header.bitmap_datasize);
    printf("Horizontal Resolution: %u pixels/meter\n", header.hresolution);
    printf("Vertical Resolution: %u pixels/meter\n", header.vresolution);
    printf("Used Colors: %u\n", header.usedcolors);
    printf("Important Colors: %u\n", header.importantcolors);
    printf("Palette: %u\n", header.palette);
}
/* A function of making two dimensions memory locate array */
ImgRGB **malloc_2D(int row, int col)
{
    ImgRGB **Array, *Data;
    int i;
    Array = (ImgRGB **)malloc(row * sizeof(ImgRGB *));
    Data = (ImgRGB *)malloc(row * col * sizeof(ImgRGB));
    for (i = 0; i < row; i++, Data += col)
    {
        Array[i] = Data;
    }
    return Array;
}
ImgYCbCr **YCBCRmalloc_2D(int row, int col)
{
    ImgYCbCr **Array, *Data;
    int i;
    Array = (ImgYCbCr **)malloc(row * sizeof(ImgYCbCr *));
    Data = (ImgYCbCr *)malloc(row * col * sizeof(ImgYCbCr));
    for (i = 0; i < row; i++, Data += col)
    {
        Array[i] = Data;
    }
    return Array;
}

/* Input data without header into RGB structure */
void InputData0(FILE *fp_R,FILE *fp_G,FILE *fp_B, ImgRGB **array, int H, int W, int skip) //decoder 0;
{
    int temp;
    char skip_buf[1];
    int i, j;
    for (i = 0; i < H; i++)
    {
        for (j = 0; j < W; j++)
        {
            fscanf(fp_B, "%d", &temp);
            array[i][j].B = (uint8_t)temp;

            fscanf(fp_G, "%d", &temp);
            array[i][j].G = (uint8_t)temp;

            fscanf(fp_R, "%d", &temp);
            array[i][j].R = (uint8_t)temp;

        }
    }
    printf("\n"); // DEBUG
}

/* Output header and data */
void write_header(Bitmap *bmpheader, int width, int height)
{

    bmpheader->identifier[0] = 'B';
    bmpheader->identifier[1] = 'M';
    bmpheader->filesize = width * height * 3 + 54;
    bmpheader->reserved = 0;
    bmpheader->reserved2 = 0;
    bmpheader->bitmap_dataoffset = 54;
    bmpheader->bitmap_headersize = 40;
    bmpheader->width = width;
    bmpheader->height = height;
    bmpheader->planes = 1;
    bmpheader->bits_perpixel = 24;
    bmpheader->compression = 0;
    bmpheader->bitmap_datasize = 0; //  width * height * 3 or 0  沒什麼差
    bmpheader->hresolution = 3780;                   // 像素/米
    bmpheader->vresolution = 3780;                   // 像素/米
    bmpheader->usedcolors = 0;
    bmpheader->importantcolors = 0;
    bmpheader->palette = 11420596; // 隨便一個數字 反正bmp用不到
}

void output_bmpRGB(ImgRGB **RGB, FILE *outfile, Bitmap bmpheader, int skip)
{
    char skip_buf[3] = {0, 0, 0};
    int x, y;

    fwrite(&bmpheader.identifier, sizeof(char), 2, outfile);
    fwrite(&bmpheader.filesize, sizeof(uint32_t), 1, outfile);
    fwrite(&bmpheader.reserved, sizeof(uint16_t), 1, outfile);
    fwrite(&bmpheader.reserved2, sizeof(uint16_t), 1, outfile);
    fwrite(&bmpheader.bitmap_dataoffset, sizeof(uint32_t), 1, outfile);
    fwrite(&bmpheader.bitmap_headersize, sizeof(uint32_t), 1, outfile);
    fwrite(&bmpheader.width, sizeof(uint32_t), 1, outfile);
    fwrite(&bmpheader.height, sizeof(uint32_t), 1, outfile);
    fwrite(&bmpheader.planes, sizeof(uint16_t), 1, outfile);
    fwrite(&bmpheader.bits_perpixel, sizeof(uint16_t), 1, outfile);
    fwrite(&bmpheader.compression, sizeof(uint32_t), 1, outfile);
    fwrite(&bmpheader.bitmap_datasize, sizeof(uint32_t), 1, outfile);
    fwrite(&bmpheader.hresolution, sizeof(uint32_t), 1, outfile);
    fwrite(&bmpheader.vresolution, sizeof(uint32_t), 1, outfile);
    fwrite(&bmpheader.usedcolors, sizeof(uint32_t), 1, outfile);
    fwrite(&bmpheader.importantcolors, sizeof(uint32_t), 1, outfile);

    for (x = 0; x < bmpheader.height; x++)
    {
        for (y = 0; y < bmpheader.width; y++)
        {

            fwrite(&RGB[x][y].B, sizeof(uint8_t), 1, outfile);
            fwrite(&RGB[x][y].G, sizeof(uint8_t), 1, outfile);
            fwrite(&RGB[x][y].R, sizeof(uint8_t), 1, outfile);
            if (x < 9 && y < 8)
                printf("%02x%02x%02x ", RGB[x][y].R, RGB[x][y].G, RGB[x][y].B); // DEBUG
        }
        if (skip != 0)
        {
            fwrite(skip_buf, skip, 1, outfile);
        }
        if (x < 9)
            printf("\n"); // DEBUG
    }
}
void RGBtoYCbCr(ImgRGB **RGB, ImgYCbCr **YCbCr, int H, int W)
{
    int i, j;
    for (i = 0; i < H; i++)
    {
        for (j = 0; j < W; j++)
        {
            YCbCr[i][j].Y = (0.257 * RGB[i][j].R + 0.564 * RGB[i][j].G + 0.098 * RGB[i][j].B + 16);
            YCbCr[i][j].Cb = (-0.148 * RGB[i][j].R - 0.291 * RGB[i][j].G + 0.439 * RGB[i][j].B + 128);
            YCbCr[i][j].Cr = (0.439 * RGB[i][j].R - 0.368 * RGB[i][j].G - 0.071 * RGB[i][j].B + 128);
        }
    }
}
void YCbCrtoRGB(ImgYCbCr **YCbCr, ImgRGB **RGB, int H, int W)
{
    int i, j;
    for (i = 0; i < H; i++)
    {
        for (j = 0; j < W; j++)
        {
            // Red
            int r = (int)(1.164 * (YCbCr[i][j].Y - 16) + 1.596 * (YCbCr[i][j].Cr - 128));
            r = r > 255 ? 255 : (r < 0 ? 0 : r);
            RGB[i][j].R = (uint8_t)r;

            // Green
            int g = (int)(1.164 * (YCbCr[i][j].Y - 16) - 0.392 * (YCbCr[i][j].Cb - 128) - 0.813 * (YCbCr[i][j].Cr - 128));
            g = g > 255 ? 255 : (g < 0 ? 0 : g);
            RGB[i][j].G = (uint8_t)g;

            // Blue
            int b = (int)(1.164 * (YCbCr[i][j].Y - 16) + 2.017 * (YCbCr[i][j].Cb - 128));
            b = b > 255 ? 255 : (b < 0 ? 0 : b);
            RGB[i][j].B = (uint8_t)b;
        }
    }
}

void decoder0(char *fn_bmp, char *fn_R, char *fn_G, char *fn_B, char *fn_dim)
{

    int i, j;
    FILE *fp_out = fopen(fn_bmp, "wb");
    if (fp_out)
        printf("Open %s as output file!\n", fn_bmp);
    else
        printf("Fail to open %s as output file!\n", fn_bmp);

    // read dim
    FILE *fp_dim = fopen(fn_dim, "r");
    int W, H;
    if (fscanf(fp_dim, "%d %d", &W, &H) != 2)
    {
        // 處理讀取錯誤
        perror("Error reading dimensions");
        exit(1); 
    }
    // write header
    Bitmap bmpheader;
    write_header(&bmpheader, W, H);
    printHeader(bmpheader);

    // skip paddings at the end of each image line
    int skip = (4 - (bmpheader.width * 3) % 4);
    if (skip == 4)
        skip = 0;
    char skip_buf[3] = {0, 0, 0};

    // 8x8, if not multiples of 8, then bitmap padded, i.e. one more block
    int block_H = H / 8;
    if (H % 8 != 0)
        block_H++;
    int block_W = W / 8;
    if (W % 8 != 0)
        block_W++;

    printf("Image size: Width: %d x Height=%d pixels, total %d bytes\n", W, H, W * H * 3);
    printf("Image line skip=%d bytes\n", skip);
    printf("# of 8x8 blocks: W=%d x H=%d blocks, total %d blocks\n", block_W, block_H, block_W * block_H);
    printf("\nLoading BMP data: RRGGBB RRGGBB ... (BGR format in BMP file)\n");

//     //read R G B data
    FILE *fp_R = fopen(fn_R, "r");
    FILE *fp_G = fopen(fn_G, "r");
    FILE *fp_B = fopen(fn_B, "r");
    ImgRGB **Data_RGB = malloc_2D(H, W);
    InputData0(fp_R,fp_G,fp_B, Data_RGB, H, W, skip);
    
    output_bmpRGB(Data_RGB, fp_out, bmpheader, skip);

    printf("\nBMP header:\n");
    printHeader(bmpheader);
    printf("\n");

    printf("Done\n");

    free(Data_RGB[0]);
    free(Data_RGB);
    fclose(fp_out);
    fclose(fp_R);
    fclose(fp_G);
    fclose(fp_B);
    fclose(fp_dim);
    printf("Job done!\n");

}
int main(int argc, char **argv)
{
    int i, j;
    char *mode = argv[1];

    if (strcmp(mode, "0") == 0)
    {
        char *fn_bmp = argv[2];
        char *fn_R = argv[3];
        char *fn_G = argv[4];
        char *fn_B = argv[5];
        char *fn_dim = argv[6];
        decoder0(fn_bmp, fn_R, fn_G, fn_B, fn_dim);
    }
    // else if(strcmp(mode,"1") == 0){

    // }
    // else if(strcmp(mode,"2") == 0){

    // }
    // else if(strcmp(mode,"3") == 0){

    // }
    else
    {
        printf("Wrong mode \n");
        exit(1); // 退出
    }

    return 0;
}
