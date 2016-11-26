//
// Created by david on 26.11.16.
//

#include "MaintainableClosableFacility.h"
#include "FacilityInvalidStateException.h"

void MaintainableClosableFacility::open(Process *proc) {
    if (isOpen()) {
        throw FacilityInvalidStateException("Cannot open already opened facility");
    }
    this->_directingProcess = proc;
    this->_isShuttingDown = false;
    ClosableFacility::open();
    this->_directingProcess->Passivate();

}

void MaintainableClosableFacility::close() {
    if (!isOpen())
        throw FacilityInvalidStateException("Cannot close a facility that is not open");
    ClosableFacility::close();
    //lastly, wake the process who was maintaining the facility
    this->_directingProcess->Activate();
    this->_directingProcess = nullptr;
}


// overriding because default param was not working
void MaintainableClosableFacility::Seize(Entity *e, ServicePriority_t sp) {
    if(isShuttingDown()){
        throw FacilityInvalidStateException("Cannot enter a queue when the facility is shutting down");
    }
    ClosableFacility::Seize(e,sp);
}

void MaintainableClosableFacility::Seize(Entity *e) {
    if(isShuttingDown()){
        throw FacilityInvalidStateException("Cannot enter a queue when the facility is shutting down");
    }
    ClosableFacility::Seize(e);
}


void MaintainableClosableFacility::Release(Entity *e) {
    // release the facility
    Facility::Release(e);
    if (checkClosingCondition() && !isShuttingDown()) {
        // if the closing condition is true, facility enters the 'shutting down' state
        // it will serve the remaining processes but does not allow new ones to get in the queue
        this->_isShuttingDown = true;
    }


    if (this->QueueLen() == 0 && isShuttingDown() && isOpen()) {
        // if there is nobody in the queue, we can shut down
        this->close();
    }
}