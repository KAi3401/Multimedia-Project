#!/bin/bash
gcc decoder.c -o decoder -lm
#./decoder 0 back.bmp R.txt G.txt B.txt dim.txt
./decoder 1 back.bmp Qt_Y.txt Qt_Cb.txt Qt_Cr.txt dim.txt qF_Y.raw qF_Cb.raw qF_Cr.raw eF_Y.raw eF_Cb.raw eF_Cr.raw
echo "done"
rm decoder.exe
