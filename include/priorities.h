#pragma once

#include "FreeRTOS.h"
#include "task.h"

#define MIN_PRIORITY                            (tskIDLE_PRIORITY + 1)
#define MAX_PRIORITY                            (configMAX_PRIORITIES - 1)

/* priority for driver tasks - set to max priority */
#define DRIVER_PRIORITY                         MAX_PRIORITY

/* priority for main task - set to min priority */
#define MAIN_PRIORITY                           MIN_PRIORITY

/* priority for game main tasks - set to one above main task */
#define GAME_MAIN_PRIORITY                      (MAIN_PRIORITY + 1)

/* priority for game input tasks - set to one above game main task */
#define GAME_INPUT_PRIORITY                     (GAME_MAIN_PRIORITY + 1)

/* priority for game attract tasks - set to main task priority */
#define GAME_ATTRACT_PRIORITY                   MAIN_PRIORITY

/* priority for UART/stdio button emulation task - set to main task priority */
#define BTN_UART_PRIORITY                       MAIN_PRIORITY
// NOTE: we don't want high priorities as it can starve other tasks