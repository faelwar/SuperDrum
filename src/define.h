//
// Created by Rafa Guerra on 16/01/26.
//
// Compile-time configuration flags. Change these to adapt the firmware
// to different hardware setups before flashing.
//
#pragma once
#ifndef SUPERDRUM_DEFINE_H
  #define SUPERDRUM_DEFINE_H

  // Output mode:
  //   1 = Print human-readable MIDI events over Serial (for debugging with a serial monitor)
  //   0 = Send raw MIDI bytes at 31250 baud (for use with a MIDI interface or DAW)
  #define TEXT_OUTPUT 1

  // ADC prescaler tuning:
  //   1 = Apply a reduced prescaler in setup() for faster ADC sampling (see utility.h)
  //   0 = Leave the Arduino default prescaler (slower but slightly higher resolution)
  #define FASTADC 1

  // Per-pad MIDI channel:
  //   1 = Each pin uses its own Channel field (configurable via SysEx)
  //   0 = All pads are hardcoded to MIDI channel 9 (standard GM drums)
  #define ENABLE_CHANNEL 1

  // Number of analog pad inputs to allocate.
  // Controls the size of Pin[] and which default pad layout is compiled in pin.cpp:
  //   1 = snare only
  //   4 = snare + 3 toms
  //   >4 = full layout including HH, kick, extras (partially disabled by default)
  #define NUM_PINS 4

  // Digital choke/aux pin support:
  //   1 = Enable reading of Choke*_Pin and Aux*_Pin digital inputs in loop()
  //   0 = Skip digital pin scanning (saves a few microseconds per loop)
  #define CHOKE 0

  #if defined(__arm__)
    // TODO: https://github.com/sebnil/DueFlashStorage
  #elif defined(__AVR__)
    #include <EEPROM.h>
  #endif

  //==============================================================================
  // REGISTER BIT MACROS
  // Standard AVR helpers for setting/clearing individual bits in I/O registers.
  // Used by setPrescaler() and fastWrite() to manipulate ADCSRA and PORTx directly.
  //==============================================================================
  #ifndef cbi
    #define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
  #endif
  #ifndef sbi
    #define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
  #endif

  // MIDI note numbers for cymbal choke events (sent when a choke button is held).
  #define choke1     78
  #define choke2     80
  #define choke3     82
  #define chokeRide  63

  // MIDI note numbers for the auxiliary digital trigger inputs.
  // All currently share note 47; change individually as needed.
  #define Aux1  47
  #define Aux2  47
  #define Aux3  47
  #define Aux4  47
  #define Aux5  47
  #define Aux6  47
  #define Aux7  47

  // Arduino Mega digital pin assignments for choke buttons (active-low, INPUT_PULLUP).
  #define Choke1_Pin     51
  #define Choke2_Pin     49
  #define Choke3_Pin     47
  #define ChokeRide_Pin  53

  // Arduino Mega digital pin assignments for auxiliary trigger inputs.
  #define Aux1_Pin  33
  #define Aux2_Pin  35
  #define Aux3_Pin  37
  #define Aux4_Pin  39
  #define Aux5_Pin  41
  #define Aux6_Pin  43
  #define Aux7_Pin  45

  // Last-read digital states for edge detection (LOW/HIGH).
  // Compared against the current digitalRead() result each loop to detect
  // press (HIGH→LOW) and release (LOW→HIGH) transitions.
  // Defined in consts.cpp; declared extern here so loop.cpp can read/write them.
  extern int Choke1_State;
  extern int Choke2_State;
  extern int Choke3_State;
  extern int ChokeRide_State;
  extern int Aux1_State;
  extern int Aux2_State;
  extern int Aux3_State;
  extern int Aux4_State;
  extern int Aux5_State;
  extern int Aux6_State;
  extern int Aux7_State;

  extern int currentSwitchState;  // Scratch variable for the current digitalRead() result

#endif //SUPERDRUM_DEFINE_H