#ifndef GAME_OF_LIFE_AUTOMATON_MENU_SCREEN_H
#define GAME_OF_LIFE_AUTOMATON_MENU_SCREEN_H

#include <string>
#include <unordered_map>
#include "menu_screen.h"

class AutomatonMenuScreen: public MenuScreen{
private:
    std::string automaton_name;
    std::vector <std::string> num_to_automaton_name;

    // These maps save, for each automaton, the 'born' and 'survive' group.
    // We use set and not unordered_set, because amount of elements is very small, and not changing
    std::unordered_map < std::string, std::set<short int> > map_born;
    std::unordered_map < std::string, std::set<short int> > map_survive;

    static std::set <short int> handleBornSurviveInput(short int start);

public:
    AutomatonMenuScreen();
    short int run() override;
};

#endif
