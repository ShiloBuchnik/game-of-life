#include <cmath>
#include "screens.h"

sf::Color GridScreen::live_cell_color(255, 0, 0); // Red
sf::Color GridScreen::dead_cell_color(127, 127, 127); // Grey
sf::Color GridScreen::outline_color(200, 200, 200); // Beige
float GridScreen::zoom = 1;

float GridScreen::distance(const sf::Vector2i& vec1, const sf::Vector2i& vec2){
    return sqrt(pow(vec2.x - vec1.x, 2) + pow(vec2.y - vec1.y, 2));
}

void GridScreen::checkForChangeViewWithKeys(const long long int& delta_time) const{
    float delta_pos = SPEED * delta_time; // It's in absolute value, we'll add the sign later

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
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) && left_top_view_pos.y + view.getSize().y + delta_pos <= grid_height){
        left_top_view_pos.y += delta_pos;
        view.move(sf::Vector2f(0, delta_pos));
        window.setView(view);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) && left_top_view_pos.x + view.getSize().x + delta_pos <= grid_width){
        left_top_view_pos.x += delta_pos;
        view.move(sf::Vector2f(delta_pos, 0));
        window.setView(view);
    }
}

// 'resize()' and 'handleZoom()' look similar, but we can't merge them, since then latter alters the 'left_top_view_pos',
// while the former does not.
void GridScreen::resize(const sf::Event& evnt) const{
    /* By default, when resizing, everything is squeezed/stretched to the new size.
    What we want to do is to *keep the top-left corner the same*, and simply extending/reducing the width or height from right or down,
    just like in a windowed video game. We pass that corner with the new width and height to the 'reset()' method.

    If we'd only change the width and height to current window's width and height, without keeping the top-left corner the same,
    then that corner would change everytime we'd resize. */

    // View's size is simply window's size times a 'zoom' factor.
    // After the resize, we need to recalculate that view's size; and then keep the same 'left_top_view_pos' (with 'reset()')
    view.setSize(sf::Vector2f(window.getSize().x, window.getSize().y));
    view.zoom(zoom);

    // We perform bound checking below, as to not show beyond the grid's bounds, when resizing close to the edge.
    left_top_view_pos.x = std::min(left_top_view_pos.x, grid_width - view.getSize().x);
    left_top_view_pos.y = std::min(left_top_view_pos.y, grid_height - view.getSize().y);

    view.reset(sf::FloatRect(left_top_view_pos.x, left_top_view_pos.y, view.getSize().x, view.getSize().y));
    window.setView(view);
}

// Note to self about the view object:
// 'zoom()' and 'setSize()' *do not* change the center position, so only the bounds themselves change.
void GridScreen::handleZoom(float delta) const{
    // 'zoom()' is by a factor. a number greater than 1 means zoom-out; a number smaller than 1 means zoom-in.
    if (0 < delta) { // Scroll up - zoom-in
        zoom = std::max(0.5, zoom - 0.1); // By using 'max' with '0.5', we set it as an upper limit.
    }
    else if (delta < 0) { // Scroll down - zoom-out
        zoom = std::min(2.0, zoom + 0.1); // By using 'min' with '2', we set it as a lower limit.
    }

    // We use 'setSize()' here to reset our view (by setting it to the default view's size).
    // Why? Because 'zoom()' is by a factor. So if we zoomed twice we'd be multiplying instead of adding.
    // For that we reset the view and then apply the zoom on it.
    view.setSize(sf::Vector2f(window.getSize().x, window.getSize().y));
    view.zoom(zoom);

    // Recalculating 'left_top_view_pos' after view's change.
    left_top_view_pos.x = view.getCenter().x - view.getSize().x / 2;
    left_top_view_pos.y = view.getCenter().y - view.getSize().y / 2;

    // We perform bound checking below, as to not show beyond the grid's bounds, when zooming close to the edge.
    if (left_top_view_pos.x < 0) left_top_view_pos.x = 0;
    if (left_top_view_pos.y < 0) left_top_view_pos.y = 0;
    left_top_view_pos.x = std::min(left_top_view_pos.x, grid_width - view.getSize().x);
    left_top_view_pos.y = std::min(left_top_view_pos.y, grid_height - view.getSize().y);

    view.reset(sf::FloatRect(left_top_view_pos.x, left_top_view_pos.y, view.getSize().x, view.getSize().y));
    window.setView(view);
}

