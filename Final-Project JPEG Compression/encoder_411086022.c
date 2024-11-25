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

typedef struct YCbCr // 只能用float
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
/* Read header */
void readheader(FILE *fp, Bitmap *x)
{
    fread(&x->identifier, sizeof(x->identifier), 1, fp);
    fread(&x->filesize, sizeof(x->filesize), 1, fp);
    fread(&x->reserved, sizeof(x->reserved), 1, fp);
    fread(&x->reserved2, sizeof(x->reserved2), 1, fp);
    fread(&x->bitmap_dataoffset, sizeof(x->bitmap_dataoffset), 1, fp);
    fread(&x->bitmap_headersize, sizeof(x->bitmap_headersize), 1, fp);
    fread(&x->width, sizeof(x->width), 1, fp);
    fread(&x->height, sizeof(x->height), 1, fp);
    fread(&x->planes, sizeof(x->planes), 1, fp);
    fread(&x->bits_perpixel, sizeof(x->bits_perpixel), 1, fp);
    fread(&x->compression, sizeof(x->compression), 1, fp);
    fread(&x->bitmap_datasize, sizeof(x->bitmap_datasize), 1, fp);
    fread(&x->hresolution, sizeof(x->hresolution), 1, fp);
    fread(&x->vresolution, sizeof(x->vresolution), 1, fp);
    fread(&x->usedcolors, sizeof(x->usedcolors), 1, fp);
    fread(&x->importantcolors, sizeof(x->importantcolors), 1, fp);
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

/* Input data without header into RGB structure */
void InputData(FILE *fp, ImgRGB **array, int H, int W, int skip)
{
    int temp;
    char skip_buf[3];
    int i, j;
    for (i = 0; i < H; i++)
    {
        for (j = 0; j < W; j++)
        {
            temp = fgetc(fp);
            array[i][j].B = (uint8_t)temp;

            temp = fgetc(fp);
            array[i][j].G = (uint8_t)temp;

            temp = fgetc(fp);
            array[i][j].R = (uint8_t)temp;
        }
        if (skip != 0)
            fread(skip_buf, skip, 1, fp);
    }
    printf("\n"); // DEBUG
}

/* Output header and data */
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
            YCbCr[i][j].Y = (0.299 * RGB[i][j].R + 0.587 * RGB[i][j].G + 0.114 * RGB[i][j].B);
            YCbCr[i][j].Cb = (-0.168736 * RGB[i][j].R - 0.33124 * RGB[i][j].G + 0.5 * RGB[i][j].B + 128);
            YCbCr[i][j].Cr = (0.5 * RGB[i][j].R - 0.418688 * RGB[i][j].G - 0.081312 * RGB[i][j].B + 128);
        }
    }
}

