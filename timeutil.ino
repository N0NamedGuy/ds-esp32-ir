#include <Arduino.h>
#include "timeutil.h"

unsigned long currentMs = 0;

unsigned long inline getCurrentMs() {
    return currentMs;
}

void tick() {
    currentMs = millis();
}

bool hasTimePassed(unsigned long *prevMs, unsigned long delay) {
  if (currentMs - (*prevMs) >= delay) {
    *prevMs = currentMs;
    return true;
  }

  return false;
}
