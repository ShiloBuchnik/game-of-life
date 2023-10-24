#ifndef GAME_OF_LIFE_PATTERNS_H
#define GAME_OF_LIFE_PATTERNS_H
#include <SFML/Graphics.hpp>

namespace Patterns{
    namespace{
        extern int grids_count_x, grids_count_y;

        sf::Vector2i centerOfMass(const std::vector<sf::Vector2i>& points);
    }

    namespace StillLifes{
        extern std::vector<sf::Vector2i> block;
        extern std::vector<sf::Vector2i> bee_hive;
        extern std::vector<sf::Vector2i> honey_farm;
        extern std::vector<sf::Vector2i> loaf;
        extern std::vector<sf::Vector2i> bakery;
        extern std::vector<sf::Vector2i> boat;
        extern std::vector<sf::Vector2i> ship;
        extern std::vector<sf::Vector2i> tub;
    }

    namespace Oscillators{
        extern std::vector<sf::Vector2i> blinker;
        extern std::vector<sf::Vector2i> toad;
        extern std::vector<sf::Vector2i> beacon;
        extern std::vector<sf::Vector2i> pulsar;
        extern std::vector<sf::Vector2i> penta_decathlon;
        extern std::vector<sf::Vector2i> cross;
        extern std::vector<sf::Vector2i> star;
        extern std::vector<sf::Vector2i> clock;
        extern std::vector<sf::Vector2i> koks_galaxy;
        extern std::vector<sf::Vector2i> octagon;
        extern std::vector<sf::Vector2i> french_kiss;
        extern std::vector<sf::Vector2i> fumarole;
        extern std::vector<sf::Vector2i> pentoad;
    }

    namespace Spaceships{
        extern std::vector<sf::Vector2i> glider;
        extern std::vector<sf::Vector2i> light_weight_spaceship;
        extern std::vector<sf::Vector2i> middle_weight_spaceship;
        extern std::vector<sf::Vector2i> heavy_weight_spaceship;
    }

    namespace Guns{
        extern std::vector<sf::Vector2i> gosper_glider_gun;
        extern std::vector<sf::Vector2i> simkin_glider_gun;
    }

    extern std::vector < std::vector<sf::Vector2i>* > numToPattern;

    // Takes a grid and a vector of points (each point is a grid coordinate), and color fill those grids
    void putPatternInGrid(std::vector< std::vector<sf::RectangleShape> >& grid, const std::vector<sf::Vector2i>& points);
}

#endif
