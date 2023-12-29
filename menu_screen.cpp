#include "screens.h"

/* For a given font and baseline (character size),
the function finds the maximum height among the glyphs of standard ASCII (0-127),
*in the global 2D world coordinates*.

Basically, because SFML is giving me a hard time, we need to iterate over all the glyphs in order to find the maximum height of a character;
so that we can give this height to the rectangle that contains each menu option.
It's a bit complicated, best to read in this thread:
https://en.sfml-dev.org/forums/index.php?topic=29332.0 */
float MenuScreen::getTopAndBottomOfGlyphs(){
    float max_height = 0;
    for (int i = 0; i < 128; i++){
        sf::Glyph glyph = font.getGlyph(i, CHARACTER_SIZE, false);
        sf::FloatRect glyph_bounds = glyph.bounds; // The bounds are relative to the baseline, so we transform them below

        float curr_top = glyph_bounds.top + CHARACTER_SIZE;
        max_height = std::max(max_height, curr_top + glyph_bounds.height);
    }

    return max_height;
}

MenuScreen::MenuScreen(): option_chosen_color(sf::Color::Red), option_not_chosen_color(sf::Color::White){
    // DISTANCE here is like vertical padding inside the rectangle
    menu_option_rectangle_height = getTopAndBottomOfGlyphs() + DISTANCE;
}

// Making sure to truncate (if needed) filenames, and setting the Text objects in a list in the middle of the screen
void MenuScreen::setText(){
    float height = 0;
    for (auto& menu_option : menu_options){
        float x = window_width / 2 - menu_option.getGlobalBounds().width / 2; // Putting string exactly at center of screen
        menu_option.setPosition(x, height);
        height += menu_option_rectangle_height;
    }
}

// Only drawing the viewed menu options
void MenuScreen::drawText(){
    int first_menu_option_index = left_top_view_pos.y / menu_option_rectangle_height - 1;
    int last_menu_option_index = (left_top_view_pos.y + window_height) / menu_option_rectangle_height + 1;
    for (int i = first_menu_option_index; i < last_menu_option_index; i++){
        // Since we add 1 or subtract 1 above, there's a chance we got out of range,
        // and it's easier to ask for forgiveness here than to ask for permission :-)
        try {
            window.draw(menu_options.at(i));
        } catch (std::out_of_range& exc) {}
    }
}