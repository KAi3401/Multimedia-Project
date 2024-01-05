#!/bin/bash
gcc bitWrite.c -o 2 -lm
gcc bitRead.c -o 3 -lm
echo "Writing bit to bin file..."
./2 output.txt
echo "done"
rm 2.exe

echo "now readBit"
./3
echo "done"
rm 3.exe