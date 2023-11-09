#include <SFML/Graphics.hpp>
#include "game_logic.h"

namespace Patterns{
    // Anonymous namespace - like a static function, its visibility is limited to current translation unit
    namespace{
        int cells_count_x = GRID_WIDTH / CELL_SIZE, cells_count_y = GRID_HEIGHT / CELL_SIZE;;

        sf::Vector2i centerOfMass(const std::vector<sf::Vector2i>& points){
            int sum_x = 0, sum_y = 0, points_size = points.size();

            for (auto point : points){
                sum_x += point.x;
                sum_y += point.y;
            }

            return {sum_x / points_size, sum_y / points_size};
        }
    }

    namespace StillLifes{
        std::vector<sf::Vector2i> block = {{0,0}, {0,1}, {1,0}, {1,1}};
        std::vector<sf::Vector2i> bee_hive = {{1,0}, {2,0}, {0,1}, {3,1}, {1,2}, {2,2}};
        std::vector<sf::Vector2i> honey_farm = {{0,6}, {1,7}, {2,7}, {1,5}, {2,5}, {3,6}, {9,6}, {10,5}, {11,5}, {12,6}, {11,7}, {10,7},
                                                {6,3}, {5,2}, {5,1}, {6,0}, {7,1}, {7,2}, {6,9}, {5,10}, {5,11}, {6,12}, {7,11}, {7,10}};
        std::vector<sf::Vector2i> loaf = {{1,0}, {2,0}, {0,1}, {1,2}, {2,3}, {3,2}, {3,1}};
        std::vector<sf::Vector2i> bakery = {{0,4}, {4,0}, {0,5}, {5,0}, {6,1}, {5,2}, {4,3}, {3,4}, {2,5}, {1,6}, {1,3}, {2,3}, {3,1},
                                            {3,2}, {8,3}, {7,4}, {6,5}, {5,6}, {4,7}, {3,8}, {4,9}, {5,9}, {9,4}, {9,5}, {8,6}, {7,6},
                                            {6,8}, {6,7}};
        std::vector<sf::Vector2i> boat = {{0,0}, {1,0}, {0,1}, {1,2}, {2,1}};
        std::vector<sf::Vector2i> ship = {{0,0}, {0,1}, {1,0}, {1,2}, {2,2}, {2,1}};
        std::vector<sf::Vector2i> tub = {{1,0}, {0,1}, {1,2}, {2,1}};
    }

    namespace Oscillators{
        std::vector<sf::Vector2i> blinker = {{0,0}, {0,1}, {0,2}};
        std::vector<sf::Vector2i> toad = {{0,1}, {1,1}, {2,1}, {1,0}, {2,0}, {3,0}};
        std::vector<sf::Vector2i> beacon = {{0,0}, {0,1}, {1,1}, {1,0}, {2,2}, {3,2}, {3,3}, {2,3}};
        std::vector<sf::Vector2i> pulsar = {{0,2}, {0,3}, {0,4}, {2,0}, {3,0}, {4,0}, {5,2}, {5,3}, {5,4}, {4,5}, {3,5}, {2,5}, {7,4}, {7,3},
                                            {7,2}, {8,0}, {9,0}, {10,0}, {8,5}, {9,5}, {10,5}, {12,4}, {12,3}, {12,2}, {8,7}, {9,7}, {10,7},
                                            {7,8}, {7,9}, {7,10}, {8,12}, {9,12}, {10,12}, {12,8}, {12,9}, {12,10}, {4,7}, {3,7}, {2,7},
                                            {5,8}, {5,9}, {5,10}, {0,8}, {0,9}, {0,10}, {4,12}, {3,12}, {2,12}};
        std::vector<sf::Vector2i> penta_decathlon = {{0,1}, {0,2}, {1,0}, {2,0}, {3,0}, {4,1}, {4,2}, {3,3}, {2,3}, {1,3}, {1,8}, {2,8},
                                                     {3,8}, {4,9}, {4,10}, {3,11}, {2,11}, {1,11}, {0,10}, {0,9}};
        std::vector<sf::Vector2i> cross = {{0,2}, {1,2}, {2,2}, {2,1}, {2,0}, {3,0}, {4,0}, {5,0}, {5,1}, {5,2}, {6,2}, {7,2}, {7,3}, {7,4},
                                           {7,5}, {6,5}, {5,5}, {5,6}, {5,7}, {4,7}, {3,7}, {2,7}, {2,6}, {2,5}, {1,5}, {0,5}, {0,4}, {0,3}};
        std::vector<sf::Vector2i> star = {{0,5}, {5,0}, {1,6}, {1,5}, {1,4}, {2,4}, {2,3}, {2,2}, {3,2}, {4,2}, {4,1}, {5,1}, {6,1}, {6,2},
                                          {7,2}, {8,2}, {8,3}, {8,4}, {9,4}, {9,5}, {9,6}, {10,5}, {8,6}, {8,7}, {8,8}, {7,8}, {6,8}, {6,9},
                                          {5,9}, {4,9}, {5,10}, {4,8}, {3,8}, {2,8}, {2,7}, {2,6}};
        std::vector<sf::Vector2i> clock = {{0,4}, {1,4}, {1,5}, {0,5}, {6,0}, {7,0}, {7,1}, {6,1}, {7,3}, {6,3}, {5,3}, {4,3}, {3,4}, {3,5},
                                           {3,6}, {3,7}, {4,8}, {5,8}, {6,8}, {7,8}, {8,7}, {8,6}, {8,5}, {8,4}, {5,6}, {5,5}, {6,4}, {4,10},
                                           {4,11}, {5,11}, {5,10}, {10,7}, {10,6}, {11,6}, {11,7}};
        std::vector<sf::Vector2i> koks_galaxy = {{0,0}, {0,1}, {0,2}, {0,3}, {0,4}, {0,5}, {1,5}, {1,4}, {1,3}, {1,2}, {1,1}, {1,0}, {3,0},
                                                 {4,0}, {5,0}, {6,0}, {7,0}, {8,0}, {8,1}, {7,1}, {6,1}, {5,1}, {4,1}, {3,1}, {0,7}, {1,7},
                                                 {2,7}, {3,7}, {4,7}, {5,7}, {5,8}, {4,8}, {3,8}, {2,8}, {1,8}, {0,8}, {7,8}, {7,7}, {8,7},
                                                 {8,8}, {7,6}, {7,5}, {7,4}, {7,3}, {8,3}, {8,4}, {8,5}, {8,6}};
        std::vector<sf::Vector2i> octagon = {{0,3}, {3,0}, {4,0}, {0,4}, {1,5}, {2,6}, {3,7}, {4,7}, {5,6}, {6,5}, {7,4}, {7,3}, {6,2},
                                             {5,1}, {2,1}, {1,2}};
        std::vector<sf::Vector2i> french_kiss = {{0,9}, {1,9}, {1,8}, {1,7}, {2,6}, {3,7}, {4,7}, {5,6}, {5,5}, {3,4}, {3,3}, {4,2}, {5,2},
                                                 {6,3}, {7,2}, {7,1}, {7,0}, {8,0}};
        std::vector<sf::Vector2i> fumarole = {{0,5}, {0,6}, {1,6}, {2,5}, {2,4}, {1,3}, {1,2}, {2,2}, {3,1}, {4,1}, {5,2}, {6,2}, {6,3},
                                              {5,4}, {5,5}, {6,6}, {7,6}, {7,5}};
        std::vector<sf::Vector2i> pentoad = {{0,11}, {1,11}, {1,10}, {1,9}, {2,8}, {3,8}, {3,9}, {6,7}, {7,7}, {6,6}, {6,5}, {6,4}, {5,4},
                                             {9,3}, {9,2}, {10,3}, {11,2}, {11,1}, {11,0}, {12,0}};
    }

