#pragma once
#include <Arduino.h>

//==============================
//    MENU     
//==============================
void Menu();   

//==============================
//    UP
//==============================

void Up();
 
 //==============================
//    DRAW
//==============================
void Draw();
 
//==============================
//    DOWN
//==============================
 void Down();

#if MENU_LOG
void DrawLog(byte x);

#endif

//========================================
//   PRINTNAME
//========================================
#if USE_DEFAULT_NAME
void DefaultPrintName(byte pin,bool sel);
#endif

void DrawDiagnostic(byte i,byte val);
void MenuInt(int inter,bool sel) ;

void MenuInt(int inter,char A,char B);
void MenuString(const PROGMEM char *s,bool sel);
