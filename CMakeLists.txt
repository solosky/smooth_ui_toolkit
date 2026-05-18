cmake_minimum_required(VERSION 3.14)

if (ESP_PLATFORM)
    # ESP-IDF component registration
    file(GLOB_RECURSE SUT_SRCS src/*.c)
    idf_component_register(
        SRCS ${SUT_SRCS}
        INCLUDE_DIRS "include"
    )
else()
    project(smooth_ui_toolkit C)

    set(CMAKE_C_STANDARD 99)
    set(CMAKE_C_STANDARD_REQUIRED ON)

    file(GLOB_RECURSE SUT_SRCS src/*.c)
    include_directories(include)

    add_library(sut STATIC ${SUT_SRCS})

    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
        target_link_libraries(sut PUBLIC m)
    endif()

    option(SUT_BUILD_TEST "Build tests" ON)
    if(SUT_BUILD_TEST)
        enable_testing()
        add_subdirectory(test)
    endif()

    option(SUT_BUILD_EXAMPLE "Build examples" ON)
    if(SUT_BUILD_EXAMPLE)
        add_subdirectory(example)
    endif()
endif()