void encoder0(char *fn_bmp, char *fn_R, char *fn_G, char *fn_B, char *fn_dim)
{

    int i, j;

    FILE *fp_in = fopen(fn_bmp, "rb");
    if (fp_in)
        printf("Open %s as input file!\n", fn_bmp);
    else
        printf("Fail to open %s as input file!\n", fn_bmp);
    // read header

    Bitmap bmpheader;
    readheader(fp_in, &bmpheader);

    printf("\nBMP header:\n");
    printHeader(bmpheader);
    printf("\n");

    printf("Done\n");

    int H = bmpheader.height;
    int W = bmpheader.width;

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
    ImgRGB **Data_RGB = malloc_2D(H, W);
    InputData(fp_in, Data_RGB, H, W, skip);

    // write RGB data
    FILE *fp_R = fopen(fn_R, "w");
    FILE *fp_G = fopen(fn_G, "w");
    FILE *fp_B = fopen(fn_B, "w");
    for (i = 0; i < H; ++i)
    {
        for (j = 0; j < W; ++j)
        {
            fprintf(fp_R, "%d ", Data_RGB[i][j].R);
            fprintf(fp_G, "%d ", Data_RGB[i][j].G);
            fprintf(fp_B, "%d ", Data_RGB[i][j].B);
        }
        fprintf(fp_R, "\n");
        fprintf(fp_G, "\n");
        fprintf(fp_B, "\n");
    }
    // write dim data
    FILE *fp_dim = fopen(fn_dim, "w");
    fprintf(fp_dim, "%d %d", W, H); // width height

    free(Data_RGB[0]);
    free(Data_RGB);
    fclose(fp_in);
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
void Padding(ImgRGB **Data_RGB, ImgRGB **Padded_Data_RGB, int H, int W, int newH, int newW)
{
    // 先複製
    for (int i = 0; i < H; i++)
    {
        for (int j = 0; j < W; j++)
        {
            Padded_Data_RGB[i][j] = Data_RGB[i][j];
        }
    }

    // 填充右邊
    for (int i = 0; i < H; i++)
    {
        for (int j = W; j < newW; j++)
        {
            Padded_Data_RGB[i][j] = Data_RGB[i][W - 1];
        }
    }

    // 填充上邊
    for (int i = H; i < newH; i++)
    {
        for (int j = 0; j < newW; j++)
        {
            Padded_Data_RGB[i][j] = (j < W) ? Data_RGB[H - 1][j] : Data_RGB[H - 1][W - 1];
        }
    }
}
void DCT(ImgYCbCr **Data_YCbCr, ImgYCbCr **DCT_Result, int curr_row, int curr_col) // curr_row、col 代表目前處理第幾個block
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
    // 計算 2D DCT
    for (u = 0; u < 8; u++)
    {
        for (v = 0; v < 8; v++)
        {
            sumY = 0;
            sumCb = 0;
            sumCr = 0;
            cu = (u == 0) ? 1.0 / sqrt(2) : 1.0;
            cv = (v == 0) ? 1.0 / sqrt(2) : 1.0;

            // DCT for Y
            for (i = 0; i < 8; i++)
            {
                for (j = 0; j < 8; j++)
                {
                    sumY += Data_YCbCr[i + curr_row * 8][j + curr_col * 8].Y * cos_table[u][v][i][j];
                    sumCb += Data_YCbCr[i + curr_row * 8][j + curr_col * 8].Cb * cos_table[u][v][i][j];
                    sumCr += Data_YCbCr[i + curr_row * 8][j + curr_col * 8].Cr * cos_table[u][v][i][j];
                }
            }
            DCT_Result[u + curr_row * 8][v + curr_col * 8].Y = 0.25 * cu * cv * sumY;
            DCT_Result[u + curr_row * 8][v + curr_col * 8].Cb = 0.25 * cu * cv * sumCb;
            DCT_Result[u + curr_row * 8][v + curr_col * 8].Cr = 0.25 * cu * cv * sumCr;
        }
    }
}
void Quantize(ImgYCbCr **DCT_Result, ImgQT_YCbCr **QT_result, ImgYCbCr **QT_err, int curr_row, int curr_col)
{
    int i, j;
    // Y 用 Y的量化表
    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 8; j++)
        {
            QT_result[i + curr_row * 8][j + curr_col * 8].Y = round(DCT_Result[i + curr_row * 8][j + curr_col * 8].Y / YTable[i * 8 + j]);
            QT_err[i + curr_row * 8][j + curr_col * 8].Y = DCT_Result[i + curr_row * 8][j + curr_col * 8].Y - QT_result[i + curr_row * 8][j + curr_col * 8].Y * YTable[i * 8 + j];
        }
    }
    // Cb Cr 用另一張表
    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 8; j++)
        {
            QT_result[i + curr_row * 8][j + curr_col * 8].Cb = round(DCT_Result[i + curr_row * 8][j + curr_col * 8].Cb / CbCrTable[i * 8 + j]);
            QT_result[i + curr_row * 8][j + curr_col * 8].Cr = round(DCT_Result[i + curr_row * 8][j + curr_col * 8].Cr / CbCrTable[i * 8 + j]);
            QT_err[i + curr_row * 8][j + curr_col * 8].Cb = DCT_Result[i + curr_row * 8][j + curr_col * 8].Cb - QT_result[i + curr_row * 8][j + curr_col * 8].Cb * CbCrTable[i * 8 + j];
            QT_err[i + curr_row * 8][j + curr_col * 8].Cr = DCT_Result[i + curr_row * 8][j + curr_col * 8].Cr - QT_result[i + curr_row * 8][j + curr_col * 8].Cr * CbCrTable[i * 8 + j];
        }
    }
}
void saveQuantizationTable(char *filename, int table[64])
{
    FILE *fp = fopen(filename, "w");
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            fprintf(fp, "%d ", table[i * 8 + j]);
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
}
void saveQuantizedValues(char *filename, ImgQT_YCbCr **data, int H, int W, char *channel)
{
    FILE *fp = fopen(filename, "wb");
    for (int i = 0; i < H; i++)
    {
        for (int j = 0; j < W; j++)
        {
            short val;
            if (strcmp(channel, "Y") == 0)
                val = (short)data[i][j].Y;
            else if (strcmp(channel, "Cb") == 0)
                val = (short)data[i][j].Cb;
            else if (strcmp(channel, "Cr") == 0)
                val = (short)data[i][j].Cr;
            fwrite(&val, sizeof(short), 1, fp);
        }
    }
    fclose(fp);
}
void saveQuantizationError(char *filename, ImgYCbCr **error, int H, int W, char *channel)
{
    FILE *fp = fopen(filename, "wb");
    for (int i = 0; i < H; i++)
    {
        for (int j = 0; j < W; j++)
        {
            float errVal;
            if (strcmp(channel, "Y") == 0)
                errVal = error[i][j].Y;
            else if (strcmp(channel, "Cb") == 0)
                errVal = error[i][j].Cb;
            else if (strcmp(channel, "Cr") == 0)
                errVal = error[i][j].Cr;
            fwrite(&errVal, sizeof(float), 1, fp);
        }
    }
    fclose(fp);
}
// read data -> Padding -> RGB2YCbCr -> 2D DCT -> Quantization -> calculate error -> output
void encoder1(char *fn_bmp, char *fn_Qt_Y, char *fn_Qt_Cb, char *fn_Qt_Cr, char *fn_dim, char *fn_qF_Y, char *fn_qF_Cb, char *fn_qF_Cr, char *fn_eF_Y, char *fn_eF_Cb, char *fn_eF_Cr)
{
    int i, j;
    FILE *fp_in = fopen(fn_bmp, "rb");
    if (fp_in)
        printf("Open %s as input file!\n", fn_bmp);
    else
        printf("Fail to open %s as input file!\n", fn_bmp);
    // read header
    Bitmap bmpheader;
    readheader(fp_in, &bmpheader);

    int H = bmpheader.height;
    int W = bmpheader.width;

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

    printf("Loading BMP data: \n");
    ImgRGB **Data_RGB = malloc_2D(H, W);
    InputData(fp_in, Data_RGB, H, W, skip);

    printf("\nBMP header:\n");
    printHeader(bmpheader);
    printf("\n");

    // Padding 讓資料變成可被分隔為 8*8 blocks
    printf("Now Padding Data... \n");
    int newH = H + (8 - (H % 8)) % 8;
    int newW = W + (8 - (W % 8)) % 8;
    ImgRGB **Padded_Data_RGB = malloc_2D(newH, newW);
    Padding(Data_RGB, Padded_Data_RGB, H, W, newH, newW);
    // RGB to YCbCr
    printf("Now trans RGB to YCbCr... \n");
    ImgYCbCr **Data_YCbCr = YCBCRmalloc_2D(newH, newW);
    ImgYCbCr **Data_dctResult = YCBCRmalloc_2D(newH, newW);
    ImgQT_YCbCr **Data_qtResult = QTmalloc_2D(newH, newW);
    ImgYCbCr **QT_err = YCBCRmalloc_2D(newH, newW);
    RGBtoYCbCr(Padded_Data_RGB, Data_YCbCr, newH, newW);
    // new 的 H、W 都是保證能被8整除的

    printf("Now perform 2D DCT...\n");
    // 2D DCT
    for (int row = 0; row < block_H; row++)
    {
        for (int col = 0; col < block_W; col++)
        {
            DCT(Data_YCbCr, Data_dctResult, row, col);
        }
    }
    printf("Now Quantize the result...\n");
    // Quantize

    for (int row = 0; row < block_H; row++)
    {
        for (int col = 0; col < block_W; col++)
        {
            Quantize(Data_dctResult, Data_qtResult, QT_err, row, col);
        }
    }

    printf("Now Saving all the result...\n");
    // 寫入檔案
    // 保存量化表
    saveQuantizationTable(fn_Qt_Y, YTable);
    saveQuantizationTable(fn_Qt_Cb, CbCrTable);
    saveQuantizationTable(fn_Qt_Cr, CbCrTable);

    // 保存尺寸
    FILE *fp_dim = fopen(fn_dim, "w");
    fprintf(fp_dim, "%d %d %d %d", W, H, newW, newH);
    fclose(fp_dim);

    // 保存量化後的值
    saveQuantizedValues(fn_qF_Y, Data_qtResult, newH, newW, /* channel */ "Y");
    saveQuantizedValues(fn_qF_Cb, Data_qtResult, newH, newW, /* channel */ "Cb");
    saveQuantizedValues(fn_qF_Cr, Data_qtResult, newH, newW, /* channel */ "Cr");

    // 保存量化誤差
    saveQuantizationError(fn_eF_Y, QT_err, newH, newW, /* channel */ "Y");
    saveQuantizationError(fn_eF_Cb, QT_err, newH, newW, /* channel */ "Cb");
    saveQuantizationError(fn_eF_Cr, QT_err, newH, newW, /* channel */ "Cr");

    // 計算 block 1的量化誤差   10 log(信號^2/noise^2)
    float Y_SQNR[64];
    float Cb_SQNR[64];
    float Cr_SQNR[64];
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            float y = (Data_dctResult[i][j].Y * Data_dctResult[i][j].Y) / (QT_err[i][j].Y * QT_err[i][j].Y);
            float cb = (Data_dctResult[i][j].Cb * Data_dctResult[i][j].Cb) / (QT_err[i][j].Cb * QT_err[i][j].Cb);
            float cr = (Data_dctResult[i][j].Cr * Data_dctResult[i][j].Cr) / (QT_err[i][j].Cr * QT_err[i][j].Cr);

            Y_SQNR[i * 8 + j] = 10 * log10(y);
            Cb_SQNR[i * 8 + j] = 10 * log10(cb);
            Cr_SQNR[i * 8 + j] = 10 * log10(cr);
        }
    }
    printf("Block 1 SQNR :\n");
    printf("Y: ");
    for (int i = 0; i < 64; i++)
    {
        printf("%.2f ", Y_SQNR[i]);
    }
    printf("\nCb: ");
    for (int i = 0; i < 64; i++)
    {
        printf("%.2f ", Cb_SQNR[i]);
    }
    printf("\nCr: ");
    for (int i = 0; i < 64; i++)
    {
        printf("%.2f ", Cr_SQNR[i]);
    }
    free(Data_RGB[0]);
    free(Data_RGB);

    free(Data_YCbCr[0]);
    free(Data_YCbCr);

    free(Data_dctResult[0]);
    free(Data_dctResult);

    free(Data_qtResult[0]);
    free(Data_qtResult);

    free(QT_err[0]);
    free(QT_err);

    fclose(fp_in);

    printf("Job done!\n");
}

