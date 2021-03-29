#ifndef DEVTERM_H
#define DEVTERM_H


#include <USBComposite.h>

typedef struct keyboard_state{

  uint8_t layer;
  uint8_t shift;
  uint8_t caps_lock;
  
}KEYBOARD_STATE;

class DEVTERM {
  public:
    HIDKeyboard *Keyboard;
    HIDMouse *Mouse; 
    HIDJoystick *Joystick;
    KEYBOARD_STATE Keyboard_state;
    USBCompositeSerial *_Serial;
    //if not to use USBCompositeSerial,then use default Serial
    //**Serial and USBCompositeSerial can not use together, otherwise the keyboard firmware uploading will be dead**
    //and you will need to find a way out to flash the stm32duino bootloader once again
    //USBSerial *_Serial;//_Serial = &Serial;
};


#endif
