#include "button.h"

#include <stdio.h>

/*
 * static void btn_uart_task(void *parameters)
 *  Task for handling UART-based button emulation.
 *  Inputs:
 *   - parameters : Parameters from xTaskCreate; ignored.
 *  Output: None.
 */
static void btn_uart_task(void *parameters) {
    (void) parameters;

    while (true) {
        char c = getchar(); // get character from stdio (UART)

        switch (c) {
            case ' ': // SPACE - short press
                xEventGroupSetBits(btn_event_group, BTN_EV_SHORT_PRESS);
                break;
            case '\r': // RETURN - long press
                xEventGroupSetBits(btn_event_group, BTN_EV_LONG_PRESS);
                break;
            default: // invalid key
                putchar('\a'); // bell character - makes a sound on term emu
                break;
        }
    }
}

void btn_uart_init() {
    hard_assert(
        xTaskCreate(
            btn_uart_task, "btn_uart", 200, NULL,
            tskIDLE_PRIORITY + 1,
            // NOTE: we don't want high priorities as it can starve other tasks
            NULL
        ) == pdPASS
    );
}