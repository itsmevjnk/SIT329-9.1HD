#pragma once

#include "FreeRTOS.h"
#include "event_groups.h"

/* button events */
#define BTN_EV_SHORT_PRESS              (1 << 0) // bitmask for event group
#define BTN_EV_LONG_PRESS               (1 << 1)
extern EventGroupHandle_t btn_event_group;

/*
 * void btn_init()
 *  Initialises button handling. Button pin change events are serviced by an
 *  ISR, and delivered as a FreeRTOS event group (see btn_event_group above).
 *  Inputs: None.
 *  Output: None.
 */
void btn_init();

/*
 * void btn_uart_init()
 *  Initialises UART-based button handling. The SPACE and RETURN keys will act
 *  as short and long presses respectively.
 *  Inputs: None.
 *  Output: None.
 */
void btn_uart_init();
