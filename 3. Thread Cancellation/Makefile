main.o:
	gcc -g -c main.c -o main.o


exe: main.o
	gcc main.o -o exe -pthread


Target: exe
	./exe


clear:
	rm -f myfile.txt
	rm -f myfile2.txt
	rm -f exe
	rm -f main.o


