#pragma once

#include <Arduino.h>

// Bidirectional settings interface between the firmware and the host editor tool.
// The host editor communicates via SysEx over the same 31250-baud serial line used
// for MIDI. ExecCommand() dispatches all inbound commands; the Send* functions
// respond with current values.

//==============================
//    SETTING
// Send current parameter values back to the host editor as SysEx packets.
// Set == 0x7F triggers a full dump of all parameters for the given target.
//==============================

// Send one or all per-pin parameters for the specified pin index.
void SendPinSetting(byte pin, byte Set);

// Read a single per-pin parameter from the Pin[] array and return its value.
// Used both for SysEx responses and for reading the current value before saving to EEPROM.
byte GetPinSetting(byte pin, byte Set);

// Send one or all hi-hat parameters (notes, thresholds, foot gesture config).
void SendHHSetting(byte Set);

// Send one or all general parameters (loop delay, sensor count, global xtalk).
void SendGeneralSetting(byte Set);

// Dispatch a parsed SysEx command from the host editor.
// Cmd 0x00–0x04 handle mode queries, mode changes, and settings read/write/save.
// Cmd 0x6D–0x7F handle diagnostics, logging, profiling, EEPROM reads, and reset.
// See settings.cpp for the full command reference.
void ExecCommand(int Cmd, int Data1, int Data2, int Data3);

//==============================
//    INPUT
// Read and parse one incoming SysEx frame from Serial (if available).
// Should be called at the top of every loop() iteration.
// Discards any non-SysEx bytes before the 0xF0 start byte.
//==============================
void Input();