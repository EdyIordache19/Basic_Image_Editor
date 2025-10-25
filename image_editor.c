// Iordache Eduard-Stefanut, 314CA

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct {
	unsigned char r, g, b;
} pixel_t;

typedef struct {
	char type[2];
	int width, height, max_value;
	void **data; // Matricea pixelilor, de tip pixel_t (RGB), sau de tip hhu
} image_t;

// int alocari = 0;
// int eliberari = 0;

void **alloc_data(image_t *img)
{ // Alocarea matricei de pixeli ai imaginii
	// alocari++;
	if (!strcmp(img->type, "P2") || !strcmp(img->type, "P5")) {
		img->data = (void **)malloc(img->height * sizeof(void *));
		if (!img->data) {
			printf("Failed to malloc\n");
			exit(-1);
		}

		for (int i = 0; i < img->height; i++) {
			img->data[i] = (void *)malloc(img->width * sizeof(unsigned char));
			if (!img->data[i]) {
				printf("Failed to malloc\n");
				exit(-1);
			}
		}
	} else if (!strcmp(img->type, "P3") || !strcmp(img->type, "P6")) {
		img->data = (void **)malloc(img->height * sizeof(void *));
		if (!img->data) {
			printf("Failed to malloc\n");
			exit(-1);
		}

		for (int i = 0; i < img->height; i++) {
			img->data[i] = (void *)malloc(img->width * sizeof(pixel_t));

			if (!img->data[i]) {
				printf("Failed to malloc\n");
				exit(-1);
			}
		}
	}

	// Initializam zonele de memorie cu 0
	if (!strcmp(img->type, "P2") || !strcmp(img->type, "P5")) {
		for (int i = 0; i < img->height; i++) {
			memset(img->data[i], 0, img->width);
		}
	} else if (!strcmp(img->type, "P3") || !strcmp(img->type, "P6")) {
		for (int i = 0; i < img->height; i++) {
			memset(img->data[i], 0, img->width * sizeof(pixel_t));
		}
	}

	return img->data;
}

void free_data(image_t *img)
{ // Eliberarea matricei de pixeli ai imaginii
	// eliberari++;
	if (!strcmp(img->type, "P2") || !strcmp(img->type, "P5")) {
		for (int i = 0; i < img->height; i++) {
			if (((unsigned char **)img->data)[i]) {
				free(((unsigned char **)img->data)[i]);
			}
		}
		if (((unsigned char **)img->data)) {
			free(((unsigned char **)img->data));
		}
	} else if (!strcmp(img->type, "P3") || !strcmp(img->type, "P6")) {
		for (int i = 0; i < img->height; i++) {
			if (((pixel_t **)img->data)[i]) {
				free(((pixel_t **)img->data)[i]);
			}
		}
		if (((pixel_t **)img->data)) {
			free(((pixel_t **)img->data));
		}
	}
}

image_t load(char file_name[100])
{ // Incarcarea unei imagini din fisierul file_name
	image_t img;
	img.data = NULL;
	strcpy(img.type, "P2");
	img.height = 0;
	img.width = 0;

	FILE *fin = fopen(file_name, "rb");
	if (!fin) {
		printf("Failed to load %s\n", file_name);
		return img;
	}

	fscanf(fin, "%s", img.type);
	fscanf(fin, "%d %d %d", &img.width, &img.height, &img.max_value);

	img.data = alloc_data(&img);
	// printf("Alocare in functia %s\n", __FUNCTION__);

	if (!strcmp(img.type, "P2")) {
		// Imagine grayscale ASCII
		for (int i = 0; i < img.height; i++) {
			for (int j = 0; j < img.width; j++) {
				fscanf(fin, "%hhu", &((unsigned char **)img.data)[i][j]);
			}
		}
	} else if (!strcmp(img.type, "P5")) {
		// Imagine grayscale BIN
		fgetc(fin);
		for (int i = 0; i < img.height; i++) {
			fread(((unsigned char **)img.data)[i], 1, img.width, fin);
		}
	} else if (!strcmp(img.type, "P3")) {
		// Imagine RGB ASCII

		for (int i = 0; i < img.height; i++) {
			for (int j = 0; j < img.width; j++) {
				fscanf(fin, "%hhu %hhu %hhu",
					   &((pixel_t **)img.data)[i][j].r,
					   &((pixel_t **)img.data)[i][j].g,
					   &((pixel_t **)img.data)[i][j].b);
			}
		}
	} else if (!strcmp(img.type, "P6")) {
		// Imagine RGB BIN

		fgetc(fin);
		for (int i = 0; i < img.height; i++) {
			fread(((pixel_t **)img.data)[i], sizeof(pixel_t), img.width, fin);
		}
	}

	fclose(fin);
	printf("Loaded %s\n", file_name);
	return img;
}

