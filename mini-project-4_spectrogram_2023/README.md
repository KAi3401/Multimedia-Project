## 程式執行說明
- 在執行mini-project-4.sh之前，請確保本資料夾內的所有檔案都放在同一個目錄下
- 此外，spectshow.py需要numpy、matplot、scipy等函式庫，請確保有安裝
- mini-project-4.sh內會使用gcc來compile .c檔
## 執行預期結果
- 在執行 mini-project-4.sh 之後，會看到一些執行過程資訊以及執行進度，預估會執行1分20秒左右
- 最終在資料夾中會看到的檔案有：
  - result 資料夾 (存放圖片.pdf)
  - tmp 資料夾 (存放過程檔案)
  - aeueo-8kHz.wav
  - aeueo-16kHz.wav
  - s-8kHz.wav
  - s-16kHz.wav
  - cascade.c
  - sinegen.c
  - spectrogram.c
  - spectshow.py
  - mini-project-4.sh
## mini-project-4.sh 內執行流程 (配合程式碼看會比較好理解)
### 1. 目錄建立與清理
- 創建名為result的目錄，以及一個暫存目錄tmp。
- 檢查tmp目錄是否有檔案，如有則清理。
- 建立空的scp.txt檔案。
### 2. 編譯並執行sinegen.c
- sinegen.c透過shell script產生的80個wav檔案，並把檔案名稱記錄在scp.txt
- 產生完 80 個 wav 檔案後 sleep 1秒，避免檔案還沒完全處理好就cascade而報錯
### 3. 編譯並執行cascade.c
- cascade.c將讀入scp.txt檔案，一行一行提取檔名，前40行當8kHz，後40行當16kHz (此控制可以在cascade.c中看見)
- cascade.c中，讀入一個檔案並把資料寫進s-8kHz.wav (以前40個為例)後，就會將該使用過的wav檔案移除
- 80個wav檔案結合成s-8kHz.wav s-16kHz.wav，並放在同一個目錄
- 結合完後，把scp.txt移到tmp資料夾，並移除sinegen.exe、cascade.exe
### 4. 編譯並執行 spectrogram.c、spectshow.py (此過程非常耗時)
- 在 mini-project-4.sh 中定義了不同設定、對應的wav檔名
- 用3層for迴圈來將不同的設定、wav檔案輸入spectrogram.c，將計算結果輸出為output.txt
- 同樣在for迴圈之內 spectshow.py將讀入 .wav、output.txt、outfile.pdf 並繪製聲紋圖、保存
- 畫完圖之後，將output.txt移到 tmp 資料夾
- 以上的output.txt、outfile.pdf等等檔案名稱實際上在for內會有對應的檔名去遍歷，這裡只是代稱
- 4個wav檔案跑完4組設定後，刪除 spectrogram.exe
- 以上就是全部流程

## 程式碼內容概述
### 1. sinegen.c
- 透過指令 ./1 fs BITS_PER_SAMPLE wave frequency amplitude duration $out_file 來執行
- 由於本次project都是使用16bits的sample depth，並且也不需要計算SQNR的功能，因此相較於mini-project-3，我移除了SQNR的相關操作，也移除了8bits、32bits的sample depth的相關操作，使程式碼更加簡短明瞭。
- 上次作業的振幅是輸入0-1(normalized)，而這次不是，是由-32768-32767 (16bits)，因此我把normalize的功能也拿掉，使程式正常運作
### 2. cascade.c
- 透過指令 ./2 scp.txt s-8kHz.wav s-16kHz.wav來執行
- 一開始會先用fopen創建s-8kHz.wav s-16kHz.wav，並且先幫他們寫好header，因為dataSize、sample rate、sample bit等等都是已知資訊，dataSize只要取原本sinegen.c產生出來的dataSize * 40即可。
- 接著fopen打開scp.txt，並且透過迴圈 for(int i=0;i<80;++i)來讀入scp.txt的每一行
- scp.txt每一行對應一個wav檔，我用fseek跳過他的header，並用while ((c = fgetc(inputFile)) != EOF) fputc(c, outputFile1);來把該wav檔案的data放進對應的.wav。
- 前40行 (i<40)將資料放進s-8kHz.wav，反之則放進s-16kHz.wav。
### 3. spectrogram.c
- 透過指令./spectrogram w_size "w_type dft_size f_itv input_file output_file 來執行
- 一開始先讀入wav檔header有需要的資訊，例如fs、dataSize等等
 - 接著定義一些需要用到的變數：
  - int fs = header.sampleRate;
  - int frameInterval = fs / 1000 * f_itv;
  - int windowSize = fs / 1000 * w_Size;
  - int DFT_window = fs / 1000 * DFT_Size;
- 接著mallac一個陣列來存wav的所有data(header除外) int16_t *data = (int16_t *)malloc((numSamples + DFT_window) * sizeof(int16_t));
- 大小為numSamples + DFT_window是因為有可能取window會取超過data，然後padding zero
- 然後先建立cos、sin表格，以後就可以查表避免重複計算
```
for (int i = 0; i < DFT_window; i++)
    {
        angle = 2 * PI * i / DFT_window;
        cos_values[i] = cos(angle);
        sin_values[i] = sin(angle);
    }
```
- 接著就可以進入以下while迴圈，這個迴圈內容為：
  - 取一個 w_size(外部輸入)大小的資料量下去套window，並且拿套完 window 的 DFT_data 去計算DFT
  - 因為DFT_data一開始宣告都是0，因此當 w_size < dft_size，就有自動補0的功效
  - 然後在 computeDFT5中，會把算完的DFT資料放進 全域變數 specData，然後接著就把specDate寫進.txt檔
  - 接著繼續做下一個frame
```
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
```
