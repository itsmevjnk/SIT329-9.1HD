#include "led.h"
#include "pin_config.h"

#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "hardware/gpio.h"

/* LED pins array */
static const uint led_pins[] = { LED1, LED2, LED3, LED4, LED5, LED6 };

#define LED_MASK  (1 << LED1) | (1 << LED2) | (1 << LED3) \
                | (1 << LED4) | (1 << LED5) | (1 << LED6)

/* LED queue item structure */
typedef struct {
    uint data : 31; // mask or blink period in milliseconds
    uint blink : 1; // set for blink command
} __attribute__((packed)) led_command_t;

#define LED_QUEUE_LENGTH                    10
static QueueHandle_t led_queue; // LED command queue (see led_task below)

/*
 * static void led_task(void *parameters)
 *  Task for receiving LED strip control commands. This effectively multiplexes
 *  LED control to the game tasks safely.
 *  Inputs:
 *   - parameters : Parameters provided by xTaskCreate; ignored.
 *  Output: None.
 */
static void led_task(void *parameters) {
    (void) parameters;

    while (true) {
        /* receive command */
        led_command_t command;
        if (xQueueReceive(led_queue, &command, portMAX_DELAY) != pdPASS)
            continue;

handle_command:
        if (command.blink && command.data) { // blink LEDs at the given period
            while (true) {
                gpio_put_masked(LED_MASK, (uint32_t)-1);
                if (
                    xQueueReceive(
                        led_queue, &command,
                        pdMS_TO_TICKS(command.data)
                    ) == pdPASS
                ) goto handle_command;
                gpio_put_masked(LED_MASK, 0);
                if (
                    xQueueReceive(
                        led_queue, &command,
                        pdMS_TO_TICKS(command.data)
                    ) == pdPASS
                ) goto handle_command;
                // NOTE: We use xQueueReceive() to delay until the period is
                //       over, or a new command is received from the queue (at
                //       which point we handle the command again).
            }
        } else { // set LED according to mask
            for (uint i = 0; i < 6; i++) {
                gpio_put(led_pins[i], (command.data & (1 << i)));
            }
        }
    }
}

void led_init() {
    gpio_init_mask(LED_MASK);
    gpio_set_dir_out_masked(LED_MASK);
    gpio_put_masked(LED_MASK, 0); // initially turn all LEDs off

    /* create command queue */
    led_queue = xQueueCreate(LED_QUEUE_LENGTH, sizeof(led_command_t));
    hard_assert(led_queue);

    /* create LED driver task */
    hard_assert(
        xTaskCreate(
            led_task, "led_driver", 200, NULL,
            configMAX_PRIORITIES - 1, // maximum priority for drivers
            NULL
        ) == pdPASS
    );
}

void led_set(uint mask) {
    led_command_t cmd = { mask, 0 };
    xQueueSend(led_queue, &cmd, portMAX_DELAY);
}

void led_blink(uint period) {
    led_command_t cmd = { period, 1 };
    xQueueSend(led_queue, &cmd, portMAX_DELAY);
}
