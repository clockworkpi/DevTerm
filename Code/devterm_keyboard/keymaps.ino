#include "devterm.h"
#include "keyboard.h"
#include "keys.h"

#define EMP 0XFFFF

/*
B1 joystick up
B2 joystick down
B3 joystick left
B4 joystick right

B5 joystick A
B6 joystick B
B7 joystick X
B8 joystick Y

B9 left shift
B10 Fn
B11 left Ctrl
B12 Cmd
B13 left Alt
B14 mouse left
B15 mouse mid
B16 mouse right
*/
#define _PRINT_KEY KEY_PRNT_SCRN
#define _PAUSE_KEY KEY_PAUSE
#define _VOLUME_M KEY_VOLUME_DOWN
#define _VOLUME_P KEY_VOLUME_UP
#define _LEFT_SHIFT_KEY KEY_LEFT_SHIFT 
#define _LEFT_CTRL_KEY  KEY_LEFT_CTRL
#define _CMD_KEY        KEY_RIGHT_GUI 
#define _LEFT_ALT       KEY_LEFT_ALT 

enum SKEYS {
  _SELECT_KEY =0xe8,  //Joystick.button(n)
  _START_KEY,          //Joystick.button(n)
  _JOYSTICK_UP, //B1 //Joystick.Y()
  _JOYSTICK_DOWN,    //Joystick.Y()
  _JOYSTICK_LEFT,    //Joystick.X()
  _JOYSTICK_RIGHT,   //Joystick.X()  
  _JOYSTICK_A,       //Joystick.button(1)
  _JOYSTICK_B,       //Joystick.button(2)
  _JOYSTICK_X,       //Joystick.button(3)
  _JOYSTICK_Y,       //Joystick.button(4)
  _FN_KEY,
  _MOUSE_LEFT,    // Mouse.press(1)
  _MOUSE_MID,     // Mouse.press(2)
  _MOUSE_RIGHT,   // Mouse.press(3)
  

};

#define DEF_LAYER   0x00
#define SHI_LAYER   0x01
#define CAPS_LAYER  0x02
#define FN_LAYER    0x03

/*
 * keyboard_maps
 * M11 - M18
 * M21 - M28
 * M31 - M38
 * M41 - M48
 * M51 - M58
 * M61 - M68
 * M71 - M78
 * M81 - M88
 */
const uint16_t keyboard_maps[][MATRIX_ROWS][MATRIX_COLS] = {
  
  [DEF_LAYER] = { _SELECT_KEY,_START_KEY,_VOLUME_M,'`','[',']','-','=', \
    '1','2','3','4','5','6','7','8',\  
    '9','0',KEY_ESC,KEY_TAB,KEY_UP_ARROW,KEY_DOWN_ARROW,KEY_LEFT_ARROW,KEY_RIGHT_ARROW, \
    'q','w','e','r','t','y','u','i', \ 
    'o','p','a','s','d','f','g','h',\  
    'j','k','l','z','x','c','v','b', \
    'n','m',',','.','/','\\',';','\'', \
    KEY_BACKSPACE,KEY_RETURN,KEY_RIGHT_ALT,KEY_RIGHT_CTRL,KEY_RIGHT_SHIFT,' ',EMP,EMP},
 
   [SHI_LAYER] = {_SELECT_KEY,_START_KEY,_VOLUME_P,'~','{','}','_','+', \ 
    '!','@','#','$','%','^','&','*',\  
    '(',')',KEY_ESC,KEY_TAB,KEY_PAGE_UP,KEY_PAGE_DOWN,KEY_HOME,KEY_END, \ 
    'Q','W','E','R','T','Y','U','I', \ 
    'O','P','A','S','D','F','G','H',\  
    'J','K','L','Z','X','C','V','B', \ 
    'N','M','<','>','?','|',':','"', \ 
    KEY_BACKSPACE,KEY_RETURN,KEY_RIGHT_ALT,KEY_RIGHT_CTRL,KEY_RIGHT_SHIFT,' ',EMP,EMP},
  
  [CAPS_LAYER] = { _SELECT_KEY,_START_KEY,_VOLUME_M,'`','[',']','-','=', \ 
    '1','2','3','4','5','6','7','8',\  
    '9','0',KEY_ESC,KEY_TAB,KEY_UP_ARROW,KEY_DOWN_ARROW,KEY_LEFT_ARROW,KEY_RIGHT_ARROW, \ 
    'Q','W','E','R','T','Y','U','I', \ 
    'O','P','A','S','D','F','G','H',\  
    'J','K','L','Z','X','C','V','B', \ 
    'N','M',',','.','/','\\',';','\'', \ 
    KEY_BACKSPACE,KEY_RETURN,KEY_RIGHT_ALT,KEY_RIGHT_CTRL,KEY_RIGHT_SHIFT,' ',EMP,EMP},

  [FN_LAYER] = { _PRINT_KEY,_PAUSE_KEY,_VOLUME_M,'`','[',']',KEY_F11,KEY_F12, \ 
    KEY_F1,KEY_F2,KEY_F3,KEY_F4,KEY_F5,KEY_F6,KEY_F7,KEY_F8,\  
    KEY_F9,KEY_F10,KEY_ESC,KEY_CAPS_LOCK,KEY_UP_ARROW,KEY_DOWN_ARROW,KEY_LEFT_ARROW,KEY_RIGHT_ARROW, \ 
    'q','w','e','r','t','y','u',KEY_INSERT, \ 
    'o','p','a','s','d','f','g','h',\  
    'j','k','l','z','x','c','v','b', \ 
    'n','m',',','.','/','\\',';','\'', \ 
    KEY_DELETE,KEY_RETURN,KEY_RIGHT_ALT,KEY_RIGHT_CTRL,KEY_RIGHT_SHIFT,' ',EMP,EMP}
    
  
  
};

