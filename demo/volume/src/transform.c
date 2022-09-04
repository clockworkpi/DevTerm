/*
 * ALGO     : PROJ. : Volume
 * RESEARCH : File  : transform.c
 *          : Date  : 20100531.0725UTC
 *          : Email : mail@algoresearch.net
 */

#include "transform.h"

void rotate_xy(VERTEX *in, VERTEX *out, float ax, float ay, VERTEX *offset)
{
	float x = in->x - offset->x,
		  y = in->y - offset->y,
		  z = in->z - offset->z,
		  Cax = cos(ax),
		  Sax = sin(ax);

	out->y = y * Cax - z * Sax;
	out->z = y * Sax + z * Cax;
	out->x = out->z * sin(ay) + x * cos(ay);
}
