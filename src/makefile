all: debug/center debug/receptor

debug/receptor: code/receptor.o code/book.h
	gcc -o debug/receptor code/receptor.o

code/receptor.o: code/receptor.c code/book.h
	gcc -c code/receptor.c

debug/center: code/commandCenter.o code/book.h
	gcc -o debug/center code/commandCenter.o

code/commandCenter.o: code/commandCenter.c code/book.h
	gcc -c code/commandCenter.c
