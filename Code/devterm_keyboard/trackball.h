#ifndef TRACKBALL_H
#define TRACKBALL_H

#include "devterm.h"

#include "keys_io_map.h"

/*
#define BOUNCE_INTERVAL       30 
#define BASE_MOVE_PIXELS      5  
#define EXPONENTIAL_BOUND     15
#define EXPONENTIAL_BASE      1.2
*/

#define BTN_PIN               KEY0
#define RIGHT_PIN             HO3
#define LEFT_PIN              HO1
#define DOWN_PIN              HO4
#define UP_PIN                HO2

typedef struct _track_speed {
  uint8_t bounce_interval;
  uint8_t base_move_pixels;
  uint8_t exponential_bound;
  double  exponential_base;
  
}TrackSpeed;

class Direction {
  public:
    Direction(int pin1, int pin2) {
      this->pins[0] = pin1;
      this->pins[1] = pin2;
      pinMode(this->pins[0], INPUT);
      pinMode(this->pins[1], INPUT);
    };
    
    int read_action() {
      for(int i = 0; i < 2; ++i) {
        this->current_actions[i] = digitalRead(this->pins[i]);
        this->current_action_times[i] = millis();
        if(this->current_actions[i] != this->last_actions[i]) {
          this->last_actions[i] = this->current_actions[i];
          exponential = ( ts->exponential_bound - (this->current_action_times[i] - this->last_action_times[i]));
          exponential = (exponential > 0) ? exponential : 1;
          move_multiply = ts->exponential_base;
          for(int i = 0; i < exponential; ++i) {
            move_multiply *= ts->exponential_base;
          }
          this->last_action_times[i] = this->current_action_times[i];
          if(i == 0) {
            return (-1) * ts->base_move_pixels * move_multiply;
          } else {
            return ts->base_move_pixels * move_multiply;
          }
        }
      }
      return 0;
    };
  
  TrackSpeed *ts;
    
  private:
    int           pins[2];
    int           current_actions[2];
    int           last_actions[2];
    int           exponential;
    double        move_multiply;
    unsigned long current_action_times[2];
    unsigned long last_action_times[2];
    
};

void trackball_init(DEVTERM*);
void trackball_task(DEVTERM*);


#endif
