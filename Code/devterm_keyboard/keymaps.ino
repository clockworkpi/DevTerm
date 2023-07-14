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

#define _LEFT_SHIFT_KEY KEY_LEFT_SHIFT
#define _LEFT_CTRL_KEY  KEY_LEFT_CTRL
#define _CMD_KEY        KEY_RIGHT_GUI
#define _LEFT_ALT       KEY_LEFT_ALT

#define _FN_KEY_UP_ARROW     KEY_PAGE_UP
#define _FN_KEY_DOWN_ARROW   KEY_PAGE_DOWN
#define _FN_KEY_LEFT_ARROW   KEY_HOME
#define _FN_KEY_RIGHT_ARROW  KEY_END

enum SKEYS {
  _SELECT_KEY = 0xe8, //Joystick.button(n)
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

  _FN_BRIGHTNESS_UP, //USB Consumer brightness up https://github.com/torvalds/linux/blob/7fe10096c1508c7f033d34d0741809f8eecc1ed4/drivers/hid/hid-input.c#L903
  _FN_BRIGHTNESS_DOWN, //USB Consumer brightness down

  _VOLUME_M,
  _VOLUME_P,
  _TRACKBALL_BTN,
};

#define DEF_LAYER      0x00
#define FN_LAYER       0x01


/*
   keyboard_maps
   M11 - M18
   M21 - M28
   M31 - M38
   M41 - M48
   M51 - M58
   M61 - M68
   M71 - M78
   M81 - M88
*/
const uint16_t keyboard_maps[][MATRIX_KEYS] = {

  [DEF_LAYER] = {
    _SELECT_KEY, _START_KEY, _VOLUME_M, '`', '[', ']', '-', '=', \
    '1', '2', '3', '4', '5', '6', '7', '8', \
    '9', '0', KEY_ESC, KEY_TAB, KEY_UP_ARROW, KEY_DOWN_ARROW, KEY_LEFT_ARROW, KEY_RIGHT_ARROW, \
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', \
    'o', 'p', 'a', 's', 'd', 'f', 'g', 'h', \
    'j', 'k', 'l', 'z', 'x', 'c', 'v', 'b', \
    'n', 'm', ',', '.', '/', '\\', ';', '\'', \
    KEY_BACKSPACE, KEY_RETURN, KEY_RIGHT_ALT, KEY_RIGHT_CTRL, KEY_RIGHT_SHIFT, ' ', EMP, EMP
  },

  [FN_LAYER] = {
    _PRINT_KEY, _PAUSE_KEY, _VOLUME_P, '`', '[', ']', KEY_F11, KEY_F12, \
    KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, \
    KEY_F9, KEY_F10, KEY_ESC, KEY_CAPS_LOCK, _FN_KEY_UP_ARROW, _FN_KEY_DOWN_ARROW, _FN_KEY_LEFT_ARROW, _FN_KEY_RIGHT_ARROW, \
    'q', 'w', 'e', 'r', 't', 'y', 'u', KEY_INSERT, \
    'o', 'p', 'a', 's', 'd', 'f', 'g', 'h', \
    'j', 'k', 'l', 'z', 'x', 'c', 'v', 'b', \
    'n', 'm', _FN_BRIGHTNESS_DOWN, _FN_BRIGHTNESS_UP, '/', '\\', ';', '\'', \
    KEY_DELETE, KEY_RETURN, KEY_RIGHT_ALT, KEY_RIGHT_CTRL, KEY_RIGHT_SHIFT, ' ', EMP, EMP
  }

};

