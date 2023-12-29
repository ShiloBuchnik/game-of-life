#ifndef GAME_OF_LIFE_GAME_SCREEN_H
#define GAME_OF_LIFE_GAME_SCREEN_H

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include "screens.h"

class GameScreen: public GridScreen{
private:
    short int timestep;
    int reserved_size; // Stores previous size of the map allocated in 'updateGrid', so that we can 'reserve()' in the next call

    sf::Text gen_text;

    void addNeighbors(std::unordered_map<sf::Vector2i, short int, pair_hash, pair_equal>& m);
    static void applyRules(const std::unordered_map<sf::Vector2i, short int, pair_hash, pair_equal>& m);
    void updateGrid();

public:
    GameScreen();
    short int run() override;

};

#endif
