GCC = gcc
GCCFLAGS =  -Wall -fsanitize=address,leak -g
LDFLAGS = -fsanitize=address,leak

.PHONY: all clean

all: main

main: main.o allocator.o
	${GCC} ${GCCFLAGS} $^ -o $@

main.o: main.c
	${GCC} ${GCCFLAGS} -c $< -o $@

allocator.o: allocator.c allocator.h
	${GCC} ${GCCFLAGS} -c $< -o $@

clean:
	rm -f *.o main