const uint16_t keys_maps[KEYS_NUM] = {_JOYSTICK_UP,_JOYSTICK_DOWN, _JOYSTICK_LEFT, \
                                      _JOYSTICK_RIGHT,_JOYSTICK_A,_JOYSTICK_B, \
                                      _JOYSTICK_X,_JOYSTICK_Y,_LEFT_SHIFT_KEY,_FN_KEY,\
                                      _LEFT_CTRL_KEY,_CMD_KEY , _LEFT_ALT,     \
                                      _MOUSE_LEFT,_MOUSE_MID,_MOUSE_RIGHT};


void keyboard_action(DEVTERM*dv,uint8_t row,uint8_t col,uint8_t mode) {

  uint16_t k;
  
  k = keyboard_maps[dv->Keyboard_state.layer][row][col];

  if(k == EMP){
    return;
  }

  switch(k) {
    case _LEFT_SHIFT_KEY:
    case  KEY_RIGHT_SHIFT:
    if(mode == KEY_PRESSED) {
      dv->_Serial->println("into shift layer");
      dv->Keyboard_state.layer = SHI_LAYER;
      dv->Keyboard->press(k);
    }else if(mode == KEY_RELEASED) {
      dv->_Serial->println("leave shift layer");
      dv->Keyboard_state.layer = DEF_LAYER;
      dv->Keyboard->release(k);
    }
    break;

    case  KEY_CAPS_LOCK:
    if(mode == KEY_PRESSED) {
      dv->Keyboard_state.layer = CAPS_LAYER;
      dv->Keyboard->press(k);
      
    }else if(mode == KEY_RELEASED) {
      dv->Keyboard->release(k);
      if(dv->Keyboard_state.caps_lock == 0) {
        dv->Keyboard_state.caps_lock = 1;
      }else{
        dv->Keyboard_state.caps_lock = 0;
        dv->Keyboard_state.layer = DEF_LAYER;
      }
    }
    
    break;   
    case _SELECT_KEY:
      dv->Joystick->button(9,mode);
    break;
    case _START_KEY:
      dv->Joystick->button(10,mode);
    break;
    
    
    default:
      if(mode == KEY_PRESSED) {
        dv->Keyboard->press(k);
      }else if(mode == KEY_RELEASED) {
        dv->Keyboard->release(k);
      }
    break;
  }

  
}


void keypad_action(DEVTERM*dv,uint8_t col,uint8_t mode) {

  uint16_t k;
  
  k = keys_maps[col];

  if(k == EMP){
    return;
  }

  switch(k) {
    case _LEFT_SHIFT_KEY:
    case KEY_RIGHT_SHIFT:
      if(mode == KEY_PRESSED) {
        dv->Keyboard_state.layer = SHI_LAYER;
        dv->Keyboard->press(k);
      }else if(mode == KEY_RELEASED) {
        dv->Keyboard_state.layer = DEF_LAYER;
        dv->Keyboard->release(k);
      }
    break;    
    case  _FN_KEY:
      if(mode == KEY_PRESSED){
        dv->Keyboard_state.layer = FN_LAYER;
      }else if(mode == KEY_RELEASED ) {
        dv->Keyboard_state.layer = DEF_LAYER;
      }
    break;
    
    case _JOYSTICK_UP:
      if(mode == KEY_RELEASED){
        dv->Joystick->Y(511);
      }else {
        dv->Joystick->Y(0);
      }
    break;
    case _JOYSTICK_DOWN:
      if(mode == KEY_RELEASED){
        dv->Joystick->Y(511);
      }else {
        dv->Joystick->Y(1023);
      }
    break;
    case _JOYSTICK_LEFT:
      if(mode == KEY_RELEASED){
        dv->Joystick->X(0);
      }else {
        dv->Joystick->X(511);
      }
    break;
    case _JOYSTICK_RIGHT:
      if(mode == KEY_RELEASED){
        dv->Joystick->X(511);
      }else {
        dv->Joystick->X(1023);
      }
    break;
    case _JOYSTICK_A:
      dv->Joystick->button(2,mode);
    break;
    case _JOYSTICK_B:
      dv->Joystick->button(3,mode);
    break;
    case _JOYSTICK_X:
      dv->Joystick->button(1,mode);
    break;
    case _JOYSTICK_Y:
      dv->Joystick->button(4,mode);
    break;
    case _MOUSE_LEFT:
      if(mode == KEY_PRESSED){
        dv->Mouse->press(1);
      }else if(mode == KEY_RELEASED){
        dv->Mouse->release(1);
      }
    break;
    case _MOUSE_MID:
      if(mode == KEY_PRESSED){
        dv->Mouse->press(2);
      }else if(mode == KEY_RELEASED){
        dv->Mouse->release(2);
      }
    break;

    case _MOUSE_RIGHT:
      if(mode == KEY_PRESSED){
        dv->Mouse->press(3);
      }else if(mode == KEY_RELEASED){
        dv->Mouse->release(3);
      }
    break;
    
    //_LEFT_CTRL_KEY,_CMD_KEY , _LEFT_ALT    
    case _LEFT_CTRL_KEY:
    case _CMD_KEY:
    case _LEFT_ALT:
      if(mode == KEY_PRESSED){
        dv->Keyboard->press(k);
      }else {
        dv->Keyboard->release(k);
      }
    break;
    
    default:break;
    
  }
  
}
