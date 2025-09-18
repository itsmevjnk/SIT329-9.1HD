/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ws2812.h"
#include "pin_config.h"

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"

#include <stdio.h>

static PIO ws2812_pio;
static uint ws2812_sm;

void ws2812_show(uint32_t colour) {
    // shuffle 0xRRGGBB to 0xGGRRBB
    colour =
        ((colour & 0xFF0000) >> 8)
        | ((colour & 0x00FF00) << 8)
        | (colour & 0x0000FF);

    pio_sm_put_blocking(ws2812_pio, ws2812_sm, colour << 8u);
}

void ws2812_init() {
    /* initialise WS2812 PIO */
    uint offset;
    hard_assert(
        pio_claim_free_sm_and_add_program_for_gpio_range(
            &ws2812_program, &ws2812_pio, &ws2812_sm, &offset,
            LED_WS2812, 1, true
        )
    );
    ws2812_program_init(
        ws2812_pio, ws2812_sm, offset, LED_WS2812, 800000, false
    );
}
