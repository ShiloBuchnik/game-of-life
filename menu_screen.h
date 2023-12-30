#ifndef GAME_OF_LIFE_MENU_SCREEN_H
#define GAME_OF_LIFE_MENU_SCREEN_H

#include "screens.h"

#define DISTANCE 5 // Distance between menu options, but also used as arbitrary number for when we don't care about pixel perfect

class MenuScreen: public BaseScreen{
protected:
    // Depending on the concrete sub class, saves filename itself or automaton name
    std::vector <sf::Text> menu_options;
    const float menu_option_rectangle_height;
    float menu_screen_total_height;

    const sf::Color option_chosen_color;
    const sf::Color option_not_chosen_color;

    float getTopAndBottomOfGlyphs() const;
    void handleHover(bool& hovering, int& rectangle_index, std::vector <std::string>* vec, sf::Text*& hovered_menu_option,
                     sf::FloatRect& hovered_rectangle_bounds);
    void handleScroll(short int delta) const;
    void setText();
    void drawText() const;
    static std::string textToString(const sf::Text* text);

public:
    MenuScreen();
};

#endif
