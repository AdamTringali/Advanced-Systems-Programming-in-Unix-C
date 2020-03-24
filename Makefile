CC=gcc
CFLAGS = 
# -g -Wall -Werror -O2 -DDEBUG1 -DEXTRACREDIT

a: main.o LKmalloc.o
	gcc main.o LKmalloc.o -o a

a.o: main.c header.h
	$(CC) $(CFLAGS) -c LKmalloc.c

main.o: main.c
	$(CC) $(CFLAGS) -c main.c 

valgrind:
	valgrind --show-leak-kinds=all ./a


clean: 
	rm *.o a