#ifndef LED_PIN_H
#define LED_PIN_H
#define LED_PIN 27 // RGB LED on M5Stack Atom
void initLed();
void handleLed();

enum LEDStatus {
  LED_ON,
  LED_OFF,
  LED_ERROR,
  LED_WAIT,
  LED_BLINK,
  LED_BLINK_FAST,
  LED_GLOW
};

void setLedState(const LEDStatus state);
#endif // LED_PIN_H