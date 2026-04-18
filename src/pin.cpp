#include "pin.h"
#include "define.h"

// Lookup tables for velocity curves. Each table has 9 control points covering
// the input range 0–1024 (split into 8 segments of 128 steps). useCurve()
// interpolates linearly between adjacent points and scales by CurveForm.
const float _Exp[9]   = {2.33, 3.85, 6.35, 10.48, 17.28, 28.5, 46.99, 77.47, 127.74};
const float _Log[9]   = {0, 83.67, 98.23, 106.74, 112.78, 117.47, 121.30, 124.53, 127.34};
const float _Sigma[9] = {2.28, 6.02, 15.13, 34.15, 63.5, 92.84, 111.86, 120.97, 127.71};
const float _Flat[9]  = {0, 32.86, 46.42, 55.82, 64.0, 72.17, 81.57, 95.13, 127};

// Default pin indices for each drum pad, selected by NUM_PINS at compile time.
// These map a logical pad slot to a physical analog input index.
#if NUM_PINS == 1
  const byte DP_SNAREHEAD = 0x00;
#elif NUM_PINS == 4
  const byte DP_SNAREHEAD = 0x00;
  const byte DP_TOM1HEAD  = 0x01;
  const byte DP_TOM2HEAD  = 0x02;
  const byte DP_TOM3HEAD  = 0x03;
#else
  const byte DP_HHC       = 0x01;
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
#endif

// Initialize a pin to safe defaults. Type=Disabled means it won't trigger
// until set() assigns it a real pad type.
pin::pin() {
  Type = Disabled;
  Note = 0;

  Gain = 20;
  #if ENABLE_CHANNEL
    Channel = 9;  // MIDI channel 9 = standard drums (0-indexed)
  #endif

  Thresold = 20;
  ScanTime = 10;
  MaskTime = 30;
  Retrigger = 30;

  Curve = Linear;
  CurveForm = 110;
  Xtalk = 0;
  XtalkGroup = 0;

  MaxReading = -1;  // -1 signals "no hit in progress"
}

//===============================
//   PRINTABLE
//===============================
static const char* typeNames[] = {"Piezo", "Switch", "HHC", "HH", "HHs", "YSwitch", "", "", "", "", "", "", "", "", "", "Disabled"};
static const char* curveNames[] = {"Linear", "Exp", "Log", "Sigma", "Flat"};

size_t pin::printTo(Print& p) const {
  size_t n = 0;
  n += p.print("Pin{_pin=");
  n += p.print(_pin);
  n += p.print(", type=");
  n += p.print(typeNames[Type]);
  n += p.print(", note=");
  n += p.print(Note);
  n += p.print(", ch=");
  n += p.print(Channel);
  n += p.print(", thresh=");
  n += p.print(Thresold);
  n += p.print(", curve=");
  n += p.print(curveNames[Curve]);
  n += p.print("}");
  return n;
}

