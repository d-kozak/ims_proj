//
// Created by david on 25.11.16.
//

#ifndef IMS_PROJ_CLOSABLEFACILITY_H
#define IMS_PROJ_CLOSABLEFACILITY_H

#include "simlib.h"

class ClosableFacility;

/**
 * Class modeling closing condition for ClosableFacility.
 * Usage: inherit from this class and specify the abstract method as a closing condition you need
 */
class ClosableFacilityCondition {
    /**
     * specifies a closing condition for the facility
     * @param fac facility, to which this condition belongs
     * @return
     */
    virtual bool closingCondition(ClosableFacility * fac) = 0;

public:
    /**
     * Object is used as a functor
     * @param fac
     * @return
     */
    bool operator()(ClosableFacility * fac) {
        return this->closingCondition(fac);
    }
};

/**
 * Special condition that always returns true
 */
class AlwaysTrue : public ClosableFacilityCondition{
    virtual bool closingCondition(ClosableFacility * fac){
        return true;
    }
};

/**
 * Special condition that always returns false
 */
class AlwaysFalse : public ClosableFacilityCondition{
    virtual bool closingCondition(ClosableFacility * fac){
        return false;
    }
};

/**
 * Special kind of facility that can be closed when closing condition is met or by calling method close(),
 * it can be openen again by calling open()
 */
class ClosableFacility : public Facility {
    /**
     * To make the closing condition use whatever it needs to make a decision
     */
    friend class ClosableFacilityCondition;

    /**
     * specifies current state of the closable facility
     */
    bool _isOpen;

    /**
     * current closing condition, it is possible to change it by calling setClosableCondition
     */
    ClosableFacilityCondition *_closingCondition;

public:
    /**
     * whole bunch of constructors,
     * @see Facility for details about other params
     * @param isOpen specifies starting state of the facility
     * @param closingCondition specifies closing condition
     */
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

    /**
     * @see Facility::Seize
     * @throws FacilityNotOpenException if the facility is closed
     */
    virtual void Seize(Entity *e, ServicePriority_t sp);

    /**
     * @see Facility::Seize
     * @throws FacilityNotOpenException if the facility is closed
     */
    virtual void Seize(Entity *e);

    /**
     * @see Facility::Release
     * @throws FacilityNotOpenException if the facility is closed
     */
    virtual void Release(Entity *e);

    /**
     * checks the underlying closing condition
     * @return true if the condition is met, false otherwise
     */
    bool checkClosingCondition();

    /**
     * force-opens the facility
     */
    virtual void open();

    /**
     * force-closes the facility
     */
    virtual void close();

    /**
     * @return true if the facility is open, false otherwise
     */
    bool isOpen() {
        return _isOpen;
    }

    /**
     * sets new closing condition
     * @warning by calling the method you pass ownership of the condition, it will be deleted in closablefacility's destructor,
     * you should not use the condition at all after calling this method
     * @param condition new condition
     */
    void setClosingCondition(ClosableFacilityCondition * condition){
        delete this->_closingCondition;
        this->_closingCondition = condition;
    }

    /**
     * desctructor,  deletes the condition
     */
    ~ClosableFacility(){
        delete this->_closingCondition;
    }
};

#endif //IMS_PROJ_CLOSABLEFACILITY_H
