#pragma once

#include <Arduino.h>
#include "define.h"

//===HiHat==================
// Per-position MIDI notes and thresholds for the hi-hat bow.
// Index 0 = most open (lowest pedal position), 3 = almost closed.
// playMIDI() selects a note by comparing the HHC's MaxReading against these thresholds.
extern byte HHNoteSensor[];
extern byte HHThresoldSensor[];
// Notes and velocity thresholds for foot gestures (splash = quick open, close = quick close).
extern byte HHFootNoteSensor[];
extern byte HHFootThresoldSensor[];
//===========================

extern bool Diagnostic;  // When true, playTOOL() reports all hits bypassing xtalk suppression

extern short N;  // Running sample counter for Tool-mode waveform logging

//===========MODE============
// Operating modes — set via SysEx cmd 0x01 from the host editor.
//   Off:     firmware is idle; loop() exits immediately after Input()
//   Standby: ready but not scanning pads
//   MIDI:    normal operation — scan pads and emit MIDI
//   Tool:    host-editor mode — emit SysEx diagnostics instead of (or alongside) MIDI
enum mode : byte
{
  Off     = 0,
  Standby = 1,
  MIDI    = 2,
  Tool    = 3
};

// Hot-path note helpers — send Note On immediately followed by Note Off (velocity 0).
// Faster than calling noteOn+noteOff separately since there's no scheduling.
void fastNote(byte _channel, byte _note, byte _velocity);
void fastMidiCC(byte _channel, byte _number, byte _value);

//===GLOBAL========================
extern mode Mode;
extern unsigned long GlobalTime;  // Updated each loop iteration via TIMEFUNCTION (millis())
//=================================

const byte NPin = NUM_PINS;  // Total number of analog pad inputs (compile-time constant)

//=================================
//   DUAL SENSOR
// Maps a pin index to its paired companion pad.
// Used to link head↔rim (Piezo↔Switch) and HH bow↔HHC pairs so play() can
// check the partner's state at fire time.
// The lookup table reorders 8-slot groups: {3,2,1,0,6,7,4,5}.
//=================================
#define DualSensor(i) (_DualSensor[(i)&0x07]+((i)&0xF8))
const byte _DualSensor[] = {3,2,1,0,6,7,4,5};
//=================================

// Timestamp source. Uses millis() rather than micros() to avoid threshold errors
// caused by 32-bit overflow on fast-running loops.
#define TIMEFUNCTION millis()

//===Xtalk===================
const byte NXtalkGroup = 4;  // Maximum number of cross-talk groups
// Per-group peak: highest reading seen this loop among all pads in the same XtalkGroup.
// Reset to -1 at the end of each loop(). Used to suppress softer simultaneous hits.
extern int MaxXtalkGroup[NXtalkGroup];
// Per-mux-channel peak: highest reading on each multiplexer channel this loop.
// A pad is suppressed if another pad on the same channel read more than 2× its peak.
extern int MaxMultiplexerXtalk[8];
//===========================

// Platform-specific fast digital write.
// AVR: directly manipulates PORT registers to avoid digitalWrite() overhead.
// ARM: falls back to digitalWrite() since port arithmetic differs by chip.
#if defined(__arm__)
	#define fastWrite(_pin_, _state_) digitalWrite(_pin_, _state_);
#elif defined(__AVR__)
	//#define fastWrite(_pin_, _state_) ( _pin_ < 8 ? (_state_ ?  PORTD |= 1 << _pin_ : PORTD &= ~(1 << _pin_ )) : (_state_ ?  PORTB |= 1 << (_pin_ -8) : PORTB &= ~(1 << (_pin_ -8)  )))
	#define fastWrite(_pin_, _state_) (_state_ ?  PORTD |= 1 << _pin_ : PORTD &= ~(1 << _pin_ ))
#endif

// ANALOGREAD macro allows virtualAnalogRead() to be substituted for testing
// without changing call sites in pin.cpp.
#define ANALOGREAD(s,p) analogRead(s)

//===============================
//   LOG
//===============================
extern byte LogPin;       // Pin selected for waveform logging (0xFF = none)
extern byte LogThresold;  // Minimum reading to include in a log frame

//===SETTING================
//===General================
const int delayTime = 10;  // Loop delay in ms (stored in EEPROM as delayTime/2)
extern byte GeneralXtalk;  // Global xtalk sensitivity applied across all groups

extern byte NSensor;  // Number of active sensors (can be set below NPin to scan fewer pads)