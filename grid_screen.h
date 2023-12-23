#ifndef GAME_OF_LIFE_GRID_SCREEN_H
#define GAME_OF_LIFE_GRID_SCREEN_H

#include "base_screen.h"
#include <cmath>

class GridScreen: public BaseScreen{
protected:
    // TODO: give also menu class its own view (both menus share it).
    // left-top coordinate of the view rectangle. We keep track of it, so we can restrict the user from moving the view out of bounds.
    static sf::Vector2i left_top_view_pos;
    // 1st and 2nd arguments are left-top coordinate of the rectangle. 3rd and 4th arguments are its width and height respectively.
    // Our initial view is exactly at the middle of the grid.
    static sf::View view;

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
