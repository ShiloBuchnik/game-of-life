#include "grid_screen.h"

GridGlobalSettings& GridScreen::grid_settings = GridGlobalSettings::getInstance();

void GridScreen::checkForChangeViewWithKeys(const int& delta_time){
    // We're not allowing the user to move the view beyond the grid's bounds.
    int delta_pos = std::round(SPEED * delta_time); // It's in absolute value, we'll add the sign later
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) && 0 <= grid_settings.left_top_view_pos.y - delta_pos){
        grid_settings.left_top_view_pos.y -= delta_pos;
        grid_settings.view.move(sf::Vector2f(0, -1 * delta_pos));
        base_settings.window.setView(grid_settings.view);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) && 0 <= grid_settings.left_top_view_pos.x - delta_pos){
        grid_settings.left_top_view_pos.x -= delta_pos;
        grid_settings.view.move(sf::Vector2f(-1 * delta_pos, 0));
        base_settings.window.setView(grid_settings.view);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) && grid_settings.left_top_view_pos.y + base_settings.window_height + delta_pos <= GRID_HEIGHT){
        grid_settings.left_top_view_pos.y += delta_pos;
        grid_settings.view.move(sf::Vector2f(0, delta_pos));
        base_settings.window.setView(grid_settings.view);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) && grid_settings.left_top_view_pos.x + base_settings.window_width + delta_pos <= GRID_WIDTH){
        grid_settings.left_top_view_pos.x += delta_pos;
        grid_settings.view.move(sf::Vector2f(delta_pos, 0));
        base_settings.window.setView(grid_settings.view);
    }
}

GridScreen::GridScreen(){
    focus = true;
    clicking = false;
}

