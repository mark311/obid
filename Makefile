all: obid-demo obid-train

obid-demo: obid.c main.c
	gcc -g -DDEBUG -o obid-demo obid.c main.c
obid-train: obid.c obid-train.c
	#gcc -g -DDEBUG -o obid-train obid.c obid-train.c
	gcc -o obid-train obid.c obid-train.c

