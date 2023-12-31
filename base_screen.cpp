#include "screens.h"

int BaseScreen::window_width = sf::VideoMode::getDesktopMode().width * WINDOW_FRACTION;
int BaseScreen::window_height = sf::VideoMode::getDesktopMode().height * WINDOW_FRACTION;
sf::RenderWindow BaseScreen::window(sf::VideoMode(BaseScreen::window_width, BaseScreen::window_height), "Game of life", sf::Style::Default);
sf::Cursor BaseScreen::cursor;

sf::Vector2i BaseScreen::left_top_view_pos(0,0);
// 1st and 2nd arguments are left-top coordinate of the rectangle. 3rd and 4th arguments are its width and height respectively.
sf::View BaseScreen::view(sf::FloatRect(0, 0, window_width, window_height));

std::set<short int> BaseScreen::born_digits;
std::set<short int> BaseScreen::survive_digits;
std::unordered_set<sf::Vector2i, pair_hash, pair_equal> BaseScreen::grid;

sf::Clock BaseScreen::code_timer;

// Grid's width and height are set to a huge number, determined by MULTIPLE
BaseScreen::BaseScreen(): important_color(sf::Color::Magenta), grid_width(window_width * MULTIPLE), grid_height(window_height * MULTIPLE) {
    if (!font.loadFromFile("resources/arial.ttf")) exit(-1);
}

// We pass height and width only if we don't want to go out of their logical bounds when resizing
void BaseScreen::resize(const sf::Event& evnt, int height, int width){
    /* By default, when resizing, everything is squeezed/stretched to the new size.
    What we want to do is to *keep the top-left corner the same*, and simply extending/reducing the width or height from right or down,
    just like in a windowed video game. We pass that corner with the new width and height to the 'reset()' method.

    If we'd only change the width and height, without keeping the top-left corner the same,
    then it would change everytime we'd resize.

    We perform bound checking below, as to not show beyond the grid's bound, when resizing close to the edge. */
    window_width = evnt.size.width;
    window_height = evnt.size.height;
    // When width is smaller than window_width, then left_top_view_pos.x can become negative (same with height)
    // We don't want to allow it, hence the absolute value
    left_top_view_pos.x = std::min(left_top_view_pos.x, abs(width - window_width));
    left_top_view_pos.y = std::min(left_top_view_pos.y, abs(height - window_height));

    view.reset(sf::FloatRect(left_top_view_pos.x, left_top_view_pos.y, window_width, window_height));
    window.setView(view);
}

// For grid: our initial view is exactly at the middle of the grid
// For menu: our initial view is in the top-left corner of the window
// We set that setting 'left_top_view_pos' before calling to this method
void BaseScreen::setInitialView(){
    view = sf::View(sf::FloatRect(left_top_view_pos.x, left_top_view_pos.y, window_width, window_height));
    // Since our initial view is different from the default view (which is just {0,0,1000,1000}), we have to set it initially
    window.setView(view);
}

void BaseScreen::centerText(sf::Text& text, float y){
    // 'left_top_view_pos.x' is in the sum because we need to adjust for current view
    float x = left_top_view_pos.x + window_width / 2 - text.getGlobalBounds().width / 2; // Putting string exactly at center of screen
    text.setPosition(x, y);
}

// For logging. when called with 'reset=true', it resets and starts counting.
// When called with 'reset=false', it prints time passed (in ms) since *reset*.
// If we call without ever resetting, it counts since construction of the clock object.
void BaseScreen::printLogTime(bool reset){
    if (reset) code_timer.restart();
    else std::cout << code_timer.getElapsedTime().asMilliseconds() << std::endl;
}

// Same as above, only it returns the elapsed time instead of printing it.
long int BaseScreen::getLogTime(bool reset){
    if (reset){
        code_timer.restart();
        return 0;
    }
    else return code_timer.getElapsedTime().asMilliseconds();
}