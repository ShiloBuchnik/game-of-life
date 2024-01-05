#ifndef GAME_OF_LIFE_GRID_SCREEN_H
#define GAME_OF_LIFE_GRID_SCREEN_H

#include "screens.h"

#define SPEED 0.0006

class GridScreen: public BaseScreen{
protected:
    static sf::Color live_cell_color;
    static sf::Color dead_cell_color;
    static sf::Color outline_color;
    static float zoom;

    static float distance(const sf::Vector2i& vec1, const sf::Vector2i& vec2);
    void checkForChangeViewWithKeys(const long long int& delta_time) const;
    void resize(const sf::Event& evnt) const;
    void handleZoom(float delta) const;
    void handleDrag(sf::Vector2i& old_pos, const sf::Vector2i& new_pos) const;
    static void drawGrid();
};

#endif