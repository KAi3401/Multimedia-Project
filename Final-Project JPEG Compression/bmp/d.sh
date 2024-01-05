#!/bin/bash
gcc decoder.c -o decoder -lm
./decoder 0 back.bmp R.txt G.txt B.txt dim.txt
echo "done"
rm decoder.exe
