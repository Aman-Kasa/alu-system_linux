#include "multithreading.h"

/**
 * convolve_pixel - Applies the kernel to a single pixel
 * @portion: Pointer to the data structure
 * @x: X coordinate of the pixel
 * @y: Y coordinate of the pixel
 */
void convolve_pixel(blur_portion_t const *portion, size_t x, size_t y)
{
	size_t kx, ky, idx;
	long px, py, radius;
	float weight, weight_sum = 0.0, r = 0.0, g = 0.0, b = 0.0;

	radius = portion->kernel->size / 2;

	for (ky = 0; ky < portion->kernel->size; ky++)
	{
		for (kx = 0; kx < portion->kernel->size; kx++)
		{
			px = (long)x + (long)kx - radius;
			py = (long)y + (long)ky - radius;

			if (px >= 0 && px < (long)portion->img->w &&
			    py >= 0 && py < (long)portion->img->h)
			{
				weight = portion->kernel->matrix[ky][kx];
				idx = py * portion->img->w + px;

				r += portion->img->pixels[idx].r * weight;
				g += portion->img->pixels[idx].g * weight;
				b += portion->img->pixels[idx].b * weight;
				weight_sum += weight;
			}
		}
	}

	idx = y * portion->img->w + x;
	portion->img_blur->pixels[idx].r = (r / weight_sum);
	portion->img_blur->pixels[idx].g = (g / weight_sum);
	portion->img_blur->pixels[idx].b = (b / weight_sum);
}

/**
 * blur_portion - Blurs a portion of an image using a Gaussian Blur
 * @portion: Pointer to the data structure defining the portion to blur
 */
void blur_portion(blur_portion_t const *portion)
{
	size_t x, y;

	if (!portion || !portion->img || !portion->img_blur || !portion->kernel)
		return;

	for (y = portion->y; y < portion->y + portion->h; y++)
	{
		for (x = portion->x; x < portion->x + portion->w; x++)
		{
			convolve_pixel(portion, x, y);
		}
	}
}
