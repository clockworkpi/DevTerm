/*
 * ALGO     : PROJ. : Volume
 * RESEARCH : File  : main.c
 *          : Date  : 20100531.0725UTC
 *          : Email : mail@algoresearch.net
 */

#include <stdlib.h>

#include "main.h"
#include "space.h"
#include "observer.h"

int main()
{
	SYS_TABLE sys;

	create_space (&sys, 320, 320, 320);
	create_observer (&sys);
	iterative_space (&sys);

	return 0;
}

