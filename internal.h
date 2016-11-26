//
// Created by david on 26.11.16.
//

#ifndef IMS_PROJ_INTERNAL_H
#define IMS_PROJ_INTERNAL_H

void log(std::string prefix, std::string &&msg) {
    std::cout << "---> " << prefix << " : " << msg << std::endl;
}

#endif //IMS_PROJ_INTERNAL_H
