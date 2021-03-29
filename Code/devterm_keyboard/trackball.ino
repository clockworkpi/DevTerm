/*
 * clockworkpi devterm trackball 
 */
#include "keys_io_map.h"

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <USBComposite.h>

#include "trackball.h"


int btn_state;
int btn_read_state;
unsigned long btn_current_action_time;
unsigned long btn_last_action_time;

// mouse move
int x_move, y_move;
Direction x_direction(LEFT_PIN, RIGHT_PIN);
Direction y_direction(UP_PIN, DOWN_PIN);

void trackball_task(DEVTERM*dv) {
  btn_read_state = digitalRead(BTN_PIN);
  
  if(btn_read_state != btn_state) {
    btn_current_action_time = millis();
    if(btn_current_action_time - btn_last_action_time > BOUNCE_INTERVAL) {
      btn_state = btn_read_state;
      btn_last_action_time = btn_current_action_time;
      
      if(btn_state == HIGH) {
        dv->Mouse->release();
      } else {
        dv->Mouse->press();
      }
    }
  }

  x_move = x_direction.read_action();
  y_move = y_direction.read_action();
  if(x_move != 0 || y_move != 0) {
    dv->Mouse->move(x_move, y_move, 0);
  }
  
}


void trackball_init(DEVTERM*){
  pinMode(BTN_PIN,INPUT);
}
