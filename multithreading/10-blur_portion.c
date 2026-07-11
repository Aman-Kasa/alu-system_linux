#include "multithreading.h"
#include <stdio.h>

#define CLAMP(v, lo, hi) (((v) < (lo)) ? (lo) : ((v) > (hi) ? (hi) : (v)))

/**
 * blur_pixel - Compute blurred colour of a single pixel
 * @portion:    Portion parameters
 * @x:          X coordinate
 * @y:          Y coordinate
 * @weight_sum: Precomputed kernel weight sum
 * Return: Blurred pixel
 */
static pixel_t blur_pixel(blur_portion_t const *portion, size_t x, size_t y,
			   float weight_sum)
{
	int half = (int)portion->kernel->size / 2;
	float kr = 0, kg = 0, kb = 0;
	size_t ky, kx;
	int ix, iy;
	pixel_t src;

	for (ky = 0; ky < portion->kernel->size; ky++)
	{
		for (kx = 0; kx < portion->kernel->size; kx++)
		{
			ix = (int)x + (int)kx - half;
			iy = (int)y + (int)ky - half;
			ix = CLAMP(ix, 0, (int)portion->img->w - 1);
			iy = CLAMP(iy, 0, (int)portion->img->h - 1);
			src = portion->img->pixels[iy * portion->img->w + ix];
			kr += src.r * portion->kernel->matrix[ky][kx];
			kg += src.g * portion->kernel->matrix[ky][kx];
			kb += src.b * portion->kernel->matrix[ky][kx];
		}
	}
	kr /= weight_sum;
	kg /= weight_sum;
	kb /= weight_sum;
	return ((pixel_t){
		.r = (uint8_t)CLAMP(kr, 0, 255),
		.g = (uint8_t)CLAMP(kg, 0, 255),
		.b = (uint8_t)CLAMP(kb, 0, 255)
	});
}

/**
 * blur_portion - Blur a rectangular portion of an image using a kernel
 * @portion: Parameters describing the image portion and kernel
 */
void blur_portion(blur_portion_t const *portion)
{
	size_t i, j, ky, kx;
	float weight_sum = 0.0f;

	for (ky = 0; ky < portion->kernel->size; ky++)
		for (kx = 0; kx < portion->kernel->size; kx++)
			weight_sum += portion->kernel->matrix[ky][kx];

	for (i = portion->y; i < portion->y + portion->h; i++)
		for (j = portion->x; j < portion->x + portion->w; j++)
			portion->img_blur->pixels[i * portion->img_blur->w + j] =
				blur_pixel(portion, j, i, weight_sum);
}
