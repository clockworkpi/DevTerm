#ifndef DEVTERM_H
#define DEVTERM_H

#define KEY_LATENCY  1400

#include <USBComposite.h>
typedef struct key_debouncing{

  bool deing;//debouncing
  uint16_t de_time;
  
}KEY_DEB;

typedef struct keyboard_state{

  uint8_t layer;
  uint8_t prev_layer;
  uint8_t fn_on;
  uint8_t shift;
  
}KEYBOARD_STATE;

class DEVTERM {
  public:
    HIDKeyboard *Keyboard;
    HIDMouse *Mouse; 
    HIDJoystick *Joystick;
    HIDConsumer *Consumer;
    KEYBOARD_STATE Keyboard_state;
    USBCompositeSerial *_Serial;
    //if not to use USBCompositeSerial,then use default Serial
    //**Serial and USBCompositeSerial can not use together, otherwise the keyboard firmware uploading will be dead**
    //and you will need to find a way out to flash the stm32duino bootloader once again
    //USBSerial *_Serial;//_Serial = &Serial;
};

#define KEYBOARD_PULL 1 // 1 for PULLUP, 0 FOR PULLDOWN

#endif
