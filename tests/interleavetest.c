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
 *  $Id: interleavetest.c $
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>

#include "gdither.h"

#define SIZE 20
#define IL   5

int main(int argc, char *argv[])
{
    float in[SIZE];
    int16_t out16[SIZE * IL];
    int32_t out32[SIZE * IL];
    int i, j;
    GDither d16 = gdither_new(GDitherNone, IL, GDither16bit, 16);
    GDither d32 = gdither_new(GDitherNone, IL, GDither32bit, 16);

    printf("checking interleaving\n");

    for (j=0; j<IL; j++) {
	for (i=0; i<SIZE; i++) {
	    in[i] = (float)j / 32768.0f;
	}
	gdither_runf(d16, j, SIZE, in, out16);
	gdither_runf(d32, j, SIZE, in, out32);
    }
    for (i=0; i<SIZE*IL; i++) {
	if (i % IL != out16[i]) {
	    fprintf(stderr, "16bit interleaving error, sample %d is %d, "
		    "should be %d\n", i, out16[i], i % IL);
	    fprintf(stderr, "failed\n");
	    exit(1);
	}
	if ((i % IL) * 65536 != out32[i]) {
	    fprintf(stderr, "32bit interleaving error, sample %d is %d, "
		    "should be %d\n", i, out32[i], (i % IL) * 65536);
	    fprintf(stderr, "failed\n");
	    exit(1);
	}
    }

    printf("ok\n");

    return 0;
}

/* vi:set ts=8 sts=4 sw=4: */
