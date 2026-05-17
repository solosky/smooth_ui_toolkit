# Raylib (installed to /tmp/raylib-install)
set(RAYLIB_INC /tmp/raylib-install/include)
set(RAYLIB_LIB /tmp/raylib-install/lib/libraylib.so)

# SUT library includes
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../lib/include)

# Raylib helper
add_library(raylib_helper STATIC raylib_helper.c)
target_include_directories(raylib_helper PUBLIC ${RAYLIB_INC})

function(add_c99_example name source)
    add_executable(${name} ${source})
    target_link_libraries(${name} raylib_helper sut ${RAYLIB_LIB} m)
    target_include_directories(${name} PRIVATE ${RAYLIB_INC})
endfunction()

add_c99_example(c99_basic_animate   basic_animate.c)
add_c99_example(c99_animate_value   animate_value.c)
add_c99_example(c99_multi_cursor    multi_cursor.c)
add_c99_example(c99_easing_curves   easing_curves.c)