void zigZagOrder(ImgQT_YCbCr **QT_Data, int curr_row, int curr_col, short *zigzagged, const char *channel)
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
                        zigzagged[index++] = QT_Data[j + curr_row * 8][i + curr_col * 8].Y;
                    }
                    else if (strcmp(channel, "Cb") == 0)
                    {
                        zigzagged[index++] = QT_Data[j + curr_row * 8][i + curr_col * 8].Cb;
                    }
                    else if (strcmp(channel, "Cr") == 0)
                    {
                        zigzagged[index++] = QT_Data[j + curr_row * 8][i + curr_col * 8].Cr;
                    }
                }
                else
                {
                    // 奇數和，從下往上遍歷
                    if (strcmp(channel, "Y") == 0)
                    {
                        zigzagged[index++] = QT_Data[i + curr_row * 8][j + curr_col * 8].Y;
                    }
                    else if (strcmp(channel, "Cb") == 0)
                    {
                        zigzagged[index++] = QT_Data[i + curr_row * 8][j + curr_col * 8].Cb;
                    }
                    else if (strcmp(channel, "Cr") == 0)
                    {
                        zigzagged[index++] = QT_Data[i + curr_row * 8][j + curr_col * 8].Cr;
                    }
                }
            }
        }
    }
}
typedef struct
{
    short zeroCount;
    short value; // 非零值
} RLEData;