    namespace Spaceships{
        std::vector<sf::Vector2i> glider = {{0,0}, {2,0}, {1,2}, {1,1}, {2,1}};
        std::vector<sf::Vector2i> light_weight_spaceship = {{0,1}, {1,0}, {2,0}, {3,0}, {4,0}, {4,1}, {4,2}, {3,3}, {0,3}};
        std::vector<sf::Vector2i> middle_weight_spaceship = {{0,1}, {0,3}, {1,4}, {2,4}, {3,4}, {4,4}, {5,4}, {5,3}, {5,2}, {4,1}, {2,0}};
        std::vector<sf::Vector2i> heavy_weight_spaceship = {{0,1}, {0,3}, {2,0}, {3,0}, {5,1}, {6,2}, {6,3}, {6,4}, {5,4}, {4,4}, {3,4},
                                                            {2,4}, {1,4}};
    }

    namespace Guns{
        std::vector<sf::Vector2i> gosper_glider_gun = {{0,4}, {1,4}, {1,5}, {0,5}, {10,4}, {10,5}, {10,6}, {11,3}, {12,2}, {13,2}, {11,7},
                                                       {12,8}, {13,8}, {14,5}, {15,3}, {15,7}, {16,6}, {16,5}, {16,4}, {17,5}, {22,5},
                                                       {21,4}, {20,4}, {20,3}, {21,3}, {21,2}, {20,2}, {22,1}, {24,1}, {24,0}, {24,5},
                                                       {24,6}, {34,2}, {35,2}, {35,3}, {34,3}};
        std::vector<sf::Vector2i> simkin_glider_gun = {{0,0}, {1,0}, {1,1}, {0,1}, {7,0}, {8,0}, {8,1}, {7,1}, {5,3}, {4,3}, {4,4}, {5,4},
                                                       {21,10}, {22,9}, {23,9}, {21,11}, {21,12}, {22,12}, {23,12}, {25,9}, {26,9}, {27,10},
                                                       {28,11}, {27,12}, {26,13}, {31,11}, {32,11}, {32,12}, {31,12}, {21,17}, {20,17},
                                                       {20,18}, {21,19}, {22,19}, {23,19}, {23,20}};
    }

    // Maps the numbers in [2-28] to their respective variable in the namespace
    std::vector < std::vector<sf::Vector2i>* > numToPattern =
        {nullptr, nullptr, &StillLifes::block, &StillLifes::bee_hive, &StillLifes::honey_farm, &StillLifes::loaf, &StillLifes::bakery,
         &StillLifes::boat, &StillLifes::ship, &StillLifes::tub, &Oscillators::blinker, &Oscillators::toad, &Oscillators::beacon,
         &Oscillators::pulsar, &Oscillators::penta_decathlon, &Oscillators::cross, &Oscillators::star, &Oscillators::clock,
         &Oscillators:: koks_galaxy, &Oscillators::octagon, &Oscillators::french_kiss, &Oscillators::fumarole, &Oscillators::pentoad,
        &Spaceships::glider, &Spaceships::light_weight_spaceship, &Spaceships::middle_weight_spaceship, &Spaceships::heavy_weight_spaceship,
        &Guns::gosper_glider_gun, &Guns::simkin_glider_gun};

    void putPatternInGrid(std::vector< std::vector<sf::RectangleShape> >& grid, const std::vector<sf::Vector2i>& points){
        sf::Vector2i center_of_mass = centerOfMass(points);

        for (auto point : points){
            int x = point.x + cells_count_x / 2 - center_of_mass.x;
            int y = point.y + cells_count_y / 2 - center_of_mass.y;
            grid[y][x].setFillColor(LIVE_CELL_COLOR);
        }
    }
}