//
// Created by david on 26.11.16.
//

#ifndef IMS_PROJ_INTERNAL_H
#define IMS_PROJ_INTERNAL_H

/**
 * logging function
 * @param prefix specifies the identification, should be unique for every object that will use this function
 * @param msg the actual message to be logged
 */
void log(std::string prefix, std::string &&msg) {
    std::cout << "---> " << prefix << " : " << msg << std::endl;
}

#endif //IMS_PROJ_INTERNAL_H
