//
// Created by david on 25.11.16.
//

#include <iostream>
#include "simlib.h"
#include "ClosableFacility.h"
#include "FacilityNotOpenException.h"


class MyCondition : public ClosableFacilityCondition {
    virtual bool closingCondition(){
        return true;
    }
};

ClosableFacility fac(new MyCondition());

class MyProc : public Process{
    void Behavior(){
        Seize(fac);
        Wait(Exponential(60));
        Release(fac);

        fac.close();

        bool except = false;
        try {
            Seize(fac);
            Wait(Exponential(60));
            Release(fac);
        } catch (FacilityNotOpenException & e){
            except = true;
        }
        if(!except){
            std::cerr << "Facility should not work now" << std::endl;
        }
        fac.open();
    }
};

class Generator : public Event {
    void Behavior() {
        (new MyProc)->Activate();
        Activate(Time + Exponential(30));
    }
};

int main(void){
    Init(0, 1000);
    (new Generator)->Activate();
    Run();
    fac.Output();
}