int RLEEncode(short *zigzagged, RLEData *rleData)
{
    int count = 0;
    short zeroCount = 0;

    for (int i = 0; i < 64; i++)
    {
        if (zigzagged[i] == 0)
        {
            zeroCount++;
        }
        else
        {
            rleData[count++] = (RLEData){zeroCount, zigzagged[i]};
            zeroCount = 0;
        }
    }

    // 後面都是 0  可以判斷是結尾
    if (zeroCount > 0)
    {
        rleData[count++] = (RLEData){0, 0};
    }

    return count; // 傳回有幾組 rleData
}
// read data -> Padding -> RGB2YCbCr -> 2D DCT -> Quantization -> ZigZag and DPCM -> RLE
/*                                                                                   / \
                                                                                    /   \
                                                                                  ascii  binary
*/
void encoder2(char *fn_bmp, char *type, char *fn_RLE_code)
{
    int i, j;
    FILE *fp_in = fopen(fn_bmp, "rb");
    if (fp_in)
        printf("Open %s as input file!\n", fn_bmp);
    else
        printf("Fail to open %s as input file!\n", fn_bmp);
    // read header
    Bitmap bmpheader;
    readheader(fp_in, &bmpheader);

    // W:3024 x H:4032 for input.bmp
    int H = bmpheader.height;
    int W = bmpheader.width;

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

    printf("Loading BMP data: \n");
    ImgRGB **Data_RGB = malloc_2D(H, W);
    InputData(fp_in, Data_RGB, H, W, skip);

    printf("\nBMP header:\n");
    printHeader(bmpheader);
    printf("\n");

    // Padding 讓資料變成可被分隔為 8*8 blocks
    printf("Now Padding Data... \n");
    int newH = H + (8 - (H % 8)) % 8;
    int newW = W + (8 - (W % 8)) % 8;
    ImgRGB **Padded_Data_RGB = malloc_2D(newH, newW);
    Padding(Data_RGB, Padded_Data_RGB, H, W, newH, newW);
    // RGB to YCbCr
    printf("Now trans RGB to YCbCr... \n");
    ImgYCbCr **Data_YCbCr = YCBCRmalloc_2D(newH, newW);
    ImgYCbCr **Data_dctResult = YCBCRmalloc_2D(newH, newW);
    ImgQT_YCbCr **Data_qtResult = QTmalloc_2D(newH, newW);
    ImgYCbCr **QT_err = YCBCRmalloc_2D(newH, newW); // encoder2 用不到
    RGBtoYCbCr(Padded_Data_RGB, Data_YCbCr, newH, newW);
    // new 的 H、W 都是保證能被8整除的

    printf("Now perform 2D DCT...\n");
    // 2D DCT
    for (int row = 0; row < block_H; row++)
    {
        for (int col = 0; col < block_W; col++)
        {
            DCT(Data_YCbCr, Data_dctResult, row, col);
        }
    }
    printf("Now Quantize the result...\n");
    // Quantize

    for (int row = 0; row < block_H; row++)
    {
        for (int col = 0; col < block_W; col++)
        {
            Quantize(Data_dctResult, Data_qtResult, QT_err, row, col);
        }
    }

    // DPCM 先處理DC值
    short previousDC_Y = 0, previousDC_Cb = 0, previousDC_Cr = 0;
    short currentDC_Y, currentDC_Cb, currentDC_Cr;
    short diffDC_Y, diffDC_Cb, diffDC_Cr;

    for (int row = 0; row < block_H; row++)
    {
        for (int col = 0; col < block_W; col++)
        {
            // 处理 Y 分量的 DC 系数
            currentDC_Y = Data_qtResult[row * 8][col * 8].Y;
            diffDC_Y = currentDC_Y - previousDC_Y;
            previousDC_Y = currentDC_Y;
            Data_qtResult[row * 8][col * 8].Y = diffDC_Y;

            // 处理 Cb 分量的 DC 系数
            currentDC_Cb = Data_qtResult[row * 8][col * 8].Cb;
            diffDC_Cb = currentDC_Cb - previousDC_Cb;
            previousDC_Cb = currentDC_Cb;
            Data_qtResult[row * 8][col * 8].Cb = diffDC_Cb;

            // 处理 Cr 分量的 DC 系数
            currentDC_Cr = Data_qtResult[row * 8][col * 8].Cr;
            diffDC_Cr = currentDC_Cr - previousDC_Cr;
            previousDC_Cr = currentDC_Cr;
            Data_qtResult[row * 8][col * 8].Cr = diffDC_Cr;
        }
    }
    // 每個block都RLE，然後存入txt / bin
    if (strcmp(type, "ascii") == 0)
    {
        FILE *fp_RLE_code = fopen(fn_RLE_code, "w");

        if (fp_RLE_code == NULL)
        {
            perror("Error opening file");
            exit(1); // 或其他錯誤處理
        }

        // 寫入圖像尺寸信息
        fprintf(fp_RLE_code, "%d %d %d %d\n", W, H, newW, newH);

        for (int row = 0; row < block_H; row++)
        {
            for (int col = 0; col < block_W; col++)
            {
                const char *channels[3] = {"Y", "Cb", "Cr"};
                for (int ch = 0; ch < 3; ch++)
                {
                    // 先進行 ZigZag 排序
                    short zigzagged[64];
                    zigZagOrder(Data_qtResult, row, col, zigzagged, channels[ch]);

                    // 進行 RLE 編碼
                    RLEData rleData[64];
                    int rleCount = RLEEncode(zigzagged, rleData);

                    // 寫入 RLE 數據
                    fprintf(fp_RLE_code, "%d %d %s  ", row, col, channels[ch]);
                    for (int i = 0; i < rleCount; i++)
                    {
                        fprintf(fp_RLE_code, "%d %d ", rleData[i].zeroCount, rleData[i].value);
                    }
                    fprintf(fp_RLE_code, "\n");
                }
            }
        }

        fclose(fp_RLE_code);
    }
    else if (strcmp(type, "binary") == 0)
    {
        FILE *fp_RLE_code = fopen(fn_RLE_code, "wb");
        if (fp_RLE_code == NULL)
        {
            perror("Error opening file");
            exit(1);
            ; // 或其他錯誤處理
        }
        // 先寫入大小
        fwrite(&W, sizeof(int), 1, fp_RLE_code);
        fwrite(&H, sizeof(int), 1, fp_RLE_code);
        fwrite(&newW, sizeof(int), 1, fp_RLE_code);
        fwrite(&newH, sizeof(int), 1, fp_RLE_code);

        int totalByte_Y = 0;
        int totalByte_Cb = 0;
        int totalByte_Cr = 0;
        // 一個block一個block處理
        for (int row = 0; row < block_H; row++)
        {
            for (int col = 0; col < block_W; col++)
            {
                // 分别處理 Y, Cb, Cr 通道
                const char *channels[3] = {"Y", "Cb", "Cr"};
                for (int ch = 0; ch < 3; ch++)
                {
                    // 先 zigzag
                    short zigzagged[64];
                    zigZagOrder(Data_qtResult, row, col, zigzagged, channels[ch]);

                    // RLE

                    RLEData *rleData = (RLEData *)malloc(64 * sizeof(RLEData));
                    int rleCount = RLEEncode(zigzagged, rleData);
                    // 寫入 按照 Y Cb Cr的順序
                    fwrite(rleData, sizeof(RLEData), rleCount, fp_RLE_code);
                    free(rleData);
                    // 計算壓縮後總大小
                    if (channels[ch] == "Y")
                    {
                        totalByte_Y += rleCount * 4; // 兩個 short  4 bytes
                    }
                    else if (channels[ch] == "Cb")
                    {
                        totalByte_Cb += rleCount * 4;
                    }
                    else if (channels[ch] == "Cr")
                    {
                        totalByte_Cr += rleCount * 4;
                    }
                }
            }
        }
        printf("total Y bytes: %d  \n", totalByte_Y);
        printf("total Cb bytes: %d  \n", totalByte_Cb);
        printf("total Cr bytes: %d  \n", totalByte_Cr);
        printf("total bytes: %d \n", totalByte_Y + totalByte_Cb + totalByte_Cr);
        printf("Y compression ratio (bmp / RLE) %.3f\n", (float)(newH * newW) / totalByte_Y);
        printf("Cb compression ratio (bmp / RLE) %.3f\n", (float)(newH * newW) / totalByte_Cb);
        printf("Cr compression ratio (bmp / RLE) %.3f\n", (float)(newH * newW) / totalByte_Cr);
        printf("Total compression ratio (bmp / RLE) %.3f\n", (float)(newH * newW * 3) / (totalByte_Y + totalByte_Cb + totalByte_Cr));
        printf("\n");
        //     for (int row = 0; row < block_H; row++)
        //     {
        //         for (int col = 0; col < block_W; col++)
        //         {
        //             // ZigZag 和 RLE
        //             short zigzagged[64];
        //             zigZagOrder(Data_qtResult, row, col, zigzagged, "Y");

        //             RLEData *rleData = (RLEData *)malloc(64 * sizeof(RLEData));
        //             int rleCount = RLEEncode(zigzagged, rleData);

        //             // 寫入 Y
        //             fwrite(rleData, sizeof(RLEData), rleCount, fp_RLE_code);
        //             free(rleData);
        //         }
        //     }
        //     // 接著處理所有 Cb 通道
        //     for (int row = 0; row < block_H; row++)
        //     {
        //         for (int col = 0; col < block_W; col++)
        //         {
        //             short zigzagged[64];
        //             zigZagOrder(Data_qtResult, row, col, zigzagged, "Cb");

        //             RLEData *rleData = (RLEData *)malloc(64 * sizeof(RLEData));
        //             int rleCount = RLEEncode(zigzagged, rleData);

        //             // 寫入 Cb
        //             fwrite(rleData, sizeof(RLEData), rleCount, fp_RLE_code);
        //             free(rleData);
        //         }
        //     }

        //     // 最後處理所有 Cr 通道
        //     for (int row = 0; row < block_H; row++)
        //     {
        //         for (int col = 0; col < block_W; col++)
        //         {
        //             short zigzagged[64];
        //             zigZagOrder(Data_qtResult, row, col, zigzagged, "Cr");

        //             RLEData *rleData = (RLEData *)malloc(64 * sizeof(RLEData));
        //             int rleCount = RLEEncode(zigzagged, rleData);

        //             // 寫入 Cr
        //             fwrite(rleData, sizeof(RLEData), rleCount, fp_RLE_code);
        //             free(rleData);
        //         }
        //     }
        //     fclose(fp_RLE_code);
    }

    free(Data_RGB[0]);
    free(Data_RGB);

    free(Data_YCbCr[0]);
    free(Data_YCbCr);

    free(Data_dctResult[0]);
    free(Data_dctResult);

    free(Data_qtResult[0]);
    free(Data_qtResult);

    free(QT_err[0]);
    free(QT_err);

    fclose(fp_in);

    printf("Job done!\n");
}

