//
// Created by david on 6.12.16.
//

#ifndef IMS_PROJ_TIMEOUTABLEPROCCESS_H
#define IMS_PROJ_TIMEOUTABLEPROCCESS_H

#include <simlib.h>

/**
 * "Library" section
 * It contains extensions for simlib classes which proved to be useful in our project
 */
class TimeoutableProcess;

class Timeout : public Event {
    TimeoutableProcess *_process;
public:
    Timeout(TimeoutableProcess *process) : Event(), _process(process) {};

    void Behavior();

};

class TimeoutableProcess : public Process {
    friend class Timeout;

    Timeout *_timeout;
    bool _timeoutHappened;
protected:
    void cancelTimeout();

    void setTimeout(double time);

    void timeout();

    bool wasTimeouted();
};


#endif //IMS_PROJ_TIMEOUTABLEPROCCESS_H
