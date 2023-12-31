#include "screens.h"

sf::Color GridScreen::live_cell_color(255, 0, 0); // Red
sf::Color GridScreen::dead_cell_color(127, 127, 127); // Grey
sf::Color GridScreen::outline_color(200, 200, 200); // Beige

void GridScreen::checkForChangeViewWithKeys(const int& delta_time) const{
    /* Note that the product 'SPEED * delta_time' can be a *float*, so moving by it can make the line not align 1:1 with the pixels,
    because their location is always in integers.
    When a line falls between 2 columns of pixels, openGL has to decide on the fly how to render it,
    and that often causes it to flicker when there is movement.
    By rounding the result, we move the view by an integer, and thus always aligning 1:1 with the pixels. */
    int delta_pos = std::round(SPEED * delta_time); // It's in absolute value, we'll add the sign later

    // We're not allowing the user to move the view beyond the grid's bounds.
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
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) && left_top_view_pos.y + window_height + delta_pos <= grid_height){
        left_top_view_pos.y += delta_pos;
        view.move(sf::Vector2f(0, delta_pos));
        window.setView(view);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) && left_top_view_pos.x + window_width + delta_pos <= grid_width){
        left_top_view_pos.x += delta_pos;
        view.move(sf::Vector2f(delta_pos, 0));
        window.setView(view);
    }
}

void GridScreen::handleDrag(sf::Vector2i& old_pos, const sf::Vector2i& new_pos){
    // We subtract 'new_pos' from 'old_pos' (not the other way around) to invert the direction and get the "drag" effect.
    // Note we use integers, so we're safe from the flicker (for now).
    sf::Vector2i delta_pos = old_pos - new_pos;

    // We don't want to allow the user to drag beyond the grid, so we bound-check
    if (0 <= left_top_view_pos.x + delta_pos.x && left_top_view_pos.x + window_width + delta_pos.x <= grid_width){
        left_top_view_pos.x += delta_pos.x;
    }
    if (0 <= left_top_view_pos.y + delta_pos.y && left_top_view_pos.y + window_height + delta_pos.y <= grid_height){
        left_top_view_pos.y += delta_pos.y;
    }
    view.reset(sf::FloatRect(left_top_view_pos.x, left_top_view_pos.y, window_width, window_height));
    window.setView(view);

    old_pos = new_pos;
}

// Draws the grid. We ONLY draw the visible view, not the entire grid; so the grid can be as big as we want, without extra runtime costs.
void GridScreen::drawGrid(){
    // We divide by 'CELL_SIZE' to transform view coordinates to grid ones. We add/subtract 1 to be on the safe side.
    int top = left_top_view_pos.y / CELL_SIZE - 1, down = (left_top_view_pos.y + window_height) / CELL_SIZE + 1;
    int left = left_top_view_pos.x / CELL_SIZE - 1, right = (left_top_view_pos.x + window_width) / CELL_SIZE + 1;

    sf::RectangleShape cell(sf::Vector2f(CELL_SIZE, CELL_SIZE));
    cell.setOutlineColor(outline_color); // Beige
    cell.setOutlineThickness(1.25);

    for (int i = top; i < down ; i++){
        for (int j = left; j < right; j++){
            if (grid.count({j,i})){
                cell.setFillColor(live_cell_color);
            }
            else cell.setFillColor(dead_cell_color);

            // Set cell position based on its grid coordinates.
            cell.setPosition(j * CELL_SIZE, i * CELL_SIZE);
            window.draw(cell);
        }
    }
}