#define MAX_TREE_HT 512 //最多512種不同符號

struct MinHeapNode  // linked list Tree
{
    char data;
    unsigned freq;  // unsigned int
    struct MinHeapNode *left, *right;
};

struct MinHeap
{
    unsigned size;
    unsigned capacity;
    struct MinHeapNode **array;  //放Node的array (Min heap tree)
};

struct MinHeapNode *newNode(char data, unsigned freq) //建立新 Node
{
    struct MinHeapNode *temp = (struct MinHeapNode *)malloc(sizeof(struct MinHeapNode));
    temp->left = temp->right = NULL;
    temp->data = data;
    temp->freq = freq;
    return temp;
}

int compare(const void *a, const void *b)  // 用頻率比大小  給Qsort用的
{
    struct MinHeapNode **n1 = (struct MinHeapNode **)a;
    struct MinHeapNode **n2 = (struct MinHeapNode **)b;
    return (*n1)->freq - (*n2)->freq;
}

struct MinHeap *createAndBuildMinHeap(char data[], int freq[], int size) // 先拿資料近來 把元素、頻率 塞進 min heap array
{
    struct MinHeap *minHeap = (struct MinHeap *)malloc(sizeof(struct MinHeap));
    minHeap->size = size;
    minHeap->capacity = size;
    minHeap->array = (struct MinHeapNode **)malloc(minHeap->capacity * sizeof(struct MinHeapNode *));

