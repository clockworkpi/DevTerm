/*
 * ALGO     : PROJ. : Volume
 * RESEARCH : File  : space.c
 *          : Date  : 20100531.0725UTC
 *          : Email : mail@algoresearch.net
 */

#include "main.h"
#include "space.h"

int adds(SYS_TABLE *sys, int x, int y, int z)
{
	if (x < 0) x += sys->space_w;
	if (y < 0) y += sys->space_h;
	if (z < 0) z += sys->space_d;
	if (x >= sys->space_w) x -= sys->space_w;
	if (y >= sys->space_h) y -= sys->space_h;
	if (z >= sys->space_d) z -= sys->space_d;

	return sys->size_plane * z + sys->space_h * y + x;
}

void cell_overlap(SYS_TABLE *sys, int v, int x, int y, int z)
{

}

void cell_set(SYS_TABLE *sys, int v, int x, int y, int z)
{
	sys->space_i[adds(sys,x,y,z)] = v;
}

int cell_get(SYS_TABLE *sys, int x, int y, int z)
{
	return sys->space_i[adds(sys, x, y, z)];
}

void init_space(SYS_TABLE *sys)
{
	int x, y, z;

	for(z=0; z<sys->space_d; z++)
		for(y=0; y<sys->space_h; y++)
			for(x=0; x<sys->space_w; x++)
			{
				sys->space_i[adds(sys,x,y,z)] = 2;
				sys->space_o[adds(sys,x,y,z)] = sys->space_i[adds(sys,x,y,z)];
			}
}

void create_space(SYS_TABLE *sys, int w, int h, int d)
{
	sys->space_w = w;
	sys->space_h = h;
	sys->space_d = d;
	sys->size_plane = w * h;
	sys->size_space = w * h * d;

	sys->space_i = malloc(sys->size_space * sizeof(int));
	sys->space_o = malloc(sys->size_space * sizeof(int));

	init_space (sys);
}

void julia_set(SYS_TABLE *sys)
{
	int c, x, y, z;

	float d, dx, dy, x0, y0, z0, q = 0.1, radius, zr, zi;

	d = 2 / 320.0;

	for (z=0; z<320; z++)
	{
		z0 = z * d - 1.5;
		for (y = 0; y < 320; y++)
		{
			y0 = y * d - 1;
			for(x = 0; x < 320; x++)
			{
				x0 = x * d - 1;

				dx = x0; dy = y0, c = 0;
				do
				{
					zr = dx * dx - dy * dy + z0;
					zi = 2.0 * dx * dy + q;
					dx = zr; dy = zi;

					radius = zr * zr + zi * zi;
					c++;
				}while(radius < 64 && c < 64);

				sys->space_i[adds(sys, x, y, z)] = c * 4 -1;
			}
		}
	}
}

void iterative_space (SYS_TABLE *sys)
{
	sys->iter = 0;

	julia_set(sys);
	sys->mapping = 1;

	while(sys->main)
	{
		usleep(1200);
	}
}

