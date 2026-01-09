#include "loop.h"
#include "utility.h"
#include "consts.h"
#include "pin.h"

//==============================
//    FASTSCAN
//==============================
#define fastScan(sensor, count) { \
  byte pin = count + (sensor << 3); \
  if (Pin[pin].Type != Disabled) { \
    Pin[pin].scan(sensor, count); \
    if (Pin[pin].State == Scan_Time) { \
      Pin[pin].scan(sensor, count); \
      Pin[pin].scan(sensor, count); \
      Pin[pin].scan(sensor, count); \
    } \
  } \
}

//==============================
//     LOOP
//==============================
void loop() {
  Input();

  #if USE_LCD
    Menu();
  #endif

  if (Mode == Off) {
    delay(100);
    return;
  }

  //---------------------------------------------------------------------------
  //      USO DO MULTIPLEX RETIRADO NO MEGA
  //---------------------------------------------------------------------------
  #if MEGA
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
  #else
    //==========UNROLLING======
    // {0, 1, 3, 2, 6, 7, 5, 4}
    //=========================

    // 0
    fastWrite(2, 0);
    delayMicroseconds(delayTime);
    for (byte Sensor = 0; Sensor < NSensor; Sensor++) {
      analogRead(Sensor);
      fastScan(Sensor, 0);
    }

    // 1
    fastWrite(4, 1);
    delayMicroseconds(delayTime);
    for (byte Sensor = 0; Sensor < NSensor; Sensor++) {
      analogRead(Sensor);
      fastScan(Sensor, 1);
    }

    // 3
    fastWrite(3, 1);
    delayMicroseconds(delayTime);
    for (byte Sensor = 0; Sensor < NSensor; Sensor++) {
      analogRead(Sensor);
      fastScan(Sensor, 3);
    }

    // 2
    fastWrite(4, 0);
    delayMicroseconds(delayTime);
    for (byte Sensor = 0; Sensor < NSensor; Sensor++) {
      analogRead(Sensor);
      fastScan(Sensor, 2);
    }

    // 6
    fastWrite(2, 1);
    delayMicroseconds(delayTime);
    for (byte Sensor = 0; Sensor < NSensor; Sensor++) {
      analogRead(Sensor);
      fastScan(Sensor, 6);
    }

    // 7
    fastWrite(4, 1);
    delayMicroseconds(delayTime);
    for (byte Sensor = 0; Sensor < NSensor; Sensor++) {
      analogRead(Sensor);
      fastScan(Sensor, 7);
    }

    // 5
    fastWrite(3, 0);
    delayMicroseconds(delayTime);
    for (byte Sensor = 0; Sensor < NSensor; Sensor++) {
      analogRead(Sensor);
      fastScan(Sensor, 5);
    }

    // 4
    fastWrite(4, 0);
    delayMicroseconds(delayTime);
    for (byte Sensor = 0; Sensor < NSensor; Sensor++) {
      analogRead(Sensor);
      fastScan(Sensor, 4);
    }

    if (Mode == Standby) return;

    for (byte i = 0; i < (NSensor * 8); i++) {
      Pin[i].play(i, &Pin[DualSensor(i)]);
    }
  #endif

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
  #if MENU_LOG
    // FASE 1: autodeterminazione del rumore massimo Nmax in un intervallo fisso di 20s
    if (log_state == 0) {
      // FASE 1.a: AVVIO
      log_T1 = TIMEFUNCTION;
      log_Nmax = yn_0;
      log_state = 1;

      Pin[MulSensor].Gain = 64;
      Pin[MulSensor].CurveForm = 32;
      Pin[MulSensor].Curve = Linear;

      d_hsum = 0;
      d_tsum = 0;
      d_tnum = 0;
      d_vmax = 0;
      d_vmin = 1024;

      N = 0;

      DrawLog(0);
    } else if (log_state == 1) {
      // FASE 1.b: ANALISI
      if (yn_0 > log_Nmax) {
        log_Nmax = yn_0 * 1.2;
        DrawLog(4);
      }
      if ((log_T1 + 20000) < TIMEFUNCTION) {
        log_state = 2;
        if (d_tnum < 25)
          DrawLog(1);
        else
          DrawLog(2);
      }
    } else if (log_state == 2) {
      // FASE 1.c: FINE
      // FASE 2: analisi del segnale per 1s
      if (yn_0 > log_Nmax) {
        DrawLog(0);
        N++;
        log_Tmax = log_T1 = TIMEFUNCTION;
        log_state = 3;
        log_Vmax = yn_0;
      }
    } else if (log_state == 3) {
      if (yn_0 > log_Nmax) {
        N++;
        if (yn_0 > log_Vmax) {
          // Salita: il segnale aumenta di intensità
          log_Vmax = yn_0;
          log_Tmax = TIMEFUNCTION;
          log_T50 = 0;
        } else {
          // Discesa: il segnale diminuisce di intensità
          if (yn_0 > (int)((float)log_Vmax * 0.5))
            log_T50 = TIMEFUNCTION;
        }
      }

      // 1 sec - FASE 2.b: fine dell'analisi del singolo colpo
      if ((TIMEFUNCTION - log_T1) > 1000) {
        log_state = 4;

        d_hsum += (log_T50 == 0 ? 0 : (log_T50 - log_Tmax));
        d_tsum += (log_Tmax - log_T1);
        d_tnum++;

        if (log_Vmax > d_vmax) d_vmax = log_Vmax;
        if (log_Vmax < d_vmin) d_vmin = log_Vmax;

        if (d_tnum < 25)
          DrawLog(1);
        else if (d_tnum < 50)
          DrawLog(2);
      }
    } else if (log_state == 4) {
      // FASE 3: report
      if (d_tnum == 25) {
        Pin[MulSensor].Gain = (16.0 / (float)d_vmin) * 64.0;
        if (Pin[MulSensor].Gain < 16) Pin[MulSensor].Gain = 16;
        Pin[MulSensor].Thresold = ((float)(d_vmin - log_Nmax)) * ((float)Pin[MulSensor].Gain / 64.0);
        Pin[MulSensor].CurveForm = ((32.0 * 64.0) / (float)Pin[MulSensor].Gain) - 1;

        log_T1 = TIMEFUNCTION;
        log_Nmax = 0;
        log_state = 1;

        d_hsum = 0;
        d_tsum = 0;
        d_vmax = 0;

        N = 0;
        DrawLog(0);
      } else if (d_tnum == 50) {
        Pin[MulSensor].ScanTime = ((float)d_tsum / 25.0);
        Pin[MulSensor].MaskTime = ((float)d_hsum / 25.0);
        Pin[MulSensor].Retrigger = (d_vmax * 8.0) / ((float)d_hsum / 25.0);
        Pin[MulSensor].CurveForm = max(Pin[MulSensor].CurveForm, (1024.0 / (float)d_vmax) * 32);
        Pin[MulSensor].Curve = Linear;

        DrawLog(3);
        log_state = 5;
      } else if (yn_0 > log_Nmax) {
        // FASE 2.a: INIZIO
        log_Tmax = log_T1 = TIMEFUNCTION;
        log_state = 3;
        log_Vmax = yn_0;

        N = 0;
        DrawLog(0);
      }
    }
  #else
    N++;
    if (yn_0 >= (LogThresold * 2))
      SendLog(MulSensor, N, yn_0, Pin[MulSensor].useCurve(), Pin[MulSensor].MaxReading, Pin[MulSensor].State);
  #endif
}
