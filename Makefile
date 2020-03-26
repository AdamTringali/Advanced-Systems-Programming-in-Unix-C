CC=gcc
CFLAGS = -g
# -g -Wall -Werror -O2 -DDEBUG1 -DEXTRACREDIT

a: main2.o LKmalloc.o
	gcc main2.o LKmalloc.o -o a

a.o: main2.c header.h
	$(CC) $(CFLAGS) -c LKmalloc.c

main2.o: main2.c
	$(CC) $(CFLAGS) -c main2.c 

valgrind:
	valgrind --show-leak-kinds=all --leak-check=full ./a

clean: 
	rm *.o a