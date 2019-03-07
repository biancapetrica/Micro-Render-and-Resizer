/* Petrica Bianca 333CB */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef HOMEWORK_H
#define HOMEWORK_H

typedef struct {
	char type[3];
	int width;
	int height;
	int maxVal;
	void** data;
}image;

typedef struct {
	unsigned char red,green,blue;
}pixel;

void readInput(const char * fileName, image *img);

void writeData(const char * fileName, image *img);

void resize(image *in, image * out);

#endif /* HOMEWORK_H */
