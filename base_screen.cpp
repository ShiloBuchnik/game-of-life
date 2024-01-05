#include <iostream>
#include "screens.h"

//int BaseScreen::window_width = sf::VideoMode::getDesktopMode().width * WINDOW_FRACTION;
//int BaseScreen::window_height = sf::VideoMode::getDesktopMode().height * WINDOW_FRACTION;
sf::RenderWindow BaseScreen::window(sf::VideoMode(sf::VideoMode::getDesktopMode().width * WINDOW_FRACTION, sf::VideoMode::getDesktopMode().height * WINDOW_FRACTION), "Game of life", sf::Style::Default);
sf::Cursor BaseScreen::cursor;

sf::Vector2f BaseScreen::left_top_view_pos(0.f,0.f);
// 1st and 2nd arguments are left-top coordinate of the rectangle. 3rd and 4th arguments are its width and height respectively.
sf::View BaseScreen::view(sf::FloatRect(0, 0, window.getSize().x, window.getSize().y));

std::set<short int> BaseScreen::born_digits;
std::set<short int> BaseScreen::survive_digits;
std::unordered_set<sf::Vector2i, pair_hash, pair_equal> BaseScreen::grid;

sf::Clock BaseScreen::code_timer;

// Grid's width and height are set to a huge number, determined by MULTIPLE
BaseScreen::BaseScreen(): important_color(sf::Color::Magenta), grid_width(window.getSize().x * MULTIPLE), grid_height(window.getSize().y * MULTIPLE) {
    if (!font.loadFromFile("resources/arial.ttf")) exit(-1);
}

// For grid: our initial view is exactly at the middle of the grid
// For menu: our initial view is in the top-left corner of the window (0,0)
// We set that setting 'left_top_view_pos' before calling to this method
void BaseScreen::setInitialView(){
    view = sf::View(sf::FloatRect(left_top_view_pos.x, left_top_view_pos.y, window.getSize().x, window.getSize().y));
    // Since our initial view is different from the default view (which is just {0,0,1000,1000}), we have to set it initially
    window.setView(view);
}

void BaseScreen::centerText(sf::Text& text, int y){
    // 'left_top_view_pos.x' is in the sum because we need to adjust for current view
    float x = left_top_view_pos.x + view.getSize().x / 2 - text.getGlobalBounds().width / 2; // Putting string exactly at center of screen
    text.setPosition(x, y);
}

// We pass 'height' only if we don't want to go out of its logical bounds when resizing.
// This method is for menu screens and that one rulestring screen.
// The GridScreen subclass overloads it with different logic (having one resize method for all screens is just too much coupling to handle)
void BaseScreen::resize(const sf::Event& evnt, int height){
    float new_left_top = height - view.getSize().y;
    // When 'height' is smaller than view's height (like it can be with menus), then the expression is negative,
    // and we want to forbid 'left_top_view_pos.y' from being negative.
    if (new_left_top < 0) new_left_top = 0;

    left_top_view_pos.y = std::min(left_top_view_pos.y, new_left_top);
    view.reset(sf::FloatRect(left_top_view_pos.x, left_top_view_pos.y, window.getSize().x, window.getSize().y));
    window.setView(view);
}

// For logging. when called with 'reset=true', it resets and starts counting.
// When called with 'reset=false', it prints time passed (in ms) since *reset*.
// If we call without ever resetting, it counts since construction of the clock object.
void BaseScreen::printLogTime(bool reset){
    if (reset) code_timer.restart();
    else std::cout << code_timer.getElapsedTime().asMicroseconds() << std::endl;
}

// Same as above, only it returns the elapsed time instead of printing it.
long long int BaseScreen::getLogTime(bool reset){
    if (reset){
        code_timer.restart();
        return 0;
    }
    else return code_timer.getElapsedTime().asMicroseconds();
}