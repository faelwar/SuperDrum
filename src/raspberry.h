#pragma once
//==============================
//    RASPBERRY PI
//==============================
#if USE_PISERIAL

#if defined(__arm__) 
  #define mySerial Serial2
#elif defined(__AVR__) 
  SoftwareSerial mySerial(6, 5); // RX, TX
#endif


void piNote(byte note, byte velocity);

#endif
