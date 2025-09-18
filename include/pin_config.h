#pragma once

/* LED pins */
#define LED1                    1
#define LED2                    3
#define LED3                    4
#define LED4                    6
#define LED5                    7
#define LED6                    8
#define LED_BUTTON              25 // to signal long press (set to onboard LED)

#define LED_WS2812              23 // WS2812 data pin

/* I2C pins (for SSD1306 OLED display) */
#define I2C_SCL                 17
#define I2C_SDA                 16

/* push button */
#define BUTTON                  24 // set to USR button on VCC-GND YD RP2040

/* button press duration configuration */
#define BTN_DEBOUNCE            50 // minimum trigger duration for debouncing
#define BTN_LONG_PRESS          1000 // long press duration
