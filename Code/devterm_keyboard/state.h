#ifndef STATE_H
#define STATE_H

#include <bitset>
#include <array>
#include <USBComposite.h>

enum class TrackballMode : uint8_t {
  Wheel,
  Mouse,
};

template <typename T, T millis>
class Timeout
{
public:
  Timeout()
  {
    timeout = 0;
  }

  void updateTime(uint8_t delta)
  {
    if (timeout > delta)
    {
      timeout -= delta;
    }
    else
    {
      timeout = 0;
    }
  }

  void expire()
  {
    timeout = 0;
  }

  bool get() const
  {
    return timeout == 0;
  }

  void reset()
  {
    timeout = millis;
  }

private:
  T timeout;
};

class State
{
  public:
    static const uint16_t MIDDLE_CLICK_TIMEOUT_MS = 0;

    State();

    void tick(uint8_t delta);

    bool fn;

    void pressMiddleClick();
    bool releaseMiddleClick();
    bool getScrolled();
    void setScrolled();
    TrackballMode moveTrackball();
  private:
    bool middleClick;
    bool scrolled;
    Timeout<uint16_t, MIDDLE_CLICK_TIMEOUT_MS> middleClickTimeout;
};

#endif
