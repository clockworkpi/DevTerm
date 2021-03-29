#ifndef KEYBOARD_H
#define KEYBOARD_H

/*
 * clockworkpi devterm keyboard test2 
 * able to correct scan the 8x8 keypads re-action
 */

#include "devterm.h"

#include "keys_io_map.h"

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define MATRIX_ROWS 8
#define MATRIX_COLS 8

#ifndef DEBOUNCE
#   define DEBOUNCE 5
#endif

void init_rows();
void init_cols();
uint8_t read_io(uint8_t io);

void matrix_init();
uint8_t matrix_scan(void);

bool matrix_is_on(uint8_t row, uint8_t col);
uint8_t matrix_get_row(uint8_t row) ;


//void matrix_print(void);



void keyboard_task(DEVTERM*);
void keyboard_init(DEVTERM*);


#define KEY_PRESSED 1
#define KEY_RELEASED 0

#define KEY_PRNT_SCRN 0xCE //Print screen
#define KEY_PAUSE  0xd0

#define KEY_VOLUME_UP 0x108
#define KEY_VOLUME_DOWN 0x109

#endif
