#include "midi.h"
#include "define.h"

// All MIDI output goes through Serial at 31250 baud (standard MIDI rate).
// When TEXT_OUTPUT is 1 (debug mode), human-readable strings are printed instead
// of raw MIDI bytes so the serial monitor can display events without a MIDI
// interface. Set TEXT_OUTPUT to 0 for real MIDI output to a sound module or DAW.

// Send a Note On message (status byte 0x9n, where n = channel).
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

// Send a Note Off message (status byte 0x8n).
// In TEXT_OUTPUT mode, Note Off is suppressed to reduce debug noise —
// only Note On events are printed.
void noteOff(byte channel, byte note, byte velocity) {
  //Serial.println("NoteOff");
  #if TEXT_OUTPUT
    //Serial.print("CHANNEL - ");
    //Serial.println(channel);
    //Serial.print("NOTE - ");
    //Serial.println(note);
    //Serial.print("VELOCITY - ");
    //Serial.println(velocity);
  #else
    Serial.write(0x80 | channel);
    Serial.write(note);
    Serial.write(velocity);
  #endif
}

// Send a Control Change message (status byte 0xBn).
// Used for the hi-hat pedal position (continuous controller).
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

// Send a 7-byte SysEx packet with a fixed manufacturer ID (0x77) and four
// data bytes. Used by the Tool mode host editor to exchange pin diagnostics
// and settings. Format: F0 77 <cmd> <data1> <data2> <data3> F7
void simpleSysex(byte cmd, byte data1, byte data2, byte data3) {
  Serial.println("simpleSysex");

  Serial.write(0xF0);   // SysEx start
  Serial.write(0x77);   // Manufacturer ID
  Serial.write(cmd);
  Serial.write(data1);
  Serial.write(data2);
  Serial.write(data3);
  Serial.write(0xF7);   // SysEx end
}

// Send a variable-length SysEx packet for larger payloads (e.g., log frames).
// Format: F0 77 <cmd> <size> <message[0..size-1]> F7
void Sysex(byte cmd, byte* message, byte size) {
  Serial.println("Sysex");
  Serial.write(0xF0);
  Serial.write(0x77);
  Serial.write(cmd);
  Serial.write(size);
  Serial.write(message, size);
  Serial.write(0xF7);
}

// Low-level raw MIDI transmitter. MESSAGE is the full status byte (already
// includes channel bits), so this bypasses channel selection logic. Used
// directly by the choke/aux digital-pin path in loop.cpp.
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