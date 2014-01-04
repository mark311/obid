all: obid

obid: obid.c main.c
	gcc -g -DDEBUG -o obid obid.c main.c -lm
