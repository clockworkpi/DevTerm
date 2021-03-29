#ifndef UTILS_H
#define UTILS_H
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> 
#include <string.h>

#include <wiringPi.h>

#define SEP printf(" ");
// a is string, b is number
#define DEBUG(a,b) printf(a);SEP;printf("%d\n",b);

#define ALINE printf("\n");

void delayus(unsigned int _us);

uint8_t invert_bit(uint8_t a);

uint8_t bits_number(uint8_t n);


#endif
