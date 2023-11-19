#ifndef GAME_OF_LIFE_GLOBALS_H
#define GAME_OF_LIFE_GLOBALS_H
#include <SFML/Graphics.hpp>
#include <iostream>

#define WINDOW_FRACTION 3/4.0
#define MULTIPLE 101 // The grid is 'MULTIPLE' times bigger than the initial window size, so that it looks like an "infinite" grid.
#define GRID_WIDTH 1440 * MULTIPLE
#define GRID_HEIGHT 810 * MULTIPLE
#define CELL_SIZE 30 // Window width and height must divide cell size
#define DEAD_CELL_COLOR sf::Color(128, 128, 128) // Grey
#define LIVE_CELL_COLOR sf::Color::Red
#define SPEED 0.3

/* We need to define a custom hash and equal functors for pair type.
 Defining a hash is not enough, since hash functions can have collisions.
 Side note: the overloaded '()' is templated, but that's okay, because C++ has type inference */
class pair_hash {
public:
    std::size_t operator() (const sf::Vector2i &pair) const {
        std::hash<int> hash_obj;
        return ~hash_obj(pair.x) ^ hash_obj(pair.y);
    }
};
class pair_equal {
public:
    bool operator() (const sf::Vector2i &pair1, const sf::Vector2i &pair2) const {
        return pair1.x == pair2.x && pair1.y == pair2.y;
    }
};
class pair_less {
public:
    // We define 'pair1' to be less than 'pair2' if the former is above the latter;
    // or if they're in the same line, and the former is to the left of the latter.
    bool operator() (const sf::Vector2i &pair1, const sf::Vector2i &pair2) const {
        if (pair1.y < pair2.y) return true;
        else if (pair1.y == pair2.y) return pair1.x < pair2.x;

        return false;
    }
};

#endif
