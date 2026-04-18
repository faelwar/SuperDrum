#pragma once

#include <Arduino.h>

//==============================
//    MIDI
// Functions for sending standard MIDI messages over Serial at 31250 baud.
// All output is conditioned on the TEXT_OUTPUT flag in define.h:
//   TEXT_OUTPUT 1 — prints human-readable strings for debugging
//   TEXT_OUTPUT 0 — sends raw MIDI bytes for use with a MIDI interface or DAW
//==============================

// Send a Note On event. In MIDI, Note On with velocity 0 is equivalent to
// Note Off — but we use the dedicated noteOff() function instead for clarity.
void noteOn(byte channel, byte note, byte velocity);

// Send a Note Off event (status byte 0x8n).
void noteOff(byte channel, byte note, byte velocity);

// Send a Control Change event (status byte 0xBn).
// Used for hi-hat pedal position (continuous controller).
void midiCC(byte channel, byte number, byte value);

//=============SYSEX=========
// Send a fixed 7-byte SysEx frame: F0 77 <cmd> <d1> <d2> <d3> F7.
// Used for bidirectional communication with the host editor tool.
void simpleSysex(byte cmd, byte data1, byte data2, byte data3);

// Send a variable-length SysEx frame for larger payloads (e.g., waveform log frames).
// Format: F0 77 <cmd> <size> <message[0..size-1]> F7
void Sysex(byte cmd, byte* message, byte size);
//============================

// Low-level raw MIDI transmitter. MESSAGE already contains the full status byte
// including the channel nibble (e.g., 0x9n for Note On on channel n).
// Used directly by the choke/aux digital-pin path in loop.cpp which pre-builds
// the status byte rather than going through noteOn/noteOff.
void MIDI_TX(byte MESSAGE, byte PITCH, byte VELOCITY);