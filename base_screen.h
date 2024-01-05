#ifndef GAME_OF_LIFE_BASE_SCREEN_H
#define GAME_OF_LIFE_BASE_SCREEN_H

#include <SFML/Graphics.hpp>
#include <unordered_set>
#include <set>
#include "pair_functors.h"

#define TITLE_CHARACTER_SIZE 35
#define OPTION_CHARACTER_SIZE 25
#define WINDOW_FRACTION 0.75
#define MULTIPLE 100// The grid is 'MULTIPLE' times bigger than the initial window size, so that it looks like an "infinite" grid.
#define CELL_SIZE 30 // Window width and height must divide cell size

class BaseScreen{
protected:
    //static int window_width, window_height;
    static sf::RenderWindow window;
    static sf::Cursor cursor; // Must not be destroyed throughout the entire use of the window, for some reason

    // left-top coordinate of the view rectangle. We keep track of it, so we can restrict the user from moving the view out of bounds.
    static sf::Vector2f left_top_view_pos;
    static sf::View view;

    static std::set<short int> born_digits;
    static std::set<short int> survive_digits;

    // We implement the grid using a sparse matrix - which is just a set that stores only the *live cells*.
    // Thus, the space complexity reduces from O(n^2) to O(num of live cells).
    static std::unordered_set<sf::Vector2i, pair_hash, pair_equal> grid;
    const int grid_width, grid_height;

    const sf::Color important_color;
    static sf::Clock code_timer; // For logging
    sf::Font font;

    static void setInitialView();
    static void centerText(sf::Text& text, int y);
    static void resize(const sf::Event& evnt, int height = INT_MAX);

    // For logging
    static void printLogTime(bool reset);
    static long long int getLogTime(bool reset);

public:
    BaseScreen();
    virtual short int run() = 0;
};

#endif