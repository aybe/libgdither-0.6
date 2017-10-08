CFLAGS = -I.. -O3 -g -Wall -Werror --std=c99
CPPFLAGS = -I.. -O3 -g -Wall -Werror
LDFLAGS = ../gdither.o -lm

PROGS = runcheck distortion shapeplot noisetest interleavetest cplusplus
TESTPROGS = runcheck distortion noisetest interleavetest cplusplus

all: $(PROGS) ../gdither.o

test: all
	@for i in $(TESTPROGS); do \
		./$$i; \
		echo; \
	done; \

../gdither.o: ../gdither.c
	(cd .. && make)

distortion: distortion.c ../gdither.o
	$(CC) $(CFLAGS) ../gdither.o -lm `pkg-config --libs fftw3` \
		distortion.c -o distortion

shapeplot: shapeplot.c ../gdither.o
	$(CC) $(CFLAGS) ../gdither.o -lm `pkg-config --libs fftw3` \
		shapeplot.c -o shapeplot

noisetest: noisetest.c ../noise.h
	$(CC) $(CFLAGS) -lm `pkg-config --libs fftw3` \
		noisetest.c -o noisetest

clean:
	rm -f $(PROGS) *.o
