cmake_minimum_required(VERSION 3.16)
project(sw_assessment LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---------------------------------------------------------------------------
# libzmq with draft APIs (RADIO/DISH + UDP). Distro packages usually ship
# libzmq *without* drafts, which causes undefined reference to
# zmq_msg_set_group / zmq_join. Build a drafts-enabled copy from source.
# ---------------------------------------------------------------------------
include(FetchContent)

set(ENABLE_DRAFTS ON CACHE BOOL "ZeroMQ draft APIs (required for UDP RADIO/DISH)" FORCE)
set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(BUILD_STATIC ON CACHE BOOL "" FORCE)
set(WITH_DOCS OFF CACHE BOOL "" FORCE)
set(WITH_DOC OFF CACHE BOOL "" FORCE)
set(ENABLE_CURVE OFF CACHE BOOL "" FORCE)
set(WITH_LIBSODIUM OFF CACHE BOOL "" FORCE)
set(WITH_LIBBSD OFF CACHE BOOL "" FORCE)
set(ZMQ_BUILD_TESTS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    libzmq
    GIT_REPOSITORY https://github.com/zeromq/libzmq.git
    GIT_TAG        v4.3.5
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(libzmq)

if(TARGET libzmq-static)
    set(ZMQ_TARGET libzmq-static)
elseif(TARGET libzmq)
    set(ZMQ_TARGET libzmq)
else()
    message(FATAL_ERROR "FetchContent libzmq did not create a usable target")
endif()

add_library(sw_assessment_common INTERFACE)
target_include_directories(sw_assessment_common
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/cppzmq
)
# Headers only expose RADIO/DISH when this is defined.
target_compile_definitions(sw_assessment_common INTERFACE ZMQ_BUILD_DRAFT_API=1)
target_link_libraries(sw_assessment_common INTERFACE ${ZMQ_TARGET})

add_executable(sender src/sender.cpp)
target_link_libraries(sender PRIVATE sw_assessment_common)

add_executable(receiver src/receiver.cpp)
target_link_libraries(receiver PRIVATE sw_assessment_common)

install(TARGETS sender receiver RUNTIME DESTINATION bin)
