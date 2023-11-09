#ifndef GAME_OF_LIFE_GAME_LOGIC_H
#define GAME_OF_LIFE_GAME_LOGIC_H
#include <SFML/Graphics.hpp>

// Window width and height must divide grid size
#define WINDOW_WIDTH 1440
#define WINDOW_HEIGHT 810
#define MULTIPLE 5
#define GRID_WIDTH WINDOW_WIDTH * MULTIPLE
#define GRID_HEIGHT WINDOW_HEIGHT * MULTIPLE
#define CELL_SIZE 30
#define DEAD_CELL_COLOR sf::Color(128, 128, 128) // Grey
#define LIVE_CELL_COLOR sf::Color::Red
#define SLEEP_DURATION 400

short int introduction();
void getUserInput(sf::RenderWindow& window, sf::View& view, std::vector< std::vector<sf::RectangleShape> >& grid);

inline void changeView(sf::RenderWindow& window, sf::View& view, sf::Event& evnt){
    if (evnt.key.code == sf::Keyboard::W){
        view.move(sf::Vector2f(0, -1 * CELL_SIZE));
        window.setView(view);
    }
    if (evnt.key.code == sf::Keyboard::A){
        view.move(sf::Vector2f(-1 * CELL_SIZE, 0));
        window.setView(view);
    }
    if (evnt.key.code == sf::Keyboard::S){
        view.move(sf::Vector2f(0, CELL_SIZE));
        window.setView(view);
    }
    if (evnt.key.code == sf::Keyboard::D){
        view.move(sf::Vector2f(CELL_SIZE, 0));
        window.setView(view);
    }
}

void blankGrid(std::vector< std::vector<sf::RectangleShape> >& grid);
void updateGrid(std::vector< std::vector<sf::RectangleShape> >& grid);
bool drawGrid(sf::RenderWindow& window, std::vector< std::vector<sf::RectangleShape> >& grid);

#endif
