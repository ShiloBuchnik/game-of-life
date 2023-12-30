#ifndef GAME_OF_LIFE_PATTERN_INPUT_SCREEN_H
#define GAME_OF_LIFE_PATTERN_INPUT_SCREEN_H

#include "screens.h"

class PatternInputScreen: public GridScreen{
private:
    void handleLeftClick(const sf::Vector2i& pixel_pos) const;

public:
    short int run() override;
};

#endif
