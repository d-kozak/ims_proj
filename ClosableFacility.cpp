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
    return this->_closingCondition->operator()(this);
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