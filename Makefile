CC=gcc
CFLAGS = -g -Wall -Werror -O2 -DDEBUG1 -DEXTRACREDIT
LDFLAGS = -L -llkmalloc

# a: main2.o 
# 	gcc main2.o -o a $(LDFLAGS)

# main2.o: main2.c
# 	$(CC) $(CFLAGS) -c main2.c $(LDFLAGS)

liblkmalloc.a: lkmalloc.o header.h
	ar cr liblkmalloc.a lkmalloc.o 

valgrind:
	valgrind --show-leak-kinds=all --leak-check=full ./a

tests:
	sh shell_scripts/test01.sh

depend: lkmalloc.c
	-$(CC) -MD lkmalloc.c

clean: 
	rm *.o a test1 test2 test3 test4 test5 test6 *.a *.d