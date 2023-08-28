main : main.o chunk.o memory.o debug.o
	gcc -o main main.o chunk.o memory.o debug.o

main.o : main.c common.h chunk.h debug.h
	gcc -c main.c
chunk.o : chunk.c chunk.h memory.h
	gcc -c chunk.c
memory.o : memory.c common.h
	gcc -c memory.c
debug.o : debug.c debug.h
	gcc -c debug.c

clean:
	rm *.o main