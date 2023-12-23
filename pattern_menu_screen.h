#ifndef GAME_OF_LIFE_PATTERN_MENU_SCREEN_H
#define GAME_OF_LIFE_PATTERN_MENU_SCREEN_H

#include "menu_screen.h"

class PatternMenuScreen: public MenuScreen{
private:
    std::vector <std::string> num_to_pattern_path;

public:
    PatternMenuScreen();
    short int run() override;
};

#endif
