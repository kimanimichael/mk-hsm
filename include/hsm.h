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

typedef enum{TRAN_STATUS, HANDLED_STATUS, IGNORED_STATUS, INIT_STATUS, SUPER_STATUS} State;

typedef State(*StateHandler)(Event const * const e);

#define TRAN(target_) (_state = (StateHandler)(target_), TRAN_STATUS)
#define SUPER(super_) (_temp = (StateHandler)(super_), SUPER_STATUS)

class HSM {
public:
    /**
     * @brief Assigns _state to the entry state
     * @param initial entry state
     */
    explicit HSM(StateHandler initial);
    /**
     * Executes first state and entry to the new state
     * @param e first event usually null
     */
    void _init(Event const * e);
    /**
     * @brief Handle event in current _state and transition if need be
     * @param e Event to be handles
     */
    void         _dispatch(Event const * e);
    /**
     * @brief HSM top state
     * @param e Event to be handled
     * @return certain State at this time IGNORED_STATUS
     */
    static State _top(Event const * e);
public:
    /** Tracks current HSM AO state */
    StateHandler _state;
    /** Tracks current HSM super state */
    StateHandler _temp = {};
};

#endif
