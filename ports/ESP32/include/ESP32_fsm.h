/* ESP32 custom HSM header
 * Created by mikael on 3/18/25.
 */

#ifndef ESP32HSM_H
#define ESP32HSM_H

#include "hsm.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
/*---------------------------------------------------------------------------*/
/* Active Object facilities... */

class Active: public HSM {
public:
    /**
     * @brief Init. Assigns active_instance to this
     * @param initial HSM initial state
     */
    explicit Active(StateHandler initial);
    /**
     * @brief Create message queue to handle events
     */
    void _start();
    /**
     * @brief Post message to _queue
     * @param e Pointer to an event
     */
    void _post(Event const * e) const;
    /**
     * @brief Trampoline for _event_loop
     */
    static void event_loop(void *param);
    /**
     * @brief Init HSM,block for event & dispatch in while loop
     */
    [[noreturn]] void _event_loop();
    /**
     * @brief Start AO with associated timers
     * @param object Active object to be started
     */
    static void _run(Active *object);
public:
    /** Task priority. Must be set before _run is called */
    UBaseType_t _priority = {};
    /** Event queue for receiving events */
    QueueHandle_t _queue = {};

private:
    /** Used for trampoline functions */
    static Active* active_instance; /* @todo Find better solution*/

    /* active object data added in subclasses of Active */
};

class TimeEvent : public Event {
public:
    /**
     * @brief Initialize time event
     * @param sig Signal dispatched on expiry of armed event
     * @param act Active Object to whom event is posted to
     */
    explicit TimeEvent(Signal sig, Active *act);
    /**
     * @brief Arm time event
     * @param timeout No of calls to _tick for event to be posted
     * @param interval Reassigned to _timeout for periodic events when timeout gets to 0
     */
    void _arm(uint32_t timeout, uint32_t interval);
    /**
     * @brief Sets _timeout and _interval to 0U
     */
    void _disarm();
    /**
     * @brief Trampoline for _tick
     * @param xTimer Handle to the timer that triggered the callback
     */
    static void tick(TimerHandle_t xTimer);
    /**
     * @brief Countdown _timeout and post event when _timeout gets to 0
     * Reassigns _timeout to _interval to allow for periodic time events
     */
    void _tick() const;
    /** Active Object to whom the event is posted to */
    Active* _act;
    /** No of calls to _tick for event to be posted */
    uint32_t _timeout = {};
    /** Reassigned to _timeout for periodic events when timeout gets to 0 */
    uint32_t _interval = {};
    /** Mutex to protect access to the TimeEvent array */
    static SemaphoreHandle_t _events_mutex;
    /** Mutex to protect access to _timeout and _interval */
    static SemaphoreHandle_t _parameters_mutex;
    /** Instance used in trampoline functions */ // @todo Find a better solution
    static TimeEvent* time_event_instance;
    /** Get default TimeEvent instance */
    static TimeEvent* get_default_instance(Signal sig, Active *act);

};

#endif //ESP32HSM_H
