#include "button.h"
#include "led.h"
#include "infrared.h"
#include <JC_Button.h>

Button btn(39, true, 10);

bool showLedOn = true;
bool toggleLedOnRelease = false;

void initButton() {
    btn.begin();
}

void handleLedState() {
    if (showLedOn) {
        Serial.println("Turning on LED");
        setLedState(LED_ON);
    } else {
        Serial.println("Turning off LED");
        setLedState(LED_OFF);
    }
}

void handleButton() {
    btn.read();

    if (btn.pressedFor(2000)) {
        toggleLedOnRelease = true;
    } else if (toggleLedOnRelease && btn.isReleased()) {
        showLedOn = !showLedOn;
        handleLedState();
        toggleLedOnRelease = false;
    } else if (btn.wasPressed()) {
        Serial.println("Turning sending POWER ON");
        setLedState(LED_WAIT);
        sendOn();
        handleLedState();
    }

}