// Assign this pin slot to a physical pad index and apply default parameters
// (MIDI note, threshold, timing windows, gain) for that pad type.
// Called once during setup for each pad in the Pin[] array.
void pin::set(byte pin) {
  // Push Time into the future so the pad is in mask immediately after setup,
  // preventing a false trigger on the very first scan.
  Time = TIMEFUNCTION;
  this->Time = Time + this->MaskTime;

  this->_pin = pin;
  switch (pin) {
    case DP_SNAREHEAD:
      this->Type = Piezo;
      this->Note = 38;       // GM: Acoustic Snare
	  this->Thresold = 400;
      this->ScanTime = 10;
      this->MaskTime = 20;
      this->Retrigger = 20;
	  this->Gain = 20;
      break;
    #if NUM_PINS > 1
      case DP_TOM1HEAD:
        this->Type = Piezo;
        this->Note = 71;     // GM: High Tom
        this->Thresold = 400;
        this->ScanTime = 10;
        this->MaskTime = 20;
        this->Retrigger = 20;
        this->Gain = 20;
        break;
      case DP_TOM2HEAD:
        this->Type = Piezo;
        this->Note = 69;     // GM: High-Mid Tom
        this->Thresold = 400;
        this->ScanTime = 10;
        this->MaskTime = 20;
        this->Retrigger = 20;
		    this->Gain = 20;
        break;
      case DP_TOM3HEAD:
        this->Type = Piezo;
        this->Note = 67;     // GM: Low-Mid Tom
        this->Thresold = 400;
        this->ScanTime = 10;
        this->MaskTime = 20;
        this->Retrigger = 20;
		    this->Gain = 20;
        break;
    #elif NUM_PINS > 4
      // Full pad layout — all disabled by default until enabled via EEPROM/SysEx
      case DP_SNARERIM:
        this->Type = Disabled;
        this->Note = 42;
        break;
      case DP_KICK:
        this->Type = Disabled;
        this->Note = 36;
        break;
      case DP_TOM4HEAD:
        this->Type = Disabled;
        this->Note = 65;
        break;
      case DP_EXTRA1:
        this->Type = Disabled;
        this->Note = 47;
        break;
      case DP_EXTRA2:
        this->Type = Disabled;
        this->Note = 73;
        break;
      case DP_HHBOW:
        this->Type = Disabled;
        this->Note = 8;
        break;
      case DP_CHINA:
        this->Type = Disabled;
        this->Note = 77;
        break;
      case DP_SPLASH:
        this->Type = Disabled;
        this->Note = 79;
        break;
      case DP_CRASHEDGE:
        this->Type = Disabled;
        this->Note = 81;
        break;
      case DP_HHC:
        this->Type = Disabled;
        this->Thresold = 10;
        break;
    #endif
  }
}

