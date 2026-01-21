#include "midi.h"
#include "define.h"
//==============================
//    MIDI
//==============================
void noteOn(byte channel, byte note, byte velocity) {
  Serial.println("NoteOn-" + String(random(10,100)));
  #if TEXT_OUTPUT
    Serial.print("CHANNEL - ");
    Serial.println(channel);
    Serial.print("NOTE - ");
    Serial.println(note);
    Serial.print("VELOCITY - ");
    Serial.println(velocity);
  #else
    Serial.write(0x90 | channel);
    Serial.write(note);
    Serial.write(velocity);
  #endif
}

void noteOff(byte channel, byte note, byte velocity) {
  Serial.println("NoteOff");
  #if TEXT_OUTPUT
    Serial.print("CHANNEL - ");
    Serial.println(channel);
    Serial.print("NOTE - ");
    Serial.println(note);
    Serial.print("VELOCITY - ");
    Serial.println(velocity);
  #else
    Serial.write(0x80 | channel);
    Serial.write(note);
    Serial.write(velocity);
  #endif
}

void midiCC(byte channel, byte number, byte value) {
  Serial.println("NoteCC");
  #if TEXT_OUTPUT
    Serial.print("CHANNEL - ");
    Serial.println(channel);
    Serial.print("NOTE - ");
    Serial.println(number);
    Serial.print("VELOCITY - ");
    Serial.println(value);
  #else
    Serial.write(0xB0 | channel);
    Serial.write(number);
    Serial.write(value);
  #endif
}

//=============SYSEX=========
void simpleSysex(byte cmd, byte data1, byte data2, byte data3) {
  Serial.println("simpleSysex");

  Serial.write(0xF0);
  Serial.write(0x77);
  Serial.write(cmd);
  Serial.write(data1);
  Serial.write(data2);
  Serial.write(data3);
  Serial.write(0xF7);
}

void Sysex(byte cmd, byte* message, byte size) {
  Serial.println("Sysex");
  Serial.write(0xF0);
  Serial.write(0x77);
  Serial.write(cmd);
  Serial.write(size);
  Serial.write(message, size);
  Serial.write(0xF7);
}
//============================

void MIDI_TX(byte MESSAGE, byte PITCH, byte VELOCITY) {
  #if TEXT_OUTPUT
    Serial.println("MIDI_TX");
	Serial.print("MESSAGE - ");
    Serial.println(MESSAGE);
    Serial.print("PITCH - ");
    Serial.println(PITCH);
    Serial.print("VELOCITY - ");
    Serial.println(VELOCITY);
  #else
    Serial.write(MESSAGE);
    Serial.write(PITCH);
    Serial.write(VELOCITY);
  #endif
}