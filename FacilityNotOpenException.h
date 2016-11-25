//
// Created by david on 25.11.16.
//

#ifndef IMS_PROJ_FACILITYNOTOPENEXCEPTION_H
#define IMS_PROJ_FACILITYNOTOPENEXCEPTION_H


#include <iostream>

class FacilityNotOpenException {
    std::string _msg;
public:
    FacilityNotOpenException(std::string msg):_msg(msg){}

    std::string what(){
        return _msg;
    }
};


#endif //IMS_PROJ_FACILITYNOTOPENEXCEPTION_H
