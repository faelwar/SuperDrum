# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

SuperDrum (MicroMegaDrum) is an Arduino Mega-based electronic drum MIDI controller firmware. It is adapted from the original MicroDrum project (Arduino UNO + multiplexers) to run directly on Arduino Mega's 16 analog inputs without additional multiplexer circuits. It also adds digital pin support for cymbal choke and auxiliary sounds.

## Build & Upload Commands

```bash
make compile    # Compile with arduino-cli targeting arduino:avr:nano
make upload     # Upload to /dev/cu.usbserial-A5069RR4
make run        # compile + upload + open serial monitor at 31250 baud (MIDI baud rate)
```

The serial port (`/dev/cu.usbserial-A5069RR4`) and board FQBN (`arduino:avr:nano`) are hardcoded in the Makefile — update them to match the connected device.

## Key Configuration (`src/define.h`)

All high-level build-time flags live here:

- `NUM_PINS` — number of analog pad inputs (currently `4`)
- `TEXT_OUTPUT` — when `1`, MIDI is printed as human-readable text over Serial (debug mode); when `0`, raw MIDI bytes are sent
- `FASTADC` — enables ADC prescaler tuning for faster sampling
- `ENABLE_CHANNEL` — when `1`, uses per-pad MIDI channel; otherwise defaults to channel 9 (standard drums)
- `CHOKE` — enables choke logic
- Choke/Aux MIDI notes and Arduino digital pin assignments

## Architecture

**Entry point:** `SuperDrum.ino` — calls `setup()` and `loop()` via the standard Arduino pattern.

**Data flow:**
1. `setup()` (`src/setup.h/.cpp`) — initializes Serial, EEPROM, pin configurations, ADC prescaler
2. `loop()` (`src/loop.h`) — main scan loop: reads analog sensors, calls `pin::scan()` per pad, then `pin::play()` to emit MIDI
3. `pin` class (`src/pin.h/pin.cpp`) — core per-pad object; holds all per-pad state (type, note, threshold, timing windows, curve, xtalk group). `scan()` reads ADC and detects hits; `play()` dispatches to `playMIDI()` or `playTOOL()` based on global `Mode`
4. MIDI output (`src/midi.h/midi.cpp`) — `noteOn`, `noteOff`, `midiCC` wrap raw Serial writes; `fastNote`/`fastMidiCC` in `consts.cpp` are the hot-path inline versions controlled by `TEXT_OUTPUT`
5. Settings / SysEx (`src/settings.h/settings.cpp`) — handles bidirectional communication with a host editor: `Input()` parses incoming SysEx commands, `SendPinSetting` / `ExecCommand` respond
6. EEPROM persistence (`src/eeprom.h/eeprom.cpp`) — `LoadAllEEPROM` / `SaveEEPROM` store per-pin parameters so settings survive power cycles

**Global state** is defined in `src/consts.cpp` and declared `extern` in `src/consts.h`: `Mode`, `GlobalTime`, `Pin[]` array, HiHat note/threshold tables, xtalk group state, digital pin states.

## Pin Types & States

`PinType` enum (in `pin.h`): `Piezo`, `Switch`, `HHC` (hi-hat controller), `HH` (hi-hat), `HHs`, `YSwitch`, `Disabled`.

`state` enum tracks the timing FSM per pad: `Normal_Time → Scan_Time → Mask_Time → Retrigger_Time`, plus `Piezo_Time`, `Switch_Time`, `Choke_Time`, `Footsplash_Time`, `Footclose_Time`, `Scanretrigger_Time`.

## Debugging

Set `TEXT_OUTPUT 1` in `define.h` to print MIDI events as text. The serial monitor in `make run` runs at 31250 baud (raw MIDI rate) — use a different baud rate in `setup()` if doing text debugging.