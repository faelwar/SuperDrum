#include "pin.h"

//===========CURVE============
const float _Exp[9]   = {2.33, 3.85, 6.35, 10.48, 17.28, 28.5, 46.99, 77.47, 127.74};
const float _Log[9]   = {0, 83.67, 98.23, 106.74, 112.78, 117.47, 121.30, 124.53, 127.34};
const float _Sigma[9] = {2.28, 6.02, 15.13, 34.15, 63.5, 92.84, 111.86, 120.97, 127.71};
const float _Flat[9]  = {0, 32.86, 46.42, 55.82, 64.0, 72.17, 81.57, 95.13, 127};
//============================

//===========================
//   DEFAULT PIN
//===========================

#if MEGA

const byte DP_HHC       = 0x01;
const byte DP_SNAREHEAD = 0x00;
const byte DP_SNARERIM  = 0x02;
const byte DP_KICK      = 0x03;
const byte DP_TOM1HEAD  = 0x04;
const byte DP_TOM2HEAD  = 0x05;
const byte DP_TOM3HEAD  = 0x06;
const byte DP_TOM4HEAD  = 0x07;
const byte DP_EXTRA1    = 0x08;
const byte DP_EXTRA2    = 0x09;
const byte DP_HHBOW     = 0x0A;
const byte DP_CHINA     = 0x0B;
const byte DP_SPLASH    = 0x0C;
const byte DP_CRASHEDGE = 0x0D;
const byte DP_RIDEBOW   = 0x0E;
const byte DP_RIDEEDGE  = 0x0F;

#else

const byte DP_SNAREHEAD  = 0x04;
const byte DP_SNARERIM   = 0x06;
const byte DP_KICK       = 0x07;
const byte DP_EFFECT     = 0x05;
const byte DP_HHBOW      = 0x03;
const byte DP_HHEDGE     = 0x00;
const byte DP_HHC        = 0x01;
const byte DP_HHC_RING   = 0x02;

const byte DP_RIDEBOW    = 0x0C;
const byte DP_RIDEEDGE   = 0x0E;
const byte DP_TOM1HEAD   = 0x0F;
const byte DP_TOM1RIM    = 0x0D;
const byte DP_TOM2HEAD   = 0x0B;
const byte DP_TOM2RIM    = 0x08;
const byte DP_CRASHBOW   = 0x09;
const byte DP_CRASHEDGE  = 0x0A;

const byte DP_TOM3HEAD   = 0x14;
const byte DP_TOM3RIM    = 0x16;
const byte DP_TOM4HEAD   = 0x17;
const byte DP_TOM4RIM    = 0x15;
const byte DP_CRASH2BOW  = 0x13;
const byte DP_CRASH2EDGE = 0x10;
const byte DP_CHINA      = 0x11;
const byte DP_SPLASH     = 0x12;

#endif

//===========PIN============
pin::pin() {
  Type = Disabled;
  Note = 0;

  Gain = 20;
  #if ENABLE_CHANNEL
    Channel = 9;
  #endif

  Thresold = 20;
  ScanTime = 10;
  MaskTime = 30;
  Retrigger = 30;

  Curve = Linear;
  CurveForm = 110;
  Xtalk = 0;
  XtalkGroup = 0;

  MaxReading = -1;
}

//===============================
//   SET
//===============================
void pin::set(byte pin) {
  Time = TIMEFUNCTION;
  this->Time = Time + this->MaskTime;

  #if MEGA
    this->_pin = pin;
  #endif

  switch (pin) {
    case DP_SNAREHEAD:
      this->Type = Piezo;
      this->Note = 38;
      break;
    case DP_SNARERIM:
      this->Type = Piezo;
      this->Note = 42;
      break;
    case DP_KICK:
      this->Type = Piezo;
      this->Note = 36;
      break;
    case DP_TOM1HEAD:
      this->Type = Piezo;
      this->Note = 71;
      break;
    case DP_TOM2HEAD:
      this->Type = Piezo;
      this->Note = 69;
      break;
    case DP_TOM3HEAD:
      this->Type = Piezo;
      this->Note = 67;
      break;
    case DP_TOM4HEAD:
      this->Type = Piezo;
      this->Note = 65;
      break;
    #if MEGA
      case DP_EXTRA1:
        this->Type = Piezo;
        this->Note = 47;
        break;
      case DP_EXTRA2:
        this->Type = Piezo;
        this->Note = 73;
        break;
    #endif
    case DP_HHBOW:
      this->Type = Piezo;
      this->Note = 8;
      break;
    case DP_CHINA:
      this->Type = Piezo;
      this->Note = 77;
      break;
    case DP_SPLASH:
      this->Type = Piezo;
      this->Note = 79;
      break;
    case DP_CRASHEDGE:
      this->Type = Piezo;
      this->Note = 81;
      break;
    case DP_HHC:
      this->Type = HHC;
      this->Thresold = 10;
      break;
  }
}

