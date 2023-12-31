#ifndef GAME_OF_LIFE_SCREENS_H
#define GAME_OF_LIFE_SCREENS_H

enum screens_names {AUTOMATON_MENU_SCREEN, RULESTRING_MENU_SCREEN, PATTERN_MENU_SCREEN, PATTERN_INPUT_SCREEN, SAVE_GRID_SCREEN, GAME_SCREEN};

// Base (abstract) class for all screen classes
#include "base_screen.h"

// Abstract sub-classes
#include "menu_screen.h"
#include "grid_screen.h"

// All concrete screen classes
#include "automaton_menu_screen.h"
#include "rulestring_screen.h"
#include "pattern_menu_screen.h"
#include "pattern_input_screen.h"
#include "save_screen.h"
#include "game_screen.h"

#endif