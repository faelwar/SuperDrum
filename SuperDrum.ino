//==============================================================================
//                    microDRUM/nanoDRUM firmware v1.4.0
//                           www.microdrum.net
//                            CC BY-NC-SA 3.0
//
// Massimo Bernava
// massimo.bernava@gmail.com
// 2016-02-10
//
// Adapted for Arduino Mega without multiplexer by:
//   Pedro Henrique Knup (facebook.com/pedroknup) - 06/06/2016
//   Marcos Cesar Monecchi - 22/06/2016
//==============================================================================

//==============================================================================
// CONFIGURATION
//==============================================================================
#define USE_LCD           0  // Use LCD
#define USE_595           1  // 1 with nanoDrum and v0.8
#define USE_PROFILER      0  // Use Profiler (only for Debug)
#define FASTADC           1  // FASTADC = Prescaler_16, VERYFASTADC = Prescaler_8, VERYVERYFASTADC = Prescaler_4
#define SERIALSPEED       0  // 1 = 115200, 0 = 31250 (MIDI), picoMIDI uses 115200, use 1 with nanoDrum and v0.8
#define USE_DEFAULT_NAME  0  // Use Default Name for pin in LCD Menu
#define USE_PISERIAL      0  // Use Raspberry Pi serial
#define USE_WAVTRIGGER    0  // Use WavTrigger
#define WT_16             0  // WT_16 = 16 Wav Trigger Input, WT_24 = 24 Wav Trigger Input
#define ENABLE_CHANNEL    0  // Enable MIDI Channel
#define MENU_LOG          0  // Enable Auto Tune (only with LCD)
#define NANO              1  // 1 = nanoDrum, 0 = microDrum
#define MEGA              1  // 1 = Arduino Mega without multiplexer, 0 = microDrum

//==============================================================================
// INCLUDES
//==============================================================================
#include "src/eeprom.h"
#include "src/setup.h"
#include "src/loop.h"
#include "src/menu.h"
#include "src/consts.h"

#if defined(__arm__)
  // TODO: https://github.com/sebnil/DueFlashStorage
#elif defined(__AVR__)
  #include <EEPROM.h>
#endif

#if defined(__AVR__)
  #if USE_WAVTRIGGER
    #include <SoftwareSerial.h>
  #endif
  #if USE_PISERIAL
    #include <SoftwareSerial.h>
  #endif
#endif

#if USE_LCD
  #if USE_595
    #include "lib/LiquidCrystal595.h"
  #else
    #include <lib/LiquidCrystal.h>
  #endif
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

//==============================================================================
// MEGA CONFIGURATION
//==============================================================================
#if MEGA

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

  // Digital pin states
  int Choke1_State     = LOW;
  int Choke2_State     = LOW;
  int Choke3_State     = LOW;
  int ChokeRide_State  = LOW;
  int Aux1_State       = LOW;
  int Aux2_State       = LOW;
  int Aux3_State       = LOW;
  int Aux4_State       = LOW;
  int Aux5_State       = LOW;
  int Aux6_State       = LOW;
  int Aux7_State       = LOW;

  int currentSwitchState = LOW;

#endif // MEGA