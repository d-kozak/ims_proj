//
// Created by david on 26.11.16.
//

#include "MaintainableClosableFacility.h"
#include "FacilityInvalidStateException.h"
#include <iostream>

void log(std::string prefix, std::string &&msg) {
    std::cout << "---> " << prefix << " : " << msg << std::endl;
}

class MyCon : public ClosableFacilityCondition {
    virtual bool closingCondition(ClosableFacility *fac) {
        return fac->QueueLen() < 3;
    }
};

MaintainableClosableFacility fac(new MyCon());

class MyProc : public Process {
    std::string TAG = "MyProc";

    void Behavior() {
        try {
            while (!fac.isFacilityAvailable()) {
                log(TAG, "facility not opened, waiting");
                Wait(10);
            }
            Seize(fac);
            Wait(Exponential(60));
            Release(fac);
            log(TAG, "Finishing");
        } catch (FacilityInvalidStateException &e) {
            std::cerr << TAG << " : " << e.what() << std::endl;
        }
    }
};

class Generator : public Event {
    int count = 0;

    void Behavior() {
        (new MyProc)->Activate();
        if (count++ < 5)
            Activate(Time + Exponential(30));
    }
};

class MaintainingProc : public Process {
    std::string TAG = "MaintainingProc";

    void Behavior() {
        try {
            log(TAG, "starting,opening the facility");
            fac.open(this);
            log(TAG, "finishing the maintenance");

            (new Generator)->Activate();
            Wait(60);


            log(TAG, "reopening the facility");
            fac.open(this);
            log(TAG, "finishing the day");

        } catch (FacilityInvalidStateException &e) {
            std::cerr << TAG << " : " << e.what() << std::endl;
        }
    }
};

int main(void) {
    Init(0, 10000);
    (new Generator)->Activate();
    (new MaintainingProc)->Activate(200);
    Run();
    fac.Output();
}

