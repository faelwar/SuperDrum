#include "settings.h"

#include <Arduino.h>
#include <EEPROM.h>

#include "consts.h"
#include "eeprom.h"
#include "pin.h"
#include "utility.h"
#include "define.h"
//===========SETTING============
const byte NOTE       = 0x00;
const byte THRESOLD   = 0x01;
const byte SCANTIME   = 0x02;
const byte MASKTIME   = 0x03;
const byte RETRIGGER  = 0x04;
const byte CURVE      = 0x05;
const byte XTALK      = 0x06;
const byte XTALKGROUP = 0x07;
const byte CURVEFORM  = 0x08;
const byte CHOKENOTE  = 0x09;
const byte DUAL       = 0x0A;
const byte PIN_TYPE   = 0x0D;
const byte CHANNEL    = 0x0E;
//===============================

//==============================
//    SETTING
//==============================
void SendPinSetting(byte pin, byte Set) {
  if (Set == 0x7F) {
    // All
    simpleSysex(0x02, pin, 0x00, Pin[pin].Note);
    simpleSysex(0x02, pin, 0x01, Pin[pin].Thresold);
    simpleSysex(0x02, pin, 0x02, Pin[pin].ScanTime);
    simpleSysex(0x02, pin, 0x03, Pin[pin].MaskTime);
    simpleSysex(0x02, pin, 0x04, Pin[pin].Retrigger);
    simpleSysex(0x02, pin, 0x05, Pin[pin].Curve);
    simpleSysex(0x02, pin, 0x06, Pin[pin].Xtalk);
    simpleSysex(0x02, pin, 0x07, Pin[pin].XtalkGroup);
    simpleSysex(0x02, pin, 0x08, Pin[pin].CurveForm);
    simpleSysex(0x02, pin, 0x09, Pin[pin].ChokeNote);
    simpleSysex(0x02, pin, 0x0A, DualSensor(pin));
    simpleSysex(0x02, pin, 0x0D, Pin[pin].Type);
    #if ENABLE_CHANNEL
      simpleSysex(0x02, pin, 0x0E, Pin[pin].Channel);
    #endif
    return;
  }

  simpleSysex(0x02, pin, Set, GetPinSetting(pin, Set));
}

byte GetPinSetting(byte pin, byte Set) {
  byte Value = 0;

  switch (Set) {
    case NOTE:
      Value = Pin[pin].Note;
      break;
    case THRESOLD:
      Value = Pin[pin].Thresold;
      break;
    case SCANTIME:
      Value = Pin[pin].ScanTime;
      break;
    case MASKTIME:
      Value = Pin[pin].MaskTime;
      break;
    case RETRIGGER:
      Value = Pin[pin].Retrigger;
      break;
    case CURVE:
      Value = Pin[pin].Curve;
      break;
    case XTALK:
      Value = Pin[pin].Xtalk;
      break;
    case XTALKGROUP:
      Value = Pin[pin].XtalkGroup;
      break;
    case CURVEFORM:
      Value = Pin[pin].CurveForm;
      break;
    case CHOKENOTE:
      Value = Pin[pin].ChokeNote;
      break;
    case DUAL:
      Value = DualSensor(pin);
      break;
    case PIN_TYPE:
      Value = Pin[pin].Type;
      break;
    case CHANNEL:
      #if ENABLE_CHANNEL
        Value = Pin[pin].Channel;
      #endif
      break;
  }

  return Value;
}

void SendHHSetting(byte Set) {
  byte Value = 0;

  if (Set == 0x7F) {
    simpleSysex(0x02, 0x4C, 0x00, HHNoteSensor[0]);
    simpleSysex(0x02, 0x4C, 0x01, HHNoteSensor[1]);
    simpleSysex(0x02, 0x4C, 0x02, HHNoteSensor[2]);
    simpleSysex(0x02, 0x4C, 0x03, HHNoteSensor[3]);
    simpleSysex(0x02, 0x4C, 0x04, HHThresoldSensor[0]);
    simpleSysex(0x02, 0x4C, 0x05, HHThresoldSensor[1]);
    simpleSysex(0x02, 0x4C, 0x06, HHThresoldSensor[2]);
    simpleSysex(0x02, 0x4C, 0x07, HHThresoldSensor[3]);
    simpleSysex(0x02, 0x4C, 0x08, HHFootNoteSensor[0]);
    simpleSysex(0x02, 0x4C, 0x09, HHFootNoteSensor[1]);
    simpleSysex(0x02, 0x4C, 0x0A, HHFootThresoldSensor[0]);
    simpleSysex(0x02, 0x4C, 0x0B, HHFootThresoldSensor[1]);
    return;
  } else if (Set < 4) {
    Value = HHNoteSensor[Set];
  } else if (Set < 8) {
    Value = HHThresoldSensor[Set - 4];
  } else if (Set < 10) {
    Value = HHFootNoteSensor[Set - 8];
  } else {
    Value = HHFootThresoldSensor[Set - 10];
  }

  simpleSysex(0x02, 0x4C, Set, Value);
}

void SendGeneralSetting(byte Set) {
  byte Value = 0;

  switch (Set) {
    case 0x00:
      Value = delayTime / 2;
      break;
    case 0x02:
      Value = NSensor;
      break;
    case 0x03:
      Value = GeneralXtalk;
      break;
    case 0x7F:
      simpleSysex(0x02, 0x7E, 0x00, (byte)(delayTime / 2));
      simpleSysex(0x02, 0x7E, 0x02, (byte)NSensor);
      simpleSysex(0x02, 0x7E, 0x03, (byte)GeneralXtalk);
      return;
  }

  simpleSysex(0x02, 0x7E, Set, Value);
}

