#format is target-name: target dependencies
#{-tab-}actions

# All Targets
all: AntiVirus

AntiVirus: AntiVirus.o
	gcc -m32 -g -Wall -o AntiVirus AntiVirus.o

AntiVirus.o: AntiVirus.c
	gcc -g -Wall -m32 -ansi -c -o AntiVirus.o AntiVirus.c

.PHONY: clean

clean: 
	rm -f *.o AntiVirus