//===============================
//   SCAN
//===============================
void pin::scan(byte sensor, byte count) {
  byte pin = count + (sensor << 3);
  int yn_0 = -1;

  //===============================
  //        HHC
  //===============================
  #if MEGA
    if (Type == HHC) {
      scanHHC(_pin, analogRead(_pin));
      return;
    }
  #else
    if (Type == HHC) {
      scanHHC(pin, analogRead(sensor) / 8);
      return;
    }
  #endif

  //===============================
  //        Switch
  //===============================
  if (Type == Switch) {
    #if MEGA
      yn_0 = analogRead(_pin);
    #else
      yn_0 = analogRead(sensor);
    #endif

    if (State == Normal_Time) {
      if (yn_0 < Thresold * 10 && yn_1 < Thresold * 10) {
        State = Scan_Time;
        MaxReading = 0;
      }
    } else if (State == Scan_Time) {
      if (yn_0 < Thresold * 10 && yn_1 < Thresold * 10) {
        MaxReading = MaxReading + 1;
      } else {
        if (MaxReading > ScanTime)
          State = Switch_Time;
        else
          State = Normal_Time;
      }

      if (MaxReading > MaskTime) State = Choke_Time;
    } else if (State == Mask_Time) {
      if (MaxReading > 0) {
        MaxReading = MaxReading - 1;
      } else {
        if (yn_0 >= Thresold * 10 && yn_1 >= Thresold * 10)
          State = Normal_Time;
      }
    }
  }
  //===============================
  //        YSwitch
  //===============================
  else if (Type == YSwitch) {
    yn_0 = analogRead(sensor);

    if (yn_0 < Thresold * 4) {
      State = Scan_Time;

      if (MaxReading <= MaskTime) MaxReading = MaxReading + 1;
    } else {
      if (MaxReading != 255 && MaxReading > ScanTime) {
        if (yn_0 > CurveForm * 4)
          MaxReading = MaxReading;
        else
          MaxReading = 512 + MaxReading;
      }
      State = Switch_Time;
    }
  }
  //===============================
  //        Piezo, HH
  //===============================
  else {
    GlobalTime = TIMEFUNCTION;

    if (State == Mask_Time) {
      if ((GlobalTime - Time) > MaskTime) {
        State = Retrigger_Time;
        Time = GlobalTime;
      }
    }

    #if MEGA
      yn_0 = 0.5 + ((float)analogRead(sensor) * (float)Gain) / 64.0;
    #else
      yn_0 = 0.5 + ((float)ANALOGREAD(sensor, pin) * (float)Gain) / 64.0;
    #endif

    if (State == Retrigger_Time) {
      int MaxRetriggerSensor = MaxReading - ((GlobalTime - Time) * (Retrigger + 1) / 16);
      if (MaxRetriggerSensor > 0) {
        if ((yn_0 - yn_1) > Thresold && yn_0 > MaxRetriggerSensor) {
          State = Scan_Time;
          Time = GlobalTime;
          MaxReading = 0;
        }
      } else {
        State = Normal_Time;
      }
    } else if (State == Normal_Time) {
      if ((yn_0 - yn_1) > Thresold) {
        State = Scan_Time;
        Time = GlobalTime;
        MaxReading = 0;
      }
    } else if (State == Scan_Time) {
      if ((GlobalTime - Time) < ScanTime) {
        if (yn_0 > MaxReading) {
          MaxReading = yn_0;

          if (MaxXtalkGroup[XtalkGroup] == -1 || MaxXtalkGroup[XtalkGroup] < yn_0)
            MaxXtalkGroup[XtalkGroup] = yn_0;

          if (MaxMultiplexerXtalk[count] == -1 || MaxMultiplexerXtalk[count] < yn_0)
            MaxMultiplexerXtalk[count] = yn_0;
        }
      } else {
        State = Piezo_Time;
      }
    }
  }

  //====================================
  // TOOL
  //====================================
  if (Mode == Tool && LogPin == pin) LogTool(yn_0, pin);
  //====================================

  yn_1 = yn_0;
}

