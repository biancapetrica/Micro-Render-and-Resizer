/* Petrica Bianca 333CB */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include "microrender.h"

int num_threads;
int resolution;
unsigned char** data;

void initialize(image *im) {
	// Creez o imagine de tip grayscale de dimensiuni resolution x resolution
	strcpy(im->type, "P5");
	im->width = resolution;
	im->height = resolution;
	im->maxVal = 255;

	// Aloc memorie pentru matricea de pixeli
	data = (unsigned char **)calloc(im->height, sizeof(unsigned char *));
	for (int i = 0; i < im->height; i++) 
		data[i] = (unsigned char*)calloc(im->width, sizeof(unsigned char));
}

void* threadFunction(void *var) {
	int thread_id = *(int*)var;
	int startRow, endRow;
	double sqr = 0, distance;
	int a = -1, b = 2, i, j;
	// Calculez sqrt(a^2 + b^2)
	sqr = sqrt(pow(a, 2) + pow(b,2));

	// Stabilesc limitele de executie pentru thread-ul curent in functie de thread_id
	startRow = thread_id * resolution / num_threads;
	endRow = (thread_id + 1) * resolution / num_threads;

	// Parcurg endRow-startRow linii din imagine in acest thread
	for (i = startRow; i < endRow; i++) 
		for(j = 0; j < resolution; j++) {
			/* 
			 * Calculez distanta scalata in functie de rezolutie a centrului 
			 * fiecarui pixel pana la linia data
			 */
			distance = (a * (j + 0.5) * 100) / resolution; // Pe OX
			distance += (b * (resolution - (i + 0.5)) * 100) / resolution; // Pe OY

			if(abs(distance) / sqr <= 3.0)
				data[i][j] = 0; // pixelul face parte din linie, deci este negru
			else data[i][j] = 255;  // pixelul este alb
		}

	return NULL;
}


void render(image *im) {
	pthread_t tid[num_threads];
	int thread_id[num_threads], i;

	// Initializez vectorul de thread_id 
	for(i = 0;i < num_threads; i++)
		thread_id[i] = i;

	// Creez num_threads thread-uri care vor executa functia threadFunction
	for(i = 0; i < num_threads; i++) 
		pthread_create(&(tid[i]), NULL, threadFunction, &(thread_id[i]));

	// Combin rezultatele obtinute 
	for(i = 0; i < num_threads; i++) 
		pthread_join(tid[i], NULL);
	
	// Copiez matricea obtinuta in poza
	im->data = data;
}

void writeData(const char * fileName, image *img) {
	FILE *fp = fopen(fileName, "wb");

	fwrite(img->type, sizeof(char), 2, fp);
	fprintf(fp, "\n%d %d\n", img->width, img->height);
	fprintf(fp, "%d\n", img->maxVal);

	// Scriu matricea pozei in fisier rand cu rand
	for (int i = 0; i < img->height; i++) {
		fwrite(img->data[i], sizeof(unsigned char), img->width, fp);
	}	

	fclose(fp);
}

