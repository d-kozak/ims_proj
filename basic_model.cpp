#include <iostream>
#include "simlib.h"


// TODO FIX CMAKE, use Makefile for now

/**
 *          CONSTANTS
 * All constants are in seconds
 */

/**
 * We are modelling three hours
 */
static const int SIMULATION_END_TIME =  60*60*3;
static const int CUSTUMER_CREATION_TIME = 10;
static const int GATE_ENTERING_TIME = 3;

Facility gate("Gate");

class Customer : public Process{
    void Behavior(){
        Seize(gate);
        Wait(GATE_ENTERING_TIME);
        Release(gate);
    }
};

class Generator: public Event{
    void Behavior(){
        (new Customer)->Activate();
        Activate(Time + Exponential(CUSTUMER_CREATION_TIME));
    }
};

int main() {
    Init(0,SIMULATION_END_TIME);
    (new Generator)->Activate();
    Run();
    gate.Output();
    return 0;
}