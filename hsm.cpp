#include "hsm.h"

/* Finite State Machine facilities... */

static constexpr Event init_evt  = {INIT_SIGNAL};
static constexpr Event entry_evt = {ENTRY_SIGNAL};
static constexpr Event exit_evt  = {EXIT_SIGNAL};

HSM::HSM(const StateHandler initial) {
    _state = initial;
}

void HSM::_init(Event const* const e) const {
    if (_state == static_cast<StateHandler>(nullptr)) {
        return;
    }
    (_state)(e);
    if (_state == static_cast<StateHandler>(nullptr)) {
        return;
    }
    (_state)(&entry_evt);
}

void HSM::_dispatch(Event const* const e) const {
    if (_state == static_cast<StateHandler>(nullptr)) {
        return;
    }
    const StateHandler prev_state = _state;
    State stat = (_state)(e); /* Updates me->state if transition is needed */

    while (stat == SUPER_STATUS) {
        stat = _temp(e);
    }

    while (stat == TRAN_STATUS) {
        if (_state == static_cast<StateHandler>(nullptr)) {
            return;
        }
        (prev_state)(&exit_evt);
        _state(&init_evt);
        stat = _state(&entry_evt);
    }

}

State HSM::_top(Event const* const e) {
    return IGNORED_STATUS;
}

