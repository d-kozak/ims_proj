//
// Created by david on 6.12.16.
//

#ifndef IMS_PROJ_TIMEOUTABLEPROCCESS_CPP_H
#define IMS_PROJ_TIMEOUTABLEPROCCESS_CPP_H

#include "TimeoutableProcess.h"

void TimeoutableProcess::cancelTimeout() {
    _timeoutHappened = false;
    this->_timeout->Cancel();
}

void TimeoutableProcess::setTimeout(double time) {
    _timeoutHappened = false;
    this->_timeout = new Timeout(this);
    this->_timeout->Activate(Time + time);
}

void TimeoutableProcess::timeout() {
    _timeoutHappened = true;
    Out();
    Activate();
}

bool TimeoutableProcess::wasTimeouted() {
    return _timeoutHappened;
}


void Timeout::Behavior() {
    _process->timeout();
    Cancel();
}




#endif //IMS_PROJ_TIMEOUTABLEPROCCESS_CPP_H
