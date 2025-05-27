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
#include "mbed_hsm.h"

#define MAX_TIME_EVENTS 10

Ticker tick_timer;

Active * Active::active_instance = nullptr;

Active::Active(const StateHandler& initial): HSM(initial) {
    printf("Active Object init\n");
    active_instance = this;
}

void Active::_start() {
    tick_timer.attach(&TimeEvent::tick, 100ms);

    static Thread active_thread;
    active_thread.start(Active::event_loop);
}

void Active::_post(Event * e)  {
    if (_queue.try_put(&e) != true) {
        delete e;
    } else {
        printf("Event posted...\n");
    }
}

void Active::event_loop() {
    active_instance->_event_loop();
}


[[noreturn]] void Active::_event_loop() {
    HSM::_init(nullptr);

    while (true) {
        const osEvent evt = _queue.get();
        if (evt.status == osEventMessage) {
            const auto* e = static_cast<Event*>(evt.value.p);
            printf("Event received...\n");
            HSM::_dispatch(e);
        }
    }
}

void Active::_run(const Active *object) {
    object->_start();
}

TimeEvent * TimeEvent::time_event_instance = nullptr;

static TimeEvent *l_t_evt[MAX_TIME_EVENTS];
static uint_fast8_t l_t_evt_num = {};

TimeEvent::TimeEvent(const Signal sig, Active* act) : Event(), _act(act) {
    _sig = sig;

    _events_mutex.lock();
    l_t_evt[l_t_evt_num] = this;
    l_t_evt_num++;
    _events_mutex.unlock();

    time_event_instance = this;
}

void TimeEvent::_arm(const uint32_t timeout, const uint32_t interval) {
    if (_parameters_mutex.trylock()) {
        _timeout = timeout;
        _interval = interval;
        printf("Armed successfully to %ld \n", _timeout);
    } else {
        printf("Armed failed. Mutex could not be acquired");
        return;
    }
    _parameters_mutex.unlock();
}

void TimeEvent::_disarm() {
    if (_parameters_mutex.trylock()) {
        _timeout = 0U;
        _interval = 0U;
        printf("Disarmed successfully");
    } else {
        printf("Disarmed failed. Mutex could not be acquired");
    }
}

void TimeEvent::tick() {
    time_event_instance->_tick();
}

void TimeEvent::_tick() {
    for (uint_fast8_t i = 0; i < l_t_evt_num; i++) {
        if (l_t_evt[i] ->_timeout > 0U) {
            l_t_evt[i] ->_timeout --;
            if (l_t_evt[i] ->_timeout == 0U) {
                printf("Timeout reached. Posting\n");
                l_t_evt[i] ->_act->_post(this);
                l_t_evt[i] -> _timeout = l_t_evt[i]->_interval;
            }
        }
    }
}

TimeEvent* TimeEvent::get_default_instance(const Signal sig, Active *act) {
    static TimeEvent te(sig, act);
    return &te;
}
