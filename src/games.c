#include "games.h"

#include <stdio.h>

int game_score = 0;
uint32_t game_speed = GAME_SPEED_SLOW; 

extern game_t game0_def;
extern game_t game1_def;

/* game definitions list */
game_t *game_defs[] = {
    &game0_def, &game1_def
};

void game_init() {
    for (uint i = 0; i < sizeof(game_defs) / sizeof(game_t *); i++) {
        /* start tasks */
        game_t *game = game_defs[i];
        hard_assert(
            xTaskCreate(
                game->attract_task_fn, game->attract_task_name, 200, NULL, 1,
                &game->attract_task
            ) == pdPASS
        );
        vTaskSuspend(game->attract_task); // put attract task into suspend ASAP
        hard_assert(
            xTaskCreate(
                game->main_task_fn, game->main_task_name, 200, NULL, 1,
                &game->main_task
            ) == pdPASS
        );
    }
}

void game_start_attract(uint index) {
    for (uint i = 0; i < sizeof(game_defs) / sizeof(game_t *); i++) {
        if (i == index) vTaskResume(game_defs[i]->attract_task);
        else vTaskSuspend(game_defs[i]->attract_task);
    }
}

void game_stop_attract(uint index) {
    vTaskSuspend(game_defs[index]->attract_task);
}

void game_finish() {
    xEventGroupSetBits(btn_event_group, GAME_EV_FINISHED);
}
