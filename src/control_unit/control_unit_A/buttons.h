#ifndef BUTTONS_H
#define BUTTONS_H

#include <SoftwareSerial.h>
#include "common_structs.h"
#include "lora_communication.h"

// GPIO piny
#define TX_PIN 14  // GP14 ako TX // CIERNY
#define RX_PIN 15  // GP15 ako RX // BIELY
#define BUTTON1_PIN 0  // Tlačidlo na GP0
//BUTTON1 - prepinanie medzi nastaveniami a zobrazovanim aktualnych dat
#define BUTTON2_PIN 1  // Tlačidlo na GP1
//BUTTON2 stlacenie - prepinanie medzi stupnami, minutami, sekundami v nastaveniach
//BUTTON2 podrzanie - odoslanie nastaveneho azimutu na inercialnu jednotku
#define BUTTON3_PIN 3  // Tlačidlo na GP3
//BUTTON3 - zvysenie cisla na ktorom sme akutane nastaveni v nastaveniach
#define BUTTON4_PIN 8  // Tlačidlo na GP8
//BUTTON4 - znizenie cisla na ktorom sme akutane nastaveni v nastaveniach

extern bool button1State;
extern bool lastButton1State;

extern bool button2State;
extern bool lastButton2State;
extern unsigned long button2PressTime;
extern bool button2LongPressSent;

extern bool button3State;
extern bool lastButton3State;

extern bool button4State;
extern bool lastButton4State;

extern SoftwareSerial toNano;

void setupButtons();
void loopButtons();
DegreesMinutesSeconds convertToDMS(double decimalDegrees, bool isElevation);
double convertToDecimalDegrees(DegreesMinutesSeconds dms);

#endif
