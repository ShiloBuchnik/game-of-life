#include "grid_screen.h"

sf::Vector2i GridScreen::left_top_view_pos;
sf::View GridScreen::view;

void GridScreen::checkForChangeViewWithKeys(const int& delta_time){
    // We're not allowing the user to move the view beyond the grid's bounds.
    int delta_pos = std::round(SPEED * delta_time); // It's in absolute value, we'll add the sign later
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) && 0 <= left_top_view_pos.y - delta_pos){
        left_top_view_pos.y -= delta_pos;
        view.move(sf::Vector2f(0, -1 * delta_pos));
        window.setView(view);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) && 0 <= left_top_view_pos.x - delta_pos){
        left_top_view_pos.x -= delta_pos;
        view.move(sf::Vector2f(-1 * delta_pos, 0));
        window.setView(view);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) && left_top_view_pos.y + window_height + delta_pos <= GRID_HEIGHT){
        left_top_view_pos.y += delta_pos;
        view.move(sf::Vector2f(0, delta_pos));
        window.setView(view);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) && left_top_view_pos.x + window_width + delta_pos <= GRID_WIDTH){
        left_top_view_pos.x += delta_pos;
        view.move(sf::Vector2f(delta_pos, 0));
        window.setView(view);
    }
}

GridScreen::GridScreen(){
    // TODO: consider if to move it outside of c'tor.
    left_top_view_pos = sf::Vector2i(window_width * (MULTIPLE / 2), window_height * (MULTIPLE / 2));
    view = sf::View(sf::FloatRect(left_top_view_pos.x, left_top_view_pos.y, window_width, window_height));

    // TODO: currently we don't need to setView at the start. Uncomment if it causes problems.
    // Since our initial view is different from the default view, we have to set it before we continue.
    window.setView(view);

    focus = true;
    clicking = false;
}

