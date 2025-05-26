# About MK-HSM
MK-HSM is a hierarchical state machines management framework. Users can use this framework to manage their state machines so long as their platform is supported.

# Getting started with MK-HSM
The best way to use MK-HSM is to add it as a submodule to your project. MK-HSM is designed as a CMake library and therefore it si more straightforwardly used in a CMake project.

# Licensing
The MK-HSM framework is licensed under :

[Open-source licensing](https://www.state-machine.com/licensing#Open) under the
   [GNU General Public License (GPLv3)](https://www.gnu.org/licenses/gpl-3.0.en.html).

> NOTE: GPL requires that all modifications to the original code
as well as your application code (Derivative Works as defined in the
Copyright Law) must also be released under the terms of the GPL
open source license.

## Supported platforms
As of now, only the ESP-IDF platform is supported. Support for mbed-os is coming soon.

## Usage with ESP-IDF

1. Add the submodule to your CMake built-project by:
> git submodule add git@github.com:kimanimichael/mk-hsm.git

2. In your CMakeLists.txt, add the following lines:

`set(EXTRA_COMPONENT_DIRS mk-hsm/)`

`idf_build_component(mk-hsm)`

3. Declare all the potential event signals you might require. You need to extend this from the `USER_SIGNAL` defined in hsm.h within MK-HSM.
An example for a state machine requiring detection of a button press and timeouts would be:

```c++
enum EventSignals {
    ARMED_BUTTON_PRESSED_SIG = USER_SIGNAL,
    ARMED_BUTTON_RELEASED_SIG,
    TIMEOUT_SIG
}
```

4. Since MK-HSM works by creating the various state-machines as active objects, to create a state machine, inherit from the Active class defined within the ESP32 port inside MK-HSM.

For example for a state machine to blink an LED with armed and defused super states, do:
```c++
#include "ESP32_fsm.h"

class Blinky: public Active {

public:

   Blinky();
   
   State _armed(Event const * e);

   State _initial(Event const * e);

   State _led1_on(Event const * e);

   State _led1_off(Event const * e);
   
   State _defused(Event const * e);
   
   /*Associated TimeEvent*/
   TimeEvent *te;
   
   static Blinky* get_default_instance();

private:

}

StateHandler armed = std::bind(&Blinky::_armed, Blinky::get_default_instance(), std::placeholders::_1);
StateHandler initial = std::bind(&Blinky::_initial, Blinky::get_default_instance(), std::placeholders::_1);
StateHandler led1_on = std::bind(&Blinky::_led1_on, Blinky::get_default_instance(), std::placeholders::_1);
StateHandler led1_off = std::bind(&Blinky::_led1_off, Blinky::get_default_instance(), std::placeholders::_1);
StateHandler defused = std::bind(&Blinky::_defused, Blinky::get_default_instance(), std::placeholders::_1)

Blinky::Blinky(): Active((std::bind(&Blinky::_initial, this, std::placeholders::_1))) {
    printf("Blinky init\n");
    te = TimeEvent::get_default_instance(TIMEOUT_SIG, this);
}

State Blinky ::_armed(Event const * const e) {
    State status;
    switch (e->_sig) {
        case ARMED_BUTTON_PRESSED_SIG: {
                status = TRAN(defused);
                break;
        }
        case INIT_SIGNAL: {
                status = TRAN(initial);
                break;
        }
        default: {
                status = SUPER(HSM::_top);
                break;
        }
    }
    return status;
}

State Blinky::_initial(Event const* const e) {
    printf("Initial state\n");
    return TRAN(led_on);
}

State Blinky::_led_on(Event const* const e) {
    switch (e->_sig) {
        case TIMEOUT_SIG:
             printf("Timeout in blink\n");
             status = TRAN(led_off);
             break;
        default:
            status = SUPER(armed);
            break
    }
    return status;
}

State Blinky::_led_off(Event const* const e) {
    switch (e->_sig) {
        case TIMEOUT_SIG:
             printf("Timeout in blink\n");
             status = TRAN(led_on);
             break;
        default:
            status = SUPER(armed);
            break
    }
    return status;
}

State Blinky::_defused(Event const * const e) {
    State status;
    switch (e->_sig) {

        case ENTRY_SIGNAL: {
                status = HANDLED_STATUS;
                break;
        }
        case EXIT_SIGNAL: {
                status = HANDLED_STATUS;
                break;
        }
        case ARMED_BUTTON_PRESSED_SIG: {
                status = TRAN(armed);
                break;
        }
        default: {
                status = SUPER(HSM::_top);
                break;
        }
    }
    return status;
}
```

The in your main.cpp or elsewhere, include the header containing your Blinky class above and :

Set priority for this active object and start it.

```c++
Blinky::get_default_instance()->_priority = 1;
Active::_run(TimeBomb::get_default_instance());
```

Considering the above steps are followed properly, the state machine should immediately start to listen for the events associated
with it.

NOTE:

You need to implement the BSP functionality for the LEDs, and define the necessary events.










