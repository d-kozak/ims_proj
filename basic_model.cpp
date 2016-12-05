#include <iostream>
#include <sstream>
#include "simlib.h"
#include "ClosableFacility.h"
#include "MaintainableClosableFacility.h"
#include "internal.h"
#include "FacilityInvalidStateException.h"


#include <sys/time.h>

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

/**
 * time related constants
 */
static const int SECOND = 1;
static const int MINUTE = 60 * SECOND;
static const int HOUR = MINUTE * 60;

/**
 * We are modelling three hours in the shop 16-19
 */
static const int SIMULATION_END_TIME = 3 * HOUR;

static const int CUSTUMER_CREATION_TIME = 11 * SECOND;
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


/**
 * the number of all customers
 */
int customerCount = 0;

/**
 * number of employees having a break
 */
int currentBreakCount = 0;

/**
 * store for trolleys
 */
Store trolleys("Trolleys", TROLLEY_STORE_CAPACITY);
/**
 * number of customer who were timeouted waiting for trolleys
 */
int timeoutTrolleyCount = 0;

/**
 * gate which every customer has to go through to enter the shop
 */
Facility gate("Gate");

/**
 * cash registers, only possible to eneter the quee if isFacilityAvailable returns true
 * @see MaintainableClosableFacility
 */
MaintainableClosableFacility cashRegisters[CASH_REGISTER_SIZE];

/**
 * Hustogram representing different categories for shopping time - short, medium,long
 */
Histogram shoppingKind("Shopping kind", 0, MINUTE, 10);

/**
 * number of customer who were timeouted waiting for some meat
 */
int timeoutMeatShopCount = 0;
/**
 * store representing the meat shop
 */
Store meatShop("meat shop", MEAT_SHOP_CAPACITY);

/**
 * number of customer who forgot something and had to re-enter
 */
int returningCustomersCount = 0;

/**
 * facility representing the boss, which solves the mistakes the employees at the cash register somethimes make
 */
Facility boss("boss");
/**
 * number of mistakes the employees made
 */
int employeeMistakeCount = 0;

/**
 * @return the number of active cash registers
 */
int countActiveCashRegisters() {
    int count = 0;
    for (MaintainableClosableFacility &fac : cashRegisters) {
        if (fac.isOpen()) {
            count++;
        }
    }
    return count;
}

/**
 * @return the average len of quete before facility (includint queues before facilities that are shutting down)
 */
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

/**
 * @return a reference to the first closed cash register(employee will use this function to find a cash register to open)
 * @throws FacilityInvalidStateException if no facility is closed
 */
MaintainableClosableFacility &findFirstClosedCashRegister() {
    for (MaintainableClosableFacility &fac : cashRegisters) {
        if (!fac.isFacilityAvailable())
            return fac;
    }
    throw FacilityInvalidStateException("No suitable cash register found");
}

/**
 * Condition for cash register
 */
class CashRegisterClosingCondition : public ClosableFacilityCondition {
    /**
     * specifies the condition that will lead to closing of a cash register
     * @param fac the facility which is being inspected
     * @return true if the facility should be closed(in practise open -> shutting down -> close)
     */
    virtual bool closingCondition(ClosableFacility *fac) {
        return countAverageCashRegisterQueueLen() < 4 && atLeastTwoCashRegistersAreWorking();
    }


    /**
     * It is only possible to close the facility if there is at least another one open
     * which means at least two open facilities together
     * @return
     */
    bool atLeastTwoCashRegistersAreWorking() {
        int workingRegisters = 0;
        for (MaintainableClosableFacility &fac : cashRegisters)
            if (fac.isFacilityAvailable())
                workingRegisters++;
        return workingRegisters > 1;
    }

};

/**
 * Process representing one customer
 */
class Customer : public TimeoutableProcess {

    /**
     * specifies returing customers, they will always do small shopping - only get the thinks they have forgotten
     */
    bool isCustomerReturning = false;

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

        // enter the shop
        Seize(gate);
        Wait(GATE_ENTERING_TIME);
        Release(gate);

        // decide how big the shopping will be
        double decision = Random();
        double shoppingTime;
        if (decision < 0.33 || isCustomerReturning) {
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
            // this should never happen, in our interpretation of system one cash register is always open
            std::cerr << "Internal error, no cash resgister is open" << std::endl;
            exit(2);
        }

        // wait in the queue until cash register is avalable
        Seize(*withShortestQueue);

        // cash register process
        Wait(Exponential(CASH_REGISTER_TIME));


        // in 3% of cases the employee makes a mistake and nees to call the boss to helo him
        decision = Random();
        if (decision > 0.97) {
            // employee has made a mistake and need to call the boss
            employeeMistakeCount++;
            Seize(boss);
            Wait(Exponential(BOSS_COMMING_TIME));
            Wait(Exponential(BOSS_PROBLEM_SOLVING_TIME));
            Release(boss);
        }

        Release(*withShortestQueue);

        // 30% of customers will want to buy some meat
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

