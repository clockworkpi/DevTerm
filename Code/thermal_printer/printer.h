#ifndef PRINTER_H
#define PRINTER_H

#include "config.h"

#define PRINT_SPLIT 6 // max points printed at the same time, 384/PRINT_SPLIT==96
#define MAX_PRINT_PTS 24

void printer_send_data8(uint8_t);

void clear_printer_buffer();
uint8_t IsPaper();

uint8_t header_init();
uint8_t header_init1();

void motor_stepper_pos1(uint8_t Position);

void motor_stepper_pos2(uint8_t Position);

uint8_t feed_pitch1(uint64_t lines, uint8_t forward_backward);

uint8_t bits_number(uint8_t n);
 
void print_dots_8bit_split(CONFIG*cfg,uint8_t *Array, uint8_t characters);

void print_dots_8bit(CONFIG*cfg,uint8_t *Array, uint8_t characters,uint8_t feed_num);

uint16_t temperature();

void print_lines8(CONFIG*);

uint8_t invert_bit(uint8_t a);

void print_image8(CONFIG*);
void print_cut_line(CONFIG*);

void printer_set_font(CONFIG*cfg,uint8_t fnbits);
void parse_serial_stream(CONFIG*cfg,uint8_t input_ch);

void reset_cmd();

#endif
