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
 *  $Id: ex1.c $
 */

#include <stdio.h>
#include <stdint.h>
#include <math.h>

#define SIZE 256

#include "gdither.h"

int main(int argc, char *argv[])
{
    GDither ds;
    float in[SIZE];	    /* test input data */
    double ind[SIZE];	    /* test input data */
    int32_t out32[SIZE];    /* test output data: 1 chan 32bit */
    int16_t out16[SIZE][4]; /* test output data: 4 chan 16bit */
    float outf[SIZE];	    /* test output data: 1 chan float */
    int i;

    /* --- Example 1a --- */

    /* create an arbitrary sinewave for testing */
    for (i=0; i<SIZE; i++) {
	ind[i] = sin(i);
	in[i] = ind[i];
    }

    /* create a dither object that produces 32bit ints, triangular dithered to
     * 24 bits */
    ds = gdither_new(GDitherTri, 1, GDither32bit, 24);

    /* run the dother algorithm over the input data */
    gdither_runf(ds, 0, SIZE, in, out32);

    /* free the dither object */
    gdither_free(ds);

    /* --- Example 1b --- */

    /* create a dither object to produce rounded (undithered) 4 channel
     * interleaved 16bit output */
    ds = gdither_new(GDitherNone, 4, GDither16bit, 16);
    gdither_runf(ds, 0, SIZE, in, out16);
    gdither_runf(ds, 1, SIZE, in, out16);
    gdither_runf(ds, 2, SIZE, in, out16);
    gdither_runf(ds, 3, SIZE, in, out16);
    gdither_free(ds);

    /* --- Example 1c --- */

    /* create a dither object to produce float output rectangular dithered to
     * 24 bits from double data */
    ds = gdither_new(GDitherRect, 4, GDitherFloat, 24);
    gdither_run(ds, 0, SIZE, ind, outf);
    gdither_free(ds);

    return 0;
}

/* vi:set ts=8 sts=4 sw=4: */
