find_package(SFML COMPONENTS graphics audio REQUIRED)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

SET(SRC 
  polygon.cc
  shape.cc
  render_engine.cc
  scene.cc
  camera.cc
)

add_library( graphics STATIC ${SRC})
target_compile_definitions( graphics PUBLIC -DVERBOSE)
target_compile_options( graphics PUBLIC -O -Wall -Wextra -Wpedantic)
target_link_libraries(graphics PUBLIC sfml-graphics common)
set_target_properties(graphics PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR} )

add_executable( graphics_test graphics_main.cc )
target_compile_definitions( graphics_test PUBLIC -DVERBOSE)
target_compile_options( graphics_test PUBLIC -O -Wall -Wextra -Wpedantic)
target_link_libraries(graphics_test graphics )