// Read the analog sensor and advance the hit-detection state machine.
// Called every loop iteration for each pad.
//
// Parameters:
//   sensor – analog pin number to read
//   count  – position within the current multiplexer scan (0–7); used for
//             cross-talk detection across pads sharing the same multiplexer channel
void pin::scan(byte sensor, byte count) {
  // Combine sensor and count into a unique logical pin index used for tool logging.
  byte pin = count + (sensor << 3);
  int yn_0 = -1;  // Current ADC reading (scaled); -1 = not yet read

  //===============================
  //        HHC (Hi-Hat Controller)
  // HHC uses a continuous position sensor (pedal), not a strike detector.
  // It gets its own dedicated scan path — scanHHC() handles CC output and
  // foot-splash / foot-close detection.
  //===============================
  if (Type == HHC) {
    scanHHC(_pin, analogRead(_pin));
    return;
  }

  //===============================
  //        Switch
  // A switch (e.g., rim trigger) stays low while pressed.
  // The FSM counts consecutive low readings to distinguish a short tap
  // (Switch_Time) from a sustained hold (Choke_Time).
  //===============================
  if (Type == Switch) {
    yn_0 = analogRead(_pin);

    if (State == Normal_Time) {
      // Waiting for a press: two consecutive readings below threshold start a scan.
      if (yn_0 < Thresold * 10 && yn_1 < Thresold * 10) {
        State = Scan_Time;
        MaxReading = 0;
      }
    } else if (State == Scan_Time) {
      if (yn_0 < Thresold * 10 && yn_1 < Thresold * 10) {
        // Still pressed — count the duration.
        MaxReading = MaxReading + 1;
      } else {
        // Released — if held long enough it's a tap, otherwise ignore.
        if (MaxReading > ScanTime)
          State = Switch_Time;   // Short tap → emit note
        else
          State = Normal_Time;   // Too brief → ignore (debounce)
      }

      // Held longer than MaskTime → treat as choke (sustain cut)
      if (MaxReading > MaskTime) State = Choke_Time;
    } else if (State == Mask_Time) {
      // Cooldown after a hit: count down MaxReading and wait for release.
      if (MaxReading > 0) {
        MaxReading = MaxReading - 1;
      } else {
        if (yn_0 >= Thresold * 10 && yn_1 >= Thresold * 10)
          State = Normal_Time;  // Switch fully released
      }
    }
  }
  //===============================
  //        YSwitch
  // A Y-cable splits one piezo into two zones. The analog value encodes
  // which zone was struck: below Thresold*4 = one zone, above = other zone.
  // MaxReading accumulates until released; values >512 indicate the second zone.
  //===============================
  else if (Type == YSwitch) {
    yn_0 = analogRead(sensor);

    if (yn_0 < Thresold * 4) {
      // Zone A pressed — count hold duration up to MaskTime.
      State = Scan_Time;
      if (MaxReading <= MaskTime) MaxReading = MaxReading + 1;
    } else {
      // Released — if long enough, decide which zone based on CurveForm threshold.
      if (MaxReading != 255 && MaxReading > ScanTime) {
        if (yn_0 > CurveForm * 4)
          MaxReading = MaxReading;       // Zone A: keep raw count
        else
          MaxReading = 512 + MaxReading; // Zone B: offset by 512 as a flag
      }
      State = Switch_Time;
    }
  }
  //===============================
  //        Piezo / HH
  // Standard piezo hit detection using a slope-based trigger:
  // a hit is detected when the reading rises sharply (delta > Threshold).
  // The FSM windows: Normal → Scan (collect peak) → Piezo (ready to fire)
  //                → Mask (cooldown) → Retrigger (allow re-hit with decay check)
  //===============================
  else {
    GlobalTime = TIMEFUNCTION;

    // Mask: ignore new hits for MaskTime ms after a hit fires.
    if (State == Mask_Time) {
      if ((GlobalTime - Time) > MaskTime) {
        State = Retrigger_Time;
        Time = GlobalTime;
      }
    }

    // Apply Gain: scale the raw ADC reading before threshold comparison.
    float read = (float)analogRead(sensor);
    yn_0 = 0.5 + (read * (float)Gain) / 64.0;

    if (State == Retrigger_Time) {
      // After mask, allow a re-hit only if the signal rises above a decaying
      // threshold. MaxRetriggerSensor decreases over time at a rate set by Retrigger,
      // preventing double-triggers while still allowing legitimate fast hits.
      int MaxRetriggerSensor = MaxReading - ((GlobalTime - Time) * (Retrigger + 1) / 16);
      if (MaxRetriggerSensor > 0) {
        if ((yn_0 - yn_1) > Thresold && yn_0 > MaxRetriggerSensor) {
          State = Scan_Time;
          Time = GlobalTime;
          MaxReading = 0;
        }
      } else {
        State = Normal_Time;  // Decay expired — fully reset
      }
    } else if (State == Normal_Time) {
      // Waiting for a hit: detect a rising edge steeper than Threshold.
      if ((yn_0 - yn_1) > Thresold) {
        State = Scan_Time;
        Time = GlobalTime;
        MaxReading = 0;
      }
    } else if (State == Scan_Time) {
      // During the scan window, track the peak reading for velocity calculation.
      // Also update cross-talk tables so louder pads on the same group can
      // suppress softer simultaneous readings (xtalk rejection).
      if ((GlobalTime - Time) < ScanTime) {
        if (yn_0 > MaxReading) {
          MaxReading = yn_0;

          // Update group-level xtalk peak (shared across pads in the same XtalkGroup)
          if (MaxXtalkGroup[XtalkGroup] == -1 || MaxXtalkGroup[XtalkGroup] < yn_0)
            MaxXtalkGroup[XtalkGroup] = yn_0;

          // Update multiplexer-level xtalk peak (shared across pads on same mux channel)
          if (MaxMultiplexerXtalk[count] == -1 || MaxMultiplexerXtalk[count] < yn_0)
            MaxMultiplexerXtalk[count] = yn_0;
        }
      } else {
        // Scan window expired — peak captured, ready to emit note in play()
        State = Piezo_Time;
      }
    }
  }

  // If Tool mode is active and this pin is selected for logging, send a
  // diagnostic packet so the host editor can display the raw waveform.
  if (Mode == Tool && LogPin == pin) LogTool(yn_0, pin);

  yn_1 = yn_0;  // Save current reading for next iteration's delta calculation
}

