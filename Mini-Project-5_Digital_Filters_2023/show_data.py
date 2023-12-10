import numpy as np
import matplotlib.pyplot as plt
import sys

# 接收使用者輸入檔案名稱
file_name = sys.argv[1]
mod = sys.argv[2]

if len(sys.argv) != 3:
    print("Usage: python3 input_file mode")
    sys.exit(1)

# 提取文件名（不包含扩展名）
file_title = file_name.split('.')[0]


if(mod=='1'): #spectrogram

    # 開啟檔案，使用 with 來確保在使用完後正確地關閉檔案
    with open(file_name, 'r', encoding='utf-8') as file:
        # 讀取檔案內容並分割成行
        lines = file.readlines()

    # 計算行數
    num_lines = len(lines)
    # 初始化一個二維 NumPy 陣列
    data = np.zeros((num_lines, 1))

    # 將數字填入陣列
    for i, line in enumerate(lines):
        # 將每一行的 double 數字分割並轉換為浮點數
        numbers = [float(format(float(x.strip()), '.15f'))**3 / 10000 for x in lines]
    Max = max(numbers)

    for i, line in enumerate(lines):
        # if(numbers[i] < Max*0.2):  # <0.2*Max的削弱  (optional)
        #     numbers[i] = 0.1 * numbers[i]
        #     # 將數字存入 NumPy 陣列
        data[i][0] = numbers[i]

    # 創建一個figure和axes的陣列
    fig, axs = plt.subplots(figsize=(8, 6))

    # 在axes上繪製spectrogram
    axs.imshow(data, aspect='auto', cmap='gray', origin='lower', extent=[20.060, 20.085, 0, 24000])
    axs.set_xlabel('Time (seconds)')
    axs.set_ylabel('Frequency (Hz)')
    axs.set_title(f'Spectrogram - {file_title}')

    # 調整subplot的間距，以免overlap
    plt.tight_layout()

    # 保存圖片
    save_path = file_title + '_Spectrogram.png'
    plt.savefig(save_path, dpi=1200)

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

elif(mod=='3'): #impulse response

    with open(file_name, 'r') as file:
        impulse_response = [float(line.strip()) for line in file]

    # 生成x軸數據
    x = range(len(impulse_response))

    # 繪製stem圖
    plt.stem(x, impulse_response, basefmt='b',linefmt=".-",markerfmt=" ")

    # 添加標題和標籤
    file_title = file_name.split('.')[0]
    plt.title(file_title + '-Impulse Response')
    plt.xlabel('Sample Index')
    plt.xlim(0, len(impulse_response))
    plt.ylim(min(impulse_response) - 0.05 ,max(impulse_response) + 0.05)
    plt.ylabel('Amplitude')
    save_path = file_title + '_impulseResponse.png'
    plt.savefig(save_path, dpi=1200)
