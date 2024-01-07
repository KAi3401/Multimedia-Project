#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
float PI = 3.14159265358979323846;

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

typedef struct QT_YCbCr
{
    int16_t Y;
    int16_t Cb;
    int16_t Cr;
} ImgQT_YCbCr;
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
ImgQT_YCbCr **QTmalloc_2D(int row, int col)
{
    ImgQT_YCbCr **Array, *Data;
    int i;
    Array = (ImgQT_YCbCr **)malloc(row * sizeof(ImgQT_YCbCr *));
    Data = (ImgQT_YCbCr *)malloc(row * col * sizeof(ImgQT_YCbCr));
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
void InputData0(FILE *fp_R, FILE *fp_G, FILE *fp_B, ImgRGB **array, int H, int W, int skip) // decoder 0;
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
    bmpheader->bitmap_datasize = width * height * 3; //  width * height * 3 or 0  沒什麼差
    bmpheader->hresolution = 2835;                   // 像素/米
    bmpheader->vresolution = 2835;                   // 像素/米
    bmpheader->usedcolors = 0;
    bmpheader->importantcolors = 0;
    bmpheader->palette = 13; // 隨便一個數字 反正bmp用不到
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
        }
        if (skip != 0)
        {
            fwrite(skip_buf, skip, 1, outfile);
        }
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
    InputData0(fp_R, fp_G, fp_B, Data_RGB, H, W, skip);

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
int YTable[64] = {
    16, 11, 10, 16, 24, 40, 51, 61,
    12, 12, 14, 19, 26, 58, 60, 55,
    14, 13, 16, 24, 40, 57, 69, 56,
    14, 17, 22, 29, 51, 87, 80, 62,
    18, 22, 37, 56, 68, 109, 103, 77,
    24, 35, 55, 64, 81, 104, 113, 92,
    49, 64, 78, 87, 103, 121, 120, 101,
    72, 92, 95, 98, 112, 100, 103, 99};
int CbCrTable[64] = {
    17, 18, 24, 47, 99, 99, 99, 99,
    18, 21, 26, 66, 99, 99, 99, 99,
    24, 26, 56, 99, 99, 99, 99, 99,
    47, 66, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99};