// Decide whether to fire a note for this pad.
// Called every loop iteration after scan(). Skips pads that are still in
// non-firing states and applies cross-talk suppression before delegating to
// playMIDI() or playTOOL().
void pin::play(byte i, pin* dual) {
  if (Type == Disabled) return;

  //===============================
  //        HHC
  // Hi-hat controller fires foot-splash or foot-close notes based on the
  // pedal velocity detected in scanHHC(), then resets to Normal.
  //===============================
  if (Type == HHC) {
    if (State == Footsplash_Time) {
      if (Mode == MIDI) fastNote(Channel, HHFootNoteSensor[0], 127);
      State = Normal_Time;
    } else if (State == Footclose_Time) {
      if (Mode == MIDI) fastNote(Channel, HHFootNoteSensor[1], 127);
      State = Normal_Time;
    }
    return;
  }

  // Only Piezo_Time, Switch_Time, Choke_Time, and YSwitch's Switch_Time
  // should reach the firing logic below. All other states mean the pad is
  // still scanning, in mask/retrigger, or idle.
  if (State == Normal_Time || State == Scan_Time || State == Mask_Time || State == Retrigger_Time)
    return;

  //=============XTALK==================
  // In diagnostic Tool mode, bypass xtalk suppression so the host editor can
  // observe raw hits. In normal operation, suppress this hit if a louder pad
  // fired simultaneously on the same multiplexer channel or xtalk group.
  if (Mode == Tool && Diagnostic == true) {
    playTOOL(i, dual);
  } else {
    // Multiplexer xtalk: if another pad on the same mux channel read more than
    // twice our peak, our signal is likely bleed — suppress it.
    if (MaxMultiplexerXtalk[i % 8] != -1 && MaxMultiplexerXtalk[i % 8] > (2 * MaxReading)) {
      State = Normal_Time;
      return;
    }
    // Group xtalk: if another pad in the same XtalkGroup read higher than our
    // peak plus the xtalk sensitivity margin, suppress this hit.
    if (MaxXtalkGroup[XtalkGroup] != -1 && MaxXtalkGroup[XtalkGroup] > (MaxReading + (64 - Xtalk) * 4)) {
      State = Normal_Time;
      return;
    }
    playMIDI(i, dual);
  }
}

// Send diagnostic SysEx packets to the host editor tool instead of MIDI notes.
// Used during calibration so the editor can display hit velocity and timing.
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
  //        Piezo / HH
  //===============================
  if (State == Piezo_Time) {
    if (Type == Piezo) {
      // Send the velocity-curved value for this pad.
      simpleSysex(0x6F, i, useCurve(), 0);
      State = Mask_Time;

      // If the dual (rim/edge) switch was held at the same time, report it too.
      if (dual->Type == Switch && dual->State == Switch_Time) {
        simpleSysex(0x6F, DualSensor(i), 127, 0);
        dual->State = Mask_Time;
        dual->MaxReading = -1;
      }
    } else {
      // HH bow/edge — just report velocity; note selection happens in playMIDI.
      simpleSysex(0x6F, i, useCurve(), 0);
    }

    MaxReading = -1;
  }
}

