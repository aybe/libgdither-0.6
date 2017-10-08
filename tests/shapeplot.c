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
 *  $Id: distortion-test.c $
 */

#include <unistd.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <complex.h>
#include <fftw3.h>

#include "gdither.h"

#define SIZE 16384

#ifndef M_PI
#define M_PI 3.1415926
#endif

float in[SIZE];
int32_t out[SIZE];
double sin_tbl[SIZE];
double sinless_tbl[SIZE];

void mkdata(GDitherType dt, const char *desc);

int main(int argc, char *argv[])
{
    int i;

    for (i=0; i<SIZE; i++) {
	sin_tbl[i] = sin((double)i * M_PI * 0.05) * 0.01;
	in[i] = (float)sin_tbl[i];
    }

    //mkdata(GDitherNone, "no dithering");
    //mkdata(GDitherRect, "rectangular dithering");
    //mkdata(GDitherTri, "triangular dithering");
    mkdata(GDitherShaped, "noise-shaped dithering");

    return 0;
}

void mkdata(GDitherType dt, const char *desc)
{
    int i;
    GDither *ds = gdither_new(dt, 1, 16, 0);
    int16_t out16[SIZE];
    fftw_plan plan_rc;
    fftw_complex *freq = fftw_malloc(sizeof(fftw_complex) * (SIZE + 1));
    double amp;

    plan_rc = fftw_plan_dft_r2c_1d(SIZE, sinless_tbl, freq, FFTW_PRESERVE_INPUT);

    gdither_runf(ds, 0, SIZE, in, out16);

    for (i=0; i<SIZE; i++) {
	sinless_tbl[i] = ((double)out16[i] / 32768.0) - sin_tbl[i];
	sinless_tbl[i] *=  -0.5 * cos(2.0f * M_PI * (float) i /
                                 (float) SIZE) + 0.5;
    }
   
    fftw_execute(plan_rc); 

    for (i=2; i<SIZE/2; i++) {
	amp = 20.0 * log10(cabs(freq[i]));

	printf("%f\t%g\n", 48000.0 / (double)SIZE * (double)i, amp);
    }
}

/* vi:set ts=8 sts=4 sw=4: */