void readQuantizationTable(char *filename, int table[64])
{
    FILE *fp = fopen(filename, "r");
    if (!fp)
    {
        perror("Error opening file");
        exit(1);
    }

    for (int i = 0; i < 64; i++)
    {
        fscanf(fp, "%d", &table[i]);
    }

    fclose(fp);
}
void readQuantizedData(char *fn_qF_Y, char *fn_qF_Cb, char *fn_qF_Cr, ImgQT_YCbCr **QT_YCbCr, int newW, int newH)
{
    FILE *file_Y = fopen(fn_qF_Y, "rb");
    FILE *file_Cb = fopen(fn_qF_Cb, "rb");
    FILE *file_Cr = fopen(fn_qF_Cr, "rb");

    if (!file_Y || !file_Cb || !file_Cr)
    {
        perror("Error opening file");
        // Handle the error appropriately.
        exit(1);
    }

    for (int i = 0; i < newH; i++)
    {
        for (int j = 0; j < newW; j++)
        {
            fread(&QT_YCbCr[i][j].Y, sizeof(short), 1, file_Y);
            fread(&QT_YCbCr[i][j].Cb, sizeof(short), 1, file_Cb);
            fread(&QT_YCbCr[i][j].Cr, sizeof(short), 1, file_Cr);
        }
    }

    fclose(file_Y);
    fclose(file_Cb);
    fclose(file_Cr);
}
void readError(char *fn_eF_Y, char *fn_eF_Cb, char *fn_eF_Cr, ImgYCbCr **QT_err, int newW, int newH)
{
    FILE *file_Y = fopen(fn_eF_Y, "rb");
    FILE *file_Cb = fopen(fn_eF_Cb, "rb");
    FILE *file_Cr = fopen(fn_eF_Cr, "rb");

    if (!file_Y || !file_Cb || !file_Cr)
    {
        perror("Error opening file");
        // Handle the error appropriately.
        exit(1);
    }

    for (int i = 0; i < newH; i++)
    {
        for (int j = 0; j < newW; j++)
        {
            fread(&QT_err[i][j].Y, sizeof(float), 1, file_Y);
            fread(&QT_err[i][j].Cb, sizeof(float), 1, file_Cb);
            fread(&QT_err[i][j].Cr, sizeof(float), 1, file_Cr);
        }
    }

    fclose(file_Y);
    fclose(file_Cb);
    fclose(file_Cr);
}
void inverse_QT(ImgQT_YCbCr **QF_data, ImgYCbCr **ERR_data, ImgYCbCr **DCT_result, int read_YTable[64], int read_CbCrTable[64], int curr_row, int curr_col)
{
    int i, j;
    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 8; j++)
        {
            DCT_result[i + curr_row * 8][j + curr_col * 8].Y = QF_data[i + curr_row * 8][j + curr_col * 8].Y * read_YTable[i * 8 + j] + ERR_data[i + curr_row * 8][j + curr_col * 8].Y;
            DCT_result[i + curr_row * 8][j + curr_col * 8].Cb = QF_data[i + curr_row * 8][j + curr_col * 8].Cb * read_CbCrTable[i * 8 + j] + ERR_data[i + curr_row * 8][j + curr_col * 8].Cb;
            DCT_result[i + curr_row * 8][j + curr_col * 8].Cr = QF_data[i + curr_row * 8][j + curr_col * 8].Cr * read_CbCrTable[i * 8 + j] + ERR_data[i + curr_row * 8][j + curr_col * 8].Cr;
        }
    }
}
void inverse_DCT(ImgYCbCr **DCT_result, ImgYCbCr **reconstruct_YCbCr, int curr_row, int curr_col)
{
    int i, j, u, v;
    double sumY, sumCb, sumCr, cu, cv;
    double cos_table[8][8][8][8];
    // 先建表
    for (int u = 0; u < 8; u++)
    {
        for (int v = 0; v < 8; v++)
        {
            for (int i = 0; i < 8; i++)
            {
                for (int j = 0; j < 8; j++)
                {
                    cos_table[u][v][i][j] = cos((2 * i + 1) * u * PI / (2 * 8)) * cos((2 * j + 1) * v * PI / (2 * 8));
                }
            }
        }
    }

    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 8; j++)
        {
            sumY = sumCb = sumCr = 0.0;

            for (u = 0; u < 8; u++)
            {
                for (v = 0; v < 8; v++)
                {
                    cu = (u == 0) ? 1.0 / sqrt(2) : 1.0;
                    cv = (v == 0) ? 1.0 / sqrt(2) : 1.0;

                    sumY += cu * cv * DCT_result[u + curr_row * 8][v + curr_col * 8].Y * cos_table[u][v][i][j];

                    sumCb += cu * cv * DCT_result[u + curr_row * 8][v + curr_col * 8].Cb * cos_table[u][v][i][j];

                    sumCr += cu * cv * DCT_result[u + curr_row * 8][v + curr_col * 8].Cr * cos_table[u][v][i][j];
                }
            }

            reconstruct_YCbCr[i + curr_row * 8][j + curr_col * 8].Y = sumY * 0.25;
            reconstruct_YCbCr[i + curr_row * 8][j + curr_col * 8].Cb = sumCb * 0.25;
            reconstruct_YCbCr[i + curr_row * 8][j + curr_col * 8].Cr = sumCr * 0.25;
        }
    }
}

