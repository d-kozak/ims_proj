//
// Created by david on 25.11.16.
//

#ifndef IMS_PROJ_CLOSABLEFACILITY_H
#define IMS_PROJ_CLOSABLEFACILITY_H

#include "simlib.h"

class ClosableFacility;

/**
 * By default, always return false
 */
class ClosableFacilityCondition {
    virtual bool closingCondition(ClosableFacility * fac) = 0;

public:
    bool operator()(ClosableFacility * fac) {
        return this->closingCondition(fac);
    }
};

class AlwaysTrue : public ClosableFacilityCondition{
    virtual bool closingCondition(ClosableFacility * fac){
        return true;
    }
};

class AlwaysFalse : public ClosableFacilityCondition{
    virtual bool closingCondition(ClosableFacility * fac){
        return false;
    }
};

class ClosableFacility : public Facility {
    friend class ClosableFacilityCondition;
    bool _isOpen;

    ClosableFacilityCondition *_closingCondition;

public:
    ClosableFacility(bool isOpen):Facility(),_closingCondition(new AlwaysFalse()),_isOpen(isOpen){}
    ClosableFacility():Facility(),_closingCondition(new AlwaysFalse()),_isOpen(true){}
    ClosableFacility(ClosableFacilityCondition *closingCondition)
            : Facility(), _closingCondition(closingCondition), _isOpen(true) {}

    ClosableFacility(ClosableFacilityCondition *closingCondition, bool isOpen) : _closingCondition(closingCondition),
                                                                                 _isOpen(isOpen) {}

    ClosableFacility(const char *_name, ClosableFacilityCondition *closingCondition) : Facility(_name),
                                                                                       _closingCondition(
                                                                                               closingCondition),
                                                                                       _isOpen(true) {}

    ClosableFacility(const char *_name, ClosableFacilityCondition *closingCondition, bool isOpen) : Facility(_name),
                                                                                                    _closingCondition(
                                                                                                            closingCondition),
                                                                                                    _isOpen(isOpen) {}

    ClosableFacility(Queue *_queue1, ClosableFacilityCondition *closingCondition) : Facility(_queue1),
                                                                                    _closingCondition(closingCondition),
                                                                                    _isOpen(true) {}

    ClosableFacility(Queue *_queue1, ClosableFacilityCondition *closingCondition, bool isOpen) : Facility(_queue1),
                                                                                                 _closingCondition(
                                                                                                         closingCondition),
                                                                                                 _isOpen(isOpen) {}

    ClosableFacility(const char *_name, Queue *_queue1, ClosableFacilityCondition *closingCondition) : Facility(_name,
                                                                                                                _queue1),
                                                                                                       _closingCondition(
                                                                                                               closingCondition),
                                                                                                       _isOpen(true) {}

    ClosableFacility(const char *_name, Queue *_queue1, ClosableFacilityCondition *closingCondition, bool isOpen)
            : Facility(_name,
                       _queue1),
              _closingCondition(
                      closingCondition),
              _isOpen(isOpen) {}

    // overriding because default param was not working
    virtual void Seize(Entity *e, ServicePriority_t sp);

    virtual void Seize(Entity *e);

    virtual void Release(Entity *e);

    bool checkClosingCondition();

    virtual void open();

    virtual void close();

    bool isOpen() {
        return _isOpen;
    }

    bool setClosingCondition(ClosableFacilityCondition * condition){
        delete this->_closingCondition;
        this->_closingCondition = condition;
    }

    ~ClosableFacility(){
        delete this->_closingCondition;
    }
};

#endif //IMS_PROJ_CLOSABLEFACILITY_H
