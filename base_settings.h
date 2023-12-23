#ifndef GAME_OF_LIFE_BASE_SETTINGS_H
#define GAME_OF_LIFE_BASE_SETTINGS_H

#include <SFML/Graphics.hpp>
#include <set>
#include <unordered_set>
#include "globals.h"

struct BaseSettings{
    int window_height, window_width;
    sf::RenderWindow window;

    std::set<short int> born_digits;
    std::set<short int> survive_digits;

    // We implement the grid using a sparse matrix - which is just a set that stores only the *live cells*.
    // Thus, the space complexity reduces from O(n^2) to O(num of live cells).
    std::unordered_set<sf::Vector2i, pair_hash, pair_equal> grid;
};

#endif
