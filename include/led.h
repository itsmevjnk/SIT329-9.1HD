#pragma once

#include <stddef.h>
#include <stdint.h>

#include "pico/types.h"

/*
 * void led_init()
 *  Initialises the LED strip.
 *  Inputs: None.
 *  Output: None.
 */
void led_init();

/*
 * void led_set(uint mask)
 *  Sets the LED strip to the given states.
 *  Using this function also disables LED blinking (see below).
 *  Inputs:
 *   - mask : Bitfield containing states of LED1-6.
 *  Output: None.
 */
void led_set(uint mask);

