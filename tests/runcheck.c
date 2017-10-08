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
#include <math.h>

#define SIZE 1024

#include "gdither.h"
#include "compare.h"
#include "gettime.h"

inline static float noise()
{
    static unsigned long rnd = 23232323;
    rnd = (rnd * 196314165) + 907633515;

    return rnd * 2.3283064365387e-10f;
}

int main(int argc, char *argv[])
{
    GDither ds;
    float in[SIZE];
    double ind[SIZE];
    int32_t out32[SIZE];
    int16_t out16[SIZE];
    float outf[SIZE];
    int i;

    for (i=0; i<SIZE; i++) {
	in[i] = ((i % 20) - 10) * 0.2f;
    }

    ds = gdither_new(GDitherNone, 1, GDither32bit, 24);
    if (!ds) {
	fprintf(stderr, "counld not get gdither object for 24 in 32 dither\n");
	fprintf(stderr, "failing\n");
	exit(1);
    }

    gdither_runf(ds, 0, SIZE, in, out32);
#if SIZE == 1024
    for (i=0; i<SIZE; i++) {
	if (out32[i] != compare[i]) {
	    fprintf(stderr, "result comparison failed, index %d\n   %d != %d\n",
		    i, out32[i], compare[i]);
	}
    }
#endif
    gdither_free(ds);

    ds = gdither_new(GDitherNone, 1, GDitherFloat, 8);
    for (i=0; i<SIZE; i++) {
	in[i] = noise() - 0.5f;
	ind[i] = in[i];
    }
    gdither_runf(ds, 0, SIZE, in, outf);
    for (i=0; i<SIZE; i++) {
	if (outf[i] * 128.0f - nearbyintf(outf[i] * 128.0f) > 0.00001f) {
	    fprintf(stderr, "bad float rounding: %d\t%f\n", i,
		    outf[i] * 128.0f);
	    fprintf(stderr, "failing\n");
	    exit(1);
	}
    }
    gdither_free(ds);

    /* Warm up the buffers and so on */
    ds = gdither_new(GDitherNone, 1, 23, 0);
    gdither_runf(ds, 0, SIZE, in, out32);

    TIME("baseline 24 in 32 test, 1024 samples",
         gdither_runf(ds, 0, SIZE, in, out32));
    gdither_free(ds);

    printf("\nOptimised:\n");


    ds = gdither_new(GDitherNone, 1, GDither32bit, 24);
    TIME("GDitherNone, 1024 samples, 24 in 32bit",
         gdither_runf(ds, 0, SIZE, in, out32));
    gdither_free(ds);

    ds = gdither_new(GDitherRect, 1, GDither32bit, 24);
    TIME("GDitherRect, 1024 samples, 24 in 32bit",
         gdither_runf(ds, 0, SIZE, in, out32));
    gdither_free(ds);

    ds = gdither_new(GDitherTri, 1, GDither32bit, 24);
    TIME("GDitherTri, 1024 samples, 24 in 32bit",
         gdither_runf(ds, 0, SIZE, in, out32));
    gdither_free(ds);

    ds = gdither_new(GDitherShaped, 1, GDither32bit, 24);
    TIME("GDitherShaped, 1024 samples, 24 in 32bit",
         gdither_runf(ds, 0, SIZE, in, out32));
    gdither_free(ds);

    printf("\n");

    ds = gdither_new(GDitherNone, 1, GDither16bit, 0);
    TIME("GDitherNone, 1024 samples, 16bit",
         gdither_runf(ds, 0, SIZE, in, out16));
    gdither_free(ds);

    ds = gdither_new(GDitherRect, 1, GDither16bit, 0);
    TIME("GDitherRect, 1024 samples, 16bit",
         gdither_runf(ds, 0, SIZE, in, out16));
    gdither_free(ds);

    ds = gdither_new(GDitherTri, 1, GDither16bit, 0);
    TIME("GDitherTri, 1024 samples, 16bit",
         gdither_runf(ds, 0, SIZE, in, out16));
    gdither_free(ds);

    ds = gdither_new(GDitherShaped, 1, GDither16bit, 0);
    TIME("GDitherShaped, 1024 samples, 16bit",
         gdither_runf(ds, 0, SIZE, in, out16));
    gdither_free(ds);

    printf("\nUnoptimised:\n");

    ds = gdither_new(GDitherNone, 1, GDither32bit, 20);
    TIME("GDitherNone, 1024 samples, 20 in 32bit",
         gdither_runf(ds, 0, SIZE, in, out32));
    gdither_free(ds);

    ds = gdither_new(GDitherRect, 1, GDither32bit, 20);
    TIME("GDitherRect, 1024 samples, 20 in 32bit",
         gdither_runf(ds, 0, SIZE, in, out32));
    gdither_free(ds);

    ds = gdither_new(GDitherTri, 1, GDither32bit, 20);
    TIME("GDitherTri, 1024 samples, 20 in 32bit",
         gdither_runf(ds, 0, SIZE, in, out32));
    gdither_free(ds);

    ds = gdither_new(GDitherShaped, 1, GDither32bit, 20);
    TIME("GDitherShaped, 1024 samples, 20 in 32bit",
         gdither_runf(ds, 0, SIZE, in, out32));
    gdither_free(ds);

    printf("\n");

    ds = gdither_new(GDitherNone, 1, GDitherFloat, 20);
    TIME("GDitherNone, 1024 samples, 24 in float",
         gdither_runf(ds, 0, SIZE, in, outf));
    gdither_free(ds);

    ds = gdither_new(GDitherRect, 1, GDitherFloat, 20);
    TIME("GDitherRect, 1024 samples, 24 in float",
         gdither_runf(ds, 0, SIZE, in, outf));
    gdither_free(ds);

    ds = gdither_new(GDitherTri, 1, GDitherFloat, 20);
    TIME("GDitherTri, 1024 samples, 24 in float",
         gdither_runf(ds, 0, SIZE, in, outf));
    gdither_free(ds);

    ds = gdither_new(GDitherShaped, 1, GDitherFloat, 20);
    TIME("GDitherShaped, 1024 samples, 24 in float",
         gdither_runf(ds, 0, SIZE, in, outf));
    gdither_free(ds);

    printf("\n");

    ds = gdither_new(GDitherTri, 1, GDitherFloat, 20);
    TIME("GDitherTri, 1024 samples, 24 in float, double input",
         gdither_run(ds, 0, SIZE, ind, outf));
    gdither_free(ds);

    return 0;
}

/* vi:set ts=8 sts=4 sw=4: */
