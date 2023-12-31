#ifndef GAME_OF_LIFE_SAVE_SCREEN_H
#define GAME_OF_LIFE_SAVE_SCREEN_H

#include "screens.h"

class SaveGridScreen: public GridScreen{
private:
    sf::Text save_prompt;
    const sf::Color live_cell_diff, dead_cell_diff, outline_diff; // The color diffs enable us to dim the screen

    static std::string gridToRLE();
    void dimOrBrightenScreen() const;

public:
    SaveGridScreen();
    short int run() override;
};

#endif
