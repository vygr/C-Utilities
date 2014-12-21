/////////////////////////////////////////////////
//  Created by Chris Hinsley.
//  Copyright (C) 2008-2014. All rights reserved.
/////////////////////////////////////////////////

#include <math.h>

void resize(unsigned int *src, unsigned int src_w, unsigned int src_h, unsigned int *dest, unsigned int dest_w, unsigned int dest_h)
{
	static unsigned int *g_x1a = 0;
	static unsigned int g_x1a_w = 0;

	int weight_shift = 0;
	float source_texels_per_out_pixel = ((src_w / (float)dest_w + 1) * (src_h / (float)dest_h + 1));
	float weight_per_pixel = source_texels_per_out_pixel * 256 * 256;
	float accum_per_pixel = weight_per_pixel * 256;
	float weight_div = accum_per_pixel / 4294967000.0f;
	if (weight_div > 1)
	{
		weight_shift = (int)ceilf(logf((float)weight_div) / logf(2.0f));
	}
	if (weight_shift > 15) { weight_shift = 15; }

	float fw = 256 * src_w / (float)dest_w;
	float fh = 256 * src_h / (float)dest_h;

	if (g_x1a_w < dest_w)
	{
		if (g_x1a) { delete [] g_x1a; }
		g_x1a = new unsigned int[dest_w + 1];
		g_x1a_w = dest_w;
	}
	for (unsigned int x2 = 0; x2 < dest_w; x2++)
	{
		g_x1a[x2] = (unsigned int)(x2 * fw);
	}
	g_x1a[dest_w] = 256 * src_w;

	for (unsigned int y2 = 0; y2 < dest_h; y2++)
	{
		unsigned int y1a = (unsigned int)((y2) * fh);
		unsigned int y1b = (unsigned int)((y2 + 1) * fh) - 1;
		unsigned int y1c = y1a >> 8;
		unsigned int y1d = y1b >> 8;

		for (unsigned int x2 = 0; x2 < dest_w; x2++)
		{
			unsigned int x1a = g_x1a[x2];
			unsigned int x1b = g_x1a[x2 + 1] - 1;
			unsigned int x1c = x1a >> 8;
			unsigned int x1d = x1b >> 8;

			unsigned int a = 0, r = 0, g = 0, b = 0, q = 0;
			for (unsigned int y = y1c; y <= y1d; y++)
			{
				unsigned int weight_y = 256;
				if (y1c != y1d)
				{
					if (y == y1c)
					{
						weight_y -= (y1a & 0xFF);
					}
					else if (y == y1d)
					{
						weight_y = (y1b & 0xFF) + 1;
					}
				}

				unsigned int *dsrc = &src[y * src_w + x1c];
				for (unsigned int x = x1c; x <= x1d; x++)
				{
					unsigned int weight_x = 256;
					if (x1c != x1d)
					{
						if (x == x1c)
						{
							weight_x -= (x1a & 0xFF);
						}
						else if (x == x1d)
						{
							weight_x = (x1b & 0xFF) + 1;
						}
					}

					unsigned int c = *dsrc++;
					unsigned int r_src = (c) & 0xFF;
					unsigned int g_src = (c >> 8) & 0xFF;
					unsigned int b_src = (c >> 16) & 0xFF;
					unsigned int a_src = (c >> 24);
					unsigned int w = (weight_x * weight_y) >> weight_shift;
					r += r_src * w;
					g += g_src * w;
					b += b_src * w;
					a += a_src * w;
					q += w;
				}
			}

			unsigned int c = ((r / q)) | ((g / q) << 8) | ((b / q) << 16) | ((a / q) << 24);
			*dest++ = c;
		}
	}
}
