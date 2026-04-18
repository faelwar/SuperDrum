#include "eeprom.h"
#include "settings.h"

// EEPROM memory map (AVR only — ARM TODO: DueFlashStorage):
//
//   Byte  0:    Reserved
//   Byte  1:    Version tag. Must equal 8 for stored settings to be loaded;
//               any other value triggers a first-boot write of the current defaults.
//   Bytes 2–49: General settings (indexed by Param: 0=delay, 2=NSensor, 3=Xtalk)
//   Bytes 50–61: HH settings (12 bytes, Param 0–11; see SendHHSetting for layout)
//   Bytes 100+: Per-pin settings at address 100 + (pin * 16) + param
//               Each pin occupies a 16-byte slot; params 0–13 are stored.

#if defined(__arm__)
  // TODO: https://github.com/sebnil/DueFlashStorage
#endif

// Load all settings from EEPROM on startup, or write defaults if the version tag
// is not present (first boot or after a firmware update that changes the layout).
void LoadAllEEPROM() {
  #if defined(__AVR__)
    byte Version = EEPROM.read(1);
    if (Version == 8) {
      // Valid stored settings — load them into RAM.
      LoadGeneralEEPROM(0);
      LoadGeneralEEPROM(2);
      LoadGeneralEEPROM(3);

      for (int h = 0; h < 12; h++) {
        LoadHHEEPROM(h);
      }

      for (int i = 0; i < NPin; i++) {
        for (int j = 0; j < 14; j++) {
          LoadEEPROM(i, j);
        }
      }
    } else {
      // First boot or version mismatch — persist current (default) values and
      // stamp the version byte so next boot loads from EEPROM.
      SaveGeneralEEPROM(0);
      SaveGeneralEEPROM(2);
      SaveGeneralEEPROM(3);

      for (int h = 0; h < 12; h++) {
        SaveHHEEPROM(h);
      }

      for (int i = 0; i < NPin; i++) {
        for (int j = 0; j < 14; j++) {
          SaveEEPROM(i, j);
        }
      }

      EEPROM.write(1, 8);  // Mark settings as valid
    }
  #endif
}

// Read one parameter for a pin from EEPROM and apply it via ExecCommand (SetSetting).
// Address: 100 + (pin * 16) + param. Values are clamped to 0–127 (MIDI safe range).
void LoadEEPROM(byte Pin, byte Param) {
  #if defined(__AVR__)
    byte Value = EEPROM.read(100 + (Pin * 16) + Param);
    if (Value > 127) Value = 127;
    ExecCommand(0x03, Pin, Param, Value);  // 0x03 = SetSetting
  #endif
}

// Write a new value for a pin parameter: update RAM via ExecCommand then persist
// to EEPROM. Called when the host editor sends a SaveSetting (cmd 0x04) message.
void SaveEEPROM(byte Pin, byte Param, byte Value) {
  #if defined(__AVR__)
    ExecCommand(0x03, Pin, Param, Value);
    EEPROM.write(100 + (Pin * 16) + Param, Value);
  #endif
}

// Write the current in-RAM value for a pin parameter to EEPROM (no RAM change).
// Used during the first-boot defaults dump in LoadAllEEPROM().
void SaveEEPROM(byte Pin, byte Param) {
  #if defined(__AVR__)
    EEPROM.write(100 + (Pin * 16) + Param, GetPinSetting(Pin, Param));
  #endif
}

// Write a new general parameter value: update RAM via ExecCommand then persist.
void SaveGeneralEEPROM(byte Param, byte Value) {
  #if defined(__AVR__)
    ExecCommand(0x03, 0x7E, Param, Value);
    EEPROM.write(Param, Value);
  #endif
}

// Write the current in-RAM general parameter value to EEPROM.
// Param 0x00 = delay (stored as delayTime/2), 0x02 = NSensor, 0x03 = GeneralXtalk.
void SaveGeneralEEPROM(byte Param) {
  #if defined(__AVR__)
    byte Value = 0;

    switch (Param) {
      case 0x00: Value = delayTime / 2; break;
      case 0x02: Value = NSensor;       break;
      case 0x03: Value = GeneralXtalk;  break;
    }

    if (Value > 127) Value = 127;
    EEPROM.write(Param, Value);
  #endif
}

// Write a new hi-hat parameter value: update RAM via ExecCommand then persist.
// HH settings live at EEPROM bytes 50–61 (address = 50 + Param).
void SaveHHEEPROM(byte Param, byte Value) {
  #if defined(__AVR__)
    ExecCommand(0x03, 0x4C, Param, Value);
    EEPROM.write(50 + Param, Value);
  #endif
}

// Write the current in-RAM HH parameter value to EEPROM.
// Parameter layout mirrors SendHHSetting(): 0–3 = notes, 4–7 = thresholds,
// 8–9 = foot notes, 10–11 = foot thresholds.
void SaveHHEEPROM(byte Param) {
  #if defined(__AVR__)
    byte Value = 0;

    if (Param < 4) {
      Value = HHNoteSensor[Param];
    } else if (Param < 8) {
      Value = HHThresoldSensor[Param - 4];
    } else if (Param < 10) {
      Value = HHFootNoteSensor[Param - 8];
    } else {
      Value = HHFootThresoldSensor[Param - 10];
    }

    if (Value > 127) Value = 127;
    EEPROM.write(50 + Param, Value);
  #endif
}

// Read a general parameter from EEPROM and apply it to RAM via ExecCommand.
void LoadGeneralEEPROM(byte Param) {
  #if defined(__AVR__)
    byte Value = EEPROM.read(Param);
    if (Value > 127) Value = 127;
    ExecCommand(0x03, 0x7E, Param, Value);
  #endif
}

// Read a hi-hat parameter from EEPROM (address 50 + Param) and apply to RAM.
void LoadHHEEPROM(byte Param) {
  #if defined(__AVR__)
    byte Value = EEPROM.read(50 + Param);
    if (Value > 127) Value = 127;
    ExecCommand(0x03, 0x4C, Param, Value);
  #endif
}