#include "multithreading.h"
#include <stdlib.h>
#include <string.h>

#define CLAMP(v, lo, hi) (((v) < (lo)) ? (lo) : ((v) > (hi) ? (hi) : (v)))

/**
 * blur_pixel - Compute the blurred colour of a single pixel
 * @portion:    Portion parameters
 * @x:          X coordinate
 * @y:          Y coordinate
 * @weight_sum: Precomputed kernel weight sum
 * @dest:       Pointer to store the resulting pixel
 */
static void blur_pixel(blur_portion_t const *portion,
		       size_t x, size_t y,
		       float weight_sum,
		       pixel_t *dest)
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
	dest->r = (uint8_t)CLAMP(kr, 0, 255);
	dest->g = (uint8_t)CLAMP(kg, 0, 255);
	dest->b = (uint8_t)CLAMP(kb, 0, 255);
}

/**
 * blur_portion - Blur a rectangular portion of an image using a kernel
 * @portion: Parameters describing the image portion and kernel
 */
void blur_portion(blur_portion_t const *portion)
{
	size_t i, j, ky, kx;
	float weight_sum = 0.0f;
	pixel_t *temp_buf = NULL;
	blur_portion_t local_portion = *portion;
	img_t src_copy;

	/* Compute kernel weight sum for normalisation */
	for (ky = 0; ky < portion->kernel->size; ky++)
		for (kx = 0; kx < portion->kernel->size; kx++)
			weight_sum += portion->kernel->matrix[ky][kx];

	/* Handle in‑place blur: work on a temporary copy of the source portion */
	if (portion->img->pixels == portion->img_blur->pixels)
	{
		temp_buf = malloc(portion->w * portion->h * sizeof(pixel_t));
		if (!temp_buf)
			return;
		for (i = 0; i < portion->h; i++)
			memcpy(temp_buf + i * portion->w,
			       portion->img->pixels +
				       (portion->y + i) * portion->img->w + portion->x,
			       portion->w * sizeof(pixel_t));
		src_copy.w = portion->w;
		src_copy.h = portion->h;
		src_copy.pixels = temp_buf;
		local_portion.img = &src_copy;
		local_portion.x = 0;
		local_portion.y = 0;
	}

	for (i = local_portion.y; i < local_portion.y + local_portion.h; i++)
	{
		for (j = local_portion.x; j < local_portion.x + local_portion.w; j++)
		{
			pixel_t dst;

			blur_pixel(&local_portion, j, i, weight_sum, &dst);
			portion->img_blur->pixels[i * portion->img_blur->w + j] = dst;
		}
	}

	free(temp_buf);
}
