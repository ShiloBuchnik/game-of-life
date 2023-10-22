#ifndef GAME_OF_LIFE_GAME_LOGIC_H
#define GAME_OF_LIFE_GAME_LOGIC_H
#include <SFML/Graphics.hpp>

void updateGrid(std::vector< std::vector<sf::RectangleShape> >& grid);
bool drawGrid(sf::RenderWindow& window, std::vector< std::vector<sf::RectangleShape> >& grid, int grid_size);
void handleLeftClick(sf::RenderWindow& window, std::vector< std::vector<sf::RectangleShape> >& grid,
                     int window_width, int window_height, int grid_size);
void getUserInput(sf::RenderWindow& window, std::vector< std::vector<sf::RectangleShape> >& grid,
                  int window_width, int window_height, int grid_size);
void blankGrid(std::vector< std::vector<sf::RectangleShape> >& grid);

#endif
