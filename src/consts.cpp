//
// Created by Rafa Guerra on 01/01/26.
#include "consts.h"
#include <Arduino.h>
byte NSensor = 2;
bool Diagnostic=false;
short N=0;
mode Mode=MIDI;
unsigned long GlobalTime;

int MaxXtalkGroup[NXtalkGroup] = {-1};
int MaxMultiplexerXtalk[8]={-1};

byte LogPin=0xFF;
byte LogThresold=0xFF;

//===HiHat==================
byte HHNoteSensor[]     = {20,50,80,100};
byte HHThresoldSensor[] = {48,36,24,12};
byte HHFootNoteSensor[] = {59,48};
byte HHFootThresoldSensor[] = {127,127};
//===========================

byte GeneralXtalk;

void fastNoteOn(byte _channel, byte _note, byte _velocity) {
  #if TEXT_OUTPUT
    Serial.println("NoteOn");
	Serial.print("channel: ");
    Serial.println(_channel);
	Serial.print("note: ");
    Serial.println(_note);
	Serial.print("velocity: ");
    Serial.println(_velocity);
  #else
    #if ENABLE_CHANNEL
      Serial.write(0x90 | _channel);
    #else
      Serial.write(0x90 | 0x09);
    #endif
    Serial.write(_note);
    Serial.write(_velocity);
  #endif
}

void fastMidiCC(byte _channel, byte _number, byte _value) {
  #if TEXT_OUTPUT
    Serial.println("MidiCC");
	Serial.print("channel: ");
    Serial.println(_channel);
	Serial.print("number: ");
    Serial.println(_number);
	Serial.print("value: ");
    Serial.println(_value);
  #else
    #if ENABLE_CHANNEL
      Serial.write(0xB0 | _channel);
    #else
      Serial.write(0xB0 | 0x09);
    #endif
    Serial.write(_number);
    Serial.write(_value);
  #endif
}