// read data -> inverse quantize -> inverse DCT -> YCbCr2RGB -> Output
void decoder1(char *fn_bmp, char *fn_Qt_Y, char *fn_Qt_Cb, char *fn_Qt_Cr, char *fn_dim, char *fn_qF_Y, char *fn_qF_Cb, char *fn_qF_Cr, char *fn_eF_Y, char *fn_eF_Cb, char *fn_eF_Cr)
{
    int read_YTable[64], read_CbCrTable[64];
    int i, j;
    // read QT table
    readQuantizationTable(fn_Qt_Y, read_YTable);
    readQuantizationTable(fn_Qt_Cb, read_CbCrTable);

    // read output bmp file name
    FILE *fp_out = fopen(fn_bmp, "wb");
    if (fp_out)
        printf("Open %s as output file!\n", fn_bmp);
    else
        printf("Fail to open %s as output file!\n", fn_bmp);

    // read dim
    FILE *fp_dim = fopen(fn_dim, "r");
    int W, H, NewW, NewH;
    if (fscanf(fp_dim, "%d %d %d %d", &W, &H, &NewW, &NewH) != 4)
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

    // read qF_Y 、 qF_Cb 、 qF_Cr raw
    ImgQT_YCbCr **qF_Data = QTmalloc_2D(NewH, NewW);
    readQuantizedData(fn_qF_Y, fn_qF_Cb, fn_qF_Cr, qF_Data, NewW, NewH);

    // read fn_eF_Y, fn_eF_Cb, fn_eF_Cr                /// QT_err 是表示正確讀入的err   而QT_err2代表沒有讀入err
    ImgYCbCr **QT_err = YCBCRmalloc_2D(NewH, NewW);
    readError(fn_eF_Y, fn_eF_Cb, fn_eF_Cr, QT_err, NewW, NewH);

    ImgYCbCr **QT_err2 = YCBCRmalloc_2D(NewH, NewW);
    for (int i = 0; i < NewH; i++)
    {
        for (int j = 0; j < NewW; j++)
        {
            QT_err2[i][j].Y = 0.0;
            QT_err2[i][j].Cb = 0.0;
            QT_err2[i][j].Cr = 0.0;
        }
    }

    // inverse Quantization
    ImgYCbCr **DCT_result = YCBCRmalloc_2D(NewH, NewW);
    ImgYCbCr **DCT_result2 = YCBCRmalloc_2D(NewH, NewW);
    for (int row = 0; row < block_H; row++)
    {
        for (int col = 0; col < block_W; col++)
        {
            inverse_QT(qF_Data, QT_err, DCT_result, read_YTable, read_CbCrTable, row, col);
            inverse_QT(qF_Data, QT_err2, DCT_result2, read_YTable, read_CbCrTable, row, col);
        }
    }

    // inverse 2D DCT
    ImgYCbCr **reconstruct_YCbCr = YCBCRmalloc_2D(NewH, NewW);
    ImgYCbCr **reconstruct_YCbCr2 = YCBCRmalloc_2D(NewH, NewW);
    for (int row = 0; row < block_H; row++)
    {
        for (int col = 0; col < block_W; col++)
        {
            inverse_DCT(DCT_result, reconstruct_YCbCr, row, col);
            inverse_DCT(DCT_result2, reconstruct_YCbCr2, row, col);
        }
    }

    // YCbCr to RGB
    ImgRGB **reconstruct_RGB = malloc_2D(H, W);
    ImgRGB **reconstruct_RGB2 = malloc_2D(H, W);
    YCbCrtoRGB(reconstruct_YCbCr, reconstruct_RGB, H, W);
    YCbCrtoRGB(reconstruct_YCbCr2, reconstruct_RGB2, H, W);
    

    // 計算total SQNR     透過 reconstruct_RGB 、 reconstruct_RGB2 比較
    double R_SQNR;
    double G_SQNR;
    double B_SQNR;
    int64_t R_sum_signal = 0, R_sum_noise = 0;
    int64_t G_sum_signal = 0, G_sum_noise = 0;
    int64_t B_sum_signal = 0, B_sum_noise = 0;

    for (int i = 0; i < H; i++)
    {
        for (int j = 0; j < W; j++)
        {
            // Calculate the square of the original values for each channel
            R_sum_signal += reconstruct_RGB2[i][j].R * reconstruct_RGB2[i][j].R;
            G_sum_signal += reconstruct_RGB2[i][j].G * reconstruct_RGB2[i][j].G;
            B_sum_signal += reconstruct_RGB2[i][j].B * reconstruct_RGB2[i][j].B;

            // Calculate the square of the error (difference) for each channel
            int64_t R_error = reconstruct_RGB[i][j].R - reconstruct_RGB2[i][j].R;
            int64_t G_error = reconstruct_RGB[i][j].G - reconstruct_RGB2[i][j].G;
            int64_t B_error = reconstruct_RGB[i][j].B - reconstruct_RGB2[i][j].B;

            R_sum_noise += R_error * R_error;
            G_sum_noise += G_error * G_error;
            B_sum_noise += B_error * B_error;
        }
    }

    // Calculate SQNR for each channel
    R_SQNR = R_sum_signal / R_sum_noise;
    R_SQNR = 10 * log10(R_SQNR);
    G_SQNR = G_sum_signal / G_sum_noise;
    G_SQNR = 10 * log10(G_SQNR);
    B_SQNR = B_sum_signal / B_sum_noise;
    B_SQNR = 10 * log10(B_SQNR);

    printf("\ntotal R_SQNR: %.3lf\n", R_SQNR);
    printf("total G_SQNR: %.3lf\n", G_SQNR);
    printf("total B_SQNR: %.3lf\n", B_SQNR);
    FILE *fp2 = fopen("QResKimberly.bmp","wb"); 
    output_bmpRGB(reconstruct_RGB, fp_out, bmpheader, skip);  //有加誤差
    output_bmpRGB(reconstruct_RGB2, fp2, bmpheader, skip);    //沒有加誤差的

    

    free(qF_Data[0]);
    free(qF_Data);

    free(QT_err[0]);
    free(QT_err);

    free(QT_err2[0]);
    free(QT_err2);

    free(DCT_result[0]);
    free(DCT_result);

    free(DCT_result2[0]);
    free(DCT_result2);

    free(reconstruct_YCbCr[0]);
    free(reconstruct_YCbCr);

    free(reconstruct_YCbCr2[0]);
    free(reconstruct_YCbCr2);

    fclose(fp_out);
    fclose(fp2);
}

