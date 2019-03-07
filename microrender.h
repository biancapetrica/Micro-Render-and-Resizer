/* Petrica Bianca 333CB */

#ifndef HOMEWORK_H1
#define HOMEWORK_H1

typedef struct {
	char type[3];
	int width;
	int height;
	int maxVal;
	unsigned char** data;
}image;

void initialize(image *im);
void render(image *im);
void writeData(const char * fileName, image *img);

#endif /* HOMEWORK_H1 */