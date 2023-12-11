# 程式執行說明
  - 執行run.sh後會先compile simple_filter.c，然後執行 window size = 1024的情況，再執行show_data,py，將產生出的結果畫出。
  - 然後將window size = 1024的情況所產生的音檔、txt檔、png檔，放入result_1024、tmp_1024的資料夾，方便整理、查看。
    - result_[windowSize] 存放著圖檔及wav檔。
    - tmp_[windowSize] 存放著txt檔。
  - 然後再用同樣的步驟去執行window size = 32、8。
  - 最後再移除exe檔。
  - simple_filter.c會使用**gcc**來compile。
  - 請確保python有安裝**numpy**和**matplotlib**。
# 程式執行預期結果
  - 執行完run.sh，並顯示**ALL done**!!後，資料夾內應該會出現以下檔案、資料夾：
    - results_8
    - results_32
    - results_1024
    - tmp_8
    - tmp_32
    - tmp_1024
    - blue_giant_fragment.wav
    - run.sh
    - show_data.py
    - simple_filter.c
# 程式碼內容概述
## simple_filter.c
執行方法為simple_filter M hL.txt hR.txt YL.txt YR.txt blue_giant_fragment.wav out.wav，後面的檔名目前在run.sh中是寫死的，更改的話會導致無法把產生的檔案移到特定資料夾。  
在這個程式中，我是使用老師提供的半成品程式碼，其中提供了低通濾波器、帶通濾波器的產生函數，我稍微更改了低通濾波器使其成為高通濾波器，改法如下：
```c
float high_pass(int m, int n)
{
	float wc = 2 * PI * FH / FS;
	if (n == m)
	{ // L'Hopital's Rule
		return 1 - wc / PI;
	}
	else
	{
		return -sinf(wc * ((float)(n - m))) / PI / ((float)(n - m)) * hamming(2 * m + 1, n);
	}
}
```
接著我就可以使用高通濾波器(過濾掉3500Hz以下) + 低通濾波器(過濾掉1500Hz以上) 來產生Bandstop濾波器：
```c
	for (n = 0; n < (2 * m + 1); n++)
	{
		h_L[n] = band_pass(m, n);
		h_R[n] = low_pass(m, n) + high_pass(m, n); //bandstop
	}
```
接著就將產生的h_R、h_L寫入txt檔案，型態為%e，並且一行一個數字。  
然後會把wavout.LChannel[n]、wavout.RChannel[n]第20.060~20.085秒的資料抓出來，並且套用Hamming Window後去做DFT，然後將log後的結果存入YL.txt、YR.txt，一樣型態為%e，一行一個資料：  
```c
void DFT(wav wavout, char *fn_YL, char *fn_YR)
{

	// 1200 samples = 0.025s
	// 20.06s = 1200 * (20.06/0.025) = 962880
	double sum1;
	double sum2;
	FILE *YL = fopen(fn_YL, "w");
	FILE *YR = fopen(fn_YR, "w");
	for (int k = 0; k < 600; k++)
	{
		double real1 = 0.0;
		double imag1 = 0.0;
		double real2 = 0.0;
		double imag2 = 0.0;

		for (int n = 0; n < 1200; n++)
		{
			int freq = (n * k) % 1200;
			// 使用查表，避免重複計算
			real1 += wavout.LChannel[962880 + n] * cos_values[freq] * hamming(1199,n);
			imag1 -= wavout.LChannel[962880 + n] * sin_values[freq] * hamming(1199,n);
			real2 += wavout.RChannel[962880 + n] * cos_values[freq] * hamming(1199,n);
			imag2 -= wavout.RChannel[962880 + n] * sin_values[freq] * hamming(1199,n);
		}

		sum1 = sqrt(real1 * real1 + imag1 * imag1);
		sum2 = sqrt(real2 * real2 + imag2 * imag2);
		if (sum1 < 1)
		{
			fprintf(YL, "0");
		}
		else
		{
			fprintf(YL, "%e", 20 * fabs(log10(sum1)));
		}
		if (sum2 < 1)
		{
			fprintf(YR, "0");
		}
		else
		{
			fprintf(YR, "%e", 20 * fabs(log10(sum2)));
		}
		if (k != 599)
		{
			fprintf(YL, "\n");
			fprintf(YR, "\n");
		}
	}
}
```
## show_data.py
執行方法為 python3 show_data.py [filename] [mode]，其中filename為txt檔的資料，而mode為繪製不同圖形所設，設定如下：
```
mode = 1 : draw spectrogram (額外產生的，拿來驗證濾波的成效)
mode = 2 : draw log spectrum
mode = 3 : draw impulse response
```
以Window_size=1024為例，我就會繪製以下6張圖片：
```sh
python3 show_data.py YL.txt 1
python3 show_data.py YL.txt 2
python3 show_data.py YR.txt 1
python3 show_data.py YR.txt 2
python3 show_data.py hL.txt 3
python3 show_data.py hR.txt 3
```
在程式碼中，除了mode1(聲紋圖)我是用上次的程式下去微調外，mode2、3幾乎一樣，並且都是使用stem來繪製。  
因為我存的資料是一行一個資料，所以我只要計算有幾行，並且把資料存到陣列、印出即可，以log spectrum為例：
```python
elif(mod=='2'):   #log spectrum
    with open(file_name, 'r') as file:
        log_data = [float(line.strip()) for line in file]

    # 生成x軸數據
    x = range(0,len(log_data)*40,40)

    # 繪製stem圖
    plt.stem(x, log_data, basefmt='b',linefmt=".-",markerfmt=" ")

    # 添加標題和標籤
    file_title = file_name.split('.')[0]
    plt.title(file_title + '-log spectrum')
    plt.xlabel('Frequency (Hz)')
    plt.xlim(0, len(log_data)*40)
    plt.ylim(min(log_data) ,max(log_data) + 10)
    plt.ylabel('Amplitude (dB)')
    save_path = file_title + '_logSpectrum.png'
    plt.savefig(save_path, dpi=1200)
```
其中
```python
x = range(0,len(log_data)*40,40)
plt.xlim(0, len(log_data)*40)
```
是因為我拿1200個資料下去做DFT，會產生600個結果 (剩下600-1200個資料是對稱的，所以只存前600)，然後我要將x軸顯示0-24000Hz，所以用這每個40個放一次資料的方法來實現。  
而impulse response就更簡單了，有幾行x就有多少，不用特別去調整。
# 濾波結果、討論
## Window Size = 1024
以下是左、右聲道的impulse response，可以看出是有2500個資料組成的sinc function，而右聲道的DC值更大，因為它是帶拒，會保留DC成分，相反的左聲道是帶通，所以DC值就比較小。
<div class="1024_h">
<img src="https://i.ibb.co/8dr4Pd4/h-L-impulse-Response.png" alt="h-L-impulse-Response" border="0" width="500" height="400">
<img src="https://i.ibb.co/XYbFwpy/h-R-impulse-Response.png" alt="h-R-impulse-Response" border="0" width="500" height="400">
</div>
然後對應所產生的YL、YR spectrum如下，可以看到YL 1500Hz以下被壓低了，1500-3500Hz相對於YR更加明顯，而YR 1500-3500Hz有明顯被壓低的樣子，並且>3500Hz的地方也都比YL還大，可以看出大致有帶通、帶拒的效果，只是沒有濾得很乾淨：
<div class="1024_y">
<img src="https://i.ibb.co/n0Q9cxy/YL-log-Spectrum.png" alt="YL-log-Spectrum" border="0" width="500" height="400">
<img src="https://i.ibb.co/jyF9Msc/YR-log-Spectrum.png" alt="YR-log-Spectrum" border="0" width="500" height="400">
</div>
然後讓我們畫出那0.025秒的聲紋圖，我一樣是有增強明暗對比，所以低頻成分有沒有被濾掉就很明顯，結果如下，可以看到YL的低頻(<1500Hz)相對於YR被濾了大部分，而YR的中頻(1500-3500Hz)相對於YL也被濾了大部分，並且高頻也有更多訊號(不明顯)，因此可以驗證我們的impulse response、log spectrum是正確的：
<div class="1024_s">
<img src="https://i.ibb.co/6NnvzCs/YL-Spectrogram.png" alt="YL-Spectrogram" border="0" width="500" height="400"> 
<img src="https://i.ibb.co/sqT17vp/YR-Spectrogram.png" alt="YR-Spectrogram" border="0" width="500" height="400">
</div>
	
