#ifndef GAME_OF_LIFE_RULESTRING_SCREEN_H
#define GAME_OF_LIFE_RULESTRING_SCREEN_H

#include "screens.h"

class RulestringMenuScreen: public BaseScreen{
private:
    const std::string initial_born_prompt_str;
    sf::Text born_prompt;
    const std::string initial_survive_prompt_str;
    sf::Text survive_prompt;

public:
    RulestringMenuScreen();
    short int run() override;
};

#endif