void swap(int *a, int *b)
{ // Swap 2 numere
	int aux = *a;
	*a = *b;
	*b = aux;
}

int select_zone(int *x1, int *y1, int *x2, int *y2, image_t img)
{ // Selectarea unei zone din imagine
  // Se verifica daca coordonatele sunt valide
	if (*x1 > *x2) {
		swap(x1, x2);
	}
	if (*y1 > *y2) {
		swap(y1, y2);
	}

	if (*x1 < 0 || *x2 < 0 || *y1 < 0 || *y2 < 0) {
		printf("Invalid set of coordinates\n");
		return 0;
	} else if (*x1 > img.width || *x2 > img.width ||
			   *y1 > img.height || *y2 > img.height) {
		printf("Invalid set of coordinates\n");
		return 0;
	} else if (*x1 == *x2 || *y1 == *y2) {
		printf("Invalid set of coordinates\n");
		return 0;
	}

	printf("Selected %d %d %d %d\n", *x1, *y1, *x2, *y2);
	return 1;
}

int is_square(int x1, int y1, int x2, int y2)
{ // Verifica daca zona este patratica
	return abs(x2 - x1) == abs(y2 - y1);
}

void rotate_all(int angle, image_t *img)
{ // Roteste toata imaginea
	image_t new_img;
	strcpy(new_img.type, img->type);
	new_img.max_value = img->max_value;

	if (angle == 90 || angle == -270 || angle == 270 || angle == -90) {
		// Daca se roteste cu 90 sau 270 de grade, se schimba dimensiunile
		new_img.width = img->height;
		new_img.height = img->width;
	} else {
		new_img.width = img->width;
		new_img.height = img->height;
	}

	new_img.data = alloc_data(&new_img);
	// printf("Alocare in functia %s\n", __FUNCTION__);

	for (int x = 0; x < img->height; x++) {
		for (int y = 0; y < img->width; y++) {
			int new_x, new_y;
			if (angle == 90 || angle == -270) {
				new_x = y;
				new_y = img->height - x - 1;
			} else if (angle == 180 || angle == -180) {
				new_x = img->height - x - 1;
				new_y = img->width - y - 1;
			} else if (angle == 270 || angle == -90) {
				new_x = img->width - y - 1;
				new_y = x;
			}

			if (!strcmp(img->type, "P2") || !strcmp(img->type, "P5")) {
				((unsigned char **)new_img.data)[new_x][new_y] =
					((unsigned char **)img->data)[x][y];
			} else if (!strcmp(img->type, "P3") ||
						!strcmp(img->type, "P6")) {
				((pixel_t **)new_img.data)[new_x][new_y].r =
					((pixel_t **)img->data)[x][y].r;
				((pixel_t **)new_img.data)[new_x][new_y].g =
					((pixel_t **)img->data)[x][y].g;
				((pixel_t **)new_img.data)[new_x][new_y].b =
					((pixel_t **)img->data)[x][y].b;
			}
		}
	}

	if (angle == 90 || angle == -270 || angle == 270 || angle == -90) {
		free_data(img);
		// printf("Eliberare in functia %s\n", __FUNCTION__);
		img->width = new_img.width;
		img->height = new_img.height;
		alloc_data(img);
		// printf("Alocare in functia %s\n", __FUNCTION__);
	}

	for (int x = 0; x < img->height; x++) {
		for (int y = 0; y < img->width; y++) {
			if (!strcmp(img->type, "P2") || !strcmp(img->type, "P5")) {
				((unsigned char **)(img->data))[x][y] =
					((unsigned char **)new_img.data)[x][y];
			} else if (!strcmp(img->type, "P3") ||
						!strcmp(img->type, "P6")) {
				((pixel_t **)(img->data))[x][y] =
					((pixel_t **)new_img.data)[x][y];
			}
		}
	}

	free_data(&new_img);
	// printf("Eliberare in functia %s\n", __FUNCTION__);

	printf("Rotated %d\n", angle);
}