## Window Size = 32
接下來是Window size = 32 的左、右聲道的impulse response，因為 size變小了，sinc圖形也更加簡化，但是一樣可以看出和 M = 1024 時一樣的特點：
<div class="32_h">
<img src="https://i.ibb.co/GkyMx2V/h-L-impulse-Response.png" alt="h-L-impulse-Response" border="0" width="500" height="400">
<img src="https://i.ibb.co/Ps2FFDk/h-R-impulse-Response.png" alt="h-R-impulse-Response" border="0" width="500" height="400">
</div>
然後是他們對應的YL、YR log spectrum，可以看出相對於 M = 1024的情況，YL的低平(<1500Hz)過濾效果更不好，YR的中頻(1500-3500Hz)也不如 M=1024 時的平整，整體來看過濾效果更不好：
<div class="32_y">
<img src="https://i.ibb.co/xYHgkch/YL-log-Spectrum.png" alt="YL-log-Spectrum" border="0" width="500" height="400">
<img src="https://i.ibb.co/BzhzPL9/YR-log-Spectrum.png" alt="YR-log-Spectrum" border="0" width="500" height="400">
</div>
接著從他們的聲紋圖來看，可以看到YL的低頻有被"<b>削弱</b>"但是並沒有被"<b>濾掉</b>"；而YR的中頻也是一樣的情況，但是中頻強度本來就沒有低頻強，因此看起來還可以：
<div class="32_s">
<img src="https://i.ibb.co/pLsqK85/YL-Spectrogram.png" alt="YL-Spectrogram" border="0" width="500" height="400">
<img src="https://i.ibb.co/dJg2Qj0/YR-Spectrogram.png" alt="YR-Spectrogram" border="0" width="500" height="400">
</div>
	
