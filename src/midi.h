#pragma once

#include <Arduino.h>

//==============================
//    MIDI
//==============================
void noteOn(byte channel, byte note, byte velocity);
void noteOff(byte channel, byte note, byte velocity);
void midiCC(byte channel, byte number, byte value);

//=============SYSEX=========
void simpleSysex(byte cmd, byte data1, byte data2, byte data3);
void Sysex(byte cmd, byte* message, byte size);
//============================

void MIDI_TX(byte MESSAGE, byte PITCH, byte VELOCITY);