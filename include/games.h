#pragma once

#include <stddef.h>
#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"

/* game speeds in milliseconds */
#define GAME_SPEED_SLOW                     1000
#define GAME_SPEED_MED                      500
#define GAME_SPEED_FAST                     250

/* game definition structure */
typedef struct {
    const char *name;

    const char *attract_task_name;
    void (*attract_task_fn)(void *); // attract task function

    const char *main_task_name;
    void (*main_task_fn)(void *); // main task function

    // NOTE: The input task is to be controlled (i.e. started and 
    //       suspended/resumed) by the main task.

    /* task handles - to be filled during init */
    TaskHandle_t attract_task;
    TaskHandle_t main_task;
} game_t;

#define NUM_GAMES                           2
extern game_t *games; // available games

extern int game_score; // game score 
extern uint32_t game_speed; // delay between steps in ticks

/* score gain and loss from button presses */
#define GAME_GAIN                           2
#define GAME_LOSS                           1

/*
 * void game_init()
 *  Initialise tasks for all games and keep them suspended initially.
 *  Inputs: None.
 *  Output: None.
 */
void game_init();

/*
 * void game_start_attract(uint index)
 *  Starts the attract mode for the specified game. This will also suspend all
 *  other attract mode tasks.
 *  Inputs:
 *   - index : The index of the game to activate attract mode for.
 *  Output: None.
 */
void game_start_attract(uint index);

/*
 * void game_stop_attract(uint index)
 *  Stops the attract mode for the specified game, by putting its attract mode
 *  task into suspension.
 *  Inputs:
 *   - index : The index of the game to stop attract mode for.
 *  Output: None.
 */
void game_stop_attract(uint index);