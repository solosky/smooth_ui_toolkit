# Build raylib from submodule
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/raylib raylib_build)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/raylib/src)

# MotionC library includes
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)

# Raylib helper
add_library(raylib_helper STATIC raylib_helper.c)

function(add_mc_example name source)
    add_executable(${name} ${source})
    target_link_libraries(${name} raylib_helper mc raylib m)
endfunction()

add_mc_example(animate            animate.c)
add_mc_example(animate_value      animate_value.c)
add_mc_example(animate_value_opts animate_value_options.c)
add_mc_example(basic_animate      basic_animate.c)
add_mc_example(bubbles            bubbles.c)
add_mc_example(multi_cursor       multi_cursor.c)
add_mc_example(easing_curves      easing_curves.c)
add_mc_example(comprehensive_test comprehensive_test.c)
