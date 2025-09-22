#include "button.h"
#include "pin_config.h"

#include "queue.h"

#include "pico/stdlib.h"

#include "hardware/gpio.h"
#include "hardware/irq.h"

EventGroupHandle_t btn_event_group;

volatile TickType_t btn_press_time; // tickstamp of button press - for debounce
volatile BaseType_t btn_long_press = pdFALSE; // set by timer callback
static TimerHandle_t btn_long_timer; // long press timer

/*
 * static void btn_handler(uint gpio, uint32_t events)
 *  Handler for button pin state toggle events. This ISR also performs
 *  debouncing and produces events for btn_event_group.
 *  Inputs:
 *   - gpio   : The GPIO pin that triggered the interrupt.
 *   - events : Bitfield indicating the interrupt event; this function handles
 *              GPIO_IRQ_EDGE_FALL and GPIO_IRQ_EDGE_RISE events.
 *  Output: None.
 */
static void btn_handler(uint gpio, uint32_t events) {
    (void) gpio;
    if (!(events & (GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE)))
        return; // invalid event (TODO: will this occur at all?)
        
    BaseType_t xHigherPriorityTaskWoken = pdFALSE; // needed for timer ops
    if (events & GPIO_IRQ_EDGE_FALL) { // falling edge (button press)
        btn_press_time = xTaskGetTickCountFromISR();

        /* start long press timer (which also turns on indicator on expire) */
        btn_long_press = pdFALSE; // reset long press flag
        xTimerStartFromISR(btn_long_timer, &xHigherPriorityTaskWoken);
    } else { // rising edge (button release) - debounce and fire events
        gpio_put(LED_BUTTON, false); // turn off indicator on button release
        xTimerStopFromISR(btn_long_timer, &xHigherPriorityTaskWoken);
        if (
            xTaskGetTickCountFromISR() - btn_press_time
            >= pdMS_TO_TICKS(BTN_DEBOUNCE)
        ) xEventGroupSetBitsFromISR(
            btn_event_group,
            (btn_long_press) ? BTN_EV_LONG_PRESS : BTN_EV_SHORT_PRESS,
            &xHigherPriorityTaskWoken // NOTE: this flag will not be cleared
        ); // fire event (after debouncing)
        
    }
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/*
 * static void btn_long_timer_cb(TimerHandle_t timer)
 *  Button long press timer expiration callback.
 *  This function sets the long press flag, and turns on the long press
 *  indicator.
 *  Inputs:
 *   - timer : The timer that triggered this callback.
 *  Output: None.
 */
static void btn_long_timer_cb(TimerHandle_t timer) {
    (void) timer;

    btn_long_press = pdTRUE;

    gpio_put(LED_BUTTON, true); // turn on indicator
}

void btn_init() {
    btn_event_group = xEventGroupCreate(); // create event group
    hard_assert(btn_event_group);
    // NOTE: ideally we want to use xEventGroupCreateStatic instead

    btn_long_timer = xTimerCreate(
        "btn_long_timer", pdMS_TO_TICKS(BTN_LONG_PRESS),
        pdFALSE, // one-shot timer
        NULL, btn_long_timer_cb
    );

    /* initialise button long press indicator */
    gpio_init_mask((1 << BUTTON) | (1 << LED_BUTTON)); // also for button below
    gpio_set_dir(LED_BUTTON, GPIO_OUT);
    gpio_put(LED_BUTTON, false); // initially off

    /* initialise button pin */
    // gpio_init(BUTTON);
    gpio_set_dir(BUTTON, GPIO_IN);
    gpio_pull_up(BUTTON); // internal pull-up
    gpio_set_irq_enabled_with_callback(
        BUTTON, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, btn_handler
    ); // enable interrupt for button state toggle
}