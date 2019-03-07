/* Petrica Bianca 333CB */

#include "ssaa.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

int num_threads;
int resize_factor;
int height, width;
int matrixGK[3][3] = {  { 1, 2, 1 },
						{ 2, 4, 2 },
						{ 1, 2, 1 } };
image* im;
pixel** dataColor;
unsigned char** dataGray;

void insertData(image* img, char* type, int height, int width, int maxVal, 
				void* data) {
	img->type[0] = type[0];
	img->type[1] = type[1];
	img->width = width;
	img->height = height;
	img->maxVal = maxVal;
	img->data = data;
}

void readInput(const char * fileName, image *img) {
	char type[3];
	int height, width, maxVal;
	
	FILE *fp = fopen(fileName, "rb");

    fscanf(fp, "%s\n%d %d\n", type, &width, &height);
    fscanf(fp, "%d\n", &maxVal);

    if (strcmp(type, "P5") == 0) {
		/* 
		 * Grayscale image 
    	 * Aloc memorie pentru o matrice de unsigned char si citesc informatia 
    	 * din fisier rand cu rand
    	 */
		unsigned char** data = (unsigned char **)calloc(height, 
								sizeof(unsigned char *));
    	for (int i = 0; i < height; i++) {
    		data[i] = (unsigned char*)calloc(width, sizeof(unsigned char));
    		fread(data[i], sizeof(unsigned char), width, fp);
    	}

    	// Copiez informatia din fisier in structura image* img
    	insertData(img, type, height, width, maxVal, data);

	} else if (strcmp(type, "P6") == 0) {
    	/* 
    	 * Color image 
    	 * Aloc memorie pentru o matrice de structuri pixel si citesc informatia 
    	 * din fisier rand cu rand
    	 */
    	pixel **data = (pixel **)malloc(height * sizeof(pixel *));
    	for (int i = 0; i < height; i++) {
    		data[i] = (pixel *)calloc(width, sizeof(pixel));
    		fread(data[i], sizeof(pixel), width, fp);
    	}

    	// Copiez informatia din fisier in structura image* img
		insertData(img, type, height, width, maxVal, data);
	}

	// Creez o copie globala a pozei
	im = (image*)malloc(sizeof(image));
	*im = *img;

	fclose(fp);
}

void writeData(const char * fileName, image *img) {
	FILE *fp = fopen(fileName, "wb");

	fwrite(img->type, sizeof(char), 2, fp);
	fprintf(fp, "\n%d %d\n", img->width, img->height);
	fprintf(fp, "%d\n", img->maxVal);

	// Scriu matricea pozei in fisier rand cu rand in functie de formatul pozei
	if (strncmp(img->type, "P5",2) == 0) {
		// Grayscale image 
		for (int i = 0; i < img->height; i++) 
    		fwrite(img->data[i], sizeof(unsigned char), img->width, fp);

	} else if (strncmp(img->type, "P6", 2) == 0) {
    	// Color image
    	for (int i = 0; i < img->height; i++) 
    		fwrite(img->data[i], sizeof(pixel), img->width, fp);
	}
	fclose(fp);
}

void* threadFunctionGray(void *var) {
	int startRow, endRow;
	int thread_id = *(int*)var;
	int i, j, k, l; 

	/*  
	 * Stabilesc limitele de parcurgere pentru thread-ul curent in functie de
	 * thread_id
	 */ 
	startRow = thread_id * height / num_threads;
	endRow = (thread_id + 1) * height / num_threads;

	if (resize_factor % 2 == 0) {
		int power = pow(resize_factor, 2);

		// Parcurg endRow-startRow linii din imaginea finala 
		for (i = startRow; i < endRow; i++) {
			for (j = 0; j < width; j++) {
				int temp = 0;
				/*
				 * Parcurgem patratul corespunzator pixelului (i,j) din matricea 
				 * imaginii originale si calculez media aritmetica a pixelilor
				 */
				for (k = i * resize_factor; k < (i + 1) * resize_factor; k++)
					for (l = j * resize_factor; l < (j + 1) * resize_factor; l++) 
						temp += ((unsigned char**)im->data)[k][l];
				
				dataGray[i][j] = temp / power;
			}
		}
	} else {
		// Parcurg endRow-startRow linii din imaginea finala 
		for (i = startRow; i < endRow; i++) {
			for (j = 0; j < width; j++) {
				int temp[3][3] = { {0} }, sum = 0;
				/*
				 * Parcurgem patratul corespunzator pixelului (i,j) din matricea 
				 * imaginii originale si il inmultesc cu valorile Kernel-ului
				 * Gaussian, element cu element
				 */
				for (k = i * resize_factor; k < (i + 1) * resize_factor; k++) 
					for (l = j * resize_factor; l < (j + 1) * resize_factor; l++) {
						temp[k % 3][l % 3] = ((unsigned char**)im->data)[k][l] 
											 * matrixGK[k % 3][l % 3];
						sum += temp[k % 3][l % 3];
					}
				// Impart suma la 16 pentru a se incadra in usigned char
				dataGray[i][j] = sum / 16;
			}
		}
	}
	return NULL;
}