void rotate_selection(int angle, int x1, int y1, int x2, int y2, image_t *img)
{ // Se roteste doar selectia patratica din imagine
	image_t new_img;
	strcpy(new_img.type, img->type);
	new_img.max_value = img->max_value;

	int dim = y2 - y1;
	new_img.width = img->width;
	new_img.height = img->height;

	new_img.data = alloc_data(&new_img);
	// printf("Alocare in functia %s\n", __FUNCTION__);

	for (int x = y1; x < y2; x++) {
		for (int y = x1; y < x2; y++) {
			int x_rel = x - y1;
			int y_rel = y - x1;
			int new_x, new_y;

			if (angle == 90 || angle == -270) {
				new_x = y_rel;
				new_y = dim - x_rel - 1;
			} else if (angle == 180 || angle == -180) {
				new_x = dim - x_rel - 1;
				new_y = dim - y_rel - 1;
			} else if (angle == 270 || angle == -90) {
				new_x = dim - y_rel - 1;
				new_y = x_rel;
			}

			if (!strcmp(img->type, "P2") || !strcmp(img->type, "P5")) {
				// printf("%d %d %d %d\n", new_x, new_y, x, y);
				((unsigned char **)new_img.data)[new_x + y1][new_y + x1] =
					((unsigned char **)img->data)[x][y];
			} else if (!strcmp(img->type, "P3") ||
						!strcmp(img->type, "P6")) {
				((pixel_t **)new_img.data)[new_x + y1][new_y + x1].r =
					((pixel_t **)img->data)[x][y].r;
				((pixel_t **)new_img.data)[new_x + y1][new_y + x1].g =
					((pixel_t **)img->data)[x][y].g;
				((pixel_t **)new_img.data)[new_x + y1][new_y + x1].b =
					((pixel_t **)img->data)[x][y].b;
			}
		}
	}

	for (int x = y1; x < y2; x++) {
		for (int y = x1; y < x2; y++) {
			if (!strcmp(img->type, "P2") || !strcmp(img->type, "P5")) {
				((unsigned char **)(img->data))[x][y] =
					((unsigned char **)new_img.data)[x][y];
			} else if (!strcmp(img->type, "P3") ||
						!strcmp(img->type, "P6")) {
				((pixel_t **)(img->data))[x][y] =
					((pixel_t **)new_img.data)[x][y];
			}
		}
	}

	free_data(&new_img);
	// printf("Eliberare in functia %s\n", __FUNCTION__);

	printf("Rotated %d\n", angle);
}

void rotate(int angle, int selected,
			int x1, int y1, int x2, int y2, image_t *img)
{ // Se verifica daca unghiul este valid si daca selectia este patratica
  // Se apeleaza functia de rotire corespunzatoare
	if (angle % 90 != 0) {
		printf("Unsupported rotation angle\n");
		return;
	} else if (!is_square(x1, y1, x2, y2) && selected) {
		printf("The selection must be square\n");
		return;
	}

	if (angle == 0 || angle == 360 || angle == -360) {
		printf("Rotated %d\n", angle);
		return;
	}

	if (!selected) {
		rotate_all(angle, img);
	} else {
		rotate_selection(angle, x1, y1, x2, y2, img);
	}
}

