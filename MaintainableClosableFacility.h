//
// Created by david on 26.11.16.
//

#ifndef IMS_PROJ_MAINTAINABLECLOSABLEFACILITY_H
#define IMS_PROJ_MAINTAINABLECLOSABLEFACILITY_H


#include "ClosableFacility.h"

class MaintainableClosableFacility : public ClosableFacility{
    Process * _directingProcess = nullptr;
    bool _isShuttingDown = false;
public:
    MaintainableClosableFacility():ClosableFacility(false){}
    MaintainableClosableFacility(ClosableFacilityCondition *closingCondition)
    : ClosableFacility(closingCondition,false) {}
    MaintainableClosableFacility(const char *_name, ClosableFacilityCondition *closingCondition) : ClosableFacility(_name,closingCondition,false){}
    MaintainableClosableFacility(Queue *queue1, ClosableFacilityCondition *closingCondition) : ClosableFacility(queue1,closingCondition,false){}

    MaintainableClosableFacility(const char *_name, Queue *_queue1, ClosableFacilityCondition *closingCondition) : ClosableFacility(_name,_queue1,closingCondition,false) {}


    virtual void open(Process * proc);
    virtual void close();

    // overriding because default param was not working
    virtual void Seize(Entity *e, ServicePriority_t sp);
    virtual void Seize(Entity *e);

    virtual void Release(Entity * e);

    bool isShuttingDown(){ return _isShuttingDown;}


    bool isFacilityAvailable()
    {
        return isOpen() && !isShuttingDown();
    }
};


#endif //IMS_PROJ_MAINTAINABLECLOSABLEFACILITY_H
