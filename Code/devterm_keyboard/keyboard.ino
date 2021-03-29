#include "keyboard.h"
#include "helper.h"


static bool debouncing = false;
static uint16_t debouncing_time = 0;

uint8_t matrix_rows[ MATRIX_ROWS ]= {ROW1,ROW2,ROW3,ROW4,ROW5,ROW6,ROW7,ROW8};
uint8_t matrix_cols[ MATRIX_COLS ] = {COL1,COL2,COL3,COL4,COL5,COL6,COL7,COL8};

/* matrix state(1:on, 0:off) */
static uint8_t matrix[MATRIX_ROWS];
static uint8_t matrix_debouncing[MATRIX_COLS];
static uint8_t matrix_prev[MATRIX_ROWS];

static int8_t jack_idx=-1;
static uint16_t jack_time = 0;



void init_rows(){
  int i;
  for(i=0;i<8;i++) {
    pinMode(matrix_rows[i],OUTPUT);
    digitalWrite(matrix_rows[i],LOW);
    
    pinMode(matrix_rows[i],INPUT_PULLDOWN);  
  }
}

void init_cols() {

    int i;
    for(i=0;i<8;i++){
      pinMode(matrix_cols[i],OUTPUT);
      digitalWrite(matrix_cols[i],LOW);
    }
}



void matrix_init() {
  init_cols();
  init_rows();
  
  for (uint8_t i=0; i < MATRIX_ROWS; i++) {
    matrix[i] = 0;
    matrix_debouncing[i] = 0;
    matrix_prev[i] = 0;
  }

  delay(500);
}

uint8_t matrix_scan(void) {

  uint8_t data;
  for(int col = 0; col < MATRIX_COLS;col++){
    data = 0;
    digitalWrite(matrix_cols[col],HIGH);
  

    delayMicroseconds(20);

    data =(
          ( read_io(matrix_rows[0]) << 0 )  |
          ( read_io(matrix_rows[1]) << 1 )  |
          ( read_io(matrix_rows[2]) << 2 )  |
          ( read_io(matrix_rows[3]) << 3 )  |
          ( read_io(matrix_rows[4]) << 4 )  |
          ( read_io(matrix_rows[5]) << 5 )  | 
          ( read_io(matrix_rows[6]) << 6 )  |
          ( read_io(matrix_rows[7]) << 7 )  
    );

    digitalWrite(matrix_cols[col],LOW);
    if (matrix_debouncing[col] != data) {
      matrix_debouncing[col] = data;
      debouncing = true;
      debouncing_time = millis();
    }
  }

  if (debouncing == true  &&  ( (millis() - debouncing_time) > DEBOUNCE )) {
    for (int row = 0; row < MATRIX_ROWS; row++) {
      matrix[row] = 0;
      for (int col = 0; col < MATRIX_COLS; col++) {
        matrix[row] |= ((matrix_debouncing[col] & (1 << row) ? 1 : 0) << col);
        
      }
     }
     debouncing = false;
  }
  
  return 1;
}


bool matrix_is_on(uint8_t row, uint8_t col) {
    return (matrix[row] & (1<<col));
}



uint8_t matrix_get_row(uint8_t row) {
    return matrix[row];
}


void matrix_press(DEVTERM*dv,uint8_t row,uint8_t col) {
  char buff[128];

  if(matrix_is_on(row,col) == true ){
    sprintf(buff,"%d %d M%d pressed\n",row,col,(row+1)*10+col+1);
    dv->_Serial->print(buff);
    keyboard_action(dv,row,col,KEY_PRESSED);
  }
  
}

void matrix_release(DEVTERM*dv,uint8_t row,uint8_t col) {
  char buff[128];

  
  if(matrix_is_on(row,col) == false ){
    sprintf(buff,"%d %d M%d released\n",row,col,(row+1)*10+col+1);
    dv->_Serial->print(buff);
    keyboard_action(dv,row,col,KEY_RELEASED);
        
  }
  
}

void keyboard_task(DEVTERM*dv)
{

  uint8_t matrix_row = 0;
  uint8_t matrix_change = 0;

  matrix_scan();
  for (uint8_t r = 0; r < MATRIX_ROWS; r++) {
    matrix_row = matrix_get_row(r);
    
      uint8_t col_mask =1;
      for(uint8_t c=0;c < MATRIX_COLS;c++,col_mask <<=1){
 
        if( ( (matrix_prev[r] & col_mask) == 0) && ( (matrix_row & col_mask) > 0) ) {
          matrix_press(dv,r,c);
        }
        
        if( ( (matrix_prev[r] & col_mask) > 0) && ( (matrix_row & col_mask) == 0)  ) {

          matrix_release(dv,r,c);
        }
        
        if( ( (matrix_prev[r] & col_mask) > 0) && ( (matrix_row & col_mask) > 0)  ) {//same key

          
          if( jack_idx == -1){
            jack_idx = r*MATRIX_ROWS+c;
          }else{
            
            if(jack_idx != r*MATRIX_ROWS+c) {
              jack_time = 0;
              jack_idx = r*MATRIX_ROWS+c;
            }else{              
              jack_time +=1;
              if( jack_time % (DEBOUNCE*20) == 0){
                if(jack_idx > 1){//skip select,start button 
                  matrix_press(dv,r,c);
                }
              } 
            }
          }
        }
      }
      
      matrix_prev[r] = matrix_row;
    }
  
  
}


void keyboard_init(DEVTERM*){
  matrix_init();
 
}