void equalize(image_t *img)
{ // Functia de egalizare a imaginii
	if (!strcmp(img->type, "P3") || !strcmp(img->type, "P6")) {
		printf("Black and white image needed\n");
	} else {
		int histogram[256] = {0};

		for (int i = 0; i < img->height; i++) {
			for (int j = 0; j < img->width; j++) {
				int value = ((unsigned char **)img->data)[i][j];
				histogram[value]++;
			}
		}

		image_t new_img;
		strcpy(new_img.type, img->type);
		new_img.width = img->width;
		new_img.height = img->height;
		new_img.max_value = img->max_value;

		new_img.data = alloc_data(&new_img);
		// printf("Alocare in functia %s\n", __FUNCTION__);

		for (int i = 0; i < img->height; i++) {
			for (int j = 0; j < img->width; j++) {
				int value = ((unsigned char **)img->data)[i][j];
				int new_value = 0;

				// Se calculeaza suma aparaitiilor de la 0 la pixelul curent
				for (int k = 0; k <= value; k++) {
					new_value += histogram[k];
				}

				// Se calculeaza noua valoare egalizata
				new_value = (double)new_value /
							(img->height * img->width) * 255;

				// Funcita clamp
				if (new_value < 0) {
					new_value = 0;
				} else if (new_value > 255) {
					new_value = 255;
				}

				((unsigned char **)new_img.data)[i][j] = new_value;
			}
		}

		for (int i = 0; i < img->height; i++) {
			for (int j = 0; j < img->width; j++) {
				((unsigned char **)(img->data))[i][j] =
					((unsigned char **)new_img.data)[i][j];
			}
		}

		free_data(&new_img);
		// printf("Eliberare in functia %s\n", __FUNCTION__);
		printf("Equalize done\n");
	}
}

void crop(int *x1, int *y1, int *x2, int *y2, image_t *img)
{ // Fumctia de crop a imaginii
	image_t new_img;
	strcpy(new_img.type, img->type);
	new_img.width = *x2 - *x1;
	new_img.height = *y2 - *y1;
	new_img.max_value = img->max_value;
	new_img.data = alloc_data(&new_img);
	// printf("Alocare in functia %s\n", __FUNCTION__);

	for (int i = 0; i < new_img.height; i++) {
		for (int j = 0; j < new_img.width; j++) {
			if (!strcmp(img->type, "P2") || !strcmp(img->type, "P5")) {
				((unsigned char **)new_img.data)[i][j] =
					((unsigned char **)img->data)[*y1 + i][*x1 + j];
			} else if (!strcmp(img->type, "P3") || !strcmp(img->type, "P6")) {
				((pixel_t **)new_img.data)[i][j] =
					((pixel_t **)img->data)[*y1 + i][*x1 + j];
			}
		}
	}

	free_data(img);
	// printf("Eliberare in functia %s\n", __FUNCTION__);
	img->width = new_img.width;
	img->height = new_img.height;
	alloc_data(img);
	// printf("Alocare in functia %s\n", __FUNCTION__);

	*x1 = 0, *y1 = 0;
	*x2 = img->width, *y2 = img->height;

	for (int x = 0; x < new_img.height; x++) {
		for (int y = 0; y < new_img.width; y++) {
			if (!strcmp(img->type, "P2") || !strcmp(img->type, "P5")) {
				((unsigned char **)(img->data))[x][y] =
					((unsigned char **)new_img.data)[x][y];
			} else if (!strcmp(img->type, "P3") || !strcmp(img->type, "P6")) {
				((pixel_t **)(img->data))[x][y] =
					((pixel_t **)new_img.data)[x][y];
			}
		}
	}

	free_data(&new_img);
	// printf("Eliberare in functia %s\n", __FUNCTION__);
	printf("Image cropped\n");
}

double **alloc_kernel(double **kernel)
{ // Se aloca memorie pentru nucleu
	kernel = (double **)malloc(3 * sizeof(double *));
	if (!kernel) {
		printf("Failed to malloc\n");
		exit(-1);
	}

	for (int i = 0; i < 3; i++) {
		kernel[i] = (double *)malloc(3 * sizeof(double));
		if (!kernel[i]) {
			printf("Failed to malloc\n");
			exit(-1);
		}
	}

	return kernel;
}

void free_kernel(double **kernel)
{ // Se elibereaza memoria de la nucleu
	for (int i = 0; i < 3; i++) {
		if (kernel[i]) {
			free(kernel[i]);
		}
	}
	if (kernel) {
		free(kernel);
	}
}

