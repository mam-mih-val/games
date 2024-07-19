project(games)
cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE debug)

add_subdirectory( Common )
add_subdirectory( Graphics )
add_subdirectory( GameControl )
add_subdirectory( Physics )
add_subdirectory( Visualization )