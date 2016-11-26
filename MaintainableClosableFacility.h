//
// Created by david on 26.11.16.
//

#ifndef IMS_PROJ_MAINTAINABLECLOSABLEFACILITY_H
#define IMS_PROJ_MAINTAINABLECLOSABLEFACILITY_H


#include "ClosableFacility.h"

/**
 * Special kind of closable facility that can only be open when a process is maintaining it(e.g. a shop assistant and a cash register
 */
class MaintainableClosableFacility : public ClosableFacility{
    /**
     * process which is maintaing the facility
     */
    Process * _directingProcess = nullptr;
    /**
     * shutting down is a special state between open and close, when processes in the queue are served but new ones are not allowed to enter
     */
    bool _isShuttingDown = false;
public:
    /**
     * whole bunch of constructors
     * @see ClosableFacility
     * isOpen is always false, to open it you have to call open a pass a process which will maintain the facility
     */
    MaintainableClosableFacility():ClosableFacility(false){}
    MaintainableClosableFacility(ClosableFacilityCondition *closingCondition)
    : ClosableFacility(closingCondition,false) {}
    MaintainableClosableFacility(const char *_name, ClosableFacilityCondition *closingCondition) : ClosableFacility(_name,closingCondition,false){}
    MaintainableClosableFacility(Queue *queue1, ClosableFacilityCondition *closingCondition) : ClosableFacility(queue1,closingCondition,false){}

    MaintainableClosableFacility(const char *_name, Queue *_queue1, ClosableFacilityCondition *closingCondition) : ClosableFacility(_name,_queue1,closingCondition,false) {}


    /**
     * opens the facility
     * @param proc specifies maintaing process, which will be Passivated until the facility is closed again
     */
    virtual void open(Process * proc);
    /**
     * closes the facility and activates the maintaining process
     */
    virtual void close();

    /**
     * @see Facility::Seize
     * @see ClosableFacility::Seize
     * @throws FacilityInvalidStateException if the facility is shutting down
     */
    virtual void Seize(Entity *e, ServicePriority_t sp);
    /**
     * @see Facility::Seize
     * @see ClosableFacility::Seize
     * @throws FacilityInvalidStateException if the facility is shutting down
     */
    virtual void Seize(Entity *e);

    /**
     * Apart from callsing super::release it also checks the closing condition
     * and changes state to shutting down if the condition is met
     * @see Facility::Release
     * @see ClosableFacility::Release
     */
    virtual void Release(Entity * e);

    /**
     * @return true if the facility is shutting down
     */
    bool isShuttingDown(){ return _isShuttingDown;}


    /**
     * @return true if the the facility can be used by 'client-like' process
     * this means that some process is currently maintaining the facility and it is not shutting down
     */
    bool isFacilityAvailable()
    {
        return isOpen() && !isShuttingDown();
    }
};


#endif //IMS_PROJ_MAINTAINABLECLOSABLEFACILITY_H
