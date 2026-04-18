#pragma once
#include <Arduino.h>

//===============================
//  PRESCALER
// Reducing the ADC prescaler speeds up analogRead() at the cost of slight
// resolution loss. At 16 MHz, Prescaler_16 gives a 1 MHz ADC clock and
// ~8.6-bit effective resolution — a good balance for piezo sensing.
// Set with setPrescaler() in setup() when FASTADC is enabled.
//===============================
#if defined(__AVR__)
  enum Prescaler {
    Prescaler_2   = B00000000, // ADC clock = 8 MHz
    Prescaler_4   = B00000010, // ADC clock = 4 MHz
    Prescaler_8   = B00000011, // ADC clock = 2 MHz    (~7.4-bit effective resolution)
    Prescaler_16  = B00000100, // ADC clock = 1 MHz    (~8.6-bit) — used by FASTADC
    Prescaler_32  = B00000101, // ADC clock = 500 kHz  (~8.9-bit)
    Prescaler_64  = B00000110, // ADC clock = 250 kHz  (~9.0-bit)
    Prescaler_128 = B00000111, // ADC clock = 125 kHz  (~9.1-bit) — Arduino default
  };

  // Directly write the lower 3 bits of ADCSRA to set the ADC clock prescaler.
  inline void setPrescaler(int prescaler) {
    ADCSRA &= B11111000;
    ADCSRA |= prescaler;
  }
#endif

inline void setPrescaler(int prescaler);

// Send a 14-byte SysEx waveform log frame to the host editor.
// Contains a timestamp, sensor index, sample counter, current reading,
// retrigger level, peak reading, and FSM state. See utility.cpp for byte layout.
void SendLog(byte Sensor, int N, int Y0, int MaxRetrigger, int MaxReading, byte State);

// Send accumulated loop profiling statistics (total time and sample count)
// to the host editor. Only compiled when USE_PROFILER is defined.
void SendProfiling();

//==============================
//    SOFTRESET
// Jump to address 0 on AVR to re-run the bootloader and setup() without
// a physical power cycle. Used by ExecCommand(0x7F) (Reset command).
//==============================
void softReset();

//==============================
//    FREERAM
// Returns the number of free bytes between the top of the heap and the
// bottom of the stack. Useful for diagnosing memory pressure on AVR.
//==============================
int freeRam();

//==============================
//    BLINK
// Blink the built-in LED (pin 13) once. Used for debug signalling.
//==============================
void blink();

// Virtual analog read for testing the waveform logging pipeline without
// physical pads. Returns a synthetic triangular pulse for the selected LogPin
// instead of calling analogRead(). Currently inactive (all constants are 0).
inline int virtualAnalogRead(byte sensor, byte pin);