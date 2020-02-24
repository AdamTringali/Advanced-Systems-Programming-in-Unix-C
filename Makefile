CC=gcc
CFLAGS =  -Wall -O2
 

filesec: main.o debug.h
	gcc main.o functions.c -o filesec 

main.o: main.c
	$(CC) $(CFLAGS) $(DEBUG) -c main.c 

debug: DEBUG = -DMYDEBUG

debug: filesec

clean: 
	rm *.o filesec