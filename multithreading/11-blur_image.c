#include "multithreading.h"
#include <stdlib.h>

#define NUM_THREADS 8

/**
 * blur_image - Blur an entire image using multiple threads
 * @img_blur: Destination image (already allocated)
 * @img:      Source image
 * @kernel:   Convolution kernel
 */
void blur_image(img_t *img_blur, img_t const *img, kernel_t const *kernel)
{
	size_t i, strip_h, y_start;
	pthread_t threads[NUM_THREADS];
	blur_portion_t portions[NUM_THREADS];

	strip_h = img->h / NUM_THREADS;

	for (i = 0; i < NUM_THREADS; i++)
	{
		y_start = i * strip_h;
		portions[i].img = img;
		portions[i].img_blur = img_blur;
		portions[i].x = 0;
		portions[i].w = img->w;
		portions[i].y = y_start;
		if (i == NUM_THREADS - 1)
			portions[i].h = img->h - y_start;
		else
			portions[i].h = strip_h;
		portions[i].kernel = kernel;
		pthread_create(&threads[i], NULL,
			(void *(*)(void *))blur_portion, &portions[i]);
	}

	for (i = 0; i < NUM_THREADS; i++)
		pthread_join(threads[i], NULL);
}
