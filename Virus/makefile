#format is target-name: target dependencies
#{-tab-}actions

# All Targets
all: Virus

Virus: Virus.o
	ld -melf_i386 Virus.o -o Virus

Virus.o: Virus.asm
	nasm -f elf Virus.asm -o Virus.o
	
.PHONY: clean

clean: 
	rm -f *.o Virus.o Virus