#include <math.h>
#include <stdint.h>

#include <gdither.h>

int main(int argc, char *argv[])
{
	float data[1024];
	int16_t out[1024];

	for (int i=0; i<1024; i++) {
		data[i] = sin(i);
	}

	GDither ds = gdither_new(GDitherNone, 1, GDither16bit, 0);
	gdither_runf(ds, 0, 1024, data, out);

	return 0;
}
