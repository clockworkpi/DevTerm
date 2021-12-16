#include <cassert>
#include <algorithm>
#include <limits>

#include "state.h"

State::State()
  : fn(false),
    middleClick(false)
{
}

void State::tick(millis_t delta)
{
  middleClickTimeout.updateTime(delta);
}

void State::pressMiddleClick() {
  middleClick = true;
  middleClickTimeout.reset();
}

bool State::releaseMiddleClick() {
  middleClick = false;
  const auto timeout = middleClickTimeout.get();
  return !timeout;
}

TrackballMode State::moveTrackball() {
  middleClickTimeout.expire();
  if (middleClick) {
    return TrackballMode::Wheel;
  } else {
    return TrackballMode::Mouse;
  }
}
