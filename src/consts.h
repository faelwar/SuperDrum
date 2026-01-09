#pragma once
#include <Arduino.h>
#define TEXT_OUTPUT 1
//===HiHat==================
extern byte HHNoteSensor[];
extern byte HHThresoldSensor[];
extern byte HHFootNoteSensor[];
extern byte HHFootThresoldSensor[];
//===========================
extern bool Diagnostic;

extern short N;//Unsent log

//===========MODE============
enum mode:byte
{
  Off     = 0,
  Standby = 1,
  MIDI    = 2,
  Tool    = 3
};

void fastNoteOn(byte _channel, byte _note, byte _velocity);

void fastMidiCC(byte _channel, byte _number, byte _value);

//===GLOBAL========================
extern mode Mode;
extern unsigned long GlobalTime;
//=================================
#if NANO    // Verifica o modelo de arduino e seta o numero de pinos: NANO= 24, MEGA=16, OUTROS=48
    const byte NPin=1;
#elif MEGA
    const byte NPin=16;
#else
    const byte NPin=48;
#endif


//=================================
//   DUAL SENSOR
//=================================
#define DualSensor(i) (_DualSensor[(i)&0x07]+((i)&0xF8))
const byte _DualSensor[] = {3,2,1,0,6,7,4,5};
//=================================

#define TIMEFUNCTION millis() //NOT micros() (thresold error)

//===Xtalk===================
const byte NXtalkGroup = 4;//Max number XTalk groups
extern int MaxXtalkGroup[NXtalkGroup];
extern int MaxMultiplexerXtalk[8];
//===========================

#if defined(__arm__)
	#define fastWrite(_pin_, _state_) digitalWrite(_pin_, _state_);
#elif defined(__AVR__)
	//#define fastWrite(_pin_, _state_) ( _pin_ < 8 ? (_state_ ?  PORTD |= 1 << _pin_ : PORTD &= ~(1 << _pin_ )) : (_state_ ?  PORTB |= 1 << (_pin_ -8) : PORTB &= ~(1 << (_pin_ -8)  )))
	#define fastWrite(_pin_, _state_) (_state_ ?  PORTD |= 1 << _pin_ : PORTD &= ~(1 << _pin_ ))
#endif

#define ANALOGREAD(s,p) analogRead(s)


//===============================
//   LOG
//===============================
extern byte LogPin;
extern byte LogThresold;

//===SETTING================
//===General================

const int delayTime = 10;
extern byte GeneralXtalk;


extern byte NSensor;