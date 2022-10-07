#include "keyboard.h"
#include "keys.h"
#include "trackball.h"
#include "devterm.h"
#include "tickwaiter.h"

#include <USBComposite.h>

#define SER_NUM_STR "20210531"

USBHID HID;
DEVTERM dev_term;

const uint8_t reportDescription[] = { 
   HID_CONSUMER_REPORT_DESCRIPTOR(),
   HID_KEYBOARD_REPORT_DESCRIPTOR(),
   HID_JOYSTICK_REPORT_DESCRIPTOR(),
   HID_MOUSE_REPORT_DESCRIPTOR()
};

static const uint32_t LOOP_INTERVAL_MS = 0;
static TickWaiter<LOOP_INTERVAL_MS> waiter;

void setup() {
  USBComposite.setManufacturerString("ClockworkPI");
  USBComposite.setProductString("DevTerm");
  USBComposite.setSerialString(SER_NUM_STR);
  
  dev_term.Keyboard = new HIDKeyboard(HID);
  dev_term.Joystick = new HIDJoystick(HID);
  dev_term.Mouse    = new HIDMouse(HID);
  dev_term.Consumer = new HIDConsumer(HID);

  dev_term.Keyboard->setAdjustForHostCapsLock(false);

  dev_term.state = new State();

  dev_term.Keyboard_state.layer = 0;
  dev_term.Keyboard_state.prev_layer = 0;
  dev_term.Keyboard_state.fn_on = 0;
  dev_term.Keyboard_state.shift = 0;
  
  dev_term._Serial = new  USBCompositeSerial;
  
  HID.begin(*dev_term._Serial,reportDescription, sizeof(reportDescription));

  while(!USBComposite);//wait until usb port been plugged in to PC
  

  keyboard_init(&dev_term);
  keys_init(&dev_term);
  trackball_init(&dev_term);
  
  dev_term._Serial->println("setup done");

  pinMode(PD2,INPUT);// switch 2 in back 
  
  delay(1000);
}

void loop() {
  dev_term.delta = waiter.waitForNextTick();
  dev_term.state->tick(dev_term.delta);
  
  trackball_task(&dev_term);
  
  keys_task(&dev_term); //keys above keyboard
  keyboard_task(&dev_term);
  

}
