#include <iostream>
#include "simlib.h"


// TODO FIX CMAKE, use Makefile for now

/**
 *          CONSTANTS
 * All constants are in seconds
 */

static const int SECOND = 1;
static const int MINUTE = 60;
static const int HOUR = MINUTE * 60;

/**
 * We are modelling three hours
 */
static const int SIMULATION_END_TIME = 3 * HOUR;

static const int CUSTUMER_CREATION_TIME = 10;
static const int GATE_ENTERING_TIME = 3;
static const int TROLLEY_STORE_CAPACITY = 200;

static const int SMALL_SHOPPING_TIME = 7 * MINUTE;
static const int MEDIUM_SHOPPING_TIME = 14 * MINUTE;
static const int BIG_SHOPPING_TIME = 30 * MINUTE - SECOND;

Store trolleys("Trolleys", TROLLEY_STORE_CAPACITY);
Facility gate("Gate");

Histogram shoppingKind ("Shopping kind",0,10 * MINUTE,3);

class Customer : public Process {
    void Behavior() {
        Enter(trolleys);

        Seize(gate);
        Wait(GATE_ENTERING_TIME);
        Release(gate);

        double decision = Random();
        if (decision < 0.33) {
            //small shopping
            Wait(SMALL_SHOPPING_TIME);
            shoppingKind(SMALL_SHOPPING_TIME);
        } else if (decision < 0.66) {
            //medium shopping
            Wait(MEDIUM_SHOPPING_TIME);
            shoppingKind(MEDIUM_SHOPPING_TIME);
        } else {
            //big shopping
            Wait(BIG_SHOPPING_TIME);
            shoppingKind(BIG_SHOPPING_TIME);
        }

        Leave(trolleys);
    }
};

class Generator : public Event {
    void Behavior() {
        (new Customer)->Activate();
        Activate(Time + Exponential(CUSTUMER_CREATION_TIME));
    }
};

int main() {
    Init(0, SIMULATION_END_TIME);
    (new Generator)->Activate();
    Run();
    gate.Output();
    trolleys.Output();
    shoppingKind.Output();
    return 0;
}