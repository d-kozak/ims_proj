cmake_minimum_required(VERSION 3.6)
project(ims_proj)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES ClosableFacility.cpp ClosableFacility.h FacilityNotOpenException.h MaintainableClosableFacility.cpp MaintainableClosableFacility.h FacilityInvalidStateException.h )
add_executable(basic_model basic_model.cpp ${SOURCE_FILES})
add_executable(closableFacilityTest closableFacilityTest.cpp ${SOURCE_FILES} )
add_executable(maintainableClosableFacilityTest maintainableClosableFacilityTest.cpp ${SOURCE_FILES} )

target_link_libraries(basic_model m)
target_link_libraries(basic_model simlib)

target_link_libraries(maintainableClosableFacilityTest m)
target_link_libraries(maintainableClosableFacilityTest simlib)

target_link_libraries(closableFacilityTest m)
target_link_libraries(closableFacilityTest simlib)