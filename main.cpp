#include <iostream>
#include <SFML/Graphics.hpp>
#include "game_logic.h"
#include "patterns.h"

int main() {
    short int mode_num = introduction();

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

    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Game of life", sf::Style::Default);
    //sf::View view(sf::Vector2f(WINDOW_HEIGHT / 2, WINDOW_WIDTH / 2), sf::Vector2f(WINDOW_HEIGHT, WINDOW_WIDTH));

    if (mode_num == 1) getUserInput(window, grid, WINDOW_WIDTH, WINDOW_HEIGHT, GRID_SIZE); // Free mode
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

                    /*if (evnt.key.code == sf::Keyboard::D){
                        view.move(50, 50);
                        window.setView(view);
                    }
                    break;

                case sf::Event::Resized:
                    {
                        sf::FloatRect visibleArea(0.f, 0.f, evnt.size.width, evnt.size.height);
                        window.setView(sf::View(visibleArea));
                    } */
            }
        }

        updateGrid(grid);
        gen++;
        std::cout << "Generation: " << gen << std::endl;

        sf::sleep(sf::milliseconds(SLEEP_DURATION));
        window.clear();
        // If we got to a blank grid, that means we need to start again and ask for user's input
        if ( drawGrid(window, grid, GRID_SIZE) ){
            getUserInput(window, grid, WINDOW_WIDTH, WINDOW_HEIGHT, GRID_SIZE);
            gen = 0;
        }
        window.display();
    }

    return 0;
}
