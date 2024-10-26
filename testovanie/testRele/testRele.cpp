#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"


int main()
{
    stdio_init_all();
    
    const uint RELE_PIN = 25;

    gpio_init(RELE_PIN);
    gpio_set_dir(RELE_PIN, GPIO_OUT);


    while (true) { 
        gpio_put(RELE_PIN, 1);
        sleep_ms(1000);

        gpio_put(RELE_PIN, 0);
        sleep_ms(1000);
    }
}
