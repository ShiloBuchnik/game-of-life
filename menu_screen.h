#ifndef GAME_OF_LIFE_MENU_SCREEN_H
#define GAME_OF_LIFE_MENU_SCREEN_H

#include "screens.h"

#define DISTANCE 5 // Distance between menu options, but also used as arbitrary number for when we don't care about pixel perfect

class MenuScreen: public BaseScreen{
protected:
    // Depending on the concrete sub class, saves filename itself or automaton name
    std::vector <sf::Text> menu_options;
    sf::Text menu_title;
    const float menu_option_rectangle_height;
    const float menu_title_rectangle_height; // Take this height from top of window - the options start from here
    float menu_screen_total_height;

    const sf::Color option_chosen_color;
    const sf::Color option_not_chosen_color;

    sf::Texture arrow_up_texture, arrow_down_texture; // Must not be destroyed throughout the entire use of the sprites, for some reason
    sf::Sprite arrow_up_sprite, arrow_down_sprite;

    static float getTopAndBottomOfGlyphs(int baseline);
    void handleHover(bool& hovering, int& rectangle_index, std::vector <std::string>* vec, sf::Text*& hovered_menu_option,
                     sf::FloatRect& hovered_rectangle_bounds);
    void handleScroll(short int delta);
    void setText();
    void drawText() const;
    void setArrows();
    void drawArrows() const;
    static std::string textToString(const sf::Text* text);

public:
    MenuScreen();
};

#endif
