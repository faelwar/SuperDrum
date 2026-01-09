#pragma once
#include <Arduino.h>
#include <EEPROM.h>
#include "consts.h"
#define TEXT_OUTPUT 1

//==============================================================================
// EEPROM
//==============================================================================

void LoadAllEEPROM();

void LoadEEPROM(byte Pin, byte Param);

void SaveEEPROM(byte Pin, byte Param, byte Value);

void SaveEEPROM(byte Pin, byte Param);

void SaveGeneralEEPROM(byte Param, byte Value);

void SaveGeneralEEPROM(byte Param);

void SaveHHEEPROM(byte Param, byte Value);

void SaveHHEEPROM(byte Param);

void LoadGeneralEEPROM(byte Param);

void LoadHHEEPROM(byte Param);