        // 5% of customers forget something and will re-enter the shop
        decision = Random();
        if (decision > 0.95) {
            //customer forgot something and will return to the shop
            returningCustomersCount++;
            isCustomerReturning = true;
            goto start_shopping;
        }
        Leave(trolleys);
    }
};

/**
 * represents current job of the employee
 */
typedef enum {
    CASH_REGISTER, // working at a cash register
    BAKERY, // working at the bakery
    STOCK,  // working at the stock
    BREAK // having a break
} EmployeeState;


/**
 * specifies current state of bakery
 */
bool bakeryReady = false;

/**
 * represents the event in which the bread is ready, should be scheduled BAKERY_TIME ahaead in the future after employee finished his 'baking routine'
 */
class BakeryReady : public Event {
    void Behavior() {
        bakeryReady = true;
    }
};

/**
 * user for identifiying indivitual employees
 */
int count = 0;
/**
 * cound of invocations of shouldGoToTheCashRegister
 */
int methodInvokeCount = 1;

/**
 * process which represents on employee
 * employee can either work at a cash register, at be the bakery, in the stock or employee can have a break (lucky b*stard)
 */
class Employee : public Process {
    /**
     * specifies wheter the employee had a break already
     */
    bool hasTakenBreak = false;

    /**
     * employee can have a break only once and only if there is room for him (max two people can have a break at the same time)
     * @return true if the employee should have a break
     */
    bool shouldTakeBreak() {
        return !hasTakenBreak && currentBreakCount < BREAK_CAPACITY;
    }

    /**
     * employee will go to the cast registers if average len of queue is bigger the 6 ane there is a closed register for him to open
     * @return true if the employe should go to cash registers
     */
    bool shouldGoToTheCashRegisters() {
        if (methodInvokeCount++ <
            3) // small hack to make first three employees go to the cash registers event if no customers are in the shop yet
            return true;
        return (countAverageCashRegisterQueueLen() > 6 && countActiveCashRegisters() < CASH_REGISTER_SIZE);
    }

    void Behavior() {
        EmployeeState state; // state of the employee
        std::stringstream ss;
        ss << "Employee" << count++;
        std::string TAG = ss.str(); //his loggin tag, @see log
        log(TAG, "starting");
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
        /**
         * his behavior is an infitine loop in which different jobs are done
         */
        while (true) {
            if (bakeryReady) {
                // bakery is the most important job
                state = BAKERY;
            } else if (shouldGoToTheCashRegisters()) {
                // than cash registers
                state = CASH_REGISTER;
            } else if (shouldTakeBreak()) {
                // then break if possible
                state = BREAK;
            } else {
                // if no other option is available, employee will help in the stock
                state = STOCK;
            }

            switch (state) {
                case CASH_REGISTER: {
                    // find first closed register, open it and work there until a closing condition is met
                    log(TAG, "started at cash register");
                    MaintainableClosableFacility &fac = findFirstClosedCashRegister();
                    fac.open(this);
                    log(TAG, "finished at cash register");
                    break;
                }
                case BAKERY: {
                    // start baking, finish it and then schedule an event in the future when the baking job is needed again
                    log(TAG, "started baking");
                    bakeryReady = false;
                    Wait(Exponential(BAKERY_WORK_TIME));
                    (new BakeryReady)->Activate(Time + BAKERY_BAKING_TIME);
                    log(TAG, "finished baking");
                    break;
                }
                case STOCK: {
                    // work at the stock for specifies amount of time
                    log(TAG, "started stock");
                    Wait(Exponential(STOCK_TIME));
                    log(TAG, "finished stock");
                    break;
                }
                case BREAK: {
                    // have a break for specified time
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

/**
 * Genetator that creates the customers
 */
class CustomerGenerator : public Event {
    void Behavior() {
        customerCount++;
        (new Customer)->Activate();
        Activate(Time + Exponential(CUSTUMER_CREATION_TIME));
    }
};

/**
 * main function of the model
 * @return
 */
int main() {
    Init(0, SIMULATION_END_TIME); //init the simulation
    struct timeval tp;
    gettimeofday(&tp, NULL);
    long int ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
    RandomSeed(ms);
    // init the cash registers and activate employees
    for (MaintainableClosableFacility &fac : cashRegisters) {
        fac.setClosingCondition(new CashRegisterClosingCondition());
        Employee *employee = new Employee();
        employee->Activate();
    }
    // activate customer generator
    (new CustomerGenerator)->Activate();
    // activate the baking constion
    (new BakeryReady)->Activate();
    // run the simulation
    Run();

    // output results
    gate.Output();
    trolleys.Output();
    shoppingKind.Output();
    for (Facility &fac : cashRegisters) {
        fac.Output();
    }
    meatShop.Output();
    boss.Output();
    std::cout << "The number of all customers  " << customerCount << std::endl;
    std::cout << "Customers that had forgotten something and had to re-enter the shop " << returningCustomersCount
              << std::endl;
    std::cout << "Timeouted customers in trolley store " << timeoutTrolleyCount << std::endl;
    std::cout << "Timeouted customers in meat shop " << timeoutMeatShopCount << std::endl;
    std::cout << "Number of mistakes the employees made " << employeeMistakeCount << std::endl;
    return 0;
}