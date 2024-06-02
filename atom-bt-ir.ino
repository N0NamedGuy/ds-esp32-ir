#include "led.h"
#include "timeutil.h"
#include "infrared.h"
//#include "bluetooth.h"
#include "http.h"

void setup() {
  Serial.begin(115200);
  initLed();
  setLedState(LED_WAIT);

  initIr();
  //initBluetooth();
  initHttp();

  Serial.flush();
  delay(50);
  Serial.println("M5Atom initializing...OK");
}

void loop() {
  tick();

  handleLed();
  handleHttp();
  //handleIr();
  //recvIr();
}
