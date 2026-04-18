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
//
// SuperDrum / MicroMegaDrum — further adapted by Rafa Guerra
//
// Entry point for the Arduino build system. The .ino file must include all
// headers that define the setup() and loop() functions; the Arduino IDE and
// arduino-cli treat the .ino as the top-level translation unit.
//
// Architecture summary:
//   setup()  — initializes Serial, pins, ADC prescaler (src/setup.cpp)
//   loop()   — scans pads, fires MIDI, handles host-editor SysEx (src/loop.cpp)
//   pin[]    — per-pad FSM objects that handle ADC reading and hit detection
//   settings — bidirectional SysEx protocol for live parameter editing
//   eeprom   — persists all settings across power cycles
//==============================================================================
#include "src/define.h"   // Compile-time flags: NUM_PINS, TEXT_OUTPUT, FASTADC, CHOKE
#include "src/setup.h"    // setup() declaration

#include "src/eeprom.h"   // LoadAllEEPROM() — called during setup (currently disabled)
#include "src/loop.h"     // loop() declaration
#include "src/consts.h"   // Global variables, enums, and inline helpers