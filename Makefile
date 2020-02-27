CC=gcc
LDFLAGS= -lssl -lcrypto

ifdef extracredit

CFLAGS = -g -Wall -Werror -O2 -DDEBUG1 -DEXTRACREDIT

else

CFLAGS = -g -Wall -Werror -O2 -DDEBUG1

endif


filesec: main.o macros.h
	gcc main.o functions.c -o filesec $(LDFLAGS)

main.o: main.c
	$(CC) $(CFLAGS) $(DEBUG) -c main.c 

debug: DEBUG = -DMYDEBUG

debug: filesec

clean: 
	rm *.o filesec