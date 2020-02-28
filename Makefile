CC=gcc
LDFLAGS= -lssl -lcrypto

ifdef extracredit

CFLAGS = -g -Wall -Werror -O2 -DDEBUG1 -DEXTRACREDIT

else

CFLAGS = -g -Wall -Werror -O2 -DDEBUG1

endif

filesec: main.o functions.o macros.h
	gcc main.o functions.o -o filesec $(LDFLAGS)

functions.o: functions.c functions.h
	$(CC) $(CFLAGS) $(DEBUG) -c functions.c 

main.o: main.c
	$(CC) $(CFLAGS) $(DEBUG) -c main.c 

debug: DEBUG = -DMYDEBUG

debug: filesec

clean: 
	rm *.o filesec