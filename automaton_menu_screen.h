#ifndef GAME_OF_LIFE_AUTOMATON_MENU_SCREEN_H
#define GAME_OF_LIFE_AUTOMATON_MENU_SCREEN_H

#include <string>
#include <unordered_map>
#include "screens.h"

class AutomatonMenuScreen: public MenuScreen{
private:
    // this maps saves, for each automaton, the 'born' (1st in pair) and 'survive' (2nd in pair) sets.
    // We use set and not unordered_set, because amount of elements is very small, and not changing
    const std::unordered_map < std::string, std::pair< std::set<short int>, std::set<short int> > > automaton_name_to_born_and_survive_sets;

public:
    AutomatonMenuScreen();
    short int run() override;
};

#endif