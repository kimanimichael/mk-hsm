#include "ESP32_fsm.h"

#define QUEUE_LENGTH 10
#define MAX_TIME_EVENTS 10

Active * Active::active_instance = nullptr;

Active::Active(const StateHandler initial): HSM(initial) {
    active_instance = this;
}

void Active::_start() {
    _queue = xQueueCreate(QUEUE_LENGTH, sizeof(Event));
    if (_queue == nullptr) {
        printf("Queue creation failed!\n");
    }
}

void Active::_post(Event const * const e) const {
    xQueueSend(_queue, e, portMAX_DELAY);
}

void Active::event_loop(void* param) {
    active_instance->_event_loop();
}


[[noreturn]] void Active::_event_loop() {
    Event e = {};

    HSM::_init((Event*)nullptr);

    while (true) {
        if (xQueueReceive(_queue, &e, portMAX_DELAY)) {
            HSM::_dispatch(&e);
        }
    }
}

TimeEvent * TimeEvent::time_event_instance = nullptr;

static TimeEvent *l_t_evt[MAX_TIME_EVENTS];
static uint_fast8_t l_t_evt_num = {};

SemaphoreHandle_t TimeEvent::_events_mutex = xSemaphoreCreateMutex();
SemaphoreHandle_t TimeEvent::_parameters_mutex = xSemaphoreCreateMutex();

TimeEvent::TimeEvent(const Signal sig, Active *act) : Event(), _act(act) {
    _sig = sig;
    if (xSemaphoreTake(TimeEvent::_events_mutex, portMAX_DELAY)) {
        l_t_evt[l_t_evt_num] = this;
        l_t_evt_num++;
    }
    xSemaphoreGive(TimeEvent::_events_mutex);
    time_event_instance = this;
}

void TimeEvent::_arm(const uint32_t timeout, const uint32_t interval) {
    if (xSemaphoreTake(TimeEvent::_parameters_mutex, portMAX_DELAY)) {
        _timeout = timeout;
        _interval = interval;
    }
    l_t_evt[0] -> _timeout = timeout;
    _timeout = timeout;
    printf("Armed successfully to %ld \n", _timeout);
    xSemaphoreGive(TimeEvent::_parameters_mutex);
}

void TimeEvent::_disarm() {
    if (xSemaphoreTake(TimeEvent::_parameters_mutex, portMAX_DELAY)) {
        _timeout = 0U;
    }
    xSemaphoreGive(TimeEvent::_parameters_mutex);
}

void TimeEvent::tick(TimerHandle_t xTimer) {
    time_event_instance->_tick();
}


void TimeEvent::_tick() {
    for (uint_fast8_t i = 0; i < l_t_evt_num; i++) {
        // ESP_ERROR_CHECK(l_t_evt[i] != static_cast<TimeEvent*>(nullptr));
        if (l_t_evt[i] ->_timeout > 0U) {
            l_t_evt[i] ->_timeout --;
            if (l_t_evt[i] ->_timeout == 0U) {
                l_t_evt[i] ->_act->_post(this);
                l_t_evt[i] -> _timeout = l_t_evt[i]->_interval;
            }
        }
    }
}

TimeEvent* TimeEvent::get_default_instance(Signal sig, Active *act) {
    static TimeEvent te(sig, act);
    return &te;
}


