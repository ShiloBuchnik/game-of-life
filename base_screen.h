#ifndef GAME_OF_LIFE_BASE_SCREEN_H
#define GAME_OF_LIFE_BASE_SCREEN_H

#include <SFML/Graphics.hpp>
#include <unordered_set>
#include <set>
#include "pair_functors.h"

#define CHARACTER_SIZE 25
#define WINDOW_FRACTION 0.75
#define MULTIPLE 101 // The grid is 'MULTIPLE' times bigger than the initial window size, so that it looks like an "infinite" grid.
#define CELL_SIZE 30 // Window width and height must divide cell size

class BaseScreen{
protected:
    static sf::Font font;
    static int window_width, window_height;
    static sf::RenderWindow window;

    // left-top coordinate of the view rectangle. We keep track of it, so we can restrict the user from moving the view out of bounds.
    static sf::Vector2i left_top_view_pos;
    // 1st and 2nd arguments are left-top coordinate of the rectangle. 3rd and 4th arguments are its width and height respectively.
    static sf::View view;

    static std::set<short int> born_digits;
    static std::set<short int> survive_digits;

    // We implement the grid using a sparse matrix - which is just a set that stores only the *live cells*.
    // Thus, the space complexity reduces from O(n^2) to O(num of live cells).
    static std::unordered_set<sf::Vector2i, pair_hash, pair_equal> grid;
    const int grid_width, grid_height;

    static sf::Clock code_timer; // For logging
    sf::Cursor cursor;

    void resize(const sf::Event& evnt) const;
    static void setInitialView();

    // For logging
    static void printLogTime(bool reset);
    static long int getLogTime(bool reset);

public:
    BaseScreen();
    virtual short int run() = 0;
};

#endif