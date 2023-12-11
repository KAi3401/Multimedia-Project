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
  - 執行完run.sh，並顯示**ALL done!!**後，資料夾內應該會出現以下檔案、資料夾：
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

<h1>
<img src="https://i.ibb.co/Chmm158/osu.jpg" alt="測試" width="300" height="200">
<img src="https://i.ibb.co/0KxsVSm/OSU-10.png" alt="OSU-10" border="0" width="300" height="200">
</h1>
