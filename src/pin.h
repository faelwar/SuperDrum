#pragma once

#include <Arduino.h>

#include "consts.h"
#include "loop.h"
#include "midi.h"

//===========TIME============
enum state : byte {
  Normal_Time        = 0,
  Scan_Time          = 1,
  Mask_Time          = 2,
  Retrigger_Time     = 3,
  Piezo_Time         = 4,
  Switch_Time        = 5,
  Choke_Time         = 6,
  Footsplash_Time    = 7,
  Footclose_Time     = 8,
  Scanretrigger_Time = 9
};
//============================

//===========TYPE============
enum PinType : byte {
  Piezo    = 0,
  Switch   = 1,
  HHC      = 2,
  HH       = 3,
  HHs      = 4,
  YSwitch  = 5,
  Disabled = 15
};

//===========CURVE============
enum curve : byte {
  Linear = 0,
  Exp    = 1,
  Log    = 2,
  Sigma  = 3,
  Flat   = 4
};

//===========================
//   PIN
//===========================
class pin {
public:
  pin();

  //===============================
  //   SET
  //===============================
  void set(byte pin);

  //===============================
  //   SCAN
  //===============================
  void scan(byte sensor, byte count);

  //===============================
  //   PLAY
  //===============================
  void play(byte i, pin* dual);

  //===============================
  //   PLAYTOOL
  //===============================
  void playTOOL(byte i, pin* dual);

  //===============================
  //   PLAYMIDI
  //===============================
  void playMIDI(byte i, pin* dual);

  //=======================
  byte Type : 4;
  byte State : 4;
  byte _pin;

  byte Note;
  union {
    byte ChokeNote;
    byte Gain;
  };

  byte Channel;

  byte Thresold;
  byte ScanTime;
  byte MaskTime;
  byte Retrigger;

  byte CurveForm;
  byte Curve : 4;
  byte XtalkGroup : 4;
  byte Xtalk;

  unsigned long Time;
  int MaxReading;
  int yn_1;

  //===============================
  //   USECURVE
  //===============================
  byte useCurve();

  //===============================
  //   SCANHHC
  //===============================
  void scanHHC(byte pin, byte sensorReading);
};
//===========================

extern pin Pin[NPin];