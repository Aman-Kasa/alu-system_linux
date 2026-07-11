#include "multithreading.h"
#include <stdlib.h>
#include <string.h>

#define NUM_THREADS 8

/**
 * blur_image - Blur an entire image using multiple threads
 * @img_blur: Destination image (already allocated)
 * @img:      Source image
 * @kernel:   Convolution kernel
 */
void blur_image(img_t *img_blur, img_t const *img, kernel_t const *kernel)
{
	size_t i, strip_h, y_start, nb_pixels = img->w * img->h;
	pthread_t threads[NUM_THREADS];
	blur_portion_t portions[NUM_THREADS];
	img_t src_copy;

	/* Work on a temporary copy to allow safe in‑place blur */
	src_copy.w = img->w;
	src_copy.h = img->h;
	src_copy.pixels = malloc(nb_pixels * sizeof(pixel_t));
	if (!src_copy.pixels)
		return;
	memcpy(src_copy.pixels, img->pixels, nb_pixels * sizeof(pixel_t));

	strip_h = img->h / NUM_THREADS;
	for (i = 0; i < NUM_THREADS; i++)
	{
		y_start = i * strip_h;
		portions[i].img = &src_copy;
		portions[i].img_blur = img_blur;
		portions[i].x = 0;
		portions[i].w = img->w;
		portions[i].y = y_start;
		portions[i].h = (i == NUM_THREADS - 1) ?
			(img->h - y_start) : strip_h;
		portions[i].kernel = kernel;
		pthread_create(&threads[i], NULL,
			(void *(*)(void *))blur_portion, &portions[i]);
	}

	for (i = 0; i < NUM_THREADS; i++)
		pthread_join(threads[i], NULL);

	free(src_copy.pixels);
}
