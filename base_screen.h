#ifndef GAME_OF_LIFE_BASE_SCREEN_H
#define GAME_OF_LIFE_BASE_SCREEN_H

#include <SFML/Graphics.hpp>
#include <unordered_set>
#include <set>
#include "globals.h"

class BaseScreen{
protected:
    static int window_height, window_width;
    static sf::RenderWindow window;

    static std::set<short int> born_digits;
    static std::set<short int> survive_digits;

    // We implement the grid using a sparse matrix - which is just a set that stores only the *live cells*.
    // Thus, the space complexity reduces from O(n^2) to O(num of live cells).
    static std::unordered_set<sf::Vector2i, pair_hash, pair_equal> grid;

public:
    virtual short int run() = 0;
};

#endif