    for (int i = 0; i < size; ++i)
        minHeap->array[i] = newNode(data[i], freq[i]);

    qsort(minHeap->array, minHeap->size, sizeof(struct MinHeapNode *), compare);

    return minHeap;
}// ex minHeap->array  ==> (元素,頻率) (a,2) (d,6) (2,6) ........ ()為一個Node元素

struct MinHeapNode *buildHuffmanTree(char data[], int freq[], int size)
{
    struct MinHeapNode *left, *right, *top;
    struct MinHeap *minHeap = createAndBuildMinHeap(data, freq, size);

    while (minHeap->size > 1)  // 建好的minHeap array 存滿各種頻率   這裡不斷取前兩個最小的放在left、right，合併成new node
    {
        left = minHeap->array[0];  // array[0] 代表 Min heap 的 root
        minHeap->array[0] = minHeap->array[minHeap->size - 1];  // 最小的給left後，array縮小1 ，最後的元素給array[0] 
        minHeap->size--;
        qsort(minHeap->array, minHeap->size, sizeof(struct MinHeapNode *), compare); //重新sort

        right = minHeap->array[0];
        minHeap->array[0] = minHeap->array[minHeap->size - 1];
        minHeap->size--;
        qsort(minHeap->array, minHeap->size, sizeof(struct MinHeapNode *), compare);

        top = newNode('$', left->freq + right->freq);
        top->left = left;
        top->right = right;

        minHeap->array[minHeap->size] = top; // 在尾端重新塞入新節點  sort  直到min Heap array 剩下一個元素 (root) 就建好tree了
        minHeap->size++;
        qsort(minHeap->array, minHeap->size, sizeof(struct MinHeapNode *), compare); 
    }

    return minHeap->array[0];
}

int isLeaf(struct MinHeapNode *root)
{
    return !(root->left) && !(root->right);
}
void storeCodes(struct MinHeapNode *root, int arr[], int top, char codes[MAX_TREE_HT][MAX_TREE_HT], char *codeTable)
{
    if (root->left) // DFS traversal
    {
        arr[top] = 0;
        storeCodes(root->left, arr, top + 1, codes, codeTable);
    }

    if (root->right)
    {
        arr[top] = 1;
        storeCodes(root->right, arr, top + 1, codes, codeTable);
    }
    //       root 
    //       0/\1
    //       a  b
    //     0/\1
    //   leaf 
    if (isLeaf(root))
    {
        int i;
        for (i = 0; i < top; ++i) // top = 你距離 root 的深度
        {
            codes[(int)root->data][i] = arr[i] + '0';
        }
        codes[(int)root->data][i] = '\0'; // 加字串結尾

        // 將符號及其編碼加入到codeTable中
        char line[100];
        sprintf(line, "%c %s\n", root->data, codes[(int)root->data]);
        strcat(codeTable, line);
    }
}

