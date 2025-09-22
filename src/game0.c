#include "games.h"
#include "led.h"
#include "button.h"
#include "priorities.h"

#include <limits.h>
#include <stdio.h>

BaseType_t game0_led_end = pdFALSE; // set when LED1 or LED6 is on

/*
 * static void game0_attract_task(void *parameter)
 *  The attract mode task for game mode 0 (Catch the Running Light).
 *  This task will be run during game selection only, and is activated by the
 *  menu task by resuming.
 *  Inputs:
 *   - parameter : Parameters provided by xTaskCreate; ignored.
 *  Output: None.
 */
static void game0_attract_task(void *parameter) {
    (void) parameter;
    while (true) {
        for (uint i = 0; i < 6; i++) { // normal direction (0 -> 5)
            led_set(1 << i);
            vTaskDelay(GAME_SPEED_MED_TICKS);
        }
        for (uint i = 4; i > 0; i--) { // reverse direction (4 -> 1)
            led_set(1 << i);
            vTaskDelay(GAME_SPEED_MED_TICKS);
        }
    }
}

/*
 * static void game0_input_task(void *parameter)
 *  The input task for game mode 0 (Catch the Running Light).
 *  This task will be run when the game is in progress or finished, and is
 *  started/stopped by the main task through resuming/suspending.
 *  Inputs:
 *   - parameter : Parameters provided by xTaskCreate; ignored.
 *  Output: None.
 */
static void game0_input_task(void *parameter) {
    while (true) {
        EventBits_t event = xEventGroupWaitBits(
            btn_event_group,
            BTN_EV_SHORT_PRESS,
            pdTRUE, pdFALSE, // wait for any event, and clear bits when recv'd
            portMAX_DELAY
        );
        if (event & BTN_EV_SHORT_PRESS) { // short press received
            if (game0_led_end) {
                game_score += GAME_GAIN;
                if (game_score >= GAME_GOAL) game_finish();
            }
            else {
                if (game_score >= GAME_LOSS) game_score -= GAME_LOSS;
                else game_score = 0;
            }
            printf(CLEAR_LINE "\rScore: %d", game_score);
        }
        // NOTE: the menu task will handle long presses
    }
}

TaskHandle_t game0_input_task_handle = NULL;
    // task handle for game0_input_task

/*
 * static void game0_main_task(void *parameter)
 *  The main (display/scoring) task for game mode 0 (Catch the Running Light).
 *  This task will be run when the game is in progress or finished, and is
 *  started/stopped by semaphore giving.
 *  Inputs:
 *   - parameter : Parameters provided by xTaskCreate; ignored.
 *  Output: None.
 */
static void game0_main_task(void *parameter) {
    (void) parameter;

    while (true) {
        while (
            ulTaskNotifyTake(pdTRUE, portMAX_DELAY) != pdTRUE
        ); // wait until we receive a notification - then we can start the game

        printf("Score: %d", game_score);
        if (!game0_input_task_handle) { // launch input task
            hard_assert(
                xTaskCreate(
                    game0_input_task, "game0_input", 200, NULL,
                    GAME_INPUT_PRIORITY,
                    &game0_input_task_handle
                ) == pdPASS
            );
        } else vTaskResume(game0_input_task_handle); // resume suspended task

        while (true) {
            for (uint i = 0; i < 6; i++) { // normal direction
                game0_led_end = (i == 0 || i == 5) ? pdTRUE : pdFALSE;
                led_set(1 << i);
                if (ulTaskNotifyTake(pdTRUE, game_speed) == pdTRUE) goto done;
                    // game finished
            }
            game0_led_end = pdFALSE;
            for (uint i = 4; i > 0; i--) { // reverse direction
                led_set(1 << i);
                if (ulTaskNotifyTake(pdTRUE, game_speed) == pdTRUE) goto done;
            }
        }
done: // game finished - go back to waiting for restart
        vTaskSuspend(game0_input_task_handle); // suspend input task
    }
}

/* game definition */
game_t game0_def = {
    "Catch the Running Light",

    "game0_attract",
    game0_attract_task,

    "game0_main",
    game0_main_task
};