void* threadFunctionColor(void *var) {
	int thread_id = *(int*)var;
	int startRow, endRow;
	int i, j, k, l; 

	/*  
	 * Stabilesc limitele de parcurgere pentru thread-ul curent in functie de
	 * thread_id
	 */ 
	startRow = thread_id * height / num_threads;
	endRow = (thread_id + 1) * height / num_threads;

	if (resize_factor % 2 == 0) {
		int power = pow(resize_factor, 2);
		// Parcurg endRow-startRow linii din imaginea finala 
		for (i = startRow; i < endRow; i++) {
			for (j = 0; j < width; j++) {
				int red = 0, green = 0, blue = 0;
				/*
				 * Parcurgem patratul corespunzator pixelului (i,j) din matricea 
				 * imaginii originale si calculez media aritmetica a pixelilor
				 */
				for (k = i * resize_factor; k < (i + 1) * resize_factor; k++) 
					for (l = j * resize_factor; l < (j + 1) * resize_factor; l++) {
						red += ((pixel**)im->data)[k][l].red;
						green += ((pixel**)im->data)[k][l].green;
						blue += ((pixel**)im->data)[k][l].blue;
					}
	
				dataColor[i][j].red = red / power;
				dataColor[i][j].green = green / power;
				dataColor[i][j].blue = blue / power;	
				
			}
		}
	} else {
		// Parcurg endRow-startRow linii din imaginea finala 
		for (i = startRow; i < endRow; i++) {
			for (j = 0; j < width; j++) {
				int red = 0, green = 0, blue = 0;
				/*
				 * Parcurgem patratul corespunzator pixelului (i,j) din matricea 
				 * imaginii originale si il inmultesc cu valorile Kernel-ului
				 * Gaussian, element cu element
				 */
				for (k = i * resize_factor; k < (i + 1) * resize_factor; k++)
					for (l = j * resize_factor; l < (j + 1) * resize_factor; l++) {
						red += ((pixel**)im->data)[k][l].red 
								* matrixGK[k % 3][l % 3];

						green += ((pixel**)im->data)[k][l].green 
								 * matrixGK[k % 3][l % 3];

						blue += ((pixel**)im->data)[k][l].blue 
								* matrixGK[k % 3][l % 3];	
					}
				// Impart sumele la 16 pentru a se incadra in usigned char
				dataColor[i][j].red = red / 16;
				dataColor[i][j].green = green / 16;
				dataColor[i][j].blue = blue / 16;	
			}
		}
	}
	return NULL;
}


void resize(image *in, image * out) { 
	pthread_t tid[num_threads];
	int thread_id[num_threads], i;

	// Modific height/width pentru a se imparti cu exactitate la resize_factor
	if (in->height % resize_factor != 0)
		in->height = in->height - in->height % resize_factor;
	if (in->width % resize_factor != 0)
		in->width = in->width - in->width % resize_factor;

	// Calculez height si width pentru poza micsoarata
	height = in->height / resize_factor;
	width = in->width / resize_factor;

	// Initializez vectorul de thread_id 
	for(i = 0;i < num_threads; i++)
		thread_id[i] = i;

	if (strncmp(in->type, "P5", 2) == 0) {
		// Grayscale image
		dataGray = (unsigned char **)calloc(height, sizeof(unsigned char *));
		for (i = 0; i < height; i++) 
			dataGray[i] = (unsigned char*)calloc(width, sizeof(unsigned char));

		/* 
		 * Creez num_threads thread-uri care vor executa functia 
		 * threadFunctionGray pentru a construi matricea pozei redimensionate
		 */
		for(i = 0; i < num_threads; i++) 
			pthread_create(&(tid[i]), NULL, threadFunctionGray, 
											&(thread_id[i]));

		// Combin rezultatele obtinute 
		for(i = 0; i < num_threads; i++) 
			pthread_join(tid[i], NULL);

		// Copiez matricea obtinuta in poza
		insertData(out, in->type, height, width, in->maxVal, dataGray);
	} else {
		// Color image
		dataColor = (pixel **)malloc(height * sizeof(pixel *));
		for (i = 0; i < height; i++) 
    		dataColor[i] = (pixel *)calloc(width, sizeof(pixel));

		/* 
		 * Creez num_threads thread-uri care vor executa functia 
		 * threadFunctionColor pentru a construi matricea pozei redimensionate
		 */
		for(i = 0; i < num_threads; i++) 
			pthread_create(&(tid[i]), NULL, threadFunctionColor, 
											&(thread_id[i]));

		// Combin rezultatele obtinute 
		for(i = 0; i < num_threads; i++) 
			pthread_join(tid[i], NULL);

		// Copiez matricea obtinuta in poza
		insertData(out, in->type, height, width, in->maxVal, dataColor);
	} 
}
