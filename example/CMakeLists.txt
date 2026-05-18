# Build raylib from submodule
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/raylib raylib_build)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/raylib/src)

# SUT library includes
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)

# Raylib helper
add_library(raylib_helper STATIC raylib_helper.c)

function(add_c99_example name source)
    add_executable(${name} ${source})
    target_link_libraries(${name} raylib_helper mc raylib m)
endfunction()

add_c99_example(c99_animate            animate.c)
add_c99_example(c99_animate_value      animate_value.c)
add_c99_example(c99_animate_value_opts animate_value_options.c)
add_c99_example(c99_basic_animate      basic_animate.c)
add_c99_example(c99_bubbles            bubbles.c)
add_c99_example(c99_multi_cursor       multi_cursor.c)
add_c99_example(c99_easing_curves      easing_curves.c)
add_c99_example(c99_comprehensive_test comprehensive_test.c)
