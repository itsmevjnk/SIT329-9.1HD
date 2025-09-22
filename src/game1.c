#include "games.h"
#include "led.h"
#include "button.h"
#include "priorities.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

int game1_num_presses = 0; // number of presses needed

/*
 * static void game1_attract_task(void *parameter)
 *  The attract mode task for game mode 1 (Identify the LED).
 *  This task will be run during game selection only, and is activated by the
 *  menu task by resuming.
 *  Inputs:
 *   - parameter : Parameters provided by xTaskCreate; ignored.
 *  Output: None.
 */
static void game1_attract_task(void *parameter) {
    (void) parameter;
    while (true) {
        uint idx = rand() % 6; // LED index to turn on
        led_set((1 << idx));
        vTaskDelay(GAME_SPEED_MED_TICKS);
        led_set(0);
        vTaskDelay(GAME_SPEED_MED_TICKS);
    }
}

/*
 * static void game1_input_task(void *parameter)
 *  The input task for game mode 1 (Identify the LED).
 *  This task will be run when the game is in progress or finished, and is
 *  started/stopped by the main task through resuming/suspending.
 *  Inputs:
 *   - parameter : Parameters provided by xTaskCreate; ignored.
 *  Output: None.
 */
static void game1_input_task(void *parameter) {
    while (true) {
        EventBits_t event = xEventGroupWaitBits(
            btn_event_group,
            BTN_EV_SHORT_PRESS,
            pdTRUE, pdFALSE, // wait for any event, and clear bits when recv'd
            portMAX_DELAY
        );
        if (event & BTN_EV_SHORT_PRESS) game1_num_presses--;
        // NOTE: the menu task will handle long presses
    }
}

TaskHandle_t game1_input_task_handle = NULL;
    // task handle for game1_input_task

#define GAME1_WAIT                          1000 // button press record period

/*
 * static void game1_main_task(void *parameter)
 *  The main (display/scoring) task for game mode 1 (Identify the LED).
 *  This task will be run when the game is in progress or finished, and is
 *  started/stopped by semaphore giving.
 *  Inputs:
 *   - parameter : Parameters provided by xTaskCreate; ignored.
 *  Output: None.
 */
static void game1_main_task(void *parameter) {
    (void) parameter;

    while (true) {
        while (
            ulTaskNotifyTake(pdTRUE, portMAX_DELAY) != pdTRUE
        ); // wait until we receive a notification - then we can start the game

        int total_num_presses = 0;
        printf("Score: %d", game_score); // should be 0
        while (game_score < GAME_GOAL) {
            int new_num_presses;
            do { // keep on randomising until we have a different count
                new_num_presses = rand() % 6 + 1;
            } while (new_num_presses == total_num_presses);
            int total_num_presses = new_num_presses; // number of button presses
            game1_num_presses = total_num_presses;
            led_set((1 << (game1_num_presses - 1))); // light up the corresponding LED

            if (!game1_input_task_handle) { // launch input task
                hard_assert(
                    xTaskCreate(
                        game1_input_task, "game1_input", 200, NULL,
                        GAME_INPUT_PRIORITY,
                        &game1_input_task_handle
                    ) == pdPASS
                );
            } else vTaskResume(game1_input_task_handle); // resume suspended task
            
            vTaskSuspend(game1_input_task_handle); // suspend input task

            if (ulTaskNotifyTake(pdTRUE, game_speed) == pdTRUE) break;
                // game ended prematurely

            led_set(0); // turn LEDs off

            if (ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(GAME1_WAIT)) == pdTRUE)
                break;
            
            if (game1_num_presses == 0) game_score += GAME_GAIN;
            else {
                if (game_score > GAME_LOSS) game_score -= GAME_LOSS;
                else game_score = 0;
            }
            printf(
                CLEAR_LINE "\rScore: %d [button pressed %d/%d times]",
                game_score,
                total_num_presses - game1_num_presses, total_num_presses
            );
        }
        putchar('\n'); // end score line
        if (game_score >= GAME_GOAL)
            game_finish(); // game finished - signal upstream
    }
}

/* game definition */
game_t game1_def = {
    "Identify the LED",

    "game1_attract",
    game1_attract_task,

    "game1_main",
    game1_main_task
};
