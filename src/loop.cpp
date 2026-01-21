#include "loop.h"
#include "utility.h"
#include "consts.h"
#include "pin.h"
#include "define.h"
//==============================
//    FASTSCAN
//==============================
void fastScan(byte sensor, int count) {
  byte pin = count + (sensor << 3);
  if (Pin[pin].Type != Disabled) {
    Pin[pin].scan(sensor, count);
    if (Pin[pin].State == Scan_Time) {
      Pin[pin].scan(sensor, count);
      Pin[pin].scan(sensor, count);
      Pin[pin].scan(sensor, count);
    }
  }
}

//==============================
//     LOOP
//==============================
void loop() {
  Input();

  if (Mode == Off) {
    delay(100);
    return;
  }
  //---------------------------------------------------------------------------
  //      USO DO MULTIPLEX RETIRADO NO MEGA
  //---------------------------------------------------------------------------
  // Lê os pinos digitais

  // CHOKE PRATO 01
  currentSwitchState = digitalRead(Choke1_Pin);
  if (currentSwitchState == LOW && Choke1_State == HIGH)  // push
    MIDI_TX(0x90 | 144, choke1, 127);
  if (currentSwitchState == HIGH && Choke1_State == LOW)  // release
    MIDI_TX(0x90 | 128, choke1, 127);
  Choke1_State = currentSwitchState;

  // CHOKE PRATO 02
  currentSwitchState = digitalRead(Choke2_Pin);
  if (currentSwitchState == LOW && Choke2_State == HIGH)  // push
    MIDI_TX(0x90 | 144, choke2, 127);
  if (currentSwitchState == HIGH && Choke2_State == LOW)  // release
    MIDI_TX(0x90 | 128, choke2, 127);
  Choke2_State = currentSwitchState;

  // CHOKE PRATO 03
  currentSwitchState = digitalRead(Choke3_Pin);
  if (currentSwitchState == LOW && Choke3_State == HIGH)  // push
    MIDI_TX(0x90 | 144, choke3, 127);
  if (currentSwitchState == HIGH && Choke3_State == LOW)  // release
    MIDI_TX(0x90 | 128, choke3, 127);
  Choke3_State = currentSwitchState;

  // CHOKE RIDE
  currentSwitchState = digitalRead(ChokeRide_Pin);
  if (currentSwitchState == LOW && ChokeRide_State == HIGH)  // push
    MIDI_TX(0x90 | 144, chokeRide, 127);
  if (currentSwitchState == HIGH && ChokeRide_State == LOW)  // release
    MIDI_TX(0x90 | 128, chokeRide, 127);
  ChokeRide_State = currentSwitchState;

  // AUX 1
  currentSwitchState = digitalRead(Aux1_Pin);
  if (currentSwitchState == LOW && Aux1_State == HIGH)  // push
    MIDI_TX(0x90 | 144, Aux1, 127);
  if (currentSwitchState == HIGH && Aux1_State == LOW)  // release
    MIDI_TX(0x90 | 128, Aux1, 127);
  Aux1_State = currentSwitchState;

  // AUX 2
  currentSwitchState = digitalRead(Aux2_Pin);
  if (currentSwitchState == LOW && Aux2_State == HIGH)  // push
    MIDI_TX(0x90 | 144, Aux2, 127);
  if (currentSwitchState == HIGH && Aux2_State == LOW)  // release
    MIDI_TX(0x90 | 128, Aux2, 127);
  Aux2_State = currentSwitchState;

  // AUX 3
  currentSwitchState = digitalRead(Aux3_Pin);
  if (currentSwitchState == LOW && Aux3_State == HIGH)  // push
    MIDI_TX(0x90 | 144, Aux3, 127);
  if (currentSwitchState == HIGH && Aux3_State == LOW)  // release
    MIDI_TX(0x90 | 128, Aux3, 127);
  Aux3_State = currentSwitchState;

  // AUX 4
  currentSwitchState = digitalRead(Aux4_Pin);
  if (currentSwitchState == LOW && Aux4_State == HIGH)  // push
    MIDI_TX(0x90 | 144, Aux4, 127);
  if (currentSwitchState == HIGH && Aux4_State == LOW)  // release
    MIDI_TX(0x90 | 128, Aux4, 127);
  Aux4_State = currentSwitchState;

  // AUX 5
  currentSwitchState = digitalRead(Aux5_Pin);
  if (currentSwitchState == LOW && Aux5_State == HIGH)  // push
    MIDI_TX(0x90 | 144, Aux5, 127);
  if (currentSwitchState == HIGH && Aux5_State == LOW)  // release
    MIDI_TX(0x90 | 128, Aux5, 127);
  Aux5_State = currentSwitchState;

  // AUX 6
  currentSwitchState = digitalRead(Aux6_Pin);
  if (currentSwitchState == LOW && Aux6_State == HIGH)  // push
    MIDI_TX(0x90 | 144, Aux6, 127);
  if (currentSwitchState == HIGH && Aux6_State == LOW)  // release
    MIDI_TX(0x90 | 128, Aux6, 127);
  Aux6_State = currentSwitchState;

  // AUX 7
  currentSwitchState = digitalRead(Aux7_Pin);
  if (currentSwitchState == LOW && Aux7_State == HIGH)  // push
    MIDI_TX(0x90 | 144, Aux7, 127);
  if (currentSwitchState == HIGH && Aux7_State == LOW)  // release
    MIDI_TX(0x90 | 128, Aux7, 127);
  Aux7_State = currentSwitchState;
  // Lê os pinos analógicos
  for (byte Sensor = 0; Sensor < NPin; Sensor++) {
    if (Pin[Sensor].Type != Disabled) {
      Pin[Sensor].scan(Sensor, 0);
      if (Pin[Sensor].State == Scan_Time) {
        Pin[Sensor].scan(Sensor, 0);
        Pin[Sensor].scan(Sensor, 0);
        Pin[Sensor].scan(Sensor, 0);
      }
    }
    Pin[Sensor].play(Sensor, &Pin[DualSensor(Sensor)]);
  }
  // RESET XTALK
  for (int i = 0; i < 8; i++) {
    MaxMultiplexerXtalk[i] = -1;
  }
  for (int i = 0; i < NXtalkGroup; i++) {
    MaxXtalkGroup[i] = -1;
  }
}

//==============================
//    LOGTOOL
//==============================
void LogTool(int yn_0, byte MulSensor) {
  N++;
  if (yn_0 >= (LogThresold * 2))
    SendLog(MulSensor, N, yn_0, Pin[MulSensor].useCurve(), Pin[MulSensor].MaxReading, Pin[MulSensor].State);
}
