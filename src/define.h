//
// Created by Rafa Guerra on 16/01/26.
//
#pragma once
#ifndef SUPERDRUM_DEFINE_H
  #define SUPERDRUM_DEFINE_H
  #define TEXT_OUTPUT 0
  #define USE_PROFILER 0
  #define FASTADC 1
  #define SERIALSPEED 0
  #define USE_DEFAULT_NAME 0
  #define ENABLE_CHANNEL 1
  #define NANO 1
  #define NUM_PINS 1
  #define CHOKE 0

  #if defined(__arm__)
    // TODO: https://github.com/sebnil/DueFlashStorage
  #elif defined(__AVR__)
    #include <EEPROM.h>
  #endif

  //==============================================================================
  // REGISTER BIT MACROS
  //==============================================================================
  #ifndef cbi
    #define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
  #endif
  #ifndef sbi
    #define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
  #endif

  // Choke notes
  #define choke1     78
  #define choke2     80
  #define choke3     82
  #define chokeRide  63

  // Auxiliary digital plug notes
  #define Aux1  47
  #define Aux2  47
  #define Aux3  47
  #define Aux4  47
  #define Aux5  47
  #define Aux6  47
  #define Aux7  47

  // Choke digital pins
  #define Choke1_Pin     51
  #define Choke2_Pin     49
  #define Choke3_Pin     47
  #define ChokeRide_Pin  53

  // Auxiliary digital pins
  #define Aux1_Pin  33
  #define Aux2_Pin  35
  #define Aux3_Pin  37
  #define Aux4_Pin  39
  #define Aux5_Pin  41
  #define Aux6_Pin  43
  #define Aux7_Pin  45

  // Digital pin states (declared here, defined in consts.cpp)
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

  extern int currentSwitchState;

#endif //SUPERDRUM_DEFINE_H