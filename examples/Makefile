CFLAGS = -I.. -O3 -g -Wall --std=c99
LDFLAGS = ../gdither.o -lm

PROGS = ex1

all: $(PROGS) ../gdither.o

../gdither.o: ../gdither.c
	(cd .. && make)

clean:
	rm -f $(PROGS) *.o
