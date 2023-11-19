#ifndef GAME_OF_LIFE_GAME_LOGIC_H
#define GAME_OF_LIFE_GAME_LOGIC_H
#include <SFML/Graphics.hpp>
#include <cmath>
#include "globals.h"

bool introduction(std::unordered_set<sf::Vector2i, pair_hash, pair_equal>& grid);
void saveInputPattern(const std::unordered_set<sf::Vector2i, pair_hash, pair_equal>& grid);
void handleLeftClick(std::unordered_set<sf::Vector2i, pair_hash, pair_equal>& grid, const sf::Vector2f& view_pos);
void updateGrid(std::unordered_set<sf::Vector2i, pair_hash, pair_equal>& grid);
void drawGrid(sf::RenderWindow& window, const std::unordered_set<sf::Vector2i, pair_hash, pair_equal>& grid,
              const sf::Vector2i& left_top_view_pos, int window_width, int window_height);

#endif