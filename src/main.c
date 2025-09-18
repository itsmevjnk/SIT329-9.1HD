#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"

#include "button.h"
#include "led.h"
#include "ws2812.h"

/* colours for WS2812 LED */
#define RED                     0x7F0000
#define GREEN                   0x007F00

void menu_task(void *parameters) {
    ws2812_show(RED);
    while (true) {
        led_blink(250);
        vTaskDelay(pdMS_TO_TICKS(3000));
        led_blink(500);
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}

int main()
{
    stdio_init_all();

    btn_init();
    led_init();
    ws2812_init();

    hard_assert(xTaskCreate(menu_task, "menu", 256, NULL, 1, NULL) == pdPASS);
    vTaskStartScheduler();

    while(1){};
}