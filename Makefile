all: DLL.o HT.o STACK.o
	gcc project_1.c -o proj.out -pthread
DLL.o:
	gcc DLL.h -o DLL.o -pthread
HT.o:
	gcc HashTables.h -o HT.o -pthread
STACK.o:
	gcc Stack.h -o Stack.o -pthread
clean:
	rm -f *.out *.o