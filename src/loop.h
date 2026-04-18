#pragma once

#include "utility.h"
#include "settings.h"

//==============================
//     LOOP
// Main Arduino loop — called repeatedly by the runtime.
// Reads digital choke/aux pins (if CHOKE enabled), scans all analog pads,
// fires MIDI notes for completed hits, then resets the xtalk peak tables.
//==============================
void loop();

//==============================
//    LOGTOOL
// Forwards a single ADC sample to the host editor via SendLog() when Tool
// mode is active and the sample exceeds the log threshold.
// Called from pin::scan() for the selected LogPin on each iteration.
//==============================
void LogTool(int yn_0, byte MulSensor);