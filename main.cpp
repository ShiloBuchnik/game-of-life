#include <SFML/Graphics.hpp>
#include<windows.h>
#include <vector>
#include "game_logic.h"
#include "global_macros.h"

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Game of life", sf::Style::Default);

    // Create the grid of cells (not displaying it yet)
    std::vector<std::vector<sf::RectangleShape> > grid;
    for (int i = 0; i < WINDOW_HEIGHT * 2 / GRID_SIZE; i++) {
        std::vector<sf::RectangleShape> row;
        for (int j = 0; j < WINDOW_WIDTH * 2 / GRID_SIZE; j++) {
            sf::RectangleShape cell(sf::Vector2f(GRID_SIZE, GRID_SIZE));
            cell.setFillColor(DEAD_CELL_COLOR);
            cell.setOutlineColor(sf::Color(200, 200, 200));
            cell.setOutlineThickness(1);
            row.push_back(cell);
        }
        grid.push_back(row);
    }

    getUserInput(window, grid, WINDOW_WIDTH, WINDOW_HEIGHT, GRID_SIZE);

    //auto view = window.getDefaultView();

    while (window.isOpen()) {
        sf::Event evnt;
        while (window.pollEvent(evnt)) {
            switch(evnt.type) {
                case sf::Event::Closed:
                    window.close();
                    return 0;
                    break;

                case sf::Event::KeyPressed:
                    if (evnt.key.code == sf::Keyboard::Escape){
                        window.close();
                        return 0;
                    }

                    if (evnt.key.code == sf::Keyboard::Enter) blankGrid(grid);
                    break;

                /*case evnt.Resized:
                    view.setSize(static_cast<float>(evnt.size.width), static_cast<float>(evnt.size.height));
                    break;*/
            }
        }

        updateGrid(grid);

        window.clear();
        if ( drawGrid(window, grid, GRID_SIZE) ){
            getUserInput(window, grid, WINDOW_WIDTH, WINDOW_HEIGHT, GRID_SIZE);
        }
        window.display();
        sf::sleep(sf::milliseconds(SLEEP_DURATION));
    }

    return 0;
}
