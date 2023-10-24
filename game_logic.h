#ifndef GAME_OF_LIFE_GAME_LOGIC_H
#define GAME_OF_LIFE_GAME_LOGIC_H
#include <SFML/Graphics.hpp>

#define WINDOW_WIDTH 1440
#define WINDOW_HEIGHT 810
#define GRID_SIZE 30
#define DEAD_CELL_COLOR sf::Color(128, 128, 128)
#define LIVE_CELL_COLOR sf::Color::Red
#define SLEEP_DURATION 750

void getUserInput(sf::RenderWindow& window, std::vector< std::vector<sf::RectangleShape> >& grid,
                  int window_width, int window_height, int grid_size);
void blankGrid(std::vector< std::vector<sf::RectangleShape> >& grid);
void updateGrid(std::vector< std::vector<sf::RectangleShape> >& grid);
bool drawGrid(sf::RenderWindow& window, std::vector< std::vector<sf::RectangleShape> >& grid, int grid_size);

#endif
