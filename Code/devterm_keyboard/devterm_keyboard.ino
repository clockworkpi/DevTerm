#include "keyboard.h"
#include "keys.h"
#include "trackball.h"
#include "devterm.h"

#include <USBComposite.h>

#define SER_NUM_STR "20210320"

USBHID HID;
DEVTERM dev_term;

void setup() {
  USBComposite.setManufacturerString("ClockworkPI");
  USBComposite.setProductString("DevTerm");
  USBComposite.setSerialString(SER_NUM_STR);
  
  dev_term.Keyboard = new HIDKeyboard(HID);
  dev_term.Joystick = new HIDJoystick(HID);
  dev_term.Mouse    = new HIDMouse(HID);
  
  dev_term.Keyboard_state.shift = 0;
  dev_term.Keyboard_state.layer = 0;
  dev_term.Keyboard_state.caps_lock = 0;
  
  dev_term._Serial = new  USBCompositeSerial;
  
  HID.begin(*dev_term._Serial,HID_KEYBOARD_MOUSE_JOYSTICK);

  while(!USBComposite);//wait until usb port been plugged in to PC
  

  keyboard_init(&dev_term);
  keys_init(&dev_term);
  trackball_init(&dev_term);
  
  dev_term._Serial->println("setup done");

  //delay(3000);
}

void loop() {

  trackball_task(&dev_term);
  keys_task(&dev_term); //keys above keyboard
  keyboard_task(&dev_term);
  

}