void generate_kernel(char filter[100], double **kernel)
{ // Se genereaza nucleul corespunzator filtrului
	if (!strcmp(filter, "EDGE")) {
		kernel[0][0] = -1, kernel[0][1] = -1, kernel[0][2] = -1;
		kernel[1][0] = -1, kernel[1][1] =  8, kernel[1][2] = -1;
		kernel[2][0] = -1, kernel[2][1] = -1, kernel[2][2] = -1;
	} else if (!strcmp(filter, "SHARPEN")) {
		kernel[0][0] =  0, kernel[0][1] = -1, kernel[0][2] =  0;
		kernel[1][0] = -1, kernel[1][1] =  5, kernel[1][2] = -1;
		kernel[2][0] =  0, kernel[2][1] = -1, kernel[2][2] =  0;
	} else if (!strcmp(filter, "BLUR")) {
		kernel[0][0] =  1, kernel[0][1] =  1, kernel[0][2] =  1;
		kernel[1][0] =  1, kernel[1][1] =  1, kernel[1][2] =  1;
		kernel[2][0] =  1, kernel[2][1] =  1, kernel[2][2] =  1;

		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				kernel[i][j] /= 9;
			}
		}
	} else if (!strcmp(filter, "GAUSSIAN_BLUR")) {
		kernel[0][0] =  1, kernel[0][1] =  2, kernel[0][2] =  1;
		kernel[1][0] =  2, kernel[1][1] =  4, kernel[1][2] =  2;
		kernel[2][0] =  1, kernel[2][1] =  2, kernel[2][2] =  1;

		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				kernel[i][j] /= 16;
			}
		}
	}
}

void apply(char filter[100], int x1, int y1, int x2, int y2, image_t *img)
{ // Se aplica matricea kernel peste imaginea curenta, pentru selectie
  // Se realizeaza o convolutie
	if (!strcmp(img->type, "P2") || !strcmp(img->type, "P5")) {
		printf("Easy, Charlie Chaplin\n");
		return;
	} else if (strcmp(filter, "EDGE") && strcmp(filter, "SHARPEN") &&
			   strcmp(filter, "BLUR") && strcmp(filter, "GAUSSIAN_BLUR")) {
		printf("APPLY parameter invalid\n");
		return;
	}
	image_t new_img;
	strcpy(new_img.type, img->type);
	new_img.width = img->width;
	new_img.height = img->height;
	new_img.max_value = img->max_value;

	new_img.data = alloc_data(&new_img);
	// printf("Alocare in functia %s\n", __FUNCTION__);
	double **kernel = NULL;
	kernel = alloc_kernel(kernel);
	// printf("Alocare in functia %s\n", __FUNCTION__);
	generate_kernel(filter, kernel);

	for (int x = 0; x < img->height; x++) {
		for (int y = 0; y < img->width; y++) {
			((pixel_t **)new_img.data)[x][y] = ((pixel_t **)img->data)[x][y];
		}
	}

	double new_r, new_g, new_b;
	for (int x = y1; x < y2; x++) {
		for (int y = x1; y < x2; y++) {
			if (x > 0 && y > 0 && x < img->height - 1 && y < img->width - 1) {
				new_r = 0, new_g = 0, new_b = 0;
				for (int i = 0; i < 3; i++) {
					for (int j = 0; j < 3; j++) {
						int px = x + (i - 1);
						int py = y + (j - 1);

						pixel_t curr_pixel = ((pixel_t **)img->data)[px][py];

						new_r += curr_pixel.r * kernel[i][j];
						new_g += curr_pixel.g * kernel[i][j];
						new_b += curr_pixel.b * kernel[i][j];
					}
				}

				// Functia clamp
				new_r = round(new_r < 0 ? 0 : (new_r > 255 ? 255 : new_r));
				new_g = round(new_g < 0 ? 0 : (new_g > 255 ? 255 : new_g));
				new_b = round(new_b < 0 ? 0 : (new_b > 255 ? 255 : new_b));

				((pixel_t **)new_img.data)[x][y].r = new_r;
				((pixel_t **)new_img.data)[x][y].g = new_g;
				((pixel_t **)new_img.data)[x][y].b = new_b;
			}
		}
	}

	for (int x = 0; x < img->height; x++) {
		for (int y = 0; y < img->width; y++) {
			((pixel_t **)(img->data))[x][y] = ((pixel_t **)new_img.data)[x][y];
		}
	}

	free_kernel(kernel);
	// printf("Eliberare in functia %s\n", __FUNCTION__);
	free_data(&new_img);
	// printf("Eliberare in functia %s\n", __FUNCTION__);
	printf("APPLY %s done\n", filter);
}

