cmake_minimum_required(VERSION 3.14)

if (ESP_PLATFORM)
    file(GLOB_RECURSE MC_SRCS src/*.c)
    idf_component_register(
        SRCS ${MC_SRCS}
        INCLUDE_DIRS "include"
    )
else()
    project(motionc C)

    set(CMAKE_C_STANDARD 99)
    set(CMAKE_C_STANDARD_REQUIRED ON)

    file(GLOB_RECURSE MC_SRCS src/*.c)
    include_directories(include)

    add_library(mc STATIC ${MC_SRCS})

    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
        target_link_libraries(mc PUBLIC m)
    endif()

    option(MC_BUILD_TEST "Build tests" ON)
    if(MC_BUILD_TEST)
        enable_testing()
        add_subdirectory(test)
    endif()

    option(MC_BUILD_EXAMPLE "Build examples" ON)
    if(MC_BUILD_EXAMPLE)
        add_subdirectory(example)
    endif()
endif()
