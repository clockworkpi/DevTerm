#ifndef TRACKBALL_H
#define TRACKBALL_H

#include "devterm.h"

#include "keys_io_map.h"

/*
#define BOUNCE_INTERVAL       30 
#define BASE_MOVE_PIXELS      5  
#define EXPONENTIAL_BOUND     15
#define EXPONENTIAL_BASE      1.2
*/

#define RIGHT_PIN             HO3
#define LEFT_PIN              HO1
#define DOWN_PIN              HO4
#define UP_PIN                HO2


void trackball_init(DEVTERM*);
void trackball_task(DEVTERM*);


#endif
