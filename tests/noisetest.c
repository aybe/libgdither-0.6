/*
 *  Copyright (C) 2004 Steve Harris
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  $Id: runcheck.c $
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <complex.h>
#include <fftw3.h>

#define SIZE 1024

#include "gdither.h"
#include "noise.h"
#include "compare.h"
#include "gettime.h"

#define REPS 10000
#define CYCLES 500.0

#define BINS 65536

int main(int argc, char *argv[])
{
    unsigned int i, j;
    fftw_complex *out = fftw_malloc(sizeof(fftw_complex) * (SIZE + 1));
    fftw_plan plan_rc;
    double in[SIZE], sum[SIZE/2];
    int distrib[BINS];
    double min = 1e10;
    double max = 0.0;
    int min_f = 1000;
    int max_f = 0;

    for (i=0; i<SIZE/2; i++) {
	sum[i] = 0.0;
    }

    plan_rc = fftw_plan_dft_r2c_1d(SIZE, in, out, 0);
    for (j=0; j<REPS; j++) {
	for (i=0; i<SIZE; i++) {
	    in[i] = gdither_noise();
	    //in[i] = random() / (float)RAND_MAX;
	}
	fftw_execute(plan_rc);
	for (i=1; i<SIZE/2; i++) {
	    sum[i] += cabs(out[i]);
	}
    }

    for (i=1; i<SIZE/2; i++) {
	if (sum[i] > max) {
	    max = sum[i];
	} else if (sum[i] < min) {
	    min = sum[i];
	}
	//printf("%d\t%f\n", i, sum[i]);
    }

    min /= REPS*16;
    max /= REPS*16;
    printf("random amp/frequecy range = [%.2f,%.2f]\n", min, max);
    if (max - min > 0.03) {
	printf("min-max range exceeds 0.03, seems excessive, failing\n");
	exit(1);
    }

    for (i=0; i<BINS; i++) {
	distrib[i] = 0;
    }
    for (i=0; i<BINS*CYCLES; i++) {
	int bin = gdither_noise() * (BINS-1);
	distrib[bin]++;
    }
    for (i=0; i<BINS-1; i++) {
	if (distrib[i] > max_f) {
	    max_f = distrib[i];
	}
	if (distrib[i] < min_f) {
	    min_f = distrib[i];
	}
    }
    printf("16bit distribution range = [%.2f,%.2f]\n", min_f/CYCLES,
	   max_f/CYCLES);
    if (min_f/CYCLES < 0.8 || max_f/CYCLES > 1.25) {
	printf("range seems excessive, failing\n");
	exit(1);
    }
    printf("ok\n");

    return 0;
}

/* vi:set ts=8 sts=4 sw=4: */
