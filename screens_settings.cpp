#include "screens_settings.h"

BaseGlobalSettings::BaseGlobalSettings():
        window_width(sf::VideoMode::getDesktopMode().width * WINDOW_FRACTION),
        window_height(sf::VideoMode::getDesktopMode().height * WINDOW_FRACTION),
        window(sf::VideoMode(window_width, window_height), "Game of life", sf::Style::Default) { }

GridGlobalSettings::GridGlobalSettings(){
    BaseGlobalSettings& base_settings = BaseGlobalSettings::getInstance();
    left_top_view_pos = sf::Vector2i(base_settings.window_width * (MULTIPLE / 2), base_settings.window_height * (MULTIPLE / 2));
    view = sf::View(sf::FloatRect(left_top_view_pos.x, left_top_view_pos.y, base_settings.window_width, base_settings.window_height));

    // Since our initial view is different from the default view, we have to set it before we continue.
    base_settings.window.setView(view);
}