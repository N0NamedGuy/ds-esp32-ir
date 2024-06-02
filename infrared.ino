#include <Arduino.h>

#define _IR_ENABLE_DEFAULT false
#define DECODE_NEC true
#define SEND_NEC true

#include "infrared.h"
#include "timeutil.h"
#include <IRrecv.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRutils.h>

int irOn = true;

int outPin = IR_OUT_PIN;

// Timers
unsigned long prevIrMs = 0;

// IR
int lastIrIn = 0;
int currentIrIn = 0;

decode_results results;
IRrecv irrecv(IR_IN_PIN);
IRsend irsend(outPin);

void initIr() {
  pinMode(IR_IN_PIN, INPUT);
  pinMode(IR_OUT_PIN, OUTPUT);
  pinMode(IR_OUT_PIN_EXT, OUTPUT);

  irsend.begin();

  irrecv.setTolerance(25U); // Override the default tolerance.
  irrecv.enableIRIn();      // Start the receiver
}

void handleIr() {
  currentIrIn = digitalRead(IR_IN_PIN);

  if (currentIrIn != lastIrIn) {
    Serial.printf("IR CHANGE %d\n", getCurrentMs());
    lastIrIn = currentIrIn;
  }
}

void recvIr() {

  // Check if the IR code has been received.
  if (irrecv.decode(&results)) {
    // Display a crude timestamp.
    uint32_t now = millis();
    Serial.printf("TIMESTAMP"
                  " : %06u.%03u\n",
                  now / 1000, now % 1000);
    // Check if we got an IR message that was to big for our capture buffer.
    if (results.overflow)
      Serial.printf("Warning, buffer full"
                    "\n");
    // Display the library version the message was captured with.
    Serial.println("LIBRARY"
                   "   : v" _IRREMOTEESP8266_VERSION_STR "\n");
    // Display the basic output of what we found.
    Serial.print(resultToHumanReadableBasic(&results));
    // Output the results as source code
    Serial.println(resultToSourceCode(&results));
    Serial.println(); // Blank line between entries
    yield();          // Feed the WDT (again)
  }
}

void sendIr() {
  if (hasTimePassed(&prevIrMs, 200)) {
    irOn = !irOn;
    if (irOn) {
      digitalWrite(outPin, HIGH);
    } else {
      digitalWrite(outPin, LOW);
    }
  }
}

void sendTvOn() {
  uint16_t rawData[67] = {
      4432, 4446, 578, 1650, 580, 1650, 580, 1650, 578, 538,  576, 538,
      604,  508,  604, 510,  608, 504,  602, 1626, 578, 1652, 602, 1626,
      604,  510,  578, 534,  578, 538,  576, 536,  578, 538,  576, 540,
      574,  1652, 576, 536,  576, 536,  578, 538,  576, 536,  576, 536,
      602,  510,  578, 1650, 602, 512,  576, 1650, 606, 1626, 578, 1650,
      578,  1652, 580, 1650, 578, 1652, 578}; // SAMSUNG E0E040BF
  uint32_t address = 0x7;
  uint32_t command = 0x2;
  uint64_t data = 0xE0E040BF;

  irsend.sendSAMSUNG(data);
}

void sendMeoOn() {
//Code: 0xD36AF9E2(12 Bits)
uint16_t rawData[23] = {
      338, 600, 676, 578, 632, 620, 660, 284, 320, 620, 636, 616,
      348, 284, 322, 308, 636, 616, 634, 312, 318, 622, 286};
    irsend.sendRaw(rawData, 23, 38);
}

void sendOn() {
  for (int i = 0; i < 10; i++) {
    sendTvOn();
  }
  delay(20);
  for (int i = 0; i < 10; i++) {
    sendMeoOn();
  }
}