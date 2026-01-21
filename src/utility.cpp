#include "utility.h"
#include "consts.h"
#include "midi.h"
#include "define.h"
//===============================

//===============================

//==============================
//   PROFILING 
//==============================
#define PROFA TimeProfA=micros();
#define PROFB TimeProf+=(micros()-TimeProfA); NProf++;
#if USE_PROFILER
  unsigned long TimeProfA;
  unsigned long TimeProf=0;
  unsigned long NProf=0;
#endif
//==============================

//==============================
//    SENDLOG
//==============================
void SendLog(byte Sensor,int N,int Y0,int MaxRetrigger,int MaxReading,byte State)
{
  //Time               4 bytes
  //Sensor             1 byte
  //N                  2 bytes
  //Reading            2 bytes
  //Y0                 2 bytes
  //MaxReading         2 bytes
  //State (Scan,Mask)  1 byte
  
  byte buf[14]; // Work only with Arduino serial (MIDI>127)
  
  buf[0] = (byte) GlobalTime;
  buf[1] = (byte) (GlobalTime >> 8);
  buf[2] = (byte) (GlobalTime >> 16);
  buf[3] = (byte) (GlobalTime >> 24);
  
  buf[4] = Sensor;

  buf[5] = (byte)N;
  buf[6] = (byte)(N>>8);
  
  buf[7] = (byte)MaxRetrigger;
  buf[8] = (byte)(MaxRetrigger>>8);
  
  buf[9] = (byte)Y0;
  buf[10] = (byte)(Y0>>8);
  
  buf[11] = (byte)MaxReading;
  buf[12] = (byte)(MaxReading>>8);
  
  buf[13] = State;
  
  Sysex(0x6E,buf,14);
  
}

//==============================
//    SENDPROFILING
//==============================
#if USE_PROFILER
  void SendProfiling()
  {
    byte buf[8];

    buf[0] = (byte) TimeProf; //Work only with Arduino serial (MIDI>127)
    buf[1] = (byte) (TimeProf >> 8);
    buf[2] = (byte) (TimeProf >> 16);
    buf[3] = (byte) (TimeProf >> 24);

    buf[4] = (byte) NProf;
    buf[5] = (byte) (NProf >> 8);
    buf[6] = (byte) (NProf >> 16);
    buf[7] = (byte) (NProf >> 24);

    Sysex(0x6D,buf,8);
  }
#endif

//==============================
//    SOFTRESET
//==============================
void softReset() {
  #if defined(__AVR__)
    asm volatile (" jmp 0");
  #endif 
//wdt_enable(WDTO_30MS);
}


//==============================
//    FREERAM
//==============================
int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

//==============================
//    BLINK
//==============================
void blink()
{
  digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(500);              // wait for a second
  digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
  delay(500);
}

//==============================
//    VIRTUALANALOGREAD
//==============================
const unsigned long virtualTmax=0;
const unsigned long virtualVmax=0;
const unsigned long virtualT50=0;

unsigned long virtualTime=0;
const unsigned long virtualFreq=1000;

inline int virtualAnalogRead(byte sensor,byte pin)
{
  if(pin!=LogPin) return analogRead(sensor);
  
  unsigned int T=GlobalTime-virtualTime;
  
  if(T>virtualFreq) virtualTime=GlobalTime;
  else
  {
    if(T<virtualTmax) return (T*virtualVmax)/virtualTmax;
    else if(T<(virtualTmax+(2*virtualT50))) return virtualVmax-(T-virtualTmax)*(virtualVmax/2)/virtualT50;
  }
  
  return 0;
}