void ExecCommand(int Cmd, int Data1, int Data2, int Data3) {
  switch (Cmd) {
    case 0x00:
      // AskMode
      simpleSysex(0x00, Mode, 0x00, 0x00);
      break;

    case 0x01:
      // SetMode
      Serial.flush();
      switch (Data1) {
        case Off:
          Mode = Off;
          break;
        case Standby:
          Mode = Standby;
          break;
        case MIDI:
          Mode = MIDI;
          break;
        case Tool:
          Mode = Tool;
          break;
      }
      simpleSysex(0x01, Mode, 0x00, 0x00);
      break;

    case 0x02:
      // AskSetting: Data1: general, pad ecc... Data2:delay, note, ecc.. Data3:value
      if (Data1 == 0x7E) {
        SendGeneralSetting(Data2);
      } else if (Data1 == 0x4C) {
        SendHHSetting(Data2);
      } else {
        SendPinSetting(Data1, Data2);
      }
      simpleSysex(0x02, 0x7F, 0x7F, 0x7F);
      break;

    case 0x03:
      // SetSetting: Data1: general, pad ecc... Data2:delay, note, ecc.. Data3:value
      if (Data1 == 0x7F) {
        // Non utilizzato
      } else if (Data1 == 0x7E) {
        // General
        switch (Data2) {
          case 0x02:
            NSensor = Data3;
            break;
          case 0x03:
            GeneralXtalk = Data3;
            break;
        }
      } else if (Data1 == 0x4C) {
        // HH
        switch (Data2) {
          case 0x00:
            HHNoteSensor[0] = Data3;
            break;
          case 0x01:
            HHNoteSensor[1] = Data3;
            break;
          case 0x02:
            HHNoteSensor[2] = Data3;
            break;
          case 0x03:
            HHNoteSensor[3] = Data3;
            break;
          case 0x04:
            HHThresoldSensor[0] = Data3;
            break;
          case 0x05:
            HHThresoldSensor[1] = Data3;
            break;
          case 0x06:
            HHThresoldSensor[2] = Data3;
            break;
          case 0x07:
            HHThresoldSensor[3] = Data3;
            break;
          case 0x08:
            HHFootNoteSensor[0] = Data3;
            break;
          case 0x09:
            HHFootNoteSensor[1] = Data3;
            break;
          case 0x0A:
            HHFootThresoldSensor[0] = Data3;
            break;
          case 0x0B:
            HHFootThresoldSensor[1] = Data3;
            break;
        }
      } else {
        switch (Data2) {
          case NOTE:
            Pin[Data1].Note = Data3;
            break;
          case THRESOLD:
            Pin[Data1].Thresold = Data3;
            break;
          case SCANTIME:
            Pin[Data1].ScanTime = Data3;
            break;
          case MASKTIME:
            Pin[Data1].MaskTime = Data3;
            break;
          case RETRIGGER:
            Pin[Data1].Retrigger = Data3;
            break;
          case CURVE:
            Pin[Data1].Curve = (curve)Data3;
            break;
          case XTALK:
            Pin[Data1].Xtalk = Data3;
            break;
          case XTALKGROUP:
            Pin[Data1].XtalkGroup = Data3;
            break;
          case CURVEFORM:
            Pin[Data1].CurveForm = Data3;
            break;
          case CHOKENOTE:
            Pin[Data1].ChokeNote = Data3;
            break;
          case PIN_TYPE:
            Pin[Data1].Type = (PinType)Data3;
            break;
          #if ENABLE_CHANNEL
            case CHANNEL:
              Pin[Data1].Channel = Data3;
              break;
          #endif
        }
      }
      break;

    case 0x04:
      // SaveSetting: Data1: general, pad ecc... Data2:delay, note, ecc.. Data3:value
      if (Data1 == 0x7F) {
        // Non utilizzato
      } else if (Data1 == 0x7E) {
        SaveGeneralEEPROM(Data2, Data3);
      } else if (Data1 == 0x4C) {
        SaveHHEEPROM(Data2, Data3);
      } else {
        SaveEEPROM(Data1, Data2, Data3);
      }
      break;

    case 0x6D:
      #if USE_PROFILER
        if (Data1 == 0) {
          TimeProf = 0;
          NProf = 0;
        } else if (Data1 == 1) {
          SendProfiling();
        }
      #endif
      break;

    case 0x6E:
      LogPin = Data1;
      LogThresold = Data2;
      N = 0;
      break;

    case 0x6F:
      Diagnostic = (Data1 == 1);
      break;

    case 0x61:
      #if defined(__AVR__)
        simpleSysex(0x61, Data1, Data2, EEPROM.read((Data1 * 256) + Data2));
      #endif
      break;

    case 0x7F:
      // RESET
      Serial.flush();
      Mode = Off;
      softReset();
      break;
  }
}

//==============================
//    INPUT
//==============================
void Input() {
  // HANDSHAKE
  while (Serial.peek() >= 0 && Serial.peek() != 0xF0) {
    Serial.read();
  }

  if (Serial.available() > 6) {
    byte Start = Serial.read();
    byte ID = Serial.read();
    int Cmd = Serial.read();
    int Data1 = Serial.read();
    int Data2 = Serial.read();
    int Data3 = Serial.read();
    byte End = Serial.read();
    Serial.println("Input");
    ExecCommand(Cmd, Data1, Data2, Data3);
  }
}