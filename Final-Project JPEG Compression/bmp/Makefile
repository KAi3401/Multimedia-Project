CC=gcc
CFLAGS=-lm

all: encoder decoder

encoder: encoder.c
	$(CC) encoder.c -o encoder $(CFLAGS)

decoder: decoder.c
	$(CC) decoder.c -o decoder $(CFLAGS)

demo0: encoder decoder
	@echo -e "Encoding....:\n\n"
	./encoder 0 KimberlyNCat.bmp R.txt G.txt B.txt dim.txt

	@echo -e "\nDecoding....:\n\n"
	./decoder 0 ReKimberlyNCat.bmp R.txt G.txt B.txt dim.txt

	@echo -e "\nCmp Images:\n"
	cmp KimberlyNCat.bmp ReKimberlyNCat.bmp

	@echo -e "\nDemo 0 done\n"
	

demo1: encoder decoder
	@echo -e "\nEncoding....:\n\n"
	./encoder 1 KimberlyNCat.bmp Qt_Y.txt Qt_Cb.txt Qt_Cr.txt dim.txt qF_Y.raw qF_Cb.raw qF_Cr.raw eF_Y.raw eF_Cb.raw eF_Cr.raw

	@echo -e "\nDecoding....:\n\n"
	./decoder 1 ReKimberlyNCat.bmp Qt_Y.txt Qt_Cb.txt Qt_Cr.txt dim.txt qF_Y.raw qF_Cb.raw qF_Cr.raw eF_Y.raw eF_Cb.raw eF_Cr.raw

	@echo -e "\nCmp Images:\n"
	cmp KimberlyNCat.bmp ReKimberlyNCat.bmp

	@echo -e "\nDemo 1 done\n"

demo2: encoder decoder
	@echo -e "\nEncoding (ascii ver)....:\n\n"
	./encoder 2 Input2.bmp ascii rle_code.txt 
	@echo -e "\nEncoding (binary ver)....:\n\n"
	./encoder 2 Input2.bmp binary rle_code.bin

	@echo -e "\nDecoding(ascii ver)....:\n\n"
	./decoder 2 ReInput2A.bmp ascii rle_code.txt
	@echo -e "\nDecoding (binary ver)....:\n\n"
	./decoder 2 ReInput2B.bmp binary rle_code.bin

	@echo -e "\nDemo 2 done\n"
clean:
	rm -f encoder decoder
