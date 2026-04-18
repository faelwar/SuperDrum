#include "setup.h"
#include "consts.h"
#include "eeprom.h"
#include "pin.h"
#include "utility.h"
#include "define.h"

// One-time initialization run by the Arduino runtime before loop() starts.
void setup() {
  // Standard MIDI baud rate. All pad hits and host-editor communication share
  // this single serial connection. TEXT_OUTPUT mode still uses 31250 baud but
  // sends ASCII strings, so use a serial monitor set to the same rate.
  Serial.begin(31250);
  Serial.flush();
  while(!Serial);  // Wait for USB serial to enumerate (relevant on Leonardo / Mega ADK)

  // Configure choke and auxiliary digital pins with internal pull-up resistors.
  // Buttons are wired to ground, so LOW = pressed, HIGH = released (active-low logic).
  #if CHOKE
    pinMode(Choke1_Pin,   INPUT_PULLUP);
    pinMode(Choke2_Pin,   INPUT_PULLUP);
    pinMode(Choke3_Pin,   INPUT_PULLUP);
    pinMode(ChokeRide_Pin, INPUT_PULLUP);
    pinMode(Aux1_Pin, INPUT_PULLUP);
    pinMode(Aux2_Pin, INPUT_PULLUP);
    pinMode(Aux3_Pin, INPUT_PULLUP);
    pinMode(Aux4_Pin, INPUT_PULLUP);
    pinMode(Aux5_Pin, INPUT_PULLUP);
    pinMode(Aux6_Pin, INPUT_PULLUP);
    pinMode(Aux7_Pin, INPUT_PULLUP);
  #endif

  // Seed the global timestamp before calling set() so the initial Time offset
  // in each pin (MaskTime ms into the future) is relative to real wall time.
  GlobalTime = TIMEFUNCTION;

  Serial.print("Pin Initialization\n");
  for (int count = 0; count < NPin; count++) {
    Pin[count].set(count);      // Assign pad type and default parameters
    Serial.println(Pin[count]); // Print initial config (uses pin::printTo)
  }

  // Use the default 5V reference voltage for ADC readings.
  #if defined(__AVR__)
    analogReference(DEFAULT);
  #endif

  // EEPROM load is disabled during development — uncomment to restore saved
  // settings on boot instead of starting from compiled defaults.
  //LoadAllEEPROM();

  // Speed up ADC sampling by reducing the prescaler. Prescaler_16 gives
  // 1 MHz ADC clock and ~8.6-bit effective resolution — a good balance between
  // speed and accuracy for piezo sensing. See utility.h for the full table.
  #if defined(__AVR__)
    #if FASTADC
      setPrescaler(Prescaler_16);
    #endif
  #elif defined(__arm__)
    // ARM (Due): set ADC clock prescaler via direct register access
    REG_ADC_MR = (REG_ADC_MR & 0xFFF0FFFF) | 0x00030000;
  #endif
}