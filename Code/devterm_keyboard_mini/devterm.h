#ifndef DEVTERM_H
#define DEVTERM_H

#define KEY_LATENCY  1400
#include "state.h"

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
  uint8_t backlight;//0 1 2 3
  uint8_t lock;//0 1
  
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
    State *state;
    uint32_t delta;
};

#define KEYBOARD_PULL 0 // 1 for PULLUP, 0 FOR PULLDOWN
#define KEYBOARD_LED_PWM_PERIOD 200

#endif