void HuffmanCodes(char *str, char **encodedStr, char *codeTable)
{
    int freq[MAX_TREE_HT] = {0};
    char data[MAX_TREE_HT];
    int size = 0;
    char codes[MAX_TREE_HT][MAX_TREE_HT];

    // 統計字符頻率
    for (int i = 0; str[i] != '\0'; i++)
    {
        int index = -1;
        for (int j = 0; j < size; j++)
        {
            if (data[j] == str[i])
            {
                index = j;
                break;
            }
        }
        if (index == -1)
        {
            data[size] = str[i];
            freq[size] = 1;
            size++;
        }
        else
        {
            freq[index]++;
        }
    }

    // 建立 Huffman 樹
    struct MinHeapNode *root = buildHuffmanTree(data, freq, size);
    int arr[MAX_TREE_HT], top = 0;
    codeTable[0] = '\0'; // 初始化編碼表字符串
    storeCodes(root, arr, top, codes, codeTable);

    // 初始化編碼後的字符串
    size_t encodedStrSize = 1024;
    *encodedStr = malloc(encodedStrSize);
    if (!(*encodedStr))
    {
        // 處理記憶體分配錯誤
        return;
    }
    (*encodedStr)[0] = '\0';

    // 進行 Huffman 編碼
    for (int i = 0; str[i] != '\0'; i++)
    {
        size_t codeLen = strlen(codes[(int)str[i]]); //算大小夠不夠
        if (strlen(*encodedStr) + codeLen + 1 > encodedStrSize)
        {
            encodedStrSize *= 2;
            char *newBuffer = realloc(*encodedStr, encodedStrSize);
            if (!newBuffer)
            {
                free(*encodedStr);
                return;
            }
            *encodedStr = newBuffer;
        }
        strcat(*encodedStr, codes[(int)str[i]]); //字元對應的編碼寫入
    }
}
void writeBitsToFile(const char *bitString, FILE *file)
{
    unsigned char buffer = 0;
    int bitLength = strlen(bitString);

    for (int i = 0; i < bitLength; ++i) // buffer本來就是一個byte，透過 OR 得到字串的值
    {
        buffer = buffer | (bitString[i] - '0') << (7 - (i % 8)); // i%8 = 0  -> '1'1011011    buffer [1],[][][][][][][]

        if (i % 8 == 7) //湊滿一個 byte
        {
            fwrite(&buffer, 1, 1, file);
            buffer = 0;
        }
    }

    // 處理剩餘的bit（Padding 0）
    if (bitLength % 8 != 0)
    {
        fwrite(&buffer, 1, 1, file);
    }
}
void encoder3(char *fn_bmp, char *type, char *fn_Codebook, char *fn_Huffman_code)
{
    int i, j;
    FILE *fp_in = fopen(fn_bmp, "rb");
    if (fp_in)
        printf("Open %s as input file!\n", fn_bmp);
    else
        printf("Fail to open %s as input file!\n", fn_bmp);
    // read header
    Bitmap bmpheader;
    readheader(fp_in, &bmpheader);

    // W:3024 x H:4032 for input.bmp
    int H = bmpheader.height;
    int W = bmpheader.width;

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

    printf("Loading BMP data: \n");
    ImgRGB **Data_RGB = malloc_2D(H, W);
    InputData(fp_in, Data_RGB, H, W, skip);

    printf("\nBMP header:\n");
    printHeader(bmpheader);
    printf("\n");

    // Padding 讓資料變成可被分隔為 8*8 blocks
    printf("Now Padding Data... \n");
    int newH = H + (8 - (H % 8)) % 8;
    int newW = W + (8 - (W % 8)) % 8;
    ImgRGB **Padded_Data_RGB = malloc_2D(newH, newW);
    Padding(Data_RGB, Padded_Data_RGB, H, W, newH, newW);
    // RGB to YCbCr
    printf("Now trans RGB to YCbCr... \n");
    ImgYCbCr **Data_YCbCr = YCBCRmalloc_2D(newH, newW);
    ImgYCbCr **Data_dctResult = YCBCRmalloc_2D(newH, newW);
    ImgQT_YCbCr **Data_qtResult = QTmalloc_2D(newH, newW);
    ImgYCbCr **QT_err = YCBCRmalloc_2D(newH, newW); // encoder2 用不到
    RGBtoYCbCr(Padded_Data_RGB, Data_YCbCr, newH, newW);
    // new 的 H、W 都是保證能被8整除的

    printf("Now perform 2D DCT...\n");
    // 2D DCT
    for (int row = 0; row < block_H; row++)
    {
        for (int col = 0; col < block_W; col++)
        {
            DCT(Data_YCbCr, Data_dctResult, row, col);
        }
    }
    printf("Now Quantize the result...\n");
    // Quantize

    for (int row = 0; row < block_H; row++)
    {
        for (int col = 0; col < block_W; col++)
        {
            Quantize(Data_dctResult, Data_qtResult, QT_err, row, col);
        }
    }

    // DPCM 先處理DC值
    short previousDC_Y = 0, previousDC_Cb = 0, previousDC_Cr = 0;
    short currentDC_Y, currentDC_Cb, currentDC_Cr;
    short diffDC_Y, diffDC_Cb, diffDC_Cr;

    for (int row = 0; row < block_H; row++)
    {
        for (int col = 0; col < block_W; col++)
        {
            // Y 分量的 DC 
            currentDC_Y = Data_qtResult[row * 8][col * 8].Y;
            diffDC_Y = currentDC_Y - previousDC_Y;
            previousDC_Y = currentDC_Y;
            Data_qtResult[row * 8][col * 8].Y = diffDC_Y;

            // Cb 分量的 DC 
            currentDC_Cb = Data_qtResult[row * 8][col * 8].Cb;
            diffDC_Cb = currentDC_Cb - previousDC_Cb;
            previousDC_Cb = currentDC_Cb;
            Data_qtResult[row * 8][col * 8].Cb = diffDC_Cb;

            // Cr 分量的 DC 
            currentDC_Cr = Data_qtResult[row * 8][col * 8].Cr;
            diffDC_Cr = currentDC_Cr - previousDC_Cr;
            previousDC_Cr = currentDC_Cr;
            Data_qtResult[row * 8][col * 8].Cr = diffDC_Cr;
        }
    }

    size_t codeTextSize = 1024; // Starting size, adjust as needed
    char *codeText = malloc(codeTextSize * sizeof(char));
    if (!codeText)
    {
        // Handle memory allocation error
        exit(1);
    }
    codeText[0] = '\0'; // Initialize as an empty string

    for (int row = 0; row < block_H; row++)
    {
        for (int col = 0; col < block_W; col++)
        {
            const char *channels[3] = {"Y", "Cb", "Cr"};
            for (int ch = 0; ch < 3; ch++)
            {
                // 先進行 ZigZag 排序
                short zigzagged[64];
                zigZagOrder(Data_qtResult, row, col, zigzagged, channels[ch]);

                // 進行 RLE 編碼
                RLEData rleData[64];
                int rleCount = RLEEncode(zigzagged, rleData);

                for (int i = 0; i < rleCount; i++)
                {
                    char strbuf[50]; // Buffer for one RLE entry
                    sprintf(strbuf, "%dn%dn", rleData[i].zeroCount, rleData[i].value); // 1n2n5n23n1n2n0n-2n0n0n.......

                    // Check if we need to resize codeText
                    if (strlen(codeText) + strlen(strbuf) + 1 > codeTextSize)
                    {
                        codeTextSize *= 2; // Double the buffer size
                        char *newBuffer = realloc(codeText, codeTextSize * sizeof(char));
                        if (!newBuffer)
                        {
                            // Handle memory allocation error
                            free(codeText);
                            exit(1);
                        }
                        codeText = newBuffer;
                    }

                    strcat(codeText, strbuf); // Append to codeText
                }
            }
        }
    }
    char *encodedStr = NULL;
    char *codeTable = malloc(10000 * sizeof(char));
    HuffmanCodes(codeText, &encodedStr, codeTable);
    // store code table
    FILE *file = fopen(fn_Codebook, "w");
    if (file != NULL)
    {
        fprintf(file, "%s", codeTable);
        fclose(file);
    }
    if (strcmp(type, "ascii") == 0)
    {
        FILE *fp_Huffman_code = fopen(fn_Huffman_code, "w");

        if (fp_Huffman_code == NULL)
        {
            perror("Error opening file");
            exit(1);
        }

        // 寫入圖像尺寸信息
        fprintf(fp_Huffman_code, "%d %d %d %d\n", W, H, newW, newH);
        // store encoded string
        fprintf(fp_Huffman_code, "%s\n", encodedStr);

        fclose(fp_Huffman_code);
    }
    else if (strcmp(type, "binary") == 0)
    {
        // 寫入string前先算 padded bits 數
        int paddedBitCount = 8 - strlen(codeText) % 8;
        FILE *fp_Huffman_code = fopen(fn_Huffman_code, "wb");
        if (fp_Huffman_code == NULL)
        {
            perror("Error opening file");
            exit(1);
        }
        int data[5] = {W, H, newW, newH, paddedBitCount};

        // 先寫入必要資訊
        fwrite(data, sizeof(int), 5, fp_Huffman_code);

        // 寫入 bits + padding
        writeBitsToFile(encodedStr, fp_Huffman_code);

        fclose(fp_Huffman_code);
    }
    // Clean up
    free(codeText);
    free(codeTable);
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
        encoder0(fn_bmp, fn_R, fn_G, fn_B, fn_dim);
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
        encoder1(fn_bmp, fn_Qt_Y, fn_Qt_Cb, fn_Qt_Cr, fn_dim, fn_qF_Y, fn_qF_Cb, fn_qF_Cr, fn_eF_Y, fn_eF_Cb, fn_eF_Cr);
    }
    else if (strcmp(mode, "2") == 0)
    {
        char *fn_bmp = argv[2];
        char *type = argv[3]; // ascii or binary
        char *fn_RLE_code = argv[4];

        encoder2(fn_bmp, type, fn_RLE_code);
    }
    else if (strcmp(mode, "3") == 0)
    {

        char *fn_bmp = argv[2];
        char *type = argv[3]; // ascii or binary
        char *fn_Codebook = argv[4];
        char *fn_Huffman_code = argv[5];
        encoder3(fn_bmp, type, fn_Codebook, fn_Huffman_code);
    }
    else
    {
        printf("Wrong mode \n");
        exit(1); // 退出
    }

    return 0;
}
