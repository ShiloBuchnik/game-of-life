#ifndef GAME_OF_LIFE_GRID_SCREEN_H
#define GAME_OF_LIFE_GRID_SCREEN_H

#include "screens.h"
#include <cmath>

#define SPEED 0.3

class GridScreen: public BaseScreen{
protected:
    static sf::Color live_cell_color;
    static sf::Color dead_cell_color;
    static sf::Color outline_color;

    static float distance(const sf::Vector2i& vec1, const sf::Vector2i& vec2){
        return sqrt(pow(vec2.x - vec1.x, 2) + pow(vec2.y - vec1.y, 2));
    }

    void handleDrag(sf::Vector2i& old_pos, const sf::Vector2i& new_pos);

    void checkForChangeViewWithKeys(const int& delta_time);
    static void drawGrid();

public:
};

#endif