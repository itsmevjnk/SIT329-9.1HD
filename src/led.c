#include "led.h"
#include "pin_config.h"

#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"

#include "hardware/gpio.h"

/* LED pins array */
static const uint led_pins[] = { LED1, LED2, LED3, LED4, LED5, LED6 };

#define LED_MASK  (1 << LED1) | (1 << LED2) | (1 << LED3) \
                | (1 << LED4) | (1 << LED5) | (1 << LED6)

void led_init() {
    gpio_init_mask(LED_MASK);
    gpio_set_dir_out_masked(LED_MASK);
    gpio_put_masked(LED_MASK, 0); // initially turn all LEDs off
}

void led_set(uint mask) {
    for (uint i = 0; i < 6; i++) {
        gpio_put(led_pins[i], (mask & (1 << i)));
    }
}
