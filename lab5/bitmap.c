#include <stdio.h>
#include <stdlib.h>
#include "bitmap.h"


/*
 * Read in the location of the pixel array, the image width, and the image 
 * height in the given bitmap file.
 */
void read_bitmap_metadata(FILE *image, int *pixel_array_offset, int *width, int *height) {
	int error;
	//Pixel array
	fseek(image, 10, SEEK_SET);
	error = fread(pixel_array_offset, sizeof(int), 1, image);
    if (error != 1) {  
		fprintf(stderr, "Error: Offset could not be read.\n");
    }
	//Width
	fseek(image, 4, SEEK_CUR);
	error = fread(width, sizeof(int), 1, image);
    if (error != 1) {  
		fprintf(stderr, "Error: Width could not be read.\n");
    }
	//Height
	error = fread(height, sizeof(int), 1, image);
    if (error != 1) {  
		fprintf(stderr, "Error: Height could not be read.\n");
    }
}

/*
 * Read in pixel array by following these instructions:
 *
 * 1. First, allocate space for m `struct pixel *` values, where m is the 
 *    height of the image.  Each pointer will eventually point to one row of 
 *    pixel data.
 * 2. For each pointer you just allocated, initialize it to point to 
 *    heap-allocated space for an entire row of pixel data.
 * 3. Use the given file and pixel_array_offset to initialize the actual 
 *    struct pixel values. Assume that `sizeof(struct pixel) == 3`, which is 
 *    consistent with the bitmap file format.
 *    NOTE: We've tested this assumption on the Teaching Lab machines, but 
 *    if you're trying to work on your own computer, we strongly recommend 
 *    checking this assumption!
 * 4. Return the address of the first `struct pixel *` you initialized.
 */
struct pixel **read_pixel_array(FILE *image, int pixel_array_offset, int width, int height) {
	struct pixel **pixel_array = malloc(height * sizeof(struct pixel *));

	for (int i = 0; i < height; i++){
		pixel_array[i] = malloc(sizeof(struct pixel) * width);
	}

	fseek(image, pixel_array_offset, SEEK_SET);
	unsigned char blue;
	unsigned char green;
	unsigned char red;
	int error;
	
	for (int i = 0; i < height; i++){
		for (int j = 0; j < width; j++){
			//Read blue
			error = fread(&blue, sizeof(unsigned char), 1, image);
			if (error != 1) {  
				fprintf(stderr, "Error: Blue could not be read.\n");
			}
			//Read green
			error = fread(&green, sizeof(unsigned char), 1, image);
			if (error != 1) {  
				fprintf(stderr, "Error: Green could not be read.\n");
			}
			//Read red
			error = fread(&red, sizeof(unsigned char), 1, image);
			if (error != 1) {  
				fprintf(stderr, "Error: Red could not be read.\n");
			}
			pixel_array[i][j].blue = blue;
			pixel_array[i][j].green = green;
			pixel_array[i][j].red = red;
		}
	}
	
	return pixel_array;
}


/*
 * Print the blue, green, and red colour values of a pixel.
 * You don't need to change this function.
 */
void print_pixel(struct pixel p) {
    printf("(%u, %u, %u)\n", p.blue, p.green, p.red);
}
