separador_digitos:
	gcc test_separador_digitos.c -o test_separador_digitos.exe
	./test_separador_digitos.exe
compile:    
	avr-gcc -g -Os -mmcu=atmega328 -c main.c
	avr-gcc -g -mmcu=atmega328p -o main.elf main.o
	avr-objcopy -j .text -j .data -O ihex main.elf main.hex	
clear:
	-rm main.o
	-rm main.elf
	-rm main.hex
	-rm test_separador_digitos.exe