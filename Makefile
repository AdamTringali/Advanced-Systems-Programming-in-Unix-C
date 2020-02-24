CC=gcc
ifdef debug
 
CFLAGS = -Wall -O2 -DMYDEBUG
 
else
 
CFLAGS = -Wall -O2
 
endif

all:filesec

filesec: main.o debug.h
	gcc main.o functions.c -o filesec 

main.o: main.c
	$(CC) $(CFLAGS) -c main.c 

clean: 
	rm *.o filesec