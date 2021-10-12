all: debug/receptor debug/center 

debug/receptor: debug/receptor.o code/book.h
	gcc -o debug/receptor debug/receptor.o

debug/receptor.o: code/receptor.c code/book.h
	gcc -c code/receptor.c

debug/center: debug/commandCenter.o code/book.h
	gcc -o debug/center debug/commandCenter.o

debug/commandCenter.o: code/commandCenter.c code/book.h
	gcc -c code/commandCenter.c
