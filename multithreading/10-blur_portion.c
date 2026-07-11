#include "multithreading.h"
#include <stdlib.h>
#include <string.h>

#define CLAMP(v, lo, hi) (((v) < (lo)) ? (lo) : ((v) > (hi) ? (hi) : (v)))

/**
 * kernel_weight_sum - Compute the sum of all elements in the kernel matrix
 * @kernel: Pointer to the kernel structure
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
 * setup_inplace_copy - Prepare a temporary copy of the source portion
 * @local:    Local portion structure to modify (will point to the copy)
 * @portion:  Original portion (where src and dest pixel arrays overlap)
 * @copy_dest: Pre‑allocated img_t structure to hold the copy metadata
 * Return: Pointer to the allocated pixel buffer, or NULL on failure
 */
static pixel_t *setup_inplace_copy(blur_portion_t *local,
				   blur_portion_t const *portion,
				   img_t *copy_dest)
{
	size_t i;
	pixel_t *buf;

	buf = malloc(portion->w * portion->h * sizeof(pixel_t));
	if (!buf)
		return (NULL);

	for (i = 0; i < portion->h; i++)
		memcpy(buf + i * portion->w,
		       portion->img->pixels +
			       (portion->y + i) * portion->img->w + portion->x,
		       portion->w * sizeof(pixel_t));

	copy_dest->w = portion->w;
	copy_dest->h = portion->h;
	copy_dest->pixels = buf;
	local->img = copy_dest;
	local->x = 0;
	local->y = 0;

	return (buf);
}

/**
 * blur_portion - Blur a rectangular portion of an image using a kernel
 * @portion: Parameters describing the image portion and kernel
 */
void blur_portion(blur_portion_t const *portion)
{
	blur_portion_t local = *portion;
	float wsum = kernel_weight_sum(portion->kernel);
	pixel_t *temp_buf = NULL;
	img_t src_copy;
	size_t i, j;

	if (portion->img->pixels == portion->img_blur->pixels)
	{
		temp_buf = setup_inplace_copy(&local, portion, &src_copy);
		if (!temp_buf)
			return;
	}

	for (i = local.y; i < local.y + local.h; i++)
	{
		for (j = local.x; j < local.x + local.w; j++)
		{
			pixel_t dst;

			blur_pixel(&local, j, i, wsum, &dst);
			portion->img_blur->pixels[i * portion->img_blur->w + j] = dst;
		}
	}
	free(temp_buf);
}
