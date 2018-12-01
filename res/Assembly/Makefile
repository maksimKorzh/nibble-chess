all:
	nasm -f elf MG.asm
	gcc -m32 -c debug.c
	gcc -m32 -o MG MG.o debug.o
	rm MG.o debug.o
