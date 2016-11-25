//
// Created by david on 25.11.16.
//

#include "ClosableFacility.h"
#include "FacilityNotOpenException.h"

void ClosableFacility::open() {
    this->_isOpen = true;
}

void ClosableFacility::close() {
    this->_isOpen = false;
}

bool ClosableFacility::checkClosingCondition() {
    bool res = (*this->_closingCondition)();
    if(res)
        close();
    return res;
}

void ClosableFacility::Seize(Entity *e){
    if(!isOpen()){
        throw FacilityNotOpenException("");
    }
    Facility::Seize(e);
}

void ClosableFacility::Seize(Entity *e, ServicePriority_t sp){
    if(!isOpen()){
        throw FacilityNotOpenException("");
    }
    Facility::Seize(e,sp);
}

void ClosableFacility::Release(Entity *e){
    if(!isOpen()){
        throw FacilityNotOpenException("");
    }
    Facility::Release(e);
}