#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include <png.h>
#include <omp.h>


#define WIDTH, 512 
#define HEIGHT 512
#define THREADS 2
#define QTD_ITERA 256

const double minX, maxX = -2.0, 2.0; 
const double minY, maxY = -2.0, 2.0;

double escalar_zreal, escalar_zimaginario, cor;

int x, y;

complex z;


typedef struct {
	double real;
	double imag;
} complex;

// Rotina para calcular e retornar uma cor para o conjunto
int calc_pixel(complex c) {

	int count;
	complex z;

	double temp, lengthsq;

	z.real, z.imag, count = 0, 0, 0;

	// Quando z for maior ou igual a 2 ou quando count alcançar um determinado valor máximo, que está sendo definido pelo QTD_ITERA ele para
	do {
		temp = z.real * z.real - z.imag * z.imag + c.real;
		z.imag = 2 * z.real * z.imag + c.imag;
		z.real = temp;
		lengthsq = z.real * z.real + z.imag * z.imag;
		count++;
	} while ((lengthsq < 4.0) && (count < QTD_ITERA));

	return count;
}


//Codigo da biblioteca LibPNG example
void setRGB(png_byte* ptr, double val)
{
	int v = (int)(val * 767);
	int offset = v % 256 * 2;

	if (v < 256) {
		ptr[0] = offset; ptr[1] = offset; ptr[2] = offset;
	}
	else if (v < 512) {
		ptr[0] = offset; ptr[1] = offset; ptr[2] = offset;
	}
	else {
		ptr[0] = 255 - offset; ptr[1] = 255 - offset; ptr[2] = 255 - offset;
	}
}


int writeImage(char* filename, int width, int height, double* buffer, char* title)
{
	int code = 0;
	FILE* fp = NULL;
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	png_bytep row = NULL;

	fp = fopen(filename, "wb");
	if (fp == NULL) {
		fprintf(stderr, "Could not open file %s for writing\n", filename);
		code = 1;
		goto finalise;
	}

	// Initialize write structure
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL) {
		fprintf(stderr, "Could not allocate write struct\n");
		code = 1;
		goto finalise;
	}

	// Initialize info structure
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		fprintf(stderr, "Could not allocate info struct\n");
		code = 1;
		goto finalise;
	}

	// Setup Exception handling
	if (setjmp(png_jmpbuf(png_ptr))) {
		fprintf(stderr, "Error during png creation\n");
		code = 1;
		goto finalise;
	}

	png_init_io(png_ptr, fp);

	// Write header (8 bit colour depth)
	png_set_IHDR(png_ptr, info_ptr, width, height,
		8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	// Set title
	if (title != NULL) {
		png_text title_text;
		title_text.compression = PNG_TEXT_COMPRESSION_NONE;
		title_text.key = "Title";
		title_text.text = title;
		png_set_text(png_ptr, info_ptr, &title_text, 1);
	}

	png_write_info(png_ptr, info_ptr);

	// Allocate memory for one row (3 bytes per pixel - RGB)
	row = (png_bytep)malloc(3 * width * sizeof(png_byte));

	// Write image data
	int x, y;
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			setRGB(&(row[x * 3]), buffer[y * width + x]);
		}
		png_write_row(png_ptr, row);
	}

	// End write
	png_write_end(png_ptr, NULL);

finalise:
	if (fp != NULL) fclose(fp);
	if (info_ptr != NULL) png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
	if (png_ptr != NULL) png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
	if (row != NULL) free(row);

	return code;
}


int main(int argc, char* argv[])
{
    escalar_zreal = (maxX - minX) / WIDTH;
    escalar_zimaginario = (maxY - minY) / HEIGHT;

    // Armazenar os valores dos pixels da imagem
    double* buffer = (double*)malloc(WIDTH * HEIGHT * sizeof(double));

#pragma omp parallel for default(shared)\ private(z, y, cor)\num_threads(THREADS)\ schedule(static,1)\

    for (x = 0; x < WIDTH; x++) {
        z.real = minX + ((double)x * escalar_zreal);
        for (y = 0; y < HEIGHT; y++) {
            z.imag = minY + ((double)y * escalar_zimaginario);
            cor = calc_pixel(z);
            buffer[y * WIDTH + x] = ((double)QTD_ITERA - cor) / (double)QTD_ITERA;
        }
    }
    //Gera a imagem png de acordo com os valores armazenados no buffer
    int png = writeImage(argv[1], WIDTH, HEIGHT, buffer, "img-mandelbrot");

    free(buffer);

    return png;
}