#pragma once
#include <Arduino.h>
#include <EEPROM.h>
#include "consts.h"
#include "define.h"

// Persistent storage for all tunable parameters (AVR EEPROM, 1024 bytes on Nano/Uno).
//
// Memory map:
//   Byte  0:    Reserved
//   Byte  1:    Version tag (must be 8 to load stored settings on boot)
//   Bytes 2–49: General settings (delay, NSensor, GeneralXtalk — indexed by Param)
//   Bytes 50–61: Hi-hat settings (12 bytes; see SendHHSetting for layout)
//   Bytes 100+: Per-pin settings at address 100 + (pin * 16) + param
//               Each pin occupies a 16-byte slot; params 0–13 map to pin fields.
//
// All values are clamped to 0–127 on load to stay within MIDI-safe range.

// Load all settings from EEPROM on startup. If the version byte is not 8
// (first boot or firmware update), writes current compiled defaults to EEPROM
// and stamps the version byte.
void LoadAllEEPROM();

// Read one pin parameter from EEPROM and apply it to the Pin[] array in RAM.
void LoadEEPROM(byte Pin, byte Param);

// Write a new value for a pin parameter: update RAM immediately then persist to EEPROM.
void SaveEEPROM(byte Pin, byte Param, byte Value);

// Write the current in-RAM value for a pin parameter to EEPROM (no RAM change).
void SaveEEPROM(byte Pin, byte Param);

// Write a new general parameter value to both RAM and EEPROM.
void SaveGeneralEEPROM(byte Param, byte Value);

// Write the current in-RAM general parameter value to EEPROM.
void SaveGeneralEEPROM(byte Param);

// Write a new hi-hat parameter value to both RAM and EEPROM.
void SaveHHEEPROM(byte Param, byte Value);

// Write the current in-RAM hi-hat parameter value to EEPROM.
void SaveHHEEPROM(byte Param);

// Read a general parameter from EEPROM and apply it to RAM.
void LoadGeneralEEPROM(byte Param);

// Read a hi-hat parameter from EEPROM and apply it to RAM.
void LoadHHEEPROM(byte Param);