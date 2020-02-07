filesec: main.o
	gcc main.o functions.c -o filesec 

main.o: main.c
	gcc -c main.c 

clean: 
	rm *.o filesec