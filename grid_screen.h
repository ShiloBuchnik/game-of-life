#ifndef GAME_OF_LIFE_GRID_SCREEN_H
#define GAME_OF_LIFE_GRID_SCREEN_H

#include "base_screen.h"
#include <cmath>

class GridScreen: public BaseScreen{
protected:
    static GridGlobalSettings& grid_settings;

    bool focus; // True iff focus is on window.
    bool clicking;
    sf::Vector2i old_pos;
    sf::Clock key_press_clock;

    static float distance(const sf::Vector2i& vec1, const sf::Vector2i& vec2){
        return sqrt(pow(vec2.x - vec1.x, 2) + pow(vec2.y - vec1.y, 2));
    }

    void checkForChangeViewWithKeys(const int& delta_time);

public:
    GridScreen();
};

#endif