void save_ascii(char file_out[100], image_t img)
{ // Functia de salvare in plain text
	FILE *fout = fopen(file_out, "w");
	if (!fout) {
		printf("Error saving in file %s", file_out);
		exit(1);
	}

	if (!strcmp(img.type, "P2") || !strcmp(img.type, "P5")) {
		strcpy(img.type, "P2");
	} else if (!strcmp(img.type, "P3") || !strcmp(img.type, "P6")) {
		strcpy(img.type, "P3");
	}

	fprintf(fout, "%s\n", img.type);
	fprintf(fout, "%d %d\n", img.width, img.height);
	fprintf(fout, "%d\n", img.max_value);
	for (int i = 0; i < img.height; i++) {
		for (int j = 0; j < img.width; j++) {
			if (!strcmp(img.type, "P2")) {
				fprintf(fout, "%d ", ((unsigned char **)img.data)[i][j]);
			} else if (!strcmp(img.type, "P3")) {
				fprintf(fout, "%d %d %d ",
						((pixel_t **)img.data)[i][j].r,
						((pixel_t **)img.data)[i][j].g,
						((pixel_t **)img.data)[i][j].b);
			}
		}
		fprintf(fout, "\n");
	}

	printf("Saved %s\n", file_out);
	fclose(fout);
}

void save_bin(char file_out[100], image_t img)
{ // Functia de salvare in binar
	FILE *fout = fopen(file_out, "wb");
	if (!fout) {
		printf("Error saving in file %s", file_out);
		exit(1);
	}

	if (!strcmp(img.type, "P2") || !strcmp(img.type, "P5")) {
		strcpy(img.type, "P5");
	} else if (!strcmp(img.type, "P3") || !strcmp(img.type, "P6")) {
		strcpy(img.type, "P6");
	}

	fprintf(fout, "%s\n", img.type);
	fprintf(fout, "%d %d\n", img.width, img.height);
	fprintf(fout, "%d\n", img.max_value);

	if (!strcmp(img.type, "P5")) {
		for (int i = 0; i < img.height; i++) {
			fwrite(((unsigned char **)img.data)[i],
				   sizeof(unsigned char), img.width, fout);
		}
	} else if (!strcmp(img.type, "P6")) {
		for (int i = 0; i < img.height; i++) {
			fwrite(((pixel_t **)img.data)[i], sizeof(pixel_t), img.width, fout);
		}
	}

	printf("Saved %s\n", file_out);
	fclose(fout);
}

void histogram(int max_stars, int bins, image_t img)
{ // Functia de calculare a histogramei
  // Se folosesc bins intervale egal distribuite, cu maxim dimensiune max_stars
	if (!strcmp(img.type, "P3") || !strcmp(img.type, "P6")) {
		printf("Black and white image needed\n");
	} else if (bins < 2 || bins > 256) {
		// Se verifica daca este intre 2 si 256
		printf("Invalid set of parameters\n");
	} else if ((bins & (bins - 1)) != 0) {
		// Se verifica daca este putere a lui 2
		printf("Invalid set of parameters\n");
	} else {
		double bin_size = (double)256 / bins; // Dimensiunea intervalului
		int histogram[256] = {0};
		int max_freq = -1;

		for (int i = 0; i < img.height; i++) {
			for (int j = 0; j < img.width; j++) {
				double value = ((unsigned char **)img.data)[i][j];

				int curr_bin = value / bin_size;
				histogram[curr_bin]++;

				if (histogram[curr_bin] > max_freq) {
					max_freq = histogram[curr_bin];
				}
			}
		}

		for (int i = 0; i < bins; i++) {
			int num_stars = histogram[i] / (double)max_freq * max_stars;

			printf("%d\t|\t", num_stars);
			for (int k = 0; k < num_stars; k++) {
				printf("*");
			}
			printf("\n");
		}
	}
}

