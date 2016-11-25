#include <iostream>
#include "simlib.h"

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
static const int TROLLEY_TIMEOUT = 10 * SECOND;

static const int SMALL_SHOPPING_TIME = 7 * MINUTE;
static const int MEDIUM_SHOPPING_TIME = 14 * MINUTE;
static const int BIG_SHOPPING_TIME = 30 * MINUTE;

static const int CASH_REGISTER_TIME = 1 * MINUTE;
static const int CASH_REGISTER_SIZE = 7;

static const int MEAT_SHOP_CAPACITY = 2;
static const int MEAT_SHOP_TIME = 1 * MINUTE;
static const int MEAT_SHOP_TIMEOUT = 3 * MINUTE;

Store trolleys("Trolleys", TROLLEY_STORE_CAPACITY);
int timeoutTrolleyCount = 0;

Facility gate("Gate");

Facility cashRegisters[CASH_REGISTER_SIZE];

Histogram shoppingKind("Shopping kind", 0, MINUTE, 10);

int timeoutMeatShopCount = 0;
Store meatShop("meat shop", MEAT_SHOP_CAPACITY);

class Customer;

class Timeout : public Event {
    Customer *customer;
public:
    Timeout(Customer *customer) : Event(), customer(customer) {};

    void Behavior();

};

class Customer : public Process {
    bool wasTimeouted = false;
    Timeout *timeoutPtr = nullptr;

    void Behavior() {
        setTimeout(TROLLEY_TIMEOUT);
        Enter(trolleys);
        if (wasTimeouted) {
            // customer did not acquire trolley in specified time
            timeoutTrolleyCount++;
            return;
        } else {
            cancelTimeout();
        }

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


        // after shopping client goes to the cash registers
        // and picks the one with shortest queue
        Facility *withShortestQueue = &cashRegisters[0];
        int min = cashRegisters[0].Q1->size();
        for (int i = 0; i < CASH_REGISTER_SIZE; i++) {
            if (min > cashRegisters[i].Q1->size()) {
                min = cashRegisters[i].Q1->size();
                withShortestQueue = &cashRegisters[i];
            }
        }

        Seize(*withShortestQueue);

        // cash register process
        Wait(Exponential(CASH_REGISTER_TIME));

        Release(*withShortestQueue);

        decision = Random();
        if (decision > 0.7) {
            // go to buy some meat
            setTimeout(MEAT_SHOP_TIMEOUT);
            Enter(meatShop);
            if (wasTimeouted) {
                // customer timeouted, he/she will return the trolley and
                // leave this god forsaken land
                timeoutMeatShopCount++;
                Leave(trolleys);
                return;
            } else {
                cancelTimeout();
            }

            Wait(Exponential(MEAT_SHOP_TIME));

            Leave(meatShop);
        }

        Leave(trolleys);
    }

    void cancelTimeout() {
        wasTimeouted = false;
        this->timeoutPtr->Cancel();
    }

    void setTimeout(double time) {
        wasTimeouted = false;
        this->timeoutPtr = new Timeout(this);
        this->timeoutPtr->Activate(Time + time);
    }

public:
    void timeout() {
        wasTimeouted = true;
        Out();
        Activate();
    }
};

void Timeout::Behavior() {
    customer->timeout();
    Cancel();
}


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
    for (Facility &fac : cashRegisters) {
        fac.Output();
    }
    meatShop.Output();
    std::cout << "Timeouted customers in trolley store " << timeoutTrolleyCount << std::endl;
    std::cout << "Timeouted customers in meat shop " << timeoutMeatShopCount << std::endl;
    return 0;
}