#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "buttons.h"

bool button1State = false;
bool lastButton1State = false;

bool button2State = false;
bool lastButton2State = false;
unsigned long button2PressTime = 0;
bool button2LongPressSent = false;

bool button3State = false;
bool lastButton3State = false;

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

  if (toNano.available()) {
    String data = toNano.readStringUntil('\n');
    Serial.println(data); 
  }
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
