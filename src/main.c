#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "pico/stdlib.h"

#include "button.h"
#include "led.h"

void hello_task(void *parameters) {
    while (true) {
        EventBits_t event = xEventGroupWaitBits(
            btn_event_group,
            BTN_EV_SHORT_PRESS | BTN_EV_LONG_PRESS,
            pdTRUE, pdFALSE, // wait for any event, and clear bits when recv'd
            portMAX_DELAY
        );
        if (event & BTN_EV_SHORT_PRESS)
            printf("Short press detected\n");
        if (event & BTN_EV_LONG_PRESS)
            printf("Long press detected\n");
    }
}

void led_task(void *parameters) {
    while (true) {
        for (uint i = 0; i < 6; i++) {
            led_set((1 << i)); // turn each LED on, one at a time
            vTaskDelay(pdMS_TO_TICKS(500));
        }
    }
}

int main()
{
    stdio_init_all();

    btn_init();
    led_init();

    hard_assert(xTaskCreate(hello_task, "HelloWorld", 256, NULL, 1, NULL) == pdPASS);
    hard_assert(xTaskCreate(led_task, "LED", 256, NULL, 1, NULL) == pdPASS);
    vTaskStartScheduler();

    while(1){};
}