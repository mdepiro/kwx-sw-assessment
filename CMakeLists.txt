cmake_minimum_required(VERSION 3.16)
project(sw_assessment LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(ZMQ REQUIRED IMPORTED_TARGET libzmq)

add_library(sw_assessment_common INTERFACE)
target_include_directories(sw_assessment_common
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/cppzmq
)
# Also pass on the command line so draft APIs are visible even if a
# precompiled header or system include pulls zmq.h in first.
target_compile_definitions(sw_assessment_common INTERFACE ZMQ_BUILD_DRAFT_API=1)
target_link_libraries(sw_assessment_common INTERFACE PkgConfig::ZMQ)

add_executable(sender src/sender.cpp)
target_link_libraries(sender PRIVATE sw_assessment_common)

add_executable(receiver src/receiver.cpp)
target_link_libraries(receiver PRIVATE sw_assessment_common)

install(TARGETS sender receiver RUNTIME DESTINATION bin)
