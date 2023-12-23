#ifndef GAME_OF_LIFE_GAME_SCREEN_H
#define GAME_OF_LIFE_GAME_SCREEN_H

#include <SFML/Graphics.hpp>
#include "grid_screen.h"

class GameScreen: public GridScreen{
private:
    short int timestep;
    sf::Clock timestep_clock;
    unsigned long long int gen;

public:
    GameScreen();
    short int run() override;
};

#endif
