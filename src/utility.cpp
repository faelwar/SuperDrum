#include "utility.h"
#include "consts.h"
#include "midi.h"
#include "define.h"

// Optional loop profiler — measures total time spent per loop() call.
// Enabled by defining USE_PROFILER. PROFA captures the start timestamp;
// PROFB accumulates the elapsed time and increments the sample count.
// Results are read back via ExecCommand(0x6D, 1, ...) → SendProfiling().
#define PROFA TimeProfA=micros();
#define PROFB TimeProf+=(micros()-TimeProfA); NProf++;
#if USE_PROFILER
  unsigned long TimeProfA;
  unsigned long TimeProf=0;
  unsigned long NProf=0;
#endif

// Send a raw sensor waveform sample to the host editor tool as a SysEx log frame.
// Called from LogTool() in loop.cpp for samples above the log threshold.
//
// The 14-byte payload packs all values as little-endian multi-byte integers because
// individual MIDI bytes must be < 128 and the host reassembles them. Layout:
//   Bytes 0–3:   GlobalTime (uint32, ms timestamp)
//   Byte  4:     Sensor index
//   Bytes 5–6:   N (sample counter, uint16)
//   Bytes 7–8:   MaxRetrigger (current retrigger level, int16)
//   Bytes 9–10:  Y0 (current scaled ADC reading, int16)
//   Bytes 11–12: MaxReading (peak captured during scan, int16)
//   Byte  13:    State (FSM state enum value)
void SendLog(byte Sensor, int N, int Y0, int MaxRetrigger, int MaxReading, byte State)
{
  byte buf[14];

  buf[0] = (byte) GlobalTime;
  buf[1] = (byte) (GlobalTime >> 8);
  buf[2] = (byte) (GlobalTime >> 16);
  buf[3] = (byte) (GlobalTime >> 24);

  buf[4] = Sensor;

  buf[5] = (byte)N;
  buf[6] = (byte)(N >> 8);

  buf[7] = (byte)MaxRetrigger;
  buf[8] = (byte)(MaxRetrigger >> 8);

  buf[9]  = (byte)Y0;
  buf[10] = (byte)(Y0 >> 8);

  buf[11] = (byte)MaxReading;
  buf[12] = (byte)(MaxReading >> 8);

  buf[13] = State;

  Sysex(0x6E, buf, 14);
}

// Send accumulated profiling statistics to the host editor.
// Reports total microseconds spent in measured code (TimeProf) and number of
// samples taken (NProf) packed as two little-endian uint32s.
#if USE_PROFILER
  void SendProfiling()
  {
    byte buf[8];

    buf[0] = (byte) TimeProf;
    buf[1] = (byte) (TimeProf >> 8);
    buf[2] = (byte) (TimeProf >> 16);
    buf[3] = (byte) (TimeProf >> 24);

    buf[4] = (byte) NProf;
    buf[5] = (byte) (NProf >> 8);
    buf[6] = (byte) (NProf >> 16);
    buf[7] = (byte) (NProf >> 24);

    Sysex(0x6D, buf, 8);
  }
#endif

// Perform a soft reset by jumping to address 0 (AVR vector table entry point).
// This re-runs the bootloader handshake and re-executes setup(), which is
// equivalent to a power cycle without actually cutting power.
void softReset() {
  #if defined(__AVR__)
    asm volatile (" jmp 0");
  #endif
  //wdt_enable(WDTO_30MS);  // Alternative: watchdog reset (currently unused)
}

// Return the number of free bytes between the stack and heap on AVR.
// Useful for diagnosing memory pressure when adding new features.
// __brkval is 0 before any heap allocation, so we fall back to __heap_start.
int freeRam() {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

// Blink the built-in LED (pin 13) once. Used as a simple alive indicator
// or for debug signalling during development.
void blink()
{
  digitalWrite(13, HIGH);
  delay(500);
  digitalWrite(13, LOW);
  delay(500);
}

// Virtual analog read for testing the waveform logging pipeline without
// physical pads. When the selected LogPin is active, this generates a
// synthetic triangular pulse at virtualFreq ms intervals instead of calling
// analogRead(). All constants are currently 0, so it always returns 0 —
// edit virtualTmax/virtualVmax/virtualT50 to produce a non-zero waveform.
const unsigned long virtualTmax  = 0;  // Duration of the rising edge (ms)
const unsigned long virtualVmax  = 0;  // Peak amplitude
const unsigned long virtualT50   = 0;  // Half-decay time after the peak

unsigned long virtualTime = 0;
const unsigned long virtualFreq  = 1000;  // Pulse period (ms)

inline int virtualAnalogRead(byte sensor, byte pin)
{
  if (pin != LogPin) return analogRead(sensor);  // Normal path for non-logged pins

  unsigned int T = GlobalTime - virtualTime;

  if (T > virtualFreq) virtualTime = GlobalTime;  // Reset pulse period
  else
  {
    // Rising edge: linearly ramp from 0 to virtualVmax over virtualTmax ms
    if (T < virtualTmax) return (T * virtualVmax) / virtualTmax;
    // Falling edge: decay from virtualVmax to virtualVmax/2 over virtualT50 ms
    else if (T < (virtualTmax + (2 * virtualT50))) return virtualVmax - (T - virtualTmax) * (virtualVmax / 2) / virtualT50;
  }

  return 0;
}