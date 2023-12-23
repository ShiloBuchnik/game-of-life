#ifndef GAME_OF_LIFE_SCREENS_SETTINGS_H
#define GAME_OF_LIFE_SCREENS_SETTINGS_H

#include <SFML/Graphics.hpp>
#include <set>
#include <unordered_set>
#include "globals.h"

class BaseGlobalSettings{
private:
    int window_height, window_width;
    sf::RenderWindow window;

    std::set<short int> born_digits;
    std::set<short int> survive_digits;

    // We implement the grid using a sparse matrix - which is just a set that stores only the *live cells*.
    // Thus, the space complexity reduces from O(n^2) to O(num of live cells).
    std::unordered_set<sf::Vector2i, pair_hash, pair_equal> grid;

    BaseGlobalSettings(); // Private c'tor so the user can't create an object themselves

public:
    BaseGlobalSettings(const BaseGlobalSettings& bs) = delete;
    BaseGlobalSettings& operator = (const BaseGlobalSettings& bs) = delete;

    static BaseGlobalSettings& getInstance(){
        static BaseGlobalSettings instance; // Instantiated only on first use

        return instance;
    }

    friend class BaseScreen;
    friend class GridScreen;
    friend class GridGlobalSettings;
};

class MenuGlobalSettings{
private:
    // TODO fit in view here. Think if you need left_top_view, or just the view object.

public:

    friend class MenuScreen;
};

class GridGlobalSettings{
private:
    // TODO: give also menu class its own view (both menus share it).
    // left-top coordinate of the view rectangle. We keep track of it, so we can restrict the user from moving the view out of bounds.
    sf::Vector2i left_top_view_pos;
    // 1st and 2nd arguments are left-top coordinate of the rectangle. 3rd and 4th arguments are its width and height respectively.
    // Our initial view is exactly at the middle of the grid.
    sf::View view;

    GridGlobalSettings();

public:
    GridGlobalSettings(const GridGlobalSettings& bs) = delete;
    GridGlobalSettings& operator = (const GridGlobalSettings& bs) = delete;

    static GridGlobalSettings& getInstance(){
        static GridGlobalSettings instance;

        return instance;
    }

    friend class GridScreen;
};

#endif