//===============================
//   PLAY
//===============================
void pin::play(byte i, pin* dual) {
  //===============================
  //        Disabled
  //===============================
  if (Type == Disabled) return;

  //===============================
  //        HHC
  //===============================
  if (Type == HHC) {
    if (State == Footsplash_Time) {
      if (Mode == MIDI) fastNoteOn(Channel, HHFootNoteSensor[0], 127);
      State = Normal_Time;
    } else if (State == Footclose_Time) {
      if (Mode == MIDI) fastNoteOn(Channel, HHFootNoteSensor[1], 127);
      State = Normal_Time;
    }
    return;
  }

  if (State == Normal_Time || State == Scan_Time || State == Mask_Time || State == Retrigger_Time)
    return;

  //=============XTALK==================
  if (Mode == Tool && Diagnostic == true) {
    playTOOL(i, dual);
  } else {
    if (MaxMultiplexerXtalk[i % 8] != -1 && MaxMultiplexerXtalk[i % 8] > (2 * MaxReading)) {
      State = Normal_Time;
      return;
    }
    if (MaxXtalkGroup[XtalkGroup] != -1 && MaxXtalkGroup[XtalkGroup] > (MaxReading + (64 - Xtalk) * 4)) {
      State = Normal_Time;
      return;
    }
    playMIDI(i, dual);
  }
}

//===============================
//   PLAYTOOL
//===============================
void pin::playTOOL(byte i, pin* dual) {
  //===============================
  //        Switch
  //===============================
  if (Type == Switch) {
    simpleSysex(0x6F, i, MaxReading, 0);

    if (State == Switch_Time) {
      State = Mask_Time;
      MaxReading = -1;
    }
    return;
  }

  //===============================
  //          YSwitch
  //===============================
  if (Type == YSwitch) {
    simpleSysex(0x6F, i, MaxReading, 0);
    MaxReading = -1;
    return;
  }

  //===============================
  //        Piezo, HH
  //===============================
  if (State == Piezo_Time) {
    if (Type == Piezo) {
      simpleSysex(0x6F, i, useCurve(), 0);

      State = Mask_Time;

      // Piezo-Switch
      if (dual->Type == Switch && dual->State == Switch_Time) {
        simpleSysex(0x6F, DualSensor(i), 127, 0);

        dual->State = Mask_Time;
        dual->MaxReading = -1;
      }
    } else {
      // HH
      simpleSysex(0x6F, i, useCurve(), 0);
    }

    MaxReading = -1;
  }
}

