//
// Created by david on 25.11.16.
//

#ifndef IMS_PROJ_CLOSABLEFACILITY_H
#define IMS_PROJ_CLOSABLEFACILITY_H

#include "simlib.h"

class ClosableFacilityCondition {
    virtual bool closingCondition() = 0;

public:
    bool operator()() {
        return this->closingCondition();
    }
};

class AlwaysTrue : public ClosableFacilityCondition{
    virtual bool closingCondition(){
        return true;
    }
};

class ClosableFacility : public Facility {
    bool _isOpen;

    ClosableFacilityCondition *_closingCondition;

public:
    ClosableFacility():Facility(),_closingCondition(new AlwaysTrue()),_isOpen(true){}
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
    virtual void Seize(Entity *e, ServicePriority_t sp = DEFAULT_PRIORITY);

    virtual void Seize(Entity *e);

    virtual void Release(Entity *e);

    bool checkClosingCondition();

    void open();

    void close();

    bool isOpen() {
        return _isOpen;
    }

    ~ClosableFacility(){
        delete this->_closingCondition;
    }
};

#endif //IMS_PROJ_CLOSABLEFACILITY_H
