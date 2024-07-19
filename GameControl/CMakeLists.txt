SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

SET(SRC 
  camera_control.cc
  mouse.cc
)

add_library( game_control STATIC ${SRC})
target_compile_definitions( game_control PUBLIC -DVERBOSE)
target_compile_options( game_control PUBLIC -O -Wall -Wextra -Wpedantic)
target_link_libraries(game_control PUBLIC common graphics)
set_target_properties(game_control PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR} )
