#ifndef GAME_OF_LIFE_MENU_SCREEN_H
#define GAME_OF_LIFE_MENU_SCREEN_H

#include "screens.h"

#define DISTANCE 5 // Distance between menu options, but also used as arbitrary number for when we don't care about pixel perfect

class MenuScreen: public BaseScreen{
protected:
    // TODO: maybe give up on defining two menu classes, and just create two menu objects?
    // Depending on the concrete sub class, saves filename itself or automaton name
    std::vector <sf::Text> menu_options;
    float menu_option_rectangle_height;

    const sf::Color option_chosen_color;
    const sf::Color option_not_chosen_color;

    float getTopAndBottomOfGlyphs();
    void setText();
    void drawText();

public:
    MenuScreen();

};

#endif
