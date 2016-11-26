#include <iostream>
#include <sstream>
#include "simlib.h"
#include "ClosableFacility.h"
#include "MaintainableClosableFacility.h"
#include "internal.h"
#include "FacilityInvalidStateException.h"


/**
 * "Library" section
 * It contains extensions for simlib classes which proved to be useful in our project
 */
class TimeoutableProcess;

class Timeout : public Event {
    TimeoutableProcess *_process;
public:
    Timeout(TimeoutableProcess *process) : Event(), _process(process) {};

    void Behavior();

};

class TimeoutableProcess : public Process {
    // TODO use behaviour parametrization design pattern for clearer timeout
    friend class Timeout;

    Timeout *_timeout;
    bool _timeoutHappened;
protected:
    void cancelTimeout() {
        _timeoutHappened = false;
        this->_timeout->Cancel();
    }

    void setTimeout(double time) {
        _timeoutHappened = false;
        this->_timeout = new Timeout(this);
        this->_timeout->Activate(Time + time);
    }

    void timeout() {
        _timeoutHappened = true;
        Out();
        Activate();
    }

    bool wasTimeouted() {
        return _timeoutHappened;
    }
};

void Timeout::Behavior() {
    _process->timeout();
    Cancel();
}

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

static const int BOSS_COMMING_TIME = 5 * MINUTE;
static const int BOSS_PROBLEM_SOLVING_TIME = 90 * SECOND;

static const int STOCK_TIME = 30 * MINUTE;
static const int BAKERY_WORK_TIME = 20 * MINUTE;
static const int BAKERY_BAKING_TIME = 1 * HOUR;

static const int BREAK_TIME = 30 * MINUTE;

static const int BREAK_CAPACITY = 2;
int currentBreakCount = 0;


Store trolleys("Trolleys", TROLLEY_STORE_CAPACITY);
int timeoutTrolleyCount = 0;

Facility gate("Gate");

MaintainableClosableFacility cashRegisters[CASH_REGISTER_SIZE];

Histogram shoppingKind("Shopping kind", 0, MINUTE, 10);

int timeoutMeatShopCount = 0;
Store meatShop("meat shop", MEAT_SHOP_CAPACITY);

int returningCustomersCount = 0;

Facility boss("boss");
int employeeMistakeCount = 0;

int countActiveCashRegisters() {
    int count = 0;
    for (MaintainableClosableFacility &fac : cashRegisters) {
        if (fac.isOpen()) {
            count++;
        }
    }
    return count;
}

float countAverageCashRegisterQueueLen() {
    float queueCount = 0;
    int activeQueues = 0;

    for (MaintainableClosableFacility &fac : cashRegisters) {
        if (fac.isOpen()) {
            activeQueues++;
            queueCount += fac.QueueLen();
        }
    }
    return queueCount / activeQueues;
}

MaintainableClosableFacility & findFirstClosedCashRegister(){
    for(MaintainableClosableFacility & fac : cashRegisters){
        if(!fac.isOpen())
            return fac;
    }
    throw FacilityInvalidStateException("No suitable cash register found");
}

class CashRegisterClosingCondition : public ClosableFacilityCondition {
    virtual bool closingCondition(ClosableFacility *fac) {
//        return fac->QueueLen() < 4 && atLeastTwoCashRegistersAreWorking();
        return countAverageCashRegisterQueueLen() < 4 && atLeastTwoCashRegistersAreWorking();
    }


    /**
     * It is only possible to close the facility if there is at least another one open
     * which means there have to be two open facilities
     * @return
     */
    bool atLeastTwoCashRegistersAreWorking(){
        int workingRegisters = 0;
        for(MaintainableClosableFacility & fac : cashRegisters)
            if(fac.isFacilityAvailable())
                workingRegisters++;
        return workingRegisters > 1;
    }

};

class Customer : public TimeoutableProcess {

