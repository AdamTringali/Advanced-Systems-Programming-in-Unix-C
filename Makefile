CC=gcc
CFLAGS = -g -Wall -Werror -O2 -DDEBUG1 -DEXTRACREDIT
LDFLAGS = -L -llkmalloc

# a: main2.o  lkmalloc.o
# 	gcc main2.o lkmalloc.o -o a 

# main2.o: main2.c lkmalloc.c
# 	$(CC) $(CFLAGS) -c main2.c lkmalloc.c

# a: main2.o 
# 	gcc main2.o -o a $(LDFLAGS)

# main2.o: main2.c
# 	$(CC) $(CFLAGS) -c main2.c $(LDFLAGS)

liblkmalloc.a: lkmalloc.o header.h
	ar cr liblkmalloc.a lkmalloc.o 

valgrind:
	valgrind --show-leak-kinds=all --leak-check=full ./driver_program

tests:
	$(CC) $(CFLAGS) driver.c lkmalloc.c -o driver_program
	./driver_program
	sh shell_scripts/test01.sh $(LDFLAGS)

depend: lkmalloc.c
	$(CC) -MD -c lkmalloc.c

clean: 
	rm *.o test1 test2 test3 test4 test5 test6 *.a *.d driver_program