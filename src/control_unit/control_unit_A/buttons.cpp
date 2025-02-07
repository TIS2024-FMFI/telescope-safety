#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "buttons.h"

//BUTTON1 - prepinanie medzi nastaveniami a zobrazovanim aktualnych dat
bool button1State = false;
bool lastButton1State = false;

//BUTTON2 stlacenie - prepinanie medzi stupnami, minutami, sekundami v nastaveniach
//BUTTON2 podrzanie - odoslanie nastaveneho azimutu na inercialnu jednotku
bool button2State = false;
bool lastButton2State = false;
unsigned long button2PressTime = 0;
bool button2LongPressSent = false;

//BUTTON3 - zvysenie cisla na ktorom sme akutane nastaveni v nastaveniach
bool button3State = false;
bool lastButton3State = false;

//BUTTON4 - znizenie cisla na ktorom sme akutane nastaveni v nastaveniach
bool button4State = false;
bool lastButton4State = false;

SoftwareSerial toNano(15,14);  // RX = 15 BIELY, TX =14 CIERNY   (should connect opposite at PICO2)



void setupButtons() {
  gpio_init(BUTTON1_PIN);
  gpio_set_dir(BUTTON1_PIN, GPIO_IN);
  gpio_pull_down(BUTTON1_PIN);
  
  gpio_init(BUTTON2_PIN);
  gpio_set_dir(BUTTON2_PIN, GPIO_IN);
  gpio_pull_down(BUTTON2_PIN);

  gpio_init(BUTTON3_PIN);
  gpio_set_dir(BUTTON3_PIN, GPIO_IN);
  gpio_pull_down(BUTTON3_PIN);

  gpio_init(BUTTON4_PIN);
  gpio_set_dir(BUTTON4_PIN, GPIO_IN);
  gpio_pull_down(BUTTON4_PIN);

  toNano.begin(9600);
}

void loopButtons() {
  button1State = gpio_get(BUTTON1_PIN);
  button2State = gpio_get(BUTTON2_PIN);
  button3State = gpio_get(BUTTON3_PIN);
  button4State = gpio_get(BUTTON4_PIN);

  if (button1State && !lastButton1State) {
    toNano.write("BUTTON1\n");
  } else if (button2State) {
    if (!lastButton2State) {
      button2PressTime = to_ms_since_boot(get_absolute_time());
      button2LongPressSent = false;
    } else {
      unsigned long pressDuration = to_ms_since_boot(get_absolute_time()) - button2PressTime;
      if (pressDuration > 2000 && !button2LongPressSent) {
        toNano.write("SEND\n");
        button2LongPressSent = true;
      }
    }
  } else if (!button2State && lastButton2State) {
    if (!button2LongPressSent) {
      toNano.write("BUTTON2\n");
      
    }
  } else if (button3State && !lastButton3State) {
    toNano.write("BUTTON3\n");
  } else if (button4State && !lastButton4State) {
    toNano.write("BUTTON4\n");
  }

  lastButton1State = button1State;
  lastButton2State = button2State;
  lastButton3State = button3State;
  lastButton4State = button4State;
}

DegreesMinutesSeconds convertToDMS(double decimalDegrees, bool isElevation) {
    DegreesMinutesSeconds dms;

    bool isNegative = decimalDegrees < 0;

    decimalDegrees = fabs(decimalDegrees);

    dms.degrees = (int)decimalDegrees;
    double fractionalPart = decimalDegrees - dms.degrees;
    double totalMinutes = fractionalPart * 60.0;
    dms.minutes = (int)totalMinutes;
    double totalSeconds = (totalMinutes - dms.minutes) * 60.0;
    dms.seconds = (int)round(totalSeconds);

    if (dms.seconds == 60) {
        dms.seconds = 0;
        dms.minutes += 1;
    }

    if (dms.minutes == 60) {
        dms.minutes = 0;
        dms.degrees += 1;
    }

    if (isElevation && isNegative) {
        dms.degrees = -dms.degrees; 
    }

    return dms;
}


double convertToDecimalDegrees(DegreesMinutesSeconds dms) {
  double decimalDegrees = dms.degrees + (dms.minutes / 60.0f) + (dms.seconds / 3600.0f);
  if (decimalDegrees < 0.0f) {
    decimalDegrees += 360.0f;
  }
  if (decimalDegrees >= 360.0f) {
    decimalDegrees -= 360.0f;
  }
  return decimalDegrees;
}