typedef struct
{
    short zeroCount;
    short value; // 非零值
} RLEData;

int readRLEDataFromFile(FILE *fp, RLEData *rleData)
{

    int rleCount = 0;
    RLEData data;
    while (fread(&data, sizeof(RLEData), 1, fp))
    {
        rleData[rleCount++] = data;
        if (data.zeroCount == 0 && data.value == 0)
        {
            break; // 遇到结束標記 (0,0)
        }
    }

    return rleCount;
}
int readRLEDataFromFileA(FILE *fp, RLEData *rleData)
{
    int rleCount = 0;
    RLEData data;
    int row, col;
    char channel[10];
    // (跳過前三個資訊不拿)
    if (fscanf(fp, "%d %d %s ", &row, &col, channel) == 3)
    {
        while (fscanf(fp, "%hd %hd", &data.zeroCount, &data.value) == 2)
        {
            // 檢查是否為結束標記 (0, 0)
            rleData[rleCount++] = data;
            if (data.zeroCount == 0 && data.value == 0)
            {
                break; // 遇到结束標記，跳出循環
            }

            // 存儲讀取的數據
        }

        return rleCount; // 返回讀取的RLE項目數量
    }
}

void RLEDecode(RLEData *rleData, int rleCount, short *zigzagged)
{
    int index = 0;
    for (int i = 0; i < rleCount; i++)
    {
        int j;
        for (j = 0; j < rleData[i].zeroCount; j++)
        {
            zigzagged[index++] = 0;
        }
        if (rleData[i].value != 0 || j != 0)
        { // 检查是否為結束
            zigzagged[index++] = rleData[i].value;
        }
        else
        {
            break; // 结束
        }
    }

    // 填充剩餘的部分
    while (index < 64)
    {
        zigzagged[index++] = 0;
    }
}
void inverseZigZagOrder(short *zigzagged, ImgQT_YCbCr **QT_Data, int curr_row, int curr_col, const char *channel)
{
    int index = 0;
    for (int s = 0; s < 16; s++)
    {
        for (int i = 0; i <= s; i++)
        {
            int j = s - i;
            if (i < 8 && j < 8)
            {
                if (s % 2 == 0)
                {
                    // 偶數和，從上往下遍歷
                    if (strcmp(channel, "Y") == 0)
                    {
                        QT_Data[j + curr_row * 8][i + curr_col * 8].Y = zigzagged[index++];
                    }
                    else if (strcmp(channel, "Cb") == 0)
                    {
                        QT_Data[j + curr_row * 8][i + curr_col * 8].Cb = zigzagged[index++];
                    }
                    else if (strcmp(channel, "Cr") == 0)
                    {
                        QT_Data[j + curr_row * 8][i + curr_col * 8].Cr = zigzagged[index++];
                    }
                }
                else
                {
                    // 奇數和，從下往上遍歷
                    if (strcmp(channel, "Y") == 0)
                    {
                        QT_Data[i + curr_row * 8][j + curr_col * 8].Y = zigzagged[index++];
                    }
                    else if (strcmp(channel, "Cb") == 0)
                    {
                        QT_Data[i + curr_row * 8][j + curr_col * 8].Cb = zigzagged[index++];
                    }
                    else if (strcmp(channel, "Cr") == 0)
                    {
                        QT_Data[i + curr_row * 8][j + curr_col * 8].Cr = zigzagged[index++];
                    }
                }
            }
        }
    }
}

