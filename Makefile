CC=gcc
CFLAGS = 
# -g -Wall -Werror -O2 -DDEBUG1 -DEXTRACREDIT

a: main.o LKmalloc.o LKreport.o LKfree.o
	gcc main.o LKmalloc.o LKreport.o LKfree.o -o a

a.o: main.c header.h
	$(CC) $(CFLAGS) -c LKmalloc.c LKreport.c LKfree.c

main.o: main.c
	$(CC) $(CFLAGS) -c main.c 

valgrind:
	valgrind --show-leak-kinds=all ./a


clean: 
	rm *.o a