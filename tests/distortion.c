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

void find_snr(GDitherType dt, const char *desc);
void find_res(GDitherType dt, const char *desc);

int main(int argc, char *argv[])
{
    int i;

    for (i=0; i<SIZE; i++) {
	sin_tbl[i] = sin((double)i * M_PI * 0.05) * 0.01;
	in[i] = (float)sin_tbl[i];
    }

    find_snr(GDitherNone, "no dithering");
    find_snr(GDitherRect, "rectangular dithering");
    find_snr(GDitherTri, "triangular dithering");
    find_snr(GDitherShaped, "noise-shaped dithering");

    printf("ok\n\n");

    find_res(GDitherNone, "no dithering");
    find_res(GDitherRect, "rectangular dithering");
    find_res(GDitherTri, "triangular dithering");
    find_res(GDitherShaped, "noise-shaped dithering");
    printf("ok\n");

    return 0;
}

void find_res(GDitherType dt, const char *desc)
{
    int i, j;
    GDither *ds = gdither_new(dt, 1, 16, 0);
    int16_t out16[SIZE];
    fftw_plan plan_rc;
    fftw_complex *freq = fftw_malloc(sizeof(fftw_complex) * (SIZE + 1));
    double amp[SIZE];
    double floor_peak = -1000.0;
    double floor_sum = 0.0;
    double floor_cnt = 0.0;
    double harm_peak = -1000.0;
    double harm_sum = 0.0;
    double harm_cnt = 0.0;
    int harmonic[SIZE];
    //char tmp[256];

    plan_rc = fftw_plan_dft_r2c_1d(SIZE, sinless_tbl, freq, FFTW_PRESERVE_INPUT);


    gdither_runf(ds, 0, SIZE, in, out16);

    for (i=0; i<SIZE; i++) {
	sinless_tbl[i] = ((double)out16[i] / 32768.0) - sin_tbl[i];
	sinless_tbl[i] *=  -0.5 * cos(2.0f * M_PI * (float) i /
                                 (float) SIZE) + 0.5;
    }
   
    fftw_execute(plan_rc); 

    /* divide bins into ones that are where harmonic peaks could be
       and ones that aren't */
    for (i=0; i<SIZE/2; i++) {
	    harmonic[i] = 0;
    }
    for (i=1; i<20; i++) {
	for (j=-4; j<5; j++) {
	    harmonic[i*SIZE/40 + j] = 1;
	}
    }

    for (i=2; i<SIZE/2; i++) {
	amp[i] = 20.0 * log10(cabs(freq[i]));
	if (harmonic[i]) {
	    if (amp[i] > harm_peak) harm_peak = amp[i];
	    if (1 || amp[i] > -100.0) {
		harm_sum += amp[i];
		harm_cnt++;
	    }
	} else {
	    if (amp[i] > floor_peak) floor_peak = amp[i];
	    floor_sum += amp[i];
	    floor_cnt++;
	}

	//sprintf(tmp, "%f\t%g\n", 48000.0 / (double)SIZE * (double)i, amp[i]);
	//write(5, tmp, strlen(tmp));
	//printf("%d\t%g\n", i, amp_db);
    }
    printf("%s mean harmonic resonace = %.1fdB\n", desc,
	    harm_sum / harm_cnt - floor_sum / floor_cnt);
    printf("%s peak harmonic resonance = %.1fdB\n", desc,
	   harm_peak - floor_peak);
    if (dt != GDitherNone && harm_sum / harm_cnt - floor_sum / floor_cnt > 6.0) {
	printf("excessive resonance peaks, failing\n");
	exit(1);
    }
    if (dt != GDitherNone && harm_peak - floor_peak > 6.0) {
	printf("excessive resonance peaks, failing\n");
	exit(1);
    }
}

void find_snr(GDitherType dt, const char *desc)
{
    GDither *ds = gdither_new(dt, 1, GDither32bit, 24);
    int i;
    double sum_sq = 0.0;
    double val;

    gdither_runf(ds, 0, SIZE, in, out);
    for (i=0; i<SIZE; i++) {
	sinless_tbl[i] = ((double)out[i] / 2147483648.0) - sin_tbl[i];
	sum_sq += sinless_tbl[i] * sinless_tbl[i];
//printf("%g\n", sinless_tbl[i]);
    }
    val = 20.0 * log10(sqrt(sum_sq / (double)SIZE));
    printf("RMS SNR for %s = %.0fdB\n", desc, val);

    switch (dt) {
    case GDitherNone:
	break;
    case GDitherRect:
	if (val > -141.0) {
	    printf("excessive noise, failed\n");
	    exit(1);
	}
	break;
    case GDitherTri:
	if (val > -144.0) {
	    printf("excessive noise, failed\n");
	    exit(1);
	}
	break;
    case GDitherShaped:
	if (val > -130.0) {
	    printf("excessive noise, failed\n");
	    exit(1);
	}
	break;
    }
}

/* vi:set ts=8 sts=4 sw=4: */
