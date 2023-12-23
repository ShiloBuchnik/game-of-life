#include "base_screen.h"

int BaseScreen::window_height = sf::VideoMode::getDesktopMode().height * WINDOW_FRACTION;
int BaseScreen::window_width = sf::VideoMode::getDesktopMode().width * WINDOW_FRACTION;
sf::RenderWindow BaseScreen::window(sf::VideoMode(BaseScreen::window_width, BaseScreen::window_height), "Game of life", sf::Style::Default);
std::set<short int> BaseScreen::born_digits;
std::set<short int> BaseScreen::survive_digits;
std::unordered_set<sf::Vector2i, pair_hash, pair_equal> BaseScreen::grid;