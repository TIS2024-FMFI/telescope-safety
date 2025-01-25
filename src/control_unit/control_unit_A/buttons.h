#include <SoftwareSerial.h>

// GPIO piny
#define TX_PIN 14  // GP14 ako TX // CIERNY
#define RX_PIN 15  // GP15 ako RX // BIELY
#define BUTTON1_PIN 0  // Tlačidlo na GP0
#define BUTTON2_PIN 1  // Tlačidlo na GP1
#define BUTTON3_PIN 3  // Tlačidlo na GP3
#define BUTTON4_PIN 8  // Tlačidlo na GP8

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
