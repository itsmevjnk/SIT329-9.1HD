#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "pico/stdlib.h"


void hello_task() {
    while (true) {
        printf("Hello, World!\n");
        vTaskDelay(1000);
    }
}

int main()
{
    stdio_init_all();

    xTaskCreate(hello_task, "HelloWorld", 256, NULL, 1, NULL);
    vTaskStartScheduler();

    while(1){};
}