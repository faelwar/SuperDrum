#pragma once
#include <Arduino.h>

//==============================================================================
// SETUP
// One-time initialization called by the Arduino runtime before loop() starts.
//
// Responsibilities:
//   1. Open Serial at 31250 baud (MIDI rate)
//   2. Configure digital choke/aux pins with INPUT_PULLUP (if CHOKE enabled)
//   3. Call pin::set() for each pad to assign types and default parameters
//   4. Set the ADC voltage reference to DEFAULT (5V on Mega)
//   5. Optionally load saved settings from EEPROM (currently disabled)
//   6. Apply the ADC prescaler for faster sampling (if FASTADC enabled)
//==============================================================================
void setup();