## Window Size = 8
最後是Window Size = 8 的情況，以下是左、右聲道的impulse response，可以看出來資料量更少，圖形更簡化，到時候濾波的效果應該會更差：
<div class="8_h">
<img src="https://i.ibb.co/MDB4FcN/h-L-impulse-Response.png" alt="h-L-impulse-Response" border="0" width="500" height="400">
<img src="https://i.ibb.co/4TFydMw/h-R-impulse-Response.png" alt="h-R-impulse-Response" border="0" width="500" height="400">
</div>
以及它們對應的YL、YR log spectrum，可以看出兩個圖基本長得差不多，也就是幾乎沒有濾掉任何資訊，又或者濾的效果極差，相對 M = 32 就可以比較出差異了：
<div class="8_y">
<img src="https://i.ibb.co/MRqCsHK/YL-log-Spectrum.png" alt="YL-log-Spectrum" border="0" width="500" height="400">
<img src="https://i.ibb.co/Qk2dRJ9/YR-log-Spectrum.png" alt="YR-log-Spectrum" border="0" width="500" height="400">
</div>
最後再來查看它們對應的聲紋圖，結果就很明顯了，幾乎沒有帶通、帶拒的效果：
<div class="8_s">
<img src="https://i.ibb.co/4NL2tdZ/YL-Spectrogram.png" alt="YL-Spectrogram" border="0" width="500" height="400">
<img src="https://i.ibb.co/M8wT2R9/YR-Spectrogram.png" alt="YR-Spectrogram" border="0" width="500" height="400">
</div>

## Output.wav
最後我們用Ocenaudio來查看濾波結果產生的wav檔案，可以看出不同Window Size所產生的效果，和上面說明的結果差不多，也應證了Window Size越小，過濾效果越差：
### M = 1024
<img src="https://i.ibb.co/X7sHtLj/M-1024.png" alt="M-1024" border="0">

### M = 32
<img src="https://i.ibb.co/4Y2Ntwb/M-32.png" alt="M-32" border="0">

### M = 8
<img src="https://i.ibb.co/QCnKH5k/M-8.png" alt="M-8" border="0">
