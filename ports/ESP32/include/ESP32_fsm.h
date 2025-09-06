/**This program is a hierarchical state machines management framework.
Copyright (C) 2025  Michael Kimani

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

In case of any enquiries, you can contact the author at muchunumike@gmail.com
**/
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
    explicit Active(const StateHandler& initial);
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
    [[noreturn]] void _event_loop() const;
    /**
     * @brief Start AO with associated timers
     * @param object Active object to be started
     */
    static void _run(Active *object);

    /** Task priority. Must be set before _run is called */
    UBaseType_t _priority = {};
    /** Event queue for receiving events */
    QueueHandle_t _queue     = {};
    /**
     * @brief Name assigned to the task.
     *
     * This variable holds the name of the task associated with the active object.
     * It is used for debugging and task identification purposes.
     */
    const char * _task_name  = {};
    /**
     * @brief Defines the stack size for the task associated with the active object.
     *
     * This variable determines the amount of stack memory allocated when creating
     * the task for the active object. It is typically specified in bytes. The
     * default value is set to 4096.
     */
    uint32_t     _stack_size = 4096;

private:
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
