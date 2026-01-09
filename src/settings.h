#pragma once

#include <Arduino.h>

//==============================
//    SETTING
//==============================
void SendPinSetting(byte pin, byte Set);
byte GetPinSetting(byte pin, byte Set);
void SendHHSetting(byte Set);
void SendGeneralSetting(byte Set);
void ExecCommand(int Cmd, int Data1, int Data2, int Data3);

//==============================
//    INPUT
//==============================
void Input();