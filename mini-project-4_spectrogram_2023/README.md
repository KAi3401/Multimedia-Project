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
- cascade將讀入scp.txt檔案，一行一行提取檔名，前40行當8kHz，後40行當16kHz (此控制可以在cascade.c中看見)
- cascade中，讀入一個檔案並把資料寫進s-8kHz.wav (以前40個為例)後，就會將該使用過的wav檔案移除
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

