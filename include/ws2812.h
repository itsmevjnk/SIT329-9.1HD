#pragma once

#include <stddef.h>
#include <stdint.h>

#include "pico/types.h"

/*
 * void ws2812_init()
 *  Initialises the WS2812 PIO.
 *  Inputs: None.
 *  Output: None.
 */
void ws2812_init();

/*
 * void ws2812_show(uint32_t colour)
 *  Shows a colour on the WS2812 LED.
 *  Inputs:
 *   - colour : The colour to be shown (0xRRGGBB format).
 *  Output: None.
 */
void ws2812_show(uint32_t colour);