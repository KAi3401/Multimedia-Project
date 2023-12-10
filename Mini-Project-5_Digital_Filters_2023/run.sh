#!/bin/bash
# mode = 1 : draw spectrogram
# mode = 2 : draw log spectrum
# mode = 3 : draw impulse response
# python3 show_data.py [filename] [mode]
echo "Compiling simple_filter.c"
gcc simple_filter.c -o 1 -lm
# excuting filter.c with windows sizer = 1024
echo "executing filter.c with windows size = 1024"
./1 1024 hL.txt hR.txt YL.txt YR.txt blue_giant_fragment.wav out.wav
echo "done"
# Drawing results ( M = 1024 )
echo "Drawing results ( M = 1024 )"
python3 show_data.py YL.txt 1
python3 show_data.py YL.txt 2
python3 show_data.py YR.txt 1
python3 show_data.py YR.txt 2
python3 show_data.py hL.txt 3
python3 show_data.py hR.txt 3
echo "Done"
echo "now moving png to results folder"
mkdir -p "results_1024"
mkdir -p "tmp_1024"

mv "hL.txt" "tmp_1024/"
mv "hR.txt" "tmp_1024/"
mv "YL.txt" "tmp_1024/"
mv "YR.txt" "tmp_1024/"

mv "hL_impulseResponse.png" "results_1024/"
mv "hR_impulseResponse.png" "results_1024/"
mv "YL_logSpectrum.png" "results_1024/"
mv "YR_logSpectrum.png" "results_1024/"
mv "YL_Spectrogram.png" "results_1024/"
mv "YR_Spectrogram.png" "results_1024/"

echo "M = 1024 Done"

# excuting filter.c with windows sizer = 32
echo "executing filter.c with windows size = 32"
./1 32 hL.txt hR.txt YL.txt YR.txt blue_giant_fragment.wav out.wav
echo "done"
# Drawing results ( M = 32 )
echo "Drawing results ( M = 32 )"
python3 show_data.py YL.txt 1
python3 show_data.py YL.txt 2
python3 show_data.py YR.txt 1
python3 show_data.py YR.txt 2
python3 show_data.py hL.txt 3
python3 show_data.py hR.txt 3
echo "Done"
echo "now moving png to results folder"
mkdir -p "results_32"
mkdir -p "tmp_32"

mv "hL.txt" "tmp_32/"
mv "hR.txt" "tmp_32/"
mv "YL.txt" "tmp_32/"
mv "YR.txt" "tmp_32/"

mv "hL_impulseResponse.png" "results_32/"
mv "hR_impulseResponse.png" "results_32/"
mv "YL_logSpectrum.png" "results_32/"
mv "YR_logSpectrum.png" "results_32/"
mv "YL_Spectrogram.png" "results_32/"
mv "YR_Spectrogram.png" "results_32/"

echo "M = 32 Done"

# excuting filter.c with windows sizer = 8
echo "executing filter.c with windows size = 8"
./1 8 hL.txt hR.txt YL.txt YR.txt blue_giant_fragment.wav out.wav
echo "done"
# Drawing results ( M = 8 )
echo "Drawing results ( M = 8 )"
python3 show_data.py YL.txt 1
python3 show_data.py YL.txt 2
python3 show_data.py YR.txt 1
python3 show_data.py YR.txt 2
python3 show_data.py hL.txt 3
python3 show_data.py hR.txt 3
echo "Done"
echo "now moving png to results folder"
mkdir -p "results_8"
mkdir -p "tmp_8"

mv "hL.txt" "tmp_8/"
mv "hR.txt" "tmp_8/"
mv "YL.txt" "tmp_8/"
mv "YR.txt" "tmp_8/"

mv "hL_impulseResponse.png" "results_8/"
mv "hR_impulseResponse.png" "results_8/"
mv "YL_logSpectrum.png" "results_8/"
mv "YR_logSpectrum.png" "results_8/"
mv "YL_Spectrogram.png" "results_8/"
mv "YR_Spectrogram.png" "results_8/"

echo "M = 8 Done"

sleep 1
echo "All done !!"
rm 1.exe