//
// Created by david on 26.11.16.
//

#ifndef IMS_PROJ_FACILITYINVALIDSTATEEXCEPTION_H
#define IMS_PROJ_FACILITYINVALIDSTATEEXCEPTION_H

#include <iostream>

class FacilityInvalidStateException{
    std::string _msg;

public:
    FacilityInvalidStateException(std::string msg):_msg(msg){}

    std::string what(){
        return _msg;
    }
};

#endif //IMS_PROJ_FACILITYINVALIDSTATEEXCEPTION_H
