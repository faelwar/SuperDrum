#include "loop.h"
#include "utility.h"
#include "consts.h"
#include "pin.h"
#include "define.h"

// Scan a single pad up to 4 times in rapid succession if it just entered Scan_Time.
// The extra scans collect more ADC samples during the short peak window, improving
// velocity accuracy on fast hits without adding latency to idle pads.
void fastScan(byte sensor, int count) {
  byte pin = count + (sensor << 3);
  if (Pin[pin].Type != Disabled) {
    Pin[pin].scan(sensor, count);
    // If the hit was just detected (Scan_Time), take 3 more immediate samples
    // so the peak tracker has more data points before ScanTime expires.
    if (Pin[pin].State == Scan_Time) {
      Pin[pin].scan(sensor, count);
      Pin[pin].scan(sensor, count);
      Pin[pin].scan(sensor, count);
    }
  }
}

// Main loop — called repeatedly by the Arduino runtime.
//
// Execution order each iteration:
//   1. Input()         — parse any incoming SysEx from the host editor
//   2. Digital pins    — read choke/aux buttons and send immediate MIDI (if CHOKE enabled)
//   3. Analog scan     — scan each pad, fire notes for completed hits
//   4. Xtalk reset     — clear per-group and per-mux-channel peak tables so the next
//                        iteration starts with a clean slate for xtalk suppression
void loop() {
  Input();

  if (Mode == Off) {
    delay(100);
    return;
  }

  // Digital pin section — reads hardware choke buttons wired to cymbal pads.
  // Each button uses INPUT_PULLUP, so LOW = pressed, HIGH = released.
  // On press:   send Note On  (0x90 | 144 = 0x90 | 0x90 — kept as-is from original)
  // On release: send Note Off (0x90 | 128 = 0x90 | 0x80)
  // Note: the multiplexer circuit present in the original MicroDrum design has been
  // removed for the Mega port; these digital reads replace that functionality.
  #if CHOKE
    // CHOKE PRATO 01
    currentSwitchState = digitalRead(Choke1_Pin);
    if (currentSwitchState == LOW && Choke1_State == HIGH)   // falling edge = press
      MIDI_TX(0x90 | 144, choke1, 127);
    if (currentSwitchState == HIGH && Choke1_State == LOW)   // rising edge = release
      MIDI_TX(0x90 | 128, choke1, 127);
    Choke1_State = currentSwitchState;

    // CHOKE PRATO 02
    currentSwitchState = digitalRead(Choke2_Pin);
    if (currentSwitchState == LOW && Choke2_State == HIGH)
      MIDI_TX(0x90 | 144, choke2, 127);
    if (currentSwitchState == HIGH && Choke2_State == LOW)
      MIDI_TX(0x90 | 128, choke2, 127);
    Choke2_State = currentSwitchState;

    // CHOKE PRATO 03
    currentSwitchState = digitalRead(Choke3_Pin);
    if (currentSwitchState == LOW && Choke3_State == HIGH)
      MIDI_TX(0x90 | 144, choke3, 127);
    if (currentSwitchState == HIGH && Choke3_State == LOW)
      MIDI_TX(0x90 | 128, choke3, 127);
    Choke3_State = currentSwitchState;

    // CHOKE RIDE
    currentSwitchState = digitalRead(ChokeRide_Pin);
    if (currentSwitchState == LOW && ChokeRide_State == HIGH)
      MIDI_TX(0x90 | 144, chokeRide, 127);
    if (currentSwitchState == HIGH && ChokeRide_State == LOW)
      MIDI_TX(0x90 | 128, chokeRide, 127);
    ChokeRide_State = currentSwitchState;

    // AUX 1–7: auxiliary digital inputs for extra sounds / triggers
    currentSwitchState = digitalRead(Aux1_Pin);
    if (currentSwitchState == LOW && Aux1_State == HIGH)
      MIDI_TX(0x90 | 144, Aux1, 127);
    if (currentSwitchState == HIGH && Aux1_State == LOW)
      MIDI_TX(0x90 | 128, Aux1, 127);
    Aux1_State = currentSwitchState;

    currentSwitchState = digitalRead(Aux2_Pin);
    if (currentSwitchState == LOW && Aux2_State == HIGH)
      MIDI_TX(0x90 | 144, Aux2, 127);
    if (currentSwitchState == HIGH && Aux2_State == LOW)
      MIDI_TX(0x90 | 128, Aux2, 127);
    Aux2_State = currentSwitchState;

    currentSwitchState = digitalRead(Aux3_Pin);
    if (currentSwitchState == LOW && Aux3_State == HIGH)
      MIDI_TX(0x90 | 144, Aux3, 127);
    if (currentSwitchState == HIGH && Aux3_State == LOW)
      MIDI_TX(0x90 | 128, Aux3, 127);
    Aux3_State = currentSwitchState;

    currentSwitchState = digitalRead(Aux4_Pin);
    if (currentSwitchState == LOW && Aux4_State == HIGH)
      MIDI_TX(0x90 | 144, Aux4, 127);
    if (currentSwitchState == HIGH && Aux4_State == LOW)
      MIDI_TX(0x90 | 128, Aux4, 127);
    Aux4_State = currentSwitchState;

    currentSwitchState = digitalRead(Aux5_Pin);
    if (currentSwitchState == LOW && Aux5_State == HIGH)
      MIDI_TX(0x90 | 144, Aux5, 127);
    if (currentSwitchState == HIGH && Aux5_State == LOW)
      MIDI_TX(0x90 | 128, Aux5, 127);
    Aux5_State = currentSwitchState;

    currentSwitchState = digitalRead(Aux6_Pin);
    if (currentSwitchState == LOW && Aux6_State == HIGH)
      MIDI_TX(0x90 | 144, Aux6, 127);
    if (currentSwitchState == HIGH && Aux6_State == LOW)
      MIDI_TX(0x90 | 128, Aux6, 127);
    Aux6_State = currentSwitchState;

    currentSwitchState = digitalRead(Aux7_Pin);
    if (currentSwitchState == LOW && Aux7_State == HIGH)
      MIDI_TX(0x90 | 144, Aux7, 127);
    if (currentSwitchState == HIGH && Aux7_State == LOW)
      MIDI_TX(0x90 | 128, Aux7, 127);
    Aux7_State = currentSwitchState;
  #endif

  // Analog pad scan and play.
  // For each pad: scan advances the hit-detection FSM, then play() fires MIDI
  // if the pad has reached a firing state. The dual pointer gives play() access
  // to the paired pad (e.g., HH bow paired with HHC, or head paired with rim).
  for (byte Sensor = 0; Sensor < NPin; Sensor++) {
    if (Pin[Sensor].Type != Disabled) {
      Pin[Sensor].scan(Sensor, 0);
      // Extra scans during the active scan window to capture the signal peak
      if (Pin[Sensor].State == Scan_Time) {
        Pin[Sensor].scan(Sensor, 0);
        Pin[Sensor].scan(Sensor, 0);
        Pin[Sensor].scan(Sensor, 0);
      }
    }
    Pin[Sensor].play(Sensor, &Pin[DualSensor(Sensor)]);
  }

  // Reset xtalk peak tables at the end of each loop so values from this
  // iteration don't bleed into the next. -1 signals "no reading yet".
  for (int i = 0; i < 8; i++) {
    MaxMultiplexerXtalk[i] = -1;
  }
  for (int i = 0; i < NXtalkGroup; i++) {
    MaxXtalkGroup[i] = -1;
  }
}

// Called from pin::scan() when Tool mode is active and a pin is selected for logging.
// Only forwards samples that exceed LogThresold*2 to avoid flooding the host with
// noise-floor data. Increments the global sample counter N each call regardless.
void LogTool(int yn_0, byte MulSensor) {
  N++;
  if (yn_0 >= (LogThresold * 2))
    SendLog(MulSensor, N, yn_0, Pin[MulSensor].useCurve(), Pin[MulSensor].MaxReading, Pin[MulSensor].State);
}