#include "utils.h"

void delayus(unsigned int _us){
  delayMicroseconds(_us);
}

uint8_t invert_bit(uint8_t a){

  return ((a&0x01)<<7)|((a&0x02)<<5)|((a&0x04)<<3)|((a&0x08)<<1)|((a&0x10)>>1)|((a&0x20)>>3)|((a&0x40)>>5)|((a&0x80)>>7);
  
}

uint8_t bits_number(uint8_t n)//count bits "1"
{ 
    uint8_t count = 0; 
    while (n) { 
        count += n & 1; 
        n >>= 1; 
    } 
    return count; 
}
