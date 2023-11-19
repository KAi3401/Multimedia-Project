import numpy as np
import matplotlib.pyplot as plt
import re
import sys
from scipy.io import wavfile

# 接收使用者輸入檔案名稱
file_name = sys.argv[1]
file_wave = sys.argv[2]
file_out = sys.argv[3]


if len(sys.argv) != 4:
    print("Usage: python script.py input_file output_file wave_file")
    sys.exit(1)

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

# 開啟檔案，使用 with 來確保在使用完後正確地關閉檔案
with open(file_name, 'r', encoding='utf-8') as file:
    # 讀取檔案內容並分割成行
    lines = file.readlines()

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
