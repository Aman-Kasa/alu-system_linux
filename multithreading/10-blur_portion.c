#include "multithreading.h"
#include <stdio.h>

#define CLAMP(v, lo, hi) (((v) < (lo)) ? (lo) : ((v) > (hi) ? (hi) : (v)))

/**
 * blur_portion - Blur a rectangular portion of an image using a kernel
 * @portion: Parameters describing the image portion and kernel
 */
void blur_portion(blur_portion_t const *portion)
{
	size_t i, j, ky, kx;
	int ix, iy;
	float kr, kg, kb;
	pixel_t src, dst;
	int half = (int)portion->kernel->size / 2;

	for (i = portion->y; i < portion->y + portion->h; i++)
	{
		for (j = portion->x; j < portion->x + portion->w; j++)
		{
			kr = 0;
			kg = 0;
			kb = 0;

			for (ky = 0; ky < portion->kernel->size; ky++)
			{
				for (kx = 0; kx < portion->kernel->size; kx++)
				{
					ix = (int)j + (int)kx - half;
					iy = (int)i + (int)ky - half;

					/* clamp to image borders */
					ix = CLAMP(ix, 0, (int)portion->img->w - 1);
					iy = CLAMP(iy, 0, (int)portion->img->h - 1);

					src = portion->img->pixels[iy * portion->img->w + ix];
					kr += src.r * portion->kernel->matrix[ky][kx];
					kg += src.g * portion->kernel->matrix[ky][kx];
					kb += src.b * portion->kernel->matrix[ky][kx];
				}
			}

			dst.r = (uint8_t)CLAMP(kr, 0, 255);
			dst.g = (uint8_t)CLAMP(kg, 0, 255);
			dst.b = (uint8_t)CLAMP(kb, 0, 255);
			portion->img_blur->pixels[i * portion->img_blur->w + j] = dst;
		}
	}
}
