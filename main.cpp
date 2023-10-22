#include <iostream>
#include <SFML/Graphics.hpp>
#include <windows.h>
#include <vector>
#include "game_logic.h"
#include "global_macros.h"

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Game of life", sf::Style::Close | sf::Style::Titlebar);

    // Create the grid of cells (not displaying it yet)
    std::vector< std::vector<sf::RectangleShape> > grid;
    for (int i = 0; i < WINDOW_HEIGHT / GRID_SIZE; i++) {
        std::vector<sf::RectangleShape> row;
        for (int j = 0; j < WINDOW_WIDTH / GRID_SIZE; j++) {
            sf::RectangleShape cell(sf::Vector2f(GRID_SIZE, GRID_SIZE));
            cell.setFillColor(DEAD_CELL_COLOR);
            cell.setOutlineColor(sf::Color(200, 200, 200)); // Beige
            cell.setOutlineThickness(1.25);
            row.push_back(cell);
        }
        grid.push_back(row);
    }

    getUserInput(window, grid, WINDOW_WIDTH, WINDOW_HEIGHT, GRID_SIZE);

    long unsigned int gen = 0;
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
                    break;
            }
        }

        updateGrid(grid);
        gen++;

        window.clear();
        // If we got to a blank grid, that means we need to start again and ask for user's input
        if ( drawGrid(window, grid, GRID_SIZE) ){
            getUserInput(window, grid, WINDOW_WIDTH, WINDOW_HEIGHT, GRID_SIZE);
            gen = 0;
        }
        window.display();
        std::cout << "Generation: " << gen << std::endl;
        sf::sleep(sf::milliseconds(SLEEP_DURATION));
    }

    return 0;
}
