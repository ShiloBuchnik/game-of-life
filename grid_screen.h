#ifndef GAME_OF_LIFE_GRID_SCREEN_H
#define GAME_OF_LIFE_GRID_SCREEN_H

#include "screens.h"

#define SPEED 0.4

class GridScreen: public BaseScreen{
protected:
    static sf::Color live_cell_color;
    static sf::Color dead_cell_color;
    static sf::Color outline_color;

    static float distance(const sf::Vector2i& vec1, const sf::Vector2i& vec2);
    void checkForChangeViewWithKeys(const int& delta_time) const;
    void handleDrag(sf::Vector2i& old_pos, const sf::Vector2i& new_pos);
    static void drawGrid();
};

#endif