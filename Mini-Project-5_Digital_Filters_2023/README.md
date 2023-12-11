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
        impulse_response = [float(line.strip()) for line in file]

    # 生成x軸數據
    x = range(0,len(impulse_response)*40,40)

    # 繪製stem圖
    plt.stem(x, impulse_response, basefmt='b',linefmt=".-",markerfmt=" ")

    # 添加標題和標籤
    file_title = file_name.split('.')[0]
    plt.title(file_title + '-log spectrum')
    plt.xlabel('Frequency (Hz)')
    plt.xlim(0, len(impulse_response)*40)
    plt.ylim(min(impulse_response) ,max(impulse_response) + 10)
    plt.ylabel('Amplitude (dB)')
    save_path = file_title + '_logSpectrum.png'
    plt.savefig(save_path, dpi=1200)
```
其中
```python
x = range(0,len(impulse_response)*40,40)
plt.xlim(0, len(impulse_response)*40)
```
是因為我拿1200個資料下去做DFT，會產生600個結果 (剩下600-1200個資料是對稱的，所以只存前600)，然後我要將x軸顯示0-24000Hz，所以用這每個40個放一次資料的方法來實現。  
而impulse response就更簡單了，有幾行x就有多少，不用特別去調整。
# 濾波結果、討論
## Window Size = 1024

<div class="image">
<img src="https://i.ibb.co/Chmm158/osu.jpg" alt="測試" width="300" height="200">
<img src="https://i.ibb.co/0KxsVSm/OSU-10.png" alt="OSU-10" border="0" width="300" height="200">
</div>
