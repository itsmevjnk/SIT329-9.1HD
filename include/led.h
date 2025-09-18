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

/*
 * void led_blink(uint period)
 *  Sets the LED strip to blink with the given toggling period.
 *  Inputs:
 *   - period : The toggle period in milliseconds.
 *              Setting this to 0 disables LED blinking.
 *  Output: None.
 */
void led_blink(uint period);

