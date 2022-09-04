/*
 * ALGO     : PROJ. : Volume
 * RESEARCH : File  : space.h
 *          : Date  : 20100531.0725UTC
 *          : Email : mail@algoresearch.net
 */

#include <stdlib.h>
#include <unistd.h>

void create_space(SYS_TABLE *sys, int w, int h, int d);
void iterative_space (SYS_TABLE *sys);
int cell_get(SYS_TABLE *sys, int x, int y, int z);
