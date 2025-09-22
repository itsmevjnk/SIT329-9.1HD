#pragma once

#include <stddef.h>
#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"
#include "button.h"

/* game speeds in milliseconds */
#define GAME_SPEED_SLOW                     1000
#define GAME_SPEED_MED                      500
#define GAME_SPEED_FAST                     250

/* game speeds in ticks */
#define GAME_SPEED_SLOW_TICKS               pdMS_TO_TICKS(GAME_SPEED_SLOW)
#define GAME_SPEED_MED_TICKS                pdMS_TO_TICKS(GAME_SPEED_MED)
#define GAME_SPEED_FAST_TICKS               pdMS_TO_TICKS(GAME_SPEED_FAST)

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

extern const uint num_games; // number of games

/* score gain and loss from button presses */
#define GAME_GAIN                           2
#define GAME_LOSS                           1

#define GAME_GOAL                           20 // goal score before stopping

#define GAME_EV_FINISHED                    (1 << 2) // finished event bit
// NOTE: this is also published onto the button event queue (see button.h).

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

/*
 * void game_finish()
 *  Signals to the main task (via the button event queue) that the game has
 *  finished. This can be used in the input task to trigger its end after a
 *  game.
 *  Inputs: None.
 *  Output: None.
 */
void game_finish();

/*
 * void game_start_stop(uint index)
 *  Starts/stops the specified game's main task.
 *  Inputs:
 *   - index : The index of the game to start or stop.
 *  Output: None.
 */
void game_start_stop(uint index);

/*
 * const char *game_get_name(uint index)
 *  Gets the declared name of the specified game.
 *  Inputs:
 *   - index : The index of the game to retrieve its name.
 *  Output: The name of the specified game as declared in its game_t structure.
 */
const char *game_get_name(uint index);

/*
 * const char *game_get_speed_str()
 *  Gets the representative string (Fast/Medium/Slow) of the currently set
 *  game speed.
 *  Inputs: None.
 *  Output: A string (Fast/Medium/Slow) representing the current speed.
 */
const char *game_get_speed_str();
