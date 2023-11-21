#ifndef GAME_OF_LIFE_PATTERNS_H
#define GAME_OF_LIFE_PATTERNS_H
#include <SFML/Graphics.hpp>
#include <unordered_set>
#include "globals.h"

std::string gridToRLE(const std::unordered_set<sf::Vector2i, pair_hash, pair_equal>& grid, const std::set <short int>& born_digits,
                      const std::set <short int>& survive_digits);
void putPatternInGrid(std::unordered_set<sf::Vector2i, pair_hash, pair_equal>& grid, const std::string& file_path);

#endif
