project(ZyROSConnectionManager)

# Build list of available ROS message types
message(STATUS "Generating ROS message and service type header files.")
SET(ROS_DISTRO "$ENV{ROS_DISTRO}")
if ("${ROS_DISTRO}" STREQUAL "")
    SET(ROS_DISTRO "melodic")
endif()

SET(ROS_INSTALL_PREFIX "$ENV{ROS_ROOT}")
if ("${ROS_INSTALL_PREFIX}" STREQUAL "")
    SET(ROS_INSTALL_PREFIX "/opt/ros/melodic")
endif()

message(STATUS "Calling list_ros_message_types.sh with arguments: \"${CMAKE_CURRENT_BINARY_DIR}\" \"${ROS_INSTALL_PREFIX}\" \"${ROS_DISTRO}\"")

execute_process(COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/list_ros_message_types.sh" "${CMAKE_CURRENT_BINARY_DIR}" "${ROS_INSTALL_PREFIX}" "${ROS_DISTRO}" OUTPUT_VARIABLE ROS_MSG_TYPES OUTPUT_STRIP_TRAILING_WHITESPACE)
message(STATUS "List of ROS message types: ${ROS_MSG_TYPES}")
string(REPLACE "\n" ";" ROS_MSG_TYPES_LIST ${ROS_MSG_TYPES})
foreach(ROS_MSG_TYPE ${ROS_MSG_TYPES_LIST})
    message(STATUS "Message type: ${ROS_MSG_TYPE}")
endforeach()

set(HEADER_FILES
    initZyROSConnectionManager.h
    ZyROSConnectionManager.h
    ${CMAKE_CURRENT_BINARY_DIR}/ZyROS_MessageType_Instantiations.h
    ${CMAKE_CURRENT_BINARY_DIR}/ZyROS_ServiceType_Instantiations.h
)

set(SOURCE_FILES
    initZyROSConnectionManager.cpp
    ZyROSConnectionManager.cpp
)

find_package(ROS QUIET REQUIRED)
message(STATUS "ROS found: ${ROS_FOUND}")
find_package(Boost COMPONENTS regex thread signals QUIET REQUIRED)
find_package(Log4cxx QUIET REQUIRED)

# Boost
find_package(Boost COMPONENTS system chrono thread REQUIRED QUIET)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

# SOFA include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/modules)

# Dependencies
include_directories(${CMAKE_SOURCE_DIR}/applications/plugins/SofaROSConnector/ZyROSConnector)
include_directories(${CMAKE_SOURCE_DIR}/applications/plugins/SofaROSConnector)
include_directories(${CMAKE_SOURCE_DIR}/SofaKernel/framework)
include_directories(${CMAKE_BINARY_DIR}/include)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# ROS (hydro)
include_directories(${ROS_INCLUDE_DIR})
include_directories(${LOG4CXX_INCLUDE_DIRS})

add_definitions("-DSOFA_BUILD_ZY_ROS_CONNECTION_MANAGER")
add_definitions(-DROS_BUILD_SHARED_LIBS)

add_library(${PROJECT_NAME} SHARED ${HEADER_FILES} ${SOURCE_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Plugins")
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "_d")

target_link_libraries(${PROJECT_NAME} SofaCore SofaHelper SofaDefaultType SofaBaseCollision SofaConstraint SofaSimulationCommon SofaMiscFem tinyxml newmat)
target_link_libraries(${PROJECT_NAME} ZyROSConnector)
target_link_libraries(${PROJECT_NAME} ${ROS_LIBRARY} ${ROS_CPP_LIBRARY} ${ROS_CPPCOMMON_LIBRARY} ${ROS_TIME_LIBRARY} ${ROS_CONSOLE_LIBRARY} ${ROS_CPP_SERIALIZATION_LIBRARY})

target_link_libraries(${PROJECT_NAME} ${Boost_REGEX_LIBRARY} ${Boost_THREAD_LIBRARY} ${Boost_SIGNALS_LIBRARY})
target_link_libraries(${PROJECT_NAME} ${LOG4CXX_LIBRARIES})

find_package(SofaTest QUIET)
if(SofaTest_FOUND)
    add_subdirectory(ZyROSConnector_test)
endif()

sofa_create_package(${PROJECT_NAME} "0.0.1" ${PROJECT_NAME} ${PROJECT_NAME})
