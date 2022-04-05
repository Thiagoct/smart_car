compile_master:    
	avr-gcc -g -Os -mmcu=atmega328 -c master/master.c
	avr-gcc -g -mmcu=atmega328p -o master/master.elf master.o
	avr-objcopy -j .text -j .data -O ihex master/master.elf master/master.hex	
compile_slave:    
	avr-gcc -g -Os -mmcu=atmega328 -c slave/slave.c
	avr-gcc -g -mmcu=atmega328p -o slave/slave.elf slave.o
	avr-objcopy -j .text -j .data -O ihex slave/slave.elf slave/slave.hex	
clear:
	-rm master.o
	-rm master/master.elf
	-rm master/master.hex
	-rm slave.o
	-rm slave/slave.elf
	-rm slave/slave.hex