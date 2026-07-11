#include "multithreading.h"
#include <stdlib.h>
#include <string.h>

#define CLAMP(v, lo, hi) (((v) < (lo)) ? (lo) : ((v) > (hi) ? (hi) : (v)))

/**
 * kernel_weight_sum - Compute the sum of all elements in the kernel matrix
 * @kernel: Pointer to the kernel structure
 *
 * Return: Sum of matrix values
 */
static float kernel_weight_sum(kernel_t const *kernel)
{
	float sum = 0;
	size_t ky, kx;

	for (ky = 0; ky < kernel->size; ky++)
		for (kx = 0; kx < kernel->size; kx++)
			sum += kernel->matrix[ky][kx];
	return (sum);
}

/**
 * blur_pixel - Compute the blurred colour of a single pixel
 * @portion:    Portion parameters (source is always a temp copy)
 * @x:          X coordinate in the portion
 * @y:          Y coordinate in the portion
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
 *
 * Always works from a temporary copy of the source portion, so overlapping
 * source and destination buffers are handled safely.
 */
void blur_portion(blur_portion_t const *portion)
{
	float wsum = kernel_weight_sum(portion->kernel);
	size_t i, j, row;
	pixel_t *src_buf, *dst;
	blur_portion_t safe_portion;
	img_t src_copy;

	/* Allocate temporary copy of the source portion */
	src_buf = malloc(portion->w * portion->h * sizeof(pixel_t));
	if (!src_buf)
		return;

	/* Copy the source pixels into the temporary buffer */
	for (row = 0; row < portion->h; row++)
		memcpy(src_buf + row * portion->w,
		       portion->img->pixels +
			       (portion->y + row) * portion->img->w + portion->x,
		       portion->w * sizeof(pixel_t));

	/* Set up a safe portion structure pointing to the copy */
	src_copy.w = portion->w;
	src_copy.h = portion->h;
	src_copy.pixels = src_buf;
	safe_portion.img = &src_copy;
	safe_portion.kernel = portion->kernel;
	safe_portion.x = 0;
	safe_portion.y = 0;
	safe_portion.w = portion->w;
	safe_portion.h = portion->h;

	/* Blur from the copy into the destination */
	for (i = 0; i < safe_portion.h; i++)
	{
		dst = portion->img_blur->pixels +
		      (portion->y + i) * portion->img_blur->w + portion->x;
		for (j = 0; j < safe_portion.w; j++)
			blur_pixel(&safe_portion, j, i, wsum, &dst[j]);
	}

	free(src_buf);
}
