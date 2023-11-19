cmake_minimum_required(VERSION 3.21)
project(game_of_life)

set(CMAKE_CXX_STANDARD 17)

add_executable(game_of_life main.cpp game_logic.cpp game_logic.h patterns.cpp patterns.h globals.h)

set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_DIR D:/SFML/lib/cmake/SFML)
find_package(SFML COMPONENTS system window graphics audio network REQUIRED)

target_link_libraries(game_of_life sfml-system sfml-window sfml-graphics sfml-audio)