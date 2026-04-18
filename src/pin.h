#pragma once

#include <Arduino.h>

#include "consts.h"
#include "loop.h"
#include "midi.h"

//===========TIME============
// FSM states for a pad's hit-detection lifecycle.
// Transitions (for Piezo/HH type):
//   Normal_Time → Scan_Time (rising edge detected)
//   Scan_Time   → Piezo_Time (scan window expired, peak captured)
//   Piezo_Time  → Mask_Time  (note fired in play())
//   Mask_Time   → Retrigger_Time (mask window expired)
//   Retrigger_Time → Normal_Time or Scan_Time (decay expired or new hit)
//
// Switch type uses: Normal_Time → Scan_Time → Switch_Time/Choke_Time → Mask_Time
// HHC type uses:    Normal_Time ↔ Footsplash_Time / Footclose_Time
enum state : byte {
  Normal_Time        = 0,
  Scan_Time          = 1,
  Mask_Time          = 2,
  Retrigger_Time     = 3,
  Piezo_Time         = 4,
  Switch_Time        = 5,
  Choke_Time         = 6,   // Switch held longer than MaskTime → send choke note
  Footsplash_Time    = 7,   // HHC: pedal opened quickly
  Footclose_Time     = 8,   // HHC: pedal closed quickly
  Scanretrigger_Time = 9
};
//============================

//===========TYPE============
// Physical sensor types. Determines which scan/play branch runs for this pad.
//   Piezo   — standard drum pad (piezo transducer)
//   Switch  — rim/edge contact switch (digital-style analog reading)
//   HHC     — hi-hat controller (continuous pedal position sensor, outputs CC)
//   HH      — hi-hat bow/edge pad (piezo, but note selected by HHC position)
//   HHs     — hi-hat with switch (unused/reserved)
//   YSwitch — Y-cable splitting one piezo into two zones
//   Disabled — pad is ignored during scan and play
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
// Velocity curve shapes applied to the raw ADC peak before emitting a MIDI velocity.
// See useCurve() in pin.cpp for the interpolation math.
//   Linear — velocity proportional to hit strength
//   Exp    — soft hits quieter, hard hits louder (good for cymbals)
//   Log    — soft hits louder, diminishing returns on hard hits (good for brushes)
//   Sigma  — S-curve, mid-range sensitive
//   Flat   — nearly uniform velocity regardless of hit strength
enum curve : byte {
  Linear = 0,
  Exp    = 1,
  Log    = 2,
  Sigma  = 3,
  Flat   = 4
};

//===========================
//   PIN
// Represents one analog pad input. The global Pin[] array holds one instance
// per physical pad (size = NPin = NUM_PINS).
//===========================
class pin : public Printable {
public:
  pin();

  // Printable interface — prints a compact human-readable summary of the pad's
  // current configuration. Used in setup() to log the initial state over Serial.
  size_t printTo(Print& p) const override;

  // Assign this pin slot to a physical index and load default parameters for
  // the corresponding drum pad (note, threshold, timing, gain). Called once
  // per pad in setup().
  void set(byte pin);

  // Advance the hit-detection state machine for this pad. Reads the ADC,
  // applies gain, and tracks the peak reading during the scan window.
  // Also handles xtalk peak table updates. Called every loop iteration.
  void scan(byte sensor, byte count);

  // Check if the pad is in a firing state and emit a MIDI note (or SysEx
  // diagnostic packet in Tool mode). Applies xtalk suppression before firing.
  void play(byte i, pin* dual);

  // Tool-mode variant of play(): sends SysEx diagnostic packets to the host
  // editor instead of (or in addition to) MIDI notes.
  void playTOOL(byte i, pin* dual);

  // MIDI-mode variant of play(): emits noteOn/noteOff for completed hits.
  // `dual` is the paired pad (e.g., rim paired with head, HHC paired with HH bow).
  void playMIDI(byte i, pin* dual);

  //=======================
  // Pad configuration and runtime state (packed to minimize RAM usage on AVR)
  byte Type  : 4;   // PinType enum
  byte State : 4;   // state enum — current FSM position
  byte _pin;        // Physical analog pin index

  byte Note;        // Primary MIDI note number
  union {
    byte ChokeNote; // For HH/Switch: note sent on choke or long-hold
    byte Gain;      // For Piezo: ADC scaling factor (applied before threshold compare)
  };

  byte Channel;     // MIDI channel (0-indexed; 9 = standard GM drums)

  byte Thresold;    // Minimum delta (or absolute level) to trigger a hit
  byte ScanTime;    // Duration of the peak-capture window (ms for Piezo, count for Switch)
  byte MaskTime;    // Cooldown after a hit before the pad can re-trigger (ms)
  byte Retrigger;   // Controls how quickly the retrigger threshold decays after mask

  byte CurveForm;   // Curve scaling factor: MIDI velocity = curve(MaxReading) * CurveForm/32
  byte Curve     : 4;  // curve enum — velocity response shape
  byte XtalkGroup: 4;  // Group index for cross-talk suppression (0–3)
  byte Xtalk;          // Xtalk sensitivity: higher = more aggressive suppression

  unsigned long Time;  // Timestamp of the last state transition (millis)
  int MaxReading;      // Peak ADC reading captured during the current scan window; -1 = idle
  int yn_1;            // Previous loop's scaled ADC reading (for slope/delta detection)

  // Map MaxReading to a MIDI velocity (0–127) using the selected Curve and CurveForm.
  byte useCurve();

  // Dedicated scan path for the hi-hat controller pedal. Sends CC messages on
  // movement and detects foot-splash / foot-close gestures via pedal velocity.
  void scanHHC(byte pin, byte sensorReading);
};
//===========================

extern pin Pin[NPin];  // Global array of all pad objects, indexed by physical pin number