#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

#include "infrared.h"
#include "led.h"
#include "timeutil.h"

unsigned long prevLedMs = 0;
unsigned long prevBlinkLedMs = 0;

int i = 0;
bool dec = false;
bool stopped = false;

bool ledBlinkOn = true;

LEDStatus ledStatus = LED_ON;

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel pixels(1, LED_PIN, NEO_GRB + NEO_KHZ800);

void initLed() {
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.clear(); // Set all pixel colors to 'off'
  setLedState(LED_ON);
}

void handleLedGlow() {
  if (i > 254) {
    dec = true;
  } else if (i == 0) {
    dec = false;
  }

  if (dec) {
    i--;
  } else {
    i++;
  }

  pixels.setPixelColor(0, pixels.Color(i, i / 2, i / 2));
  pixels.show();
}

void handleLedBlink(unsigned int waitTime) {
  if (hasTimePassed(&prevBlinkLedMs, waitTime)) {
    ledBlinkOn = !ledBlinkOn;
  }

  if (ledBlinkOn) {
    pixels.setPixelColor(0, pixels.Color(255, 255, 255));
    pixels.show();
  } else {
    pixels.clear();
    pixels.show();
  }
}

void handleLed() {
  switch (ledStatus) {

  case LED_BLINK_FAST:
    handleLedBlink(20);
    break;
  case LED_BLINK:
    handleLedBlink(500);
    break;

  case LED_GLOW:
    handleLedGlow();
    break;
  }

  yield();
}

void setLedState(const LEDStatus state) {
  ledStatus = state;

  switch (state) {
  case LED_ON:
    pixels.setPixelColor(0, pixels.Color(0, 255, 0));
    pixels.show();
    break;
  case LED_OFF:
    pixels.clear();
    break;
  case LED_ERROR:
    pixels.setPixelColor(0, pixels.Color(255, 0, 0));
    pixels.show();
    break;
  case LED_WAIT:
    pixels.setPixelColor(0, pixels.Color(0, 0, 255));
    pixels.show();
    break;

  case LED_GLOW:
  case LED_BLINK_FAST:
  case LED_BLINK:
  default:
    break;
  }
}