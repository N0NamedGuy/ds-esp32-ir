#include "led.h"
#include "timeutil.h"
#include "infrared.h"
//#include "bluetooth.h"
#include "http.h"
#include "button.h"

void setup() {
  Serial.begin(115200);
  initLed();
  setLedState(LED_WAIT);

  initIr();
  //initBluetooth();
  initHttp();
  initButton();

  Serial.flush();
  delay(50);
  Serial.println("M5Atom initializing...OK");
}

void loop() {
  tick();

  handleLed();
  handleHttp();
  handleButton();
  //handleIr();
  //recvIr();
}
