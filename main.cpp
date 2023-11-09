#include <iostream>
#include <SFML/Graphics.hpp>
#include "game_logic.h"
#include "patterns.h"

int main() {
    short int mode_num = introduction();

    // Create the grid of cells (not displaying it yet). Note that the grid is 4 times larger than the initial window/view.
    // That is to allow an "infinite" (which is just finite, but very big) grid.
    std::vector< std::vector<sf::RectangleShape> > grid;
    for (int i = 0; i < GRID_HEIGHT / CELL_SIZE; i++) {
        std::vector<sf::RectangleShape> row;
        for (int j = 0; j < GRID_WIDTH / CELL_SIZE; j++) {
            sf::RectangleShape cell(sf::Vector2f(CELL_SIZE, CELL_SIZE));
            cell.setFillColor(DEAD_CELL_COLOR);
            cell.setOutlineColor(sf::Color(200, 200, 200)); // Beige
            cell.setOutlineThickness(1.25);
            row.push_back(cell);
        }
        grid.push_back(row);
    }

    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Game of life", sf::Style::Default);
    // 1st and 2nd arguments are top-left coordinates of the rectangle. 3rd and 4th arguments are its width and height.
    // Our initial view is exactly at the middle of the grid.
    sf::View view(sf::FloatRect(WINDOW_WIDTH * (MULTIPLE / 2), WINDOW_HEIGHT * (MULTIPLE / 2), WINDOW_WIDTH, WINDOW_HEIGHT));
    // Since our initial view is different from the default view, we have to set it before we continue.
    window.setView(view);

    if (mode_num == 1) getUserInput(window, view, grid); // Custom pattern
    else Patterns::putPatternInGrid(grid, *Patterns::numToPattern[mode_num]); // Pre-defined pattern

    long long int gen = 0;
    while (window.isOpen()) {
        sf::Event evnt;
        while (window.pollEvent(evnt)) {
            switch(evnt.type) {
                // The program exits when user closes the window by pressing 'X'
                case sf::Event::Closed:
                    window.close();
                    return 0;
                    break;

                case sf::Event::KeyPressed:
                    // The program exits when user presses 'esc'
                    if (evnt.key.code == sf::Keyboard::Escape){
                        window.close();
                        return 0;
                    }

                    // The grid "restarts" when user presses 'Enter'
                    if (evnt.key.code == sf::Keyboard::Enter) blankGrid(grid);

                    if (evnt.key.code == sf::Keyboard::W || evnt.key.code == sf::Keyboard::A || evnt.key.code == sf::Keyboard::S || evnt.key.code == sf::Keyboard::D){
                        changeView(window, view, evnt);
                    }
                    break;

                case sf::Event::Resized: {
                    /* By default, when resizing, everything is squeezed/stretched to the new size.
                    What we want to do is to *keep the top-left corner the same*, and simply extending/reducing the width or height from right or down
                    just like in a windowed video game.

                    By calling 'mapPixelToCoords(0,0)' we get *the view's* top-left corner.
                    We pass that same top-left corner with the new width and height to the 'reset()' method.

                    If we'd only change the width and height, without keeping the top-left corner the same,
                    then it would change everytime we'd resize. */
                    sf::Vector2i top_left_view_pos = static_cast<sf::Vector2i> (window.mapPixelToCoords(sf::Vector2i(0, 0)));
                    view.reset(sf::FloatRect(top_left_view_pos.x, top_left_view_pos.y, evnt.size.width, evnt.size.height));
                    window.setView(view);
                    break;
                }
            }
        }

        updateGrid(grid);
        gen++;
        std::cout << "Generation: " << gen << std::endl;

        sf::sleep(sf::milliseconds(SLEEP_DURATION));
        window.clear();
        // If we got to a blank grid, that means we need to start again and ask for user's input
        if (drawGrid(window, grid)){
            getUserInput(window, view, grid);
            gen = 0;
        }
        window.display();
    }

    return 0;
}
