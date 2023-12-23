#ifndef GAME_OF_LIFE_PATTERN_INPUT_SCREEN_H
#define GAME_OF_LIFE_PATTERN_INPUT_SCREEN_H

#include "grid_screen.h"

class PatternInputScreen: public GridScreen{
private:
    bool dragging;
    sf::Vector2i initial_click_pos;
public:
    PatternInputScreen();
    short int run() override;
};

#endif
