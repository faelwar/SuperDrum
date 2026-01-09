#pragma once
#include <Arduino.h>
//============================




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