uint16_t keyboard_pick_map[MATRIX_KEYS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

static uint8_t fn_actions[MATRIX_KEYS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

const uint16_t keys_maps[KEYS_NUM] = {_JOYSTICK_UP, _JOYSTICK_DOWN, _JOYSTICK_LEFT, \
                                      _JOYSTICK_RIGHT, _JOYSTICK_A, _JOYSTICK_B, \
                                      _JOYSTICK_X, _JOYSTICK_Y, _LEFT_SHIFT_KEY, _FN_KEY, \
                                      _LEFT_CTRL_KEY, _CMD_KEY , _LEFT_ALT,     \
                                      _MOUSE_LEFT, _MOUSE_MID, _MOUSE_RIGHT, _TRACKBALL_BTN
                                     };



uint8_t check_pd2() { // if swtich 2 in back is set to on(HIGH)

  return digitalRead(PD2);
}

void dt_kbd_set_layer(DEVTERM*dv, uint8_t new_layer) {

  if ( dv->Keyboard_state.layer != new_layer) {

    dv->Keyboard_state.prev_layer = dv->Keyboard_state.layer;
    dv->Keyboard_state.layer = new_layer;
  }
}

void dt_kbd_restore_layer(DEVTERM*dv) {

  dv->Keyboard_state.layer = dv->Keyboard_state.prev_layer;

}
void keyboard_release_core(DEVTERM*dv, uint16_t k) {

  switch (k) {
    case KEY_RIGHT_SHIFT:
      dv->Keyboard->release(k);
      break;

    case  KEY_CAPS_LOCK:
      dv->Keyboard->setAdjustForHostCapsLock(false);
      dv->Keyboard->release(k);
      break;

    case _SELECT_KEY:
      if (check_pd2() == HIGH) {
        k = ' ';
        dv->Keyboard->release(k);
      } else {
        dv->Joystick->button(9, KEY_RELEASED);
      }
      break;
    case _START_KEY:
      if (check_pd2() == HIGH) {
        k = KEY_RETURN;
        dv->Keyboard->release(k);
      } else {
        dv->Joystick->button(10, KEY_RELEASED);
      }
      break;

    case _FN_BRIGHTNESS_UP:
    case _FN_BRIGHTNESS_DOWN:
    case _VOLUME_P:
    case _VOLUME_M:
      dv->Consumer->release();
      break;

    default:
      dv->Keyboard->release(k);
      break;
  }
}

void keyboard_release(DEVTERM*dv, uint8_t addr, uint16_t k) {
  if ( keyboard_pick_map[addr] == 0) {
    keyboard_release_core(dv, k);
  } else {
    keyboard_release_core(dv, keyboard_pick_map[addr]);
    keyboard_pick_map[addr] = 0;
  }
}

void keyboard_action(DEVTERM * dv, uint8_t row, uint8_t col, uint8_t mode) {

  uint16_t k;
  uint8_t addr;
  addr = row * MATRIX_COLS + col;
  if (dv->Keyboard_state.fn_on > 0) {
    k = keyboard_maps[dv->Keyboard_state.fn_on][addr];
    fn_actions[addr] = 1;
  } else {
    k = keyboard_maps[dv->Keyboard_state.layer][addr];
  }

  if (k == EMP) {
    return;
  }

  switch (k) {
    case KEY_RIGHT_SHIFT: {
        if (mode == KEY_PRESSED) {
          dv->Keyboard->press(k);
        } else {
          keyboard_release(dv, addr, k);
        }
      } break;

    case  KEY_CAPS_LOCK:
      if (mode == KEY_PRESSED) {
        dv->Keyboard->press(k);
        dv->Keyboard->setAdjustForHostCapsLock(true);
      } else if (mode == KEY_RELEASED) {
        keyboard_release(dv, addr, k);
      }
      break;
    case _SELECT_KEY:
      if (check_pd2() == HIGH) {
        k = ' ';
        if (mode == KEY_PRESSED) {
          dv->Keyboard->press(k);
        } else if (mode == KEY_RELEASED) {
          keyboard_release(dv, addr, k);
        }
      } else {
        if ( mode == KEY_PRESSED) {
          dv->Joystick->button(9, mode);
        } else {
          keyboard_release(dv, addr, k);
        }
      }
      break;
    case _START_KEY:
      if (check_pd2() == HIGH) {
        k = KEY_RETURN;
        if (mode == KEY_PRESSED) {
          dv->Keyboard->press(k);
        } else if (mode == KEY_RELEASED) {
          keyboard_release(dv, addr, k);
        }
      } else {
        if (mode == KEY_PRESSED) {
          dv->Joystick->button(10, mode);
        } else {
          keyboard_release(dv, addr, k);
        }
      }
      break;

    case _FN_BRIGHTNESS_UP:
      if (mode == KEY_PRESSED) {
        dv->Consumer->press(HIDConsumer::BRIGHTNESS_UP);
      } else {
        keyboard_release(dv, addr, k);
      }
      break;
    case _FN_BRIGHTNESS_DOWN:
      if (mode == KEY_PRESSED) {
        dv->Consumer->press(HIDConsumer::BRIGHTNESS_DOWN);
      } else {
        keyboard_release(dv, addr, k);
      }
      break;

    case _VOLUME_P: {
        if (mode == KEY_PRESSED) {
          dv->Consumer->press(HIDConsumer::VOLUME_UP);
        } else {
          keyboard_release(dv, addr, k);
        }
      } break;
    case _VOLUME_M: {
        if (mode == KEY_PRESSED) {
          dv->Consumer->press(HIDConsumer::VOLUME_DOWN);
        } else {
          keyboard_release(dv, addr, k);
        }
      } break;

    default:
      if (mode == KEY_PRESSED) {
        dv->Keyboard->press(k);
      } else if (mode == KEY_RELEASED) {
        keyboard_release(dv, addr, k);
      }
      break;
  }
}

void keypad_action(DEVTERM * dv, uint8_t col, uint8_t mode) {

  uint16_t k;

  k = keys_maps[col];

  if (k == EMP) {
    return;
  }


  switch (k) {
    case _LEFT_SHIFT_KEY:
      if (mode == KEY_PRESSED) {
        dv->Keyboard->press(k);
      } else if (mode == KEY_RELEASED) {
        dv->Keyboard->release(k);
      }
      break;
    case  _FN_KEY:
      if (mode == KEY_PRESSED) {
        dv->Keyboard_state.fn_on = FN_LAYER;

      } else if (mode == KEY_RELEASED ) {
        //release all pressed fn keys if they still been pressing
        dv->Keyboard_state.fn_on = 0;
      }
      break;

    case _JOYSTICK_UP:
      if (check_pd2() == HIGH) {
        k = KEY_UP_ARROW;
        if (mode == KEY_PRESSED) {
          dv->Keyboard->press(k);
        } else if (mode == KEY_RELEASED) {
          dv->Keyboard->release(k);
        }
      } else {
        if (mode == KEY_RELEASED) {
          dv->Joystick->Y(511);
        } else {
          dv->Joystick->Y(0);
        }
      }
      break;
    case _JOYSTICK_DOWN:
      if (check_pd2() == HIGH) {
        k = KEY_DOWN_ARROW;
        if (mode == KEY_PRESSED) {
          dv->Keyboard->press(k);
        } else if (mode == KEY_RELEASED) {
          dv->Keyboard->release(k);
        }
      } else {
        if (mode == KEY_RELEASED) {
          dv->Joystick->Y(511);
        } else {
          dv->Joystick->Y(1023);
        }
      }
      break;
    case _JOYSTICK_LEFT:
      if (check_pd2() == HIGH) {
        k = KEY_LEFT_ARROW;
        if (mode == KEY_PRESSED) {
          dv->Keyboard->press(k);
        } else if (mode == KEY_RELEASED) {
          dv->Keyboard->release(k);
        }
      } else {
        if (mode == KEY_RELEASED) {
          dv->Joystick->X(511);
        } else {
          dv->Joystick->X(0);
        }
      }
      break;
    case _JOYSTICK_RIGHT:
      if (check_pd2() == HIGH) {
        k = KEY_RIGHT_ARROW;
        if (mode == KEY_PRESSED) {
          dv->Keyboard->press(k);
        } else if (mode == KEY_RELEASED) {
          dv->Keyboard->release(k);
        }
      } else {
        if (mode == KEY_RELEASED) {
          dv->Joystick->X(511);
        } else {
          dv->Joystick->X(1023);
        }
      }
      break;
    case _JOYSTICK_A:
      if (check_pd2() == HIGH) {
        k = 'j';
        if (mode == KEY_PRESSED) {
          dv->Keyboard->press(k);
        } else if (mode == KEY_RELEASED) {
          dv->Keyboard->release(k);
        }
      } else {
        dv->Joystick->button(2, mode);
      }
      break;
    case _JOYSTICK_B:
      if (check_pd2() == HIGH) {
        k = 'k';
        if (mode == KEY_PRESSED) {
          dv->Keyboard->press(k);
        } else if (mode == KEY_RELEASED) {
          dv->Keyboard->release(k);
        }
      } else {
        dv->Joystick->button(3, mode);
      }
      break;
    case _JOYSTICK_X:
      if (check_pd2() == HIGH) {
        k = 'u';
        if (mode == KEY_PRESSED) {
          dv->Keyboard->press(k);
        } else if (mode == KEY_RELEASED) {
          dv->Keyboard->release(k);
        }
      } else {
        dv->Joystick->button(1, mode);
      }
      break;
    case _JOYSTICK_Y:
      if (check_pd2() == HIGH) {
        k = 'i';
        if (mode == KEY_PRESSED) {
          dv->Keyboard->press(k);
        } else if (mode == KEY_RELEASED) {
          dv->Keyboard->release(k);
        }
      } else {
        dv->Joystick->button(4, mode);
      }
      break;
    case _MOUSE_LEFT:
      if (mode == KEY_PRESSED) {
        dv->Mouse->press(1);
      } else if (mode == KEY_RELEASED) {
        dv->Mouse->release(1);
      }
      break;
    case _MOUSE_MID:
      if (mode == KEY_PRESSED) {
        dv->state->pressMiddleClick();
      } else {
        if (dv->state->getScrolled() == false) {
          //if no scrolling happend ,do as a normal mid mouse key click
          dv->Mouse->click(MOUSE_MIDDLE);
        }
        dv->state->releaseMiddleClick();

      }
      break;

    case _MOUSE_RIGHT:
      if (mode == KEY_PRESSED) {
        dv->Mouse->press(2);
      } else if (mode == KEY_RELEASED) {
        dv->Mouse->release(2);
      }
      break;

    //_LEFT_CTRL_KEY,_CMD_KEY , _LEFT_ALT
    case _LEFT_CTRL_KEY:
    case _CMD_KEY:
    case _LEFT_ALT:
      if (mode == KEY_PRESSED) {
        dv->Keyboard->press(k);
      } else {
        dv->Keyboard->release(k);
      }
      break;

    case _TRACKBALL_BTN:
      if (mode == KEY_PRESSED) {
        dv->Mouse->press(1);
      } else if (mode == KEY_RELEASED) {
        dv->Mouse->release(1);
      }
      break;
    default: break;

  }

}
