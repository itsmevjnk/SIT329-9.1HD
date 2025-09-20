#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"

#include "button.h"
#include "led.h"
#include "ws2812.h"
#include "games.h"

/* colours for WS2812 LED */
#define OFF                     0x000000
#define RED                     0x7F0000
#define GREEN                   0x007F00

void menu_task(void *parameters) {
    ws2812_show(OFF);
    while (true) {
        EventBits_t event = xEventGroupWaitBits(
            btn_event_group,
            BTN_EV_SHORT_PRESS | BTN_EV_LONG_PRESS,
            pdTRUE, pdFALSE, // wait for any event, and clear bits when recv'd
            portMAX_DELAY
        );
        if (event & BTN_EV_SHORT_PRESS) {
            ws2812_show(GREEN);
            game_start_attract(0);
        }
        if (event & BTN_EV_LONG_PRESS) {
            ws2812_show(RED);
            game_stop_attract(0);
            led_set(0);
        }
    }
}

int main()
{
    stdio_init_all();

    sleep_ms(10000);

    btn_init();
    led_init();
    ws2812_init();
    game_init();

    hard_assert(xTaskCreate(menu_task, "menu", 256, NULL, 1, NULL) == pdPASS);
    vTaskStartScheduler();

    while(1){};
}