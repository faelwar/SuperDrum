#pragma once
#if USE_WAVTRIGGER
#include <Arduino.h>
void wavChoke(byte pin);

void wavTriggerHHC(byte hhc);

void wavTrigger(byte pin, byte velocity);

short Pin2Sound(byte pin);

void demo(byte repeat);


void wavChoke_Custom(byte pin);

void wavTrigger_Custom(byte pin, byte velocity);


void wavChoke_Fixed(byte pin);

void wavTrigger_Fixed(byte pin, byte velocity);


void wavChoke_BX(byte pin);

void wavTrigger_BX(byte pin, byte velocity);


void wavChoke_FS(byte pin);

void wavTrigger_FS(byte pin, byte velocity);



void wavChoke_BM(byte pin);

void wavTrigger_BM(byte pin, byte velocity);

//========================================
//  DRIVER
//========================================

void wavTriggerPlay(int n, int vol);

void wavTriggerFade(short start, byte count, byte rr);

#endif