    void Behavior() {
        setTimeout(TROLLEY_TIMEOUT);
        Enter(trolleys);
        if (wasTimeouted()) {
            // customer did not acquire trolley in specified time
            timeoutTrolleyCount++;
            return;
        } else {
            cancelTimeout();
        }

        // goto for easy re-entering if the customer has forgotten something
        start_shopping:

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
        Facility *withShortestQueue = nullptr;
        int min = INT32_MAX;
        for (int i = 0; i < CASH_REGISTER_SIZE; i++) {
            if (!cashRegisters[i].isFacilityAvailable())
                continue;

            if (min > cashRegisters[i].QueueLen()) {
                min = cashRegisters[i].QueueLen();
                withShortestQueue = &cashRegisters[i];
            }
        }

        if (withShortestQueue == nullptr) {
            std::cerr << "Internal error, no cash resgister is open" << std::endl;
            exit(2);
        }

        Seize(*withShortestQueue);

        // cash register process
        Wait(Exponential(CASH_REGISTER_TIME));

        decision = Random();
        if (decision > 0.95) {
            // employee has made a mistake and need to call the boss
            employeeMistakeCount++;
            Seize(boss);
            Wait(Exponential(BOSS_COMMING_TIME));
            Wait(Exponential(BOSS_PROBLEM_SOLVING_TIME));
            Release(boss);
        }

        Release(*withShortestQueue);

        decision = Random();
        if (decision > 0.7) {
            // go to buy some meat
            setTimeout(MEAT_SHOP_TIMEOUT);
            Enter(meatShop);
            if (wasTimeouted()) {
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

        decision = Random();
        if (decision > 0.95) {
            //customer forgot something and will return to the shop
            returningCustomersCount++;
            goto start_shopping;
        }
        Leave(trolleys);
    }
};

typedef enum {
    CASH_REGISTER, BAKERY, STOCK, BREAK
} EmployeeState;

bool bakeryReady = false;
class BakeryReady : public Event {
    void Behavior() {
        bakeryReady = true;
    }
};

int count = 0;
int methodInvokeCount = 1;
class Employee : public Process {
    bool hasTakenBreak = false;

    bool shouldTakeBreak(){
        return !hasTakenBreak && currentBreakCount < BREAK_CAPACITY;
    }

    bool shouldGoToTheCashRegisters(){
        if(methodInvokeCount++ < 3) // small hack to make first three employees go to the cash registers event if no customers are in the shop yet
            return true;
        return (countAverageCashRegisterQueueLen() > 6 && countActiveCashRegisters() < CASH_REGISTER_SIZE);
    }

    void Behavior() {
        EmployeeState state;
        std::stringstream ss;
        ss << "Employee" << count++;
        std::string TAG = ss.str();
        log(TAG, "starting");
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
        while (true) {
            if (bakeryReady) {
                state = BAKERY;
            } else if (shouldGoToTheCashRegisters()) {
                state = CASH_REGISTER;
            } else if (shouldTakeBreak())
                state = BREAK;
            else state = STOCK;

            switch (state) {
                case CASH_REGISTER: {
                    log(TAG, "started at cash register");
                    MaintainableClosableFacility &fac = findFirstClosedCashRegister();
                    fac.open(this);
                    log(TAG, "finished at cash register");
                    break;
                }
                case BAKERY: {
                    log(TAG, "started baking");
                    bakeryReady = false;
                    Wait(Exponential(BAKERY_WORK_TIME));
                    (new BakeryReady)->Activate(Time + BAKERY_BAKING_TIME);
                    log(TAG, "finished baking");
                    break;
                }
                case STOCK: {
                    log(TAG, "started stock");
                    Wait(Exponential(STOCK_TIME));
                    log(TAG, "finished stock");
                    break;
                }
                case BREAK: {
                    log(TAG, "started break");
                    currentBreakCount++;
                    hasTakenBreak = true;
                    Wait(BREAK_TIME);
                    currentBreakCount--;
                    log(TAG, "finished break");
                    break;
                }
            }
        }
#pragma clang diagnostic pop
    }
};

class CustomerGenerator : public Event {
    void Behavior() {
        (new Customer)->Activate();
        Activate(Time + Exponential(CUSTUMER_CREATION_TIME));
    }
};

int main() {
    Init(0, SIMULATION_END_TIME);
    for (MaintainableClosableFacility &fac : cashRegisters) {
        fac.setClosingCondition(new CashRegisterClosingCondition());
        Employee *employee = new Employee();
        employee->Activate();
    }
    (new CustomerGenerator)->Activate();
    (new BakeryReady)->Activate();
    Run();
    gate.Output();
    trolleys.Output();
    shoppingKind.Output();
    for (Facility &fac : cashRegisters) {
        fac.Output();
    }
    meatShop.Output();
    boss.Output();
    std::cout << "Customers that had forgotten something and had to re-enter the shop " << returningCustomersCount
              << std::endl;
    std::cout << "Timeouted customers in trolley store " << timeoutTrolleyCount << std::endl;
    std::cout << "Timeouted customers in meat shop " << timeoutMeatShopCount << std::endl;
    std::cout << "Number of mistakes the employees made " << employeeMistakeCount << std::endl;
    return 0;
}