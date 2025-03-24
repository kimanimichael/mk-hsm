#ifndef HSM_H_
#define HSM_H_

#include <cstdint>

/*---------------------------------------------------------------------------*/
/* Event facilities... */

typedef uint16_t Signal;

enum ReservedSignals {
    INIT_SIGNAL = 0, /* dispatched to AO before entering event-loop */
    ENTRY_SIGNAL,
    EXIT_SIGNAL,
    USER_SIGNAL, /* first signal available to the users */
};

class Event {
public:
    Signal _sig; /* event signal */
    /* event parameters added in subclasses of Event */
};

/*---------------------------------------------------------------------------*/
/* Hierarchical State Machine facilities... */

class HSM;

typedef enum{TRAN_STATUS, HANDLED_STATUS, IGNORED_STATUS, INIT_STATUS, SUPER_STATUS} State;

typedef State(*StateHandler)(Event const * const e);

#define TRAN(target_) (_state = (StateHandler)(target_), TRAN_STATUS)
#define SUPER(super_) (_temp = (StateHandler)(super_), SUPER_STATUS)

class HSM {
public:
    explicit HSM(StateHandler initial);

    void _init(Event const * const e);

    void _dispatch(Event const * const e);

    static State _top(Event const * const e);
public:
    StateHandler _state; /* the "state" variable */
    StateHandler _temp = {}; /* top state machine variable */
};

#endif
