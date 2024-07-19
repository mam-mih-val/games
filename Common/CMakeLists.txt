find_package(SFML COMPONENTS graphics REQUIRED)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

SET(SRC 
  coordinates.cc
)

add_library( common STATIC ${SRC})
target_compile_definitions( common PUBLIC -DVERBOSE)
target_compile_options( common PUBLIC -O -Wall -Wextra -Wpedantic)
target_link_libraries(common PUBLIC sfml-graphics)
set_target_properties( common PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR} )
