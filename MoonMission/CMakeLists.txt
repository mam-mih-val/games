find_package(SFML COMPONENTS graphics audio REQUIRED)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

SET(SRC 
  src/physics.cc
  src/window.cc
  src/input.cc
  src/coordinates.cc
)

add_executable( moon_landing src/main.cc ${SRC})
target_compile_definitions( moon_landing PUBLIC -DVERBOSE)
target_compile_options( moon_landing PUBLIC -O -Wall -Wextra -Wpedantic)
target_link_libraries(moon_landing sfml-graphics)