void GridScreen::handleDrag(sf::Vector2i& old_pos, const sf::Vector2i& new_pos) const{
    // We subtract 'new_pos' from 'old_pos' (not the other way around) to invert the direction and get the "drag" effect.
    // Note we use integers, so we're safe from the flicker (for now).

    // We need to transform to view positions, to adjust to possible zoom
    sf::Vector2f old_view_pos = window.mapPixelToCoords(old_pos), new_view_pos = window.mapPixelToCoords(new_pos);
    sf::Vector2f delta_pos = old_view_pos - new_view_pos;

    // We don't want to allow the user to drag beyond the grid, so we bound-check
    if (0 <= left_top_view_pos.x + delta_pos.x && left_top_view_pos.x + view.getSize().x + delta_pos.x <= grid_width){
        left_top_view_pos.x += delta_pos.x;
    }
    if (0 <= left_top_view_pos.y + delta_pos.y && left_top_view_pos.y + view.getSize().y + delta_pos.y <= grid_height){
        left_top_view_pos.y += delta_pos.y;
    }
    view.reset(sf::FloatRect(left_top_view_pos.x, left_top_view_pos.y, view.getSize().x, view.getSize().y));
    window.setView(view);

    old_pos = new_pos;
}

/* Draws the grid. We ONLY draw the visible view, not the entire grid;
so the grid can be as big as we want (It's O(visible_cells), and not O(cells))

We use vertexArray (instead of drawing rectangle shapes) for 2 reasons:
 1) It rids us of the flickering lines' problem we've faced with the 'zoom()' function.
Don't know how exactly, but don't care either
2) TL;DR: it's faster (about 50% faster, but 'drawGrid()' is already very cheap so...)
The GPU can handle a lot of geometric shapes in each 'draw()' call, so calling it multiple times with a *single* shape is wasteful.
It's much better to store all of our shapes in a VertexArray (preprocessing), and then call 'draw()' once on the array.
Also, we store only the live cells, and we don't need to draw dead cells.
This is because we draw the background in dead_cell_color, so if we didn't draw anything there - it looks like a dead cell. */
void GridScreen::drawGrid(){
    sf::VertexArray live_quads_vertex_arr(sf::PrimitiveType::Quads); // Stores *only* the live cells
    sf::VertexArray lines_vertex_arr(sf::PrimitiveType::Lines);

    // We divide by 'CELL_SIZE' to transform view coordinates to grid ones. We add/subtract 1 to be on the safe side.
    // It's important to use grid coordinates instead of view ones,
    // so that the grid in the real 2d world always stays in the same place (only the view moves).
    int top = left_top_view_pos.y / CELL_SIZE - 1, down = (left_top_view_pos.y + view.getSize().y) / CELL_SIZE + 1;
    int left = left_top_view_pos.x / CELL_SIZE - 1, right = (left_top_view_pos.x + view.getSize().x) / CELL_SIZE + 1;

    for (int i = top; i < down ; i++){
        // Creating the horizontal lines
        lines_vertex_arr.append(sf::Vertex(sf::Vector2f(left * CELL_SIZE, i * CELL_SIZE), outline_color));
        lines_vertex_arr.append(sf::Vertex(sf::Vector2f(right * CELL_SIZE, i * CELL_SIZE), outline_color));

        for (int j = left; j < right; j++){
            if (i == top){ // Creating the vertical lines (only on upper loop's first iteration)
                lines_vertex_arr.append(sf::Vertex(sf::Vector2f(j * CELL_SIZE, top * CELL_SIZE), outline_color));
                lines_vertex_arr.append(sf::Vertex(sf::Vector2f(j * CELL_SIZE, down * CELL_SIZE), outline_color));
            }

            if (grid.count({j,i})){ // If a cell is live - we include it and draw it
                live_quads_vertex_arr.append(sf::Vertex(sf::Vector2f(j * CELL_SIZE, i * CELL_SIZE), live_cell_color));
                live_quads_vertex_arr.append(sf::Vertex(sf::Vector2f((j + 1) * CELL_SIZE, i * CELL_SIZE), live_cell_color));
                live_quads_vertex_arr.append(sf::Vertex(sf::Vector2f((j + 1) * CELL_SIZE, (i + 1) * CELL_SIZE), live_cell_color));
                live_quads_vertex_arr.append(sf::Vertex(sf::Vector2f(j * CELL_SIZE, (i + 1) * CELL_SIZE), live_cell_color));
            }
        }
    }

    window.draw(live_quads_vertex_arr);
    window.draw(lines_vertex_arr);
}