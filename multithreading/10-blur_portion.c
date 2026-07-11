#include "multithreading.h"

/**
 * blur_portion - Blurs a portion of an image using a Gaussian Blur
 * @portion: Pointer to the data structure defining the portion to blur
 */
void blur_portion(blur_portion_t const *portion)
{
	size_t x, y, kx, ky, idx;
	long px, py, radius; /* Changed from ssize_t to standard long */
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
					px = (long)x + (long)kx - radius;
					py = (long)y + (long)ky - radius;

					/* Only apply weight if neighbor is inside image boundaries */
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

			/* Calculate final averaged pixel values */
			idx = y * portion->img->w + x;
			portion->img_blur->pixels[idx].r = (r / weight_sum);
			portion->img_blur->pixels[idx].g = (g / weight_sum);
			portion->img_blur->pixels[idx].b = (b / weight_sum);
		}
	}
}