void process_command(char command[100], int *selected, image_t *img,
					 int *x1, int *y1, int *x2, int *y2)
{ // Se proceseaza comanda citita de la tastatura cu ajutorul functiei sscanf
	char param1[100], param2[100];
	int angle, max_stars = 0, bins = 0, next_x1, next_y1, next_x2, next_y2;

	if (*x1 == 0 && *y1 == 0 && *x2 == img->width && *y2 == img->height) {
		*selected = 0;
	}

	if (sscanf(command, "LOAD %s", param1) == 1) {
		if (img->data) {
			free_data(img);
			// printf("Eliberare in functia %s\n", __FUNCTION__);
		}
		*img = load(param1);
		if (img->data) {
			*selected = 0;
			*x1 = *y1 = 0, *x2 = img->width, *y2 = img->height;
		}
	} else if (!img->data) {
		printf("No image loaded\n");
	} else if (sscanf(command, "SELECT %d %d %d %d",
					  &next_x1, &next_y1, &next_x2, &next_y2) == 4) {
		if (select_zone(&next_x1, &next_y1, &next_x2, &next_y2, *img)) {
			*selected = 1;
			*x1 = next_x1, *y1 = next_y1;
			*x2 = next_x2, *y2 = next_y2;
		}
	} else if (!strcmp(command, "SELECT ALL") ||
			   !strcmp(command, "SELECT ALL ")) {
		*x1 = 0, *y1 = 0;
		*x2 = img->width, *y2 = img->height;

		printf("Selected ALL\n");
	} else if (sscanf(command, "ROTATE %d", &angle) == 1) {
		rotate(angle, *selected, *x1, *y1, *x2, *y2, img);
	} else if (!strcmp(command, "EQUALIZE")) {
		equalize(img);
	} else if (!strcmp(command, "CROP")) {
		if (!*selected) {
			*x1 = 0;
			*y1 = 0;
			*x2 = img->width;
			*y2 = img->height;
		}
		crop(x1, y1, x2, y2, img);
		*selected = 0;
	} else if (sscanf(command, "APPLY %s", param1) == 1) {
		apply(param1, *x1, *y1, *x2, *y2, img);
	} else if (!strcmp(command, "APPLY")) {
		printf("Invalid command\n");
	} else if (sscanf(command, "SAVE %s %s", param1, param2) == 2) {
		if (!strcmp(param2, "ascii")) {
			save_ascii(param1, *img);
		}
	} else if (sscanf(command, "SAVE %s", param1) == 1) {
		save_bin(param1, *img);
	} else if (sscanf(command, "HISTOGRAM %d %d %s",
					  &max_stars, &bins, param1) == 2) {
		histogram(max_stars, bins, *img);
	} else if (sscanf(command, "HISTOGRAM %d %d %s",
			   &max_stars, &bins, param1) > 2 ||
			   !strcmp(command, "HISTOGRAM")) {
		printf("Invalid command\n");
	} else if (!strcmp(command, "EXIT")) {
		exit(0);
	} else {
		printf("Invalid command\n");
	}
}

void read_command(void)
{ // Se citesc toate comenzile de la tastatura, pana la EXIT
	int x1, y1, x2, y2, selected = 0;
	x1 = 0, x2 = 0, y1 = 0, y2 = 0;
	image_t img;
	img.data = NULL;
	strcpy(img.type, "P2");
	img.height = 0;
	img.width = 0;
	char command[100] = "";
	while (1) {
		fgets(command, 100, stdin);
		if (!strcmp(command, "EXIT")) {
			if (!img.data) {
				printf("No image loaded\n");
			}
			break;
		}

		command[strlen(command) - 1] = '\0';
		process_command(command, &selected, &img, &x1, &y1, &x2, &y2);
	}

	if (img.data) {
		free_data(&img);
		// printf("Eliberare in functia %s\n", __FUNCTION__);
	}
}

int main(void)
{
	read_command();
	// printf("Alocari: %d\n", alocari);
	// printf("Eliberari: %d\n", eliberari);

	return 0;
}