void decoder2(char *fn_bmp, char *type, char *fn_RLE_code)
{
    int i, j;
    // 量化表用內建的
    //  read output bmp file name
    FILE *fp_out = fopen(fn_bmp, "wb");
    if (fp_out)
        printf("Open %s as output file!\n", fn_bmp);
    else
        printf("Fail to open %s as output file!\n", fn_bmp);

    if (strcmp(type, "ascii") == 0)
    {
        int W, H, NewW, NewH, block_H, block_W;
        FILE *fp_in = fopen(fn_RLE_code, "r");
        fscanf(fp_in, "%d %d %d %d", &W, &H, &NewW, &NewH);
        block_H = H / 8;
        if (H % 8 != 0)
            block_H++;
        block_W = W / 8;
        if (W % 8 != 0)
            block_W++;
        ImgQT_YCbCr **qF_Data = QTmalloc_2D(NewH, NewW);
        for (int row = 0; row < block_H; row++) // 這裡放資料是 Y Cb Cr Y Cb Cr
        {
            for (int col = 0; col < block_W; col++)
            {

                RLEData *decodedRLEData_Y = (RLEData *)malloc(64 * sizeof(RLEData));
                short *decodedZigzag_Y = (short *)malloc(64 * sizeof(short));
                int decodedRLECount_Y = readRLEDataFromFileA(fp_in, decodedRLEData_Y); // Y
                RLEDecode(decodedRLEData_Y, decodedRLECount_Y, decodedZigzag_Y);
                inverseZigZagOrder(decodedZigzag_Y, qF_Data, row, col, "Y");
                free(decodedZigzag_Y);
                free(decodedRLEData_Y);

                RLEData *decodedRLEData_Cb = (RLEData *)malloc(64 * sizeof(RLEData));
                short *decodedZigzag_Cb = (short *)malloc(64 * sizeof(short));
                int decodedRLECount_Cb = readRLEDataFromFileA(fp_in, decodedRLEData_Cb); // Cb
                RLEDecode(decodedRLEData_Cb, decodedRLECount_Cb, decodedZigzag_Cb);
                inverseZigZagOrder(decodedZigzag_Cb, qF_Data, row, col, "Cb");
                free(decodedZigzag_Cb);
                free(decodedRLEData_Cb);

                RLEData *decodedRLEData_Cr = (RLEData *)malloc(64 * sizeof(RLEData));
                short *decodedZigzag_Cr = (short *)malloc(64 * sizeof(short));
                int decodedRLECount_Cr = readRLEDataFromFileA(fp_in, decodedRLEData_Cr); // Cr
                RLEDecode(decodedRLEData_Cr, decodedRLECount_Cr, decodedZigzag_Cr);
                inverseZigZagOrder(decodedZigzag_Cr, qF_Data, row, col, "Cr");
                free(decodedZigzag_Cr);
                free(decodedRLEData_Cr);
            }
        }

        // DPCM 加回DC值
        short previousDC_Y = 0, previousDC_Cb = 0, previousDC_Cr = 0;
        short currentDC_Y, currentDC_Cb, currentDC_Cr;

        for (int row = 0; row < block_H; row++)
        {
            for (int col = 0; col < block_W; col++)
            {
                // 处理 Y 分量的 DC 系数
                currentDC_Y = qF_Data[row * 8][col * 8].Y + previousDC_Y;
                qF_Data[row * 8][col * 8].Y = currentDC_Y;
                previousDC_Y = currentDC_Y;

                // 处理 Cb 分量的 DC 系数
                currentDC_Cb = qF_Data[row * 8][col * 8].Cb + previousDC_Cb;
                qF_Data[row * 8][col * 8].Cb = currentDC_Cb;
                previousDC_Cb = currentDC_Cb;

                // 处理 Cr 分量的 DC 系数
                currentDC_Cr = qF_Data[row * 8][col * 8].Cr + previousDC_Cr;
                qF_Data[row * 8][col * 8].Cr = currentDC_Cr;
                previousDC_Cr = currentDC_Cr;
            }
        }
        // write

        Bitmap bmpheader;
        write_header(&bmpheader, W, H);
        printHeader(bmpheader);

        // skip paddings at the end of each image line
        int skip = (4 - (bmpheader.width * 3) % 4);
        if (skip == 4)
            skip = 0;
        char skip_buf[3] = {0, 0, 0};

        printf("Image size: Width: %d x Height=%d pixels, total %d bytes\n", W, H, W * H * 3);
        printf("Image line skip=%d bytes\n", skip);
        printf("# of 8x8 blocks: W=%d x H=%d blocks, total %d blocks\n", block_W, block_H, block_W * block_H);

        ImgYCbCr **QT_err = YCBCRmalloc_2D(NewH, NewW);

        // decoder 2沒有讀入err，假設 = 0
        // decoder 2沒有讀入err，假設 = 0
        FILE *fp_qrrY = fopen("eF_Y.raw", "rb");
        FILE *fp_qrrCb = fopen("eF_Cb.raw", "rb");
        FILE *fp_qrrCr = fopen("eF_Cr.raw", "rb");
        if (fp_qrrY && fp_qrrCb && fp_qrrCr)
            readError("eF_Y.raw", "eF_Cb.raw", "eF_Cr.raw", QT_err, NewW, NewH);
        else
        {
            for (int i = 0; i < NewH; i++)
            {
                for (int j = 0; j < NewW; j++)
                {
                    QT_err[i][j].Y = 0.0;
                    QT_err[i][j].Cb = 0.0;
                    QT_err[i][j].Cr = 0.0;
                }
            }
        }
        // inverse Quantization
        ImgYCbCr **DCT_result = YCBCRmalloc_2D(NewH, NewW);
        for (int row = 0; row < block_H; row++)
        {
            for (int col = 0; col < block_W; col++)
            {
                inverse_QT(qF_Data, QT_err, DCT_result, YTable, CbCrTable, row, col);
            }
        }

        // inverse 2D DCT
        ImgYCbCr **reconstruct_YCbCr = YCBCRmalloc_2D(NewH, NewW);
        for (int row = 0; row < block_H; row++)
        {
            for (int col = 0; col < block_W; col++)
            {
                inverse_DCT(DCT_result, reconstruct_YCbCr, row, col);
            }
        }

        // YCbCr to RGB
        ImgRGB **reconstruct_RGB = malloc_2D(H, W);
        YCbCrtoRGB(reconstruct_YCbCr, reconstruct_RGB, H, W);
        output_bmpRGB(reconstruct_RGB, fp_out, bmpheader, skip);

        free(qF_Data[0]);
        free(qF_Data);

        free(QT_err[0]);
        free(QT_err);

        free(DCT_result[0]);
        free(DCT_result);

        free(reconstruct_YCbCr[0]);
        free(reconstruct_YCbCr);

        fclose(fp_out);
    }
    else if (strcmp(type, "binary") == 0)
    {
        int W, H, NewW, NewH, block_H, block_W;
        FILE *fp_in = fopen(fn_RLE_code, "rb");
        fread(&W, sizeof(int), 1, fp_in);
        fread(&H, sizeof(int), 1, fp_in);
        fread(&NewW, sizeof(int), 1, fp_in);
        fread(&NewH, sizeof(int), 1, fp_in);
        block_H = NewH / 8;
        block_W = NewW / 8;
        // 針對每個block 、 每個Channel 先把RLE解回程Zigzag  在從zigzag還原成block
        ImgQT_YCbCr **qF_Data = QTmalloc_2D(NewH, NewW);
        for (int row = 0; row < block_H; row++) // 這裡放資料是 Y Cb Cr Y Cb Cr ....
        {
            for (int col = 0; col < block_W; col++)
            {

                RLEData *decodedRLEData_Y = (RLEData *)malloc(64 * sizeof(RLEData));
                RLEData *decodedRLEData_Cb = (RLEData *)malloc(64 * sizeof(RLEData));
                RLEData *decodedRLEData_Cr = (RLEData *)malloc(64 * sizeof(RLEData));
                short *decodedZigzag_Y = (short *)malloc(64 * sizeof(short));
                short *decodedZigzag_Cb = (short *)malloc(64 * sizeof(short));
                short *decodedZigzag_Cr = (short *)malloc(64 * sizeof(short));

                int decodedRLECount_Y = readRLEDataFromFile(fp_in, decodedRLEData_Y);   // Y
                int decodedRLECount_Cb = readRLEDataFromFile(fp_in, decodedRLEData_Cb); // Cb
                int decodedRLECount_Cr = readRLEDataFromFile(fp_in, decodedRLEData_Cr); // Cr

                RLEDecode(decodedRLEData_Y, decodedRLECount_Y, decodedZigzag_Y);
                RLEDecode(decodedRLEData_Cb, decodedRLECount_Cb, decodedZigzag_Cb);
                RLEDecode(decodedRLEData_Cr, decodedRLECount_Cr, decodedZigzag_Cr);

                // zigzag还原成block
                inverseZigZagOrder(decodedZigzag_Y, qF_Data, row, col, "Y");
                inverseZigZagOrder(decodedZigzag_Cb, qF_Data, row, col, "Cb");
                inverseZigZagOrder(decodedZigzag_Cr, qF_Data, row, col, "Cr");

                // 释放分配的内存
                free(decodedRLEData_Y);
                free(decodedRLEData_Cb);
                free(decodedRLEData_Cr);
                free(decodedZigzag_Y);
                free(decodedZigzag_Cb);
                free(decodedZigzag_Cr);
            }
        }

        // for (int row = 0; row < block_H; row++) // 這裡放資料是 Y Y Y Y... Cb Cb Cb... Cr Cr Cr....
        // {
        //     for (int col = 0; col < block_W; col++)
        //     {

        //         RLEData *decodedRLEData_Y = (RLEData *)malloc(64 * sizeof(RLEData));
        //         short *decodedZigzag_Y = (short *)malloc(64 * sizeof(short));
        //         int decodedRLECount_Y = readRLEDataFromFile(fp_in, decodedRLEData_Y); // Y
        //         RLEDecode(decodedRLEData_Y, decodedRLECount_Y, decodedZigzag_Y);
        //         inverseZigZagOrder(decodedZigzag_Y, qF_Data, row, col, "Y");
        //         free(decodedZigzag_Y);
        //         free(decodedRLEData_Y);
        //     }
        // }

        // for (int row = 0; row < block_H; row++)
        // {
        //     for (int col = 0; col < block_W; col++)
        //     {

        //         RLEData *decodedRLEData_Cb = (RLEData *)malloc(64 * sizeof(RLEData));
        //         short *decodedZigzag_Cb = (short *)malloc(64 * sizeof(short));
        //         int decodedRLECount_Cb = readRLEDataFromFile(fp_in, decodedRLEData_Cb); // Y
        //         RLEDecode(decodedRLEData_Cb, decodedRLECount_Cb, decodedZigzag_Cb);
        //         inverseZigZagOrder(decodedZigzag_Cb, qF_Data, row, col, "Cb");
        //         free(decodedZigzag_Cb);
        //         free(decodedRLEData_Cb);
        //     }
        // }
        // for (int row = 0; row < block_H; row++)
        // {
        //     for (int col = 0; col < block_W; col++)
        //     {

        //         RLEData *decodedRLEData_Cr = (RLEData *)malloc(64 * sizeof(RLEData));
        //         short *decodedZigzag_Cr = (short *)malloc(64 * sizeof(short));
        //         int decodedRLECount_Cr = readRLEDataFromFile(fp_in, decodedRLEData_Cr); // Y
        //         RLEDecode(decodedRLEData_Cr, decodedRLECount_Cr, decodedZigzag_Cr);
        //         inverseZigZagOrder(decodedZigzag_Cr, qF_Data, row, col, "Cr");
        //         free(decodedZigzag_Cr);
        //         free(decodedRLEData_Cr);
        //     }
        // }
        // DPCM 加回DC值
        short previousDC_Y = 0, previousDC_Cb = 0, previousDC_Cr = 0;
        short currentDC_Y, currentDC_Cb, currentDC_Cr;

        for (int row = 0; row < block_H; row++)
        {
            for (int col = 0; col < block_W; col++)
            {
                // 处理 Y 分量的 DC 系数
                currentDC_Y = qF_Data[row * 8][col * 8].Y + previousDC_Y;
                qF_Data[row * 8][col * 8].Y = currentDC_Y;
                previousDC_Y = currentDC_Y;

                // 处理 Cb 分量的 DC 系数
                currentDC_Cb = qF_Data[row * 8][col * 8].Cb + previousDC_Cb;
                qF_Data[row * 8][col * 8].Cb = currentDC_Cb;
                previousDC_Cb = currentDC_Cb;

                // 处理 Cr 分量的 DC 系数
                currentDC_Cr = qF_Data[row * 8][col * 8].Cr + previousDC_Cr;
                qF_Data[row * 8][col * 8].Cr = currentDC_Cr;
                previousDC_Cr = currentDC_Cr;
            }
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

        printf("Image size: Width: %d x Height=%d pixels, total %d bytes\n", W, H, W * H * 3);
        printf("Image line skip=%d bytes\n", skip);
        printf("# of 8x8 blocks: W=%d x H=%d blocks, total %d blocks\n", block_W, block_H, block_W * block_H);

        // read fn_eF_Y, fn_eF_Cb, fn_eF_Cr
        ImgYCbCr **QT_err = YCBCRmalloc_2D(NewH, NewW);

        // decoder 2沒有讀入err，假設 = 0
        FILE *fp_qrrY = fopen("eF_Y.raw", "rb");
        FILE *fp_qrrCb = fopen("eF_Cb.raw", "rb");
        FILE *fp_qrrCr = fopen("eF_Cr.raw", "rb");
        if (fp_qrrY && fp_qrrCb && fp_qrrCr)
            readError("eF_Y.raw", "eF_Cb.raw", "eF_Cr.raw", QT_err, NewW, NewH);
        else
        {
            for (int i = 0; i < NewH; i++)
            {
                for (int j = 0; j < NewW; j++)
                {
                    QT_err[i][j].Y = 0.0;
                    QT_err[i][j].Cb = 0.0;
                    QT_err[i][j].Cr = 0.0;
                }
            }
        }
        // inverse Quantization
        ImgYCbCr **DCT_result = YCBCRmalloc_2D(NewH, NewW);
        for (int row = 0; row < block_H; row++)
        {
            for (int col = 0; col < block_W; col++)
            {
                inverse_QT(qF_Data, QT_err, DCT_result, YTable, CbCrTable, row, col);
            }
        }

        // inverse 2D DCT
        ImgYCbCr **reconstruct_YCbCr = YCBCRmalloc_2D(NewH, NewW);
        for (int row = 0; row < block_H; row++)
        {
            for (int col = 0; col < block_W; col++)
            {
                inverse_DCT(DCT_result, reconstruct_YCbCr, row, col);
            }
        }

        // YCbCr to RGB
        ImgRGB **reconstruct_RGB = malloc_2D(H, W);
        YCbCrtoRGB(reconstruct_YCbCr, reconstruct_RGB, H, W);
        output_bmpRGB(reconstruct_RGB, fp_out, bmpheader, skip);

        free(qF_Data[0]);
        free(qF_Data);

        free(QT_err[0]);
        free(QT_err);

        free(DCT_result[0]);
        free(DCT_result);

        free(reconstruct_YCbCr[0]);
        free(reconstruct_YCbCr);

        fclose(fp_out);
    }
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
    else if (strcmp(mode, "1") == 0)
    {
        // Kimberly.bmp Qt_Y.txt Qt_Cb.txt Qt_Cr.txt dim.txt qF_Y.raw qF_Cb.raw qF_Cr.raw eF_Y.raw eF_Cb.raw eF_Cr.raw
        char *fn_bmp = argv[2];

        char *fn_Qt_Y = argv[3];
        char *fn_Qt_Cb = argv[4];
        char *fn_Qt_Cr = argv[5];

        char *fn_dim = argv[6];

        char *fn_qF_Y = argv[7];
        char *fn_qF_Cb = argv[8];
        char *fn_qF_Cr = argv[9];

        char *fn_eF_Y = argv[10];
        char *fn_eF_Cb = argv[11];
        char *fn_eF_Cr = argv[12];
        decoder1(fn_bmp, fn_Qt_Y, fn_Qt_Cb, fn_Qt_Cr, fn_dim, fn_qF_Y, fn_qF_Cb, fn_qF_Cr, fn_eF_Y, fn_eF_Cb, fn_eF_Cr);
    }
    else if (strcmp(mode, "2") == 0)
    {
        char *fn_bmp = argv[2];
        char *type = argv[3]; // ascii or binary
        char *fn_RLE_code = argv[4];

        decoder2(fn_bmp, type, fn_RLE_code);
    }
    // else if(strcmp(mode,"3") == 0){

    // }
    else
    {
        printf("Wrong mode \n");
        exit(1); // 退出
    }

    return 0;
}
