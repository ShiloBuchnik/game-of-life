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

// We need to define a custom hash and equal functors for pair type.
// Defining a hash is not enough, since hash functions can have collisions.
// Side note: the overloaded '()' is templated, but that's okay, because C++ has type inference
class pair_hash {
public:
    std::size_t operator() (const sf::Vector2i &pair) const {
        std::hash<int> hash_obj;
        return ~hash_obj(pair.x) ^ hash_obj(pair.y);
    }
};
class pair_equal {
public:
    bool operator() (const sf::Vector2i &pair1, const sf::Vector2i &pair2) const {
        return pair1.x == pair2.x && pair1.y == pair2.y;
    }
};

void getUserInput(sf::RenderWindow& window, sf::View& view, std::unordered_set<sf::Vector2i, pair_hash, pair_equal>& grid);
void updateGrid(std::unordered_set<sf::Vector2i, pair_hash, pair_equal>& grid);
void drawGrid(sf::RenderWindow& window, std::unordered_set<sf::Vector2i, pair_hash, pair_equal>& grid);

#endif
