cmake_minimum_required(VERSION 3.14)
project(smooth_ui_toolkit_c99 C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Library sources
file(GLOB_RECURSE SUT_SRCS src/*.c)
include_directories(include)

add_library(sut STATIC ${SUT_SRCS})

# Need -lm for sqrtf when SUT_USE_FLOAT is enabled
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    target_link_libraries(sut PUBLIC m)
endif()

# Test subdirectory
option(SUT_BUILD_TEST "Build tests" ON)
if(SUT_BUILD_TEST)
    enable_testing()
    add_subdirectory(test)
endif()
