#include <iostream>
#include "simlib.h"


// TODO FIX CMAKE, use Makefile for now

/**
 *          CONSTANTS
 * All constants are in seconds
 */

static const int SECOND = 1;
static const int MINUTE = 60 * SECOND;
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
static const int BIG_SHOPPING_TIME = 30 * MINUTE;

static const int CASH_REGISTER_TIME = MINUTE;
static const int CASH_REGISTER_SIZE = 7;

static const int MEAT_SHOP_CAPACITY = 2;
static const int MEAT_SHOP_TIME = 1 * MINUTE;

Store trolleys("Trolleys", TROLLEY_STORE_CAPACITY);
Facility gate("Gate");


Facility cashRegisters[CASH_REGISTER_SIZE];

Histogram shoppingKind ("Shopping kind",0,MINUTE,10);

Store meatShop("meat shop",MEAT_SHOP_CAPACITY);

class Customer : public Process {
    void Behavior() {
        Enter(trolleys);

        Seize(gate);
        Wait(GATE_ENTERING_TIME);
        Release(gate);

        double decision = Random();
        double shoppingTime;
        if (decision < 0.33) {
            //small shopping
            shoppingTime = Exponential(SMALL_SHOPPING_TIME);
        } else if (decision < 0.66) {
            //medium shopping
            shoppingTime = Exponential(MEDIUM_SHOPPING_TIME);
        } else {
            //big shopping
            shoppingTime = Exponential(BIG_SHOPPING_TIME);
        }

        Wait(shoppingTime);
        shoppingKind(shoppingTime);


        // after shopping client goes to the cash register
        // and picks the one with shortest queue
        Facility * withShortestQueue = &cashRegisters[0];
        int min = cashRegisters[0].Q1->size();
        for(int i = 0; i < CASH_REGISTER_SIZE ; i++){
            if(min > cashRegisters[i].Q1->size()){
                min = cashRegisters[i].Q1->size();
                withShortestQueue = &cashRegisters[i];
            }
        }

        Seize(*withShortestQueue);

        // cash register process
        Wait(Exponential(CASH_REGISTER_TIME));

        Release(*withShortestQueue);

        decision = Random();
        if(decision > 0.7){
            // go to buy some meat
            Enter(meatShop);

            Wait(Exponential(MEAT_SHOP_TIME));

            Leave(meatShop);
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
    for(Facility & fac : cashRegisters){
        fac.Output();
    }
    meatShop.Output();
    return 0;
}