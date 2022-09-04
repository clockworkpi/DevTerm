/*
 * ALGO     : PROJ. : Volume
 * RESEARCH : File  : transform.h
 *          : Date  : 20100531.0725UTC
 *          : Email : mail@algoresearch.net
 */

#include <math.h>

typedef struct{
    float x, y, z;
    int v;
}VERTEX;

void rotate_xy(VERTEX*, VERTEX*, float, float, VERTEX*);