// Emit MIDI notes for a completed hit.
// `dual` points to the paired pad (e.g., rim paired with head, or HHC paired
// with HH bow) so that combined head+rim or open/closed hi-hat logic can check
// the partner's state at fire time.
void pin::playMIDI(byte i, pin* dual) {
  //===============================
  //        Single Switch
  // A rim or cymbal edge switch fires at fixed velocity 127.
  // Choke_Time means the switch was held long — send the choke note instead.
  //===============================
  if (Type == Switch) {
    if (State == Switch_Time) {
      fastNote(Channel, Note, 127);
      State = Mask_Time;
      MaxReading = Retrigger;
    } else if (State == Choke_Time) {
      fastNote(Channel, ChokeNote, 127);
      State = Mask_Time;
      MaxReading = Retrigger;
    }
    return;
  }

  GlobalTime = TIMEFUNCTION;

  //===============================
  //          YSwitch
  // MaxReading <= 512 → zone A (Note); > 512 → zone B (DualSensor note).
  // Velocity is proportional to how long the zone was pressed (capped at 127).
  //===============================
  if (Type == YSwitch) {
    if (MaxReading <= 512) {
      fastNote(Channel, Note, min(127, MaxReading * 8));
    } else {
      // Zone B: subtract the 512 offset added during scan to recover duration.
      fastNote(Channel, DualSensor(i), min(127, (MaxReading - 512) * 8));
    }

    // Reset the dual pad's timing so it doesn't falsely fire.
    if (DualSensor(i) != 127) {
      dual->MaxReading = -1;
      dual->Time = GlobalTime - dual->ScanTime;
    }

    MaxReading = -1;
    return;
  }

  //====================================================================
  if (State == Piezo_Time) {
    if (Type == Piezo) {
      byte v = useCurve();  // Map peak reading to a MIDI velocity 0–127

      fastNote(Channel, Note, v);
      State = Mask_Time;

      // Piezo-Switch combo (e.g., tom head + rim):
      // If the rim switch was held when the head was struck, fire the rim note too.
      if (dual->Type == Switch && dual->State == Switch_Time) {
        fastNote(dual->Channel, dual->Note, 127);
        dual->State = Mask_Time;
      }
    } else {
      // HH bow/edge hit — select the note based on hi-hat pedal position.
      // dual here is the HHC pad; its MaxReading encodes the pedal depth.
      byte note = Note;  // Default: open hi-hat

      // Walk through position thresholds from tightest (most closed) to most open.
      if (dual->MaxReading > dual->Thresold)
        note = ChokeNote;                     // Fully choked / foot-closed
      else if (dual->MaxReading > HHThresoldSensor[3])
        note = HHNoteSensor[3];               // Almost closed
      else if (dual->MaxReading > HHThresoldSensor[2])
        note = HHNoteSensor[2];               // Half-open
      else if (dual->MaxReading > HHThresoldSensor[1])
        note = HHNoteSensor[1];               // Mostly open
      else if (dual->MaxReading > HHThresoldSensor[0])
        note = HHNoteSensor[0];               // Open

      fastNote(Channel, note, useCurve());
    }
  }
}

// Map MaxReading (peak ADC value captured during scan) to a MIDI velocity (0–127)
// using the selected curve shape and CurveForm scaling factor.
//
// CurveForm acts as a gain: f = CurveForm / 32, so CurveForm=32 → f=1 (unity).
// For non-linear curves, MaxReading is split into a segment index (i = reading/128)
// and a fractional offset (m = reading%128), then linearly interpolated between
// the two surrounding control points.
byte pin::useCurve() {
  int ret = 0;
  float f = ((float)CurveForm) / 32.0;

  if (Curve == Linear) {
    ret = 0.5 + ((float)MaxReading * f / 8.0);
  } else {
    int i = MaxReading / 128;   // Segment index into the 9-point table
    int m = MaxReading % 128;   // Offset within the segment

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

// Handle the hi-hat controller (pedal position sensor) on each scan iteration.
// Sends a MIDI CC whenever the pedal moves beyond the Threshold dead-band,
// and detects foot-splash (quick open) / foot-close (quick close) gestures
// by computing the instantaneous velocity of pedal movement.
//
// Parameters:
//   pin           – logical pin index (for Tool mode SysEx reporting)
//   sensorReading – raw ADC value from the pedal position sensor
void pin::scanHHC(byte pin, byte sensorReading) {
  // Enforce a minimum interval between CC messages (debounce / rate-limit).
  if ((GlobalTime - Time) > MaskTime) {
    if (sensorReading > (MaxReading + Thresold) || sensorReading < (MaxReading - Thresold)) {
      if (Mode == MIDI) {
        fastMidiCC(Channel, Note, sensorReading);
      } else if (Mode == Tool && Diagnostic == true) {
        simpleSysex(0x6F, pin, sensorReading, 0);
      }

      // Compute pedal velocity (units/100ms). Positive = opening, negative = closing.
      float m = (((float)MaxReading - (float)sensorReading) / ((float)Time - (float)GlobalTime)) * 100;

      MaxReading = sensorReading;  // Update last known position

      // Foot Splash: pedal opened quickly → open hi-hat accent note
      if (m > 0 && m > HHFootThresoldSensor[0])
        State = Footsplash_Time;
      // Foot Close: pedal closed quickly → closed hi-hat accent note
      else if (m < 0 && -m > HHFootThresoldSensor[1])
        State = Footclose_Time;

      Time = GlobalTime;
    }
  }
}

// Global array of all pad objects. Sized by NUM_PINS (set in define.h).
pin Pin[NPin];