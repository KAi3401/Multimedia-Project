#!/bin/bash
gcc encoder.c -o encoder -lm
# ./encoder 0 Input2.bmp R.txt G.txt B.txt dim.txt
./encoder 1 Input2.bmp Qt_Y.txt Qt_Cb.txt Qt_Cr.txt dim.txt qF_Y.raw qF_Cb.raw qF_Cr.raw eF_Y.raw eF_Cb.raw eF_Cr.raw
echo "done"
rm encoder.exe
