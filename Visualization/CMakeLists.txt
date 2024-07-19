SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

SET(SRC 
  visualizer.cc
)

add_library( visualization STATIC ${SRC})
target_compile_definitions( visualization PUBLIC -DVERBOSE)
target_compile_options( visualization PUBLIC -O -Wall -Wextra -Wpedantic)
target_link_libraries(visualization PUBLIC common physics graphics)
set_target_properties(visualization PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR} )

add_executable( visualization_main visualization_main.cc )
target_link_libraries( visualization_main visualization game_control )