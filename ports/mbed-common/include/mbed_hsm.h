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
#ifndef MBED_HSM_H
#define MBED_HSM_H

#include "hsm.h"
#include "mbed.h"

/*---------------------------------------------------------------------------*/
/* Active Object facilities... */

class Active : public HSM {
public:
    /**
     * @brief Init. Assigns active_instance to this
     * @param initial HSM initial state
     */
    explicit Active(const StateHandler& initial);
    /**
     * @brief Create message queue to handle events
     */
    static void _start();
    /**
     * @brief Post message to _queue
     * @param e Pointer to an event
     */
    void _post(Event const * e);

    static void event_loop();
    /**
     * @brief Init HSM,block for event & dispatch in while loop
     */
    [[noreturn]] void _event_loop();
    /**
     * @brief Start AO with associated timers
     * @param object Active object to be started
     */
    static void _run(const Active *object);

    [[noreturn]] static void _time_tick();

    uint8_t _priority = {};

    rtos::Mutex _queue_mutex;
    /* Event queue */
    Queue<Event const, 32> _queue; //capacity of 32 events

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
    /** Used for trampoline functions */
    static Active* active_instance; /* @todo Find better solution*/
};

class TimeEvent: public Event {
public:
    explicit TimeEvent(Signal sig, Active* act);

    void _arm(uint32_t timeout, uint32_t interval);

    void _disarm();

    static void tick();

    void _tick() const;

    Active* _act;

    uint32_t _timeout = {};

    uint32_t _interval = {};

    rtos::Mutex _events_mutex;

    rtos::Mutex _parameters_mutex;

    static TimeEvent* time_event_instance;

    static TimeEvent* get_default_instance(const Signal sig, Active *act);
};

#endif
