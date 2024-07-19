SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

SET(SRC 
  gravitation.cc
  body.cc
  world.cc
)

add_library( physics STATIC ${SRC})
target_compile_definitions( physics PUBLIC -DVERBOSE)
target_compile_options( physics PUBLIC -O -Wall -Wextra -Wpedantic)
target_link_libraries(physics PUBLIC common)
set_target_properties(physics PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR} )


add_executable( physics_test physics_main.cc )
target_compile_definitions( physics_test PUBLIC -DVERBOSE)
target_compile_options( physics_test PUBLIC -O -Wall -Wextra -Wpedantic)
target_link_libraries(physics_test physics )
