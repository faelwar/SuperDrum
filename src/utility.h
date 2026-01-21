#pragma once
#include <Arduino.h>
//============================

//===============================
//  PRESCALER
//===============================
#if defined(__AVR__)
    // Maximum sampling frequency    // Resolution
  enum Prescaler {
      Prescaler_2 = B00000000, // 16 MHz / 2 = 8 MHz            //
      Prescaler_4 = B00000010, // 16 MHz / 4 = 4 MHz            // ~5.9
      Prescaler_8 = B00000011, // 16 MHz / 8 = 2 MHz            // ~7.4
      Prescaler_16 = B00000100, // 16 MHz / 16 = 1 MHz           // ~8.6
      Prescaler_32 = B00000101, // 16 MHz / 32 = 500 kHz         // ~8.9
      Prescaler_64 = B00000110, // 16 MHz / 64 = 250 kHz         // ~9.0
      Prescaler_128 = B00000111, // 16 MHz / 128 = 125 kHz        // ~9.1
  };

  inline void setPrescaler(int prescaler) {
    ADCSRA &= B11111000;
    ADCSRA |= prescaler;
  }
#endif


inline void setPrescaler(int prescaler);

void SendLog(byte Sensor,int N,int Y0,int MaxRetrigger,int MaxReading,byte State);

void SendProfiling();

//==============================
//    SOFTRESET
//==============================
void softReset();


//==============================
//    FREERAM
//==============================
int freeRam ();

//==============================
//    BLINK
//==============================
void blink();

inline int virtualAnalogRead(byte sensor,byte pin);

