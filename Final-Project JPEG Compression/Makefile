CC=gcc
CFLAGS=-lm

all: encoder decoder mk

mk:
	mkdir demo0
	mkdir demo1
	mkdir demo2
	mkdir demo3

encoder: encoder_411086022.c
	$(CC) encoder_411086022.c -o encoder $(CFLAGS)

decoder: decoder_411086022.c
	$(CC) decoder_411086022.c -o decoder $(CFLAGS)



demo0: encoder decoder
	@echo -e "Encoding....:\n\n"
	./encoder 0 KimberlyNCat.bmp R.txt G.txt B.txt dim.txt

	@echo -e "\nDecoding....:\n\n"
	./decoder 0 ReKimberlyNCat.bmp R.txt G.txt B.txt dim.txt

	@echo -e "\nCmp Images:\n"
	cmp KimberlyNCat.bmp ReKimberlyNCat.bmp

	@echo -e "\nDemo 0 done\n"
	mv ReKimberlyNCat.bmp *.txt demo0/

demo1: encoder decoder
	@echo -e "\nEncoding....:\n\n"
	./encoder 1 KimberlyNCat.bmp Qt_Y.txt Qt_Cb.txt Qt_Cr.txt dim.txt qF_Y.raw qF_Cb.raw qF_Cr.raw eF_Y.raw eF_Cb.raw eF_Cr.raw

	@echo -e "\nDecoding....:\n\n"
	./decoder 1 ReKimberlyNCat.bmp Qt_Y.txt Qt_Cb.txt Qt_Cr.txt dim.txt qF_Y.raw qF_Cb.raw qF_Cr.raw eF_Y.raw eF_Cb.raw eF_Cr.raw

	@echo -e "\nCmp Images:\n"
	cmp KimberlyNCat.bmp ReKimberlyNCat.bmp

	@echo -e "\nDemo 1 done\n"

	mv *.txt *.raw ReKimberlyNCat.bmp QResKimberlyNCat.bmp demo1/
	
#   使用原圖會出問題
demo2: encoder decoder
	@echo -e "\nEncoding (ascii ver)....:\n\n"
	./encoder 2 KimberlyNCat2.bmp ascii rle_code.txt 
	@echo -e "\nEncoding (binary ver)....:\n\n"
	./encoder 2 KimberlyNCat2.bmp binary rle_code.bin

	@echo -e "\nDecoding(ascii ver)....:\n\n"
	./decoder 2 A_KimberlyNCat2.bmp ascii rle_code.txt
	@echo -e "\nDecoding (binary ver)....:\n\n"
	./decoder 2 B_ReKimberlyNCat2.bmp binary rle_code.bin

	@echo -e "\nDemo 2 done\n"

	mv *.txt *.bin A_KimberlyNCat2.bmp B_ReKimberlyNCat2.bmp demo2/
	
#   演算法還沒優化   Huffman 跑大圖太久
demo3: encoder decoder
	@echo -e "\nEncoding (ascii ver)....:\n\n"
	./encoder 3 Input2.bmp ascii codebook.txt huffman_code.txt
	@echo -e "\nEncoding (binary ver)....:\n\n"
	./encoder 3 Input2.bmp binary codebook.txt huffman_code.bin

	@echo -e "\nEncoding (ascii ver)....:\n\n"
	./decoder 3 A_ReInput2.bmp ascii codebook.txt huffman_code.txt
	@echo -e "\nEncoding (binary ver)....:\n\n"
	./decoder 3 B_ReInput2.bmp binary codebook.txt huffman_code.bin

	@echo -e "\nDemo 3 done\n"

	mv *.txt *.bin A_ReInput2.bmp B_ReInput2.bmp demo3/

clean:
	rm -f encoder decoder
.PHONY: all encoder decoder demo0 demo1 demo2 demo3 clean