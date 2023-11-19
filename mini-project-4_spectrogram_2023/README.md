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
- scp.txt每一行對應一個wav檔，我用fseek跳過他的header，並用
```c
while ((c = fgetc(inputFile)) != EOF) fputc(c, outputFile1);
```
- 來把該wav檔案的data放進對應的.wav。
- 前40行 (i<40)將資料放進s-8kHz.wav，反之則放進s-16kHz.wav。
### 3. spectrogram.c
- 透過指令./spectrogram w_size "w_type dft_size f_itv input_file output_file 來執行
- 一開始先讀入wav檔header有需要的資訊，例如fs、dataSize等等
- 接著定義一些需要用到的變數：
  - int fs = header.sampleRate;
  - int frameInterval = fs / 1000 * f_itv;
  - int windowSize = fs / 1000 * w_Size;
  - int DFT_window = fs / 1000 * DFT_Size;
- 接著mallac一個陣列來存wav的所有data(header除外)
```c
int16_t *data = (int16_t *)malloc((numSamples + DFT_window) * sizeof(int16_t));
```
- 大小為numSamples + DFT_window是因為有可能取window會取超過data，然後padding zero
- 然後先建立cos、sin表格，以後就可以查表避免重複計算
```c
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
  - 然後在 computeDFT中，會把算完的DFT資料放進 全域變數 specData，然後接著就把specDate寫進.txt檔
  - .txt檔案中，每一行為一個frame的資訊，裡面存放的頻率強度由逗號隔開
  - 接著繼續做下一個frame，直到切的window超出原始dataSize
```c
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
```c
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
```
### 4. spectrogram.py
- 透過指令python3 spectshow.py spec.txt input.wav output_file.pdf來執行
- 一開始我用正規表達式來提取檔名中的一些資訊，可以做為後面輸出檔案、圖片title的元素
- 接著我判斷檔名中寫的是'8k' 或是'16k'，並且有對應的設定方便以下做處理
```python
# 使用正規表達式匹配所需部分
match = re.match(r'([a-zA-Z-]+-[0-9]+kHz)_setting\d+\.txt', file_name)
if match:
    prefix = match.group(1)
else:
    prefix = "Unknown"

# 根據檔案名稱判斷 X 和 Y 的值
if "16k" in file_name:
    X = 256
    Y = 8000
else:
    X = 128
    Y = 4000

# 使用正規表達式提取 "setting" 後的數字
set_number = re.search(r'setting(\d+)', file_name).group(1)
```
- Y為頻率軸(y軸)的上限，X為一個frame中有多少頻率的資訊 (假設8k取樣率、DFT_SIZE=32ms，就會有256個資訊，但是有一半會是對稱的，所以取128)
- 接著我會稍微加強spectrogram的明暗對比度，假設原始資料是x，我透過 $\(x' = \frac{x^3}{10000}\)$的方式來加強對比，使其圖片更像ocenaudio中所展示的
```python
# 計算行數
num_lines = len(lines)

# 初始化一個二維 NumPy 陣列
data = np.zeros((num_lines, X))

# 將數字填入陣列
for i, line in enumerate(lines):
    # 將每一行的 double 數字分割並轉換為浮點數
    numbers = [float(x) * float(x) * float(x) / 10000 for x in line.split(',')]
    # 將數字存入 NumPy 陣列
    data[i, :] = numbers
```
- 最後將wav檔案讀入，並且使wav、spectrogram圖片的x軸互相對應
- 接著設定好標題、xy軸、強度用灰度表示、frameInterval，就可以產生出圖片了，最後將結果存為pdf即可
```python
# 讀取WAV檔案
sample_rate, wave_data = wavfile.read(file_wave)

# 計算wave的時間軸
wave_time = np.arange(0, len(wave_data)) / sample_rate

# 定義時間間隔和頻率範圍
time_interval = 0.01  # 32ms per line
frequency_range = np.arange(0, Y, Y/X)

# 創建一個figure和axes的陣列
fig, axs = plt.subplots(2, 1, figsize=(8, 6), sharex=True)

# 分別在axes[0]和axes[1]上繪製waveform和spectrogram
axs[0].plot(wave_time, wave_data)
axs[0].set_ylabel('Amplitude')
axs[0].set_title('Waveform')
axs[0].set_ylim(-32768, 32767)

axs[1].imshow(data.T, aspect='auto', cmap='gray', origin='lower', extent=[0, num_lines*time_interval, 0, Y])
axs[1].set_xlabel('Time (seconds)')
axs[1].set_ylabel('Frequency (Hz)')
axs[1].set_title(f'Spectrogram - {prefix}_setting {set_number}')

# 調整subplot的間距，以免overlap
plt.tight_layout()

# 保存圖片
save_path = file_out
plt.savefig(save_path, dpi=1200)
plt.close()
```
