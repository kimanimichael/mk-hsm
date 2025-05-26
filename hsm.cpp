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
#include "hsm.h"

/* Finite State Machine facilities... */

static constexpr Event init_evt  = {INIT_SIGNAL};
static constexpr Event entry_evt = {ENTRY_SIGNAL};
static constexpr Event exit_evt  = {EXIT_SIGNAL};

HSM::HSM(const StateHandler& initial) {
    _state = initial;
}

void HSM::_init(Event const* const e) const {
    if (!_state) {
        return;
    }
    (_state)(e);
    if (!_state) {
        return;
    }
    (_state)(&entry_evt);
}

void HSM::_dispatch(Event const* const e) const {
    if (!_state) {
        return;
    }
    const StateHandler prev_state = _state;
    State stat = (_state)(e); /* Updates me->state if transition is needed */

    while (stat == SUPER_STATUS) {
        stat = _temp(e);
    }

    while (stat == TRAN_STATUS) {
        if (!_state) {
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

