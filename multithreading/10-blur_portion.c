#include "multithreading.h"

/**
 * blur_portion - Blurs a portion of an image using a Gaussian Blur
 * @portion: Pointer to the data structure defining the portion to blur
 */
void blur_portion(blur_portion_t const *portion)
{
	size_t x, y, kx, ky, idx;
	ssize_t px, py, radius;
	float weight, weight_sum, r, g, b;

	if (!portion || !portion->img || !portion->img_blur || !portion->kernel)
		return;

	/* The radius is half the size of the kernel */
	radius = portion->kernel->size / 2;

	/* Loop through the specific portion of the image */
	for (y = portion->y; y < portion->y + portion->h; y++)
	{
		for (x = portion->x; x < portion->x + portion->w; x++)
		{
			weight_sum = r = g = b = 0.0;

			/* Apply the convolution kernel */
			for (ky = 0; ky < portion->kernel->size; ky++)
			{
				for (kx = 0; kx < portion->kernel->size; kx++)
				{
					/* Calculate absolute coordinates in the image */
					px = (ssize_t)x + (ssize_t)kx - radius;
					py = (ssize_t)y + (ssize_t)ky - radius;

					/* Only apply weight if the neighbor is inside image boundaries */
					if (px >= 0 && px < (ssize_t)portion->img->w &&
					    py >= 0 && py < (ssize_t)portion->img->h)
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

			/* Calculate final averaged pixel values */
			idx = y * portion->img->w + x;
			portion->img_blur->pixels[idx].r = (r / weight_sum);
			portion->img_blur->pixels[idx].g = (g / weight_sum);
			portion->img_blur->pixels[idx].b = (b / weight_sum);
		}
	}
}