//===============================
//   PLAYMIDI
//===============================
void pin::playMIDI(byte i, pin* dual) {
  //===============================
  //        Single Switch
  //===============================
  if (Type == Switch) {
    if (State == Switch_Time) {
      fastNoteOn(Channel, Note, 127);

      #if USE_WAVTRIGGER
        wavTrigger(i, 126);
      #endif
      #if USE_PISERIAL
        piNote(Note, 126);
      #endif

      State = Mask_Time;
      MaxReading = Retrigger;
    } else if (State == Choke_Time) {
      fastNoteOn(Channel, ChokeNote, 127);

      #if USE_WAVTRIGGER
        wavChoke(i);
      #endif

      State = Mask_Time;
      MaxReading = Retrigger;
    }
    return;
  }
  //===============================

  GlobalTime = TIMEFUNCTION;

  //===============================
  //          YSwitch
  //===============================
  if (Type == YSwitch) {
    if (MaxReading <= 512) {
      fastNoteOn(Channel, Note, min(127, MaxReading * 8));
    } else {
      fastNoteOn(Channel, DualSensor(i), min(127, (MaxReading - 512) * 8));
    }

    if (DualSensor(i) != 127) {
      dual->MaxReading = -1;
      dual->Time = GlobalTime - dual->ScanTime;
    }

    MaxReading = -1;
    return;
  }
  //===============================

  //====================================================================
  if (State == Piezo_Time) {
    if (Type == Piezo) {
      byte v = useCurve();

      #if USE_WAVTRIGGER
        wavTrigger(i, v);
      #endif

      #if USE_PISERIAL
        piNote(Note, v);
      #endif

      fastNoteOn(Channel, Note, v);

      State = Mask_Time;

      // Piezo-Switch
      if (dual->Type == Switch && dual->State == Switch_Time) {
        fastNoteOn(dual->Channel, dual->Note, 127);

        #if USE_WAVTRIGGER
          wavTrigger(DualSensor(i), 126);
        #endif
        #if USE_PISERIAL
          piNote(DualSensor(i), 126);
        #endif

        dual->State = Mask_Time;
      }
    } else {
      // HH
      byte note = Note;

      if (dual->MaxReading > dual->Thresold)
        note = ChokeNote;
      else if (dual->MaxReading > HHThresoldSensor[3])
        note = HHNoteSensor[3];
      else if (dual->MaxReading > HHThresoldSensor[2])
        note = HHNoteSensor[2];
      else if (dual->MaxReading > HHThresoldSensor[1])
        note = HHNoteSensor[1];
      else if (dual->MaxReading > HHThresoldSensor[0])
        note = HHNoteSensor[0];

      fastNoteOn(Channel, note, useCurve());
    }
  }
}
//=======================

//===============================
//   USECURVE
//===============================
byte pin::useCurve() {
  int ret = 0;
  float f = ((float)CurveForm) / 32.0;

  if (Curve == Linear) {
    ret = 0.5 + ((float)MaxReading * f / 8.0);
  } else {
    int i = MaxReading / 128;
    int m = MaxReading % 128;

    switch (Curve) {
      case Exp:
        ret = 0.5 + (((float)m * (_Exp[i + 1] - _Exp[i]) / 128.0) + _Exp[i]) * f;
        break;
      case Log:
        ret = 0.5 + (((float)m * (_Log[i + 1] - _Log[i]) / 128.0) + _Log[i]) * f;
        break;
      case Sigma:
        ret = 0.5 + (((float)m * (_Sigma[i + 1] - _Sigma[i]) / 128.0) + _Sigma[i]) * f;
        break;
      case Flat:
        ret = 0.5 + (((float)m * (_Flat[i + 1] - _Flat[i]) / 128.0) + _Flat[i]) * f;
        break;
      default:
        ret = i * 16;
        break;
    }
  }

  if (ret <= 0) return 0;
  if (ret >= 127) return 127;
  return ret;
}

//===============================
//   SCANHHC
//===============================
void pin::scanHHC(byte pin, byte sensorReading) {
  if ((GlobalTime - Time) > MaskTime) {
    if (sensorReading > (MaxReading + Thresold) || sensorReading < (MaxReading - Thresold)) {
      if (Mode == MIDI) {
        #if USE_WAVTRIGGER
          wavTriggerHHC(sensorReading);
        #endif

        fastMidiCC(Channel, Note, sensorReading);
      } else if (Mode == Tool && Diagnostic == true) {
        simpleSysex(0x6F, pin, sensorReading, 0);
      }

      float m = (((float)MaxReading - (float)sensorReading) / ((float)Time - (float)GlobalTime)) * 100;

      MaxReading = sensorReading;

      // Foot Splash
      if (m > 0 && m > HHFootThresoldSensor[0])
        State = Footsplash_Time;
      // FootClose
      else if (m < 0 && -m > HHFootThresoldSensor[1])
        State = Footclose_Time;

      Time = GlobalTime;
    }
  }
}

//===========================

pin Pin[NPin];