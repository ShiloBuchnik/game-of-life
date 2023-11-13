#ifndef GAME_OF_LIFE_GAME_LOGIC_H
#define GAME_OF_LIFE_GAME_LOGIC_H
#include <SFML/Graphics.hpp>
#include <cmath>

#define MULTIPLE 101 // The grid is 'MULTIPLE' times bigger than the initial window size, so that it looks like an "infinite" grid.
#define GRID_WIDTH 1440 * MULTIPLE
#define GRID_HEIGHT 810 * MULTIPLE
#define CELL_SIZE 30 // Window width and height must divide cell size
#define DEAD_CELL_COLOR sf::Color(128, 128, 128) // Grey
#define LIVE_CELL_COLOR sf::Color::Red
#define TIMESTEP sf::milliseconds(350) // Returns an 'sf::Time' object
#define SPEED 0.3

/* We need to define a custom hash and equal functors for pair type.
 Defining a hash is not enough, since hash functions can have collisions.
 Side note: the overloaded '()' is templated, but that's okay, because C++ has type inference */
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

inline float distance(const sf::Vector2i& vec1, const sf::Vector2i& vec2){
    return sqrt(pow(vec2.x - vec1.x, 2) + pow(vec2.y - vec1.y, 2));
}

// Although it's a long function, it gets called every iteration, so dear compiler - please accept my proposal.
inline void changeViewWithKeys(sf::RenderWindow& window, sf::View& view, sf::Vector2i& left_top_view_pos, int delta_time,
                        int window_width, int window_height){
    // We're not allowing the user to move the view beyond the grid's bounds.
    int delta_pos = std::round(SPEED * delta_time); // It's in absolute value, we'll add the sign later
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
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) && left_top_view_pos.y + window_height + delta_pos <= GRID_HEIGHT){
        left_top_view_pos.y += delta_pos;
        view.move(sf::Vector2f(0, delta_pos));
        window.setView(view);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) && left_top_view_pos.x + window_width + delta_pos <= GRID_WIDTH){
        left_top_view_pos.x += delta_pos;
        view.move(sf::Vector2f(delta_pos, 0));
        window.setView(view);
    }
}

short int introduction();
void handleLeftClick(std::unordered_set<sf::Vector2i, pair_hash, pair_equal>& grid, const sf::Vector2f& view_pos);
void updateGrid(std::unordered_set<sf::Vector2i, pair_hash, pair_equal>& grid);
void drawGrid(sf::RenderWindow& window, const std::unordered_set<sf::Vector2i, pair_hash, pair_equal>& grid, const sf::Vector2i& left_top_view_pos,
              int window_width, int window_height);
#endif