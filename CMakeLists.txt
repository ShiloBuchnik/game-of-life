cmake_minimum_required(VERSION 3.21)
project(game_of_life)

set(CMAKE_CXX_STANDARD 17)

add_executable(game_of_life main.cpp game_logic.cpp game_logic.h patterns.cpp patterns.h globals.h game_screen.h base_screen.h screens.h game_screen.cpp automaton_menu_screen.h automaton_menu_screen.cpp pattern_menu_screen.h pattern_menu_screen.cpp base_screen.cpp pattern_input_screen.h pattern_input_screen.cpp grid_screen.cpp grid_screen.h menu_screen.cpp menu_screen.h screens_settings.h screens_settings.cpp)

set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_DIR D:/SFML/lib/cmake/SFML)
find_package(SFML COMPONENTS system window graphics audio network REQUIRED)

target_link_libraries(game_of_life sfml-system sfml-window sfml-graphics sfml-audio)