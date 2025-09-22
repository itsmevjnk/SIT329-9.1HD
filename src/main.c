#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"

#include "button.h"
#include "led.h"
#include "ws2812.h"
#include "games.h"

/* colours for WS2812 LED */
#define OFF                     0x000000
#define RED                     0x7F0000
#define GREEN                   0x007F00

void menu_task(void *parameters) {
    uint game = 0;
    while (true) {
        /* attract mode - select game */
        ws2812_show(GREEN);
        game_start_attract(game);
        printf(
            "[RETURN/LONG PRESS] Select this game [SPACE/PRESS] Next game\n"
            "Select a game: %s", game_get_name(game)
        );
        while (true) {
            EventBits_t event = xEventGroupWaitBits(
                btn_event_group,
                BTN_EV_SHORT_PRESS | BTN_EV_LONG_PRESS,
                pdTRUE, pdFALSE, // wait for any event, and clear when received
                portMAX_DELAY
            );
            if (event & BTN_EV_SHORT_PRESS) {
                game++;
                if (game >= num_games) game = 0;
                game_start_attract(game);
                printf(CLEAR_LINE "\rSelect a game: %s", game_get_name(game));
                // NOTE: go back to line start and clear it for new game name
            }
            if (event & BTN_EV_LONG_PRESS) break;
        }
        game_stop_attract(game);

        /* select game speed */
        ws2812_show(RED);
        led_blink(game_speed);
        printf(
            "\n[RETURN/LONG PRESS] Select this speed "
            "[SPACE/PRESS] Next speed\n"
            // NOTE: we need to send a newline for the previous section
            "Select a speed: %s", game_get_speed_str()
        );
        while (true) {
            EventBits_t event = xEventGroupWaitBits(
                btn_event_group,
                BTN_EV_SHORT_PRESS | BTN_EV_LONG_PRESS,
                pdTRUE, pdFALSE, // wait for any event, and clear when received
                portMAX_DELAY
            );
            if (event & BTN_EV_SHORT_PRESS) {
                switch (game_speed) { // cycle through game speeds
                    case GAME_SPEED_SLOW_TICKS:
                        game_speed = GAME_SPEED_MED_TICKS;
                        break;
                    case GAME_SPEED_MED_TICKS:
                        game_speed = GAME_SPEED_FAST_TICKS;
                        break;
                    case GAME_SPEED_FAST_TICKS:
                        game_speed = GAME_SPEED_SLOW_TICKS;
                        break;
                }
                led_blink(game_speed); // update blink rate
                printf(
                    CLEAR_LINE "\rSelect a speed: %s", game_get_speed_str()
                );
            }
            if (event & BTN_EV_LONG_PRESS) break;
        }
        led_set(0); // turn off all LEDs and stop blinking

        /* run the game */
        printf("\nPress RETURN or LONG PRESS at any time to restart.\n");
        ws2812_show(OFF);
        game_score = 0; // remember to reset the game score!
        TickType_t t_start = xTaskGetTickCount();
        game_start_stop(game);
        while (true) {
            EventBits_t event = xEventGroupWaitBits(
                btn_event_group,
                BTN_EV_LONG_PRESS | GAME_EV_FINISHED,
                pdTRUE, pdFALSE, // wait for any event, and clear when received
                portMAX_DELAY
            );
            if (event & BTN_EV_LONG_PRESS) { // game reset
                printf("Game reset\n");
                break;
            }
            if (event & GAME_EV_FINISHED) { // game finished
                TickType_t t_stop = xTaskGetTickCount();
                printf(
                    "Playing time: %.3f sec\n",
                    (float)(t_stop - t_start) / configTICK_RATE_HZ
                    /* tick = time (sec) * configTICK_RATE_HZ */
                );
                break;
            }
        }   
        game_start_stop(game); // stop game
    }
}

int main()
{
    stdio_init_all();

    btn_init();
    btn_uart_init();
    led_init();
    ws2812_init();
    game_init();

    hard_assert(xTaskCreate(menu_task, "menu", 256, NULL, 1, NULL) == pdPASS);
    vTaskStartScheduler();

    while(1){};
}