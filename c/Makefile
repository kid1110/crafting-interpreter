main : main.o chunk.o memory.o debug.o value.o vm.o scanner.o compiler.o object.o
	gcc -o main main.o chunk.o memory.o debug.o value.o vm.o scanner.o compiler.o object.o

main.o : main.c common.h chunk.h debug.h
	gcc -c main.c
chunk.o : chunk.c chunk.h memory.h value.h
	gcc -c chunk.c
memory.o : memory.c common.h
	gcc -c memory.c
debug.o : debug.c debug.h value.h
	gcc -c debug.c
value.o : value.c value.h memory.h
	gcc -c value.c
vm.o : vm.c vm.h common.h debug.h
	gcc -c vm.c
object.o: object.c common.h value.h
	gcc -c object.c
scanner.o : scanner.c scanner.h
compiler.o : compiler.c compiler.h debug.h common.h scanner.h
clean:
	rm *.o main