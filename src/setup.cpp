#include "setup.h"
#include "consts.h"
#include "eeprom.h"
#include "pin.h"
#include "utility.h"
#include "define.h"
//==============================================================================
// SETUP
//==============================================================================
void setup() {
  Serial.begin(31250);  // MIDI
  Serial.flush();
  while(!Serial);
  //----------------------------------------------------------------------------
  // Multiplexer / Digital Pins Configuration
  //----------------------------------------------------------------------------
  // Enable digital pins with pull-up resistors
  pinMode(Choke1_Pin, INPUT_PULLUP);
  pinMode(Choke2_Pin, INPUT_PULLUP);
  pinMode(Choke3_Pin, INPUT_PULLUP);
  pinMode(ChokeRide_Pin, INPUT_PULLUP);
  pinMode(Aux1_Pin, INPUT_PULLUP);
  pinMode(Aux2_Pin, INPUT_PULLUP);
  pinMode(Aux3_Pin, INPUT_PULLUP);
  pinMode(Aux4_Pin, INPUT_PULLUP);
  pinMode(Aux5_Pin, INPUT_PULLUP);
  pinMode(Aux6_Pin, INPUT_PULLUP);
  pinMode(Aux7_Pin, INPUT_PULLUP);

  //----------------------------------------------------------------------------
  // Pin Initialization
  //----------------------------------------------------------------------------
  GlobalTime = TIMEFUNCTION;
  Serial.print("Pin Initialization\n");
  for (int count = 0; count < NPin; count++) {
    Pin[count].set(count);
	Serial.println(Pin[count]);
  }


  //----------------------------------------------------------------------------
  // Serial Communication
  //----------------------------------------------------------------------------


  //----------------------------------------------------------------------------
  // Analog Reference
  //----------------------------------------------------------------------------
  #if defined(__AVR__)
    analogReference(DEFAULT);
  #endif

  //----------------------------------------------------------------------------
  // Load EEPROM Settings
  //----------------------------------------------------------------------------
  //LoadAllEEPROM();

  //----------------------------------------------------------------------------
  // ADC Prescaler Configuration
  //----------------------------------------------------------------------------
  #if defined(__AVR__)
    #if FASTADC
      setPrescaler(Prescaler_16);
    #endif
  #elif defined(__arm__)
    REG_ADC_MR = (REG_ADC_MR & 0xFFF0FFFF) | 0x00030000;
  #endif
}