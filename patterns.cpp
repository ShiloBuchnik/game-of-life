#include <SFML/Graphics.hpp>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <set>
#include <unordered_set>
#include "globals.h"

// If 'x' is bigger than 1, returns its string version. Otherwise, returns empty string.
inline std::string biggerThan1(unsigned int x){
    return 1 < x ? std::to_string(x) : "";
}

/* We want to allow multiple custom files in the directory.
Filenames in 'custom' directory will be of pattern "custom pattern <int>.rle".
We search for the minimal one that doesn't already exist. */
inline std::string findAvailableName(){
    int index = 0;
    std::string file_path;

    do{
        index++;
        file_path = "patterns\\custom\\custom pattern " + std::to_string(index) + ".rle";
    }
    while (std::filesystem::exists(file_path));

    return file_path;
}

// Takes a grid and creates an .rle file based on its contents.
std::string gridToRLE(const std::unordered_set<sf::Vector2i, pair_hash, pair_equal>& grid){
    std::ofstream rle_file; // write-only
    std::string available_file_path = findAvailableName();
    rle_file.open(available_file_path);

    // Edge case
    if (grid.empty()){
        rle_file << "x = 0, y = 0\n!";
        rle_file.close();
        return available_file_path;
    }

    // In order to get the pattern's width and height, and to write the file content, we need the coordinates to be ordered by 'pair_less'.
    std::set<sf::Vector2i, pair_less> ordered_grid(grid.begin(), grid.end());

    // We can't use the corners to get the width (for example, beehive), so we have to iterate over all the cells.
    int min_x = INT_MAX, max_x = 0;
    for (const auto& cell : grid){
        min_x = std::min(min_x, cell.x);
        max_x = std::max(max_x, cell.x);
    }

    int pattern_width = max_x - min_x + 1;
    int pattern_height = (*ordered_grid.rbegin()).y - (*ordered_grid.begin()).y + 1; // We get the height from top-left and down-right corners.
    rle_file << "x = " + std::to_string(pattern_width) + ", y = " + std::to_string(pattern_height) + "\n";

    int curr_line = (*ordered_grid.begin()).y, live_cell_streak = 1;
    std::string temp_str;

    // We start from 2nd cell in the loop, so we start by adding the dead cells at the beginning.
    // Note we only add it if they exist (so 'dead_cells_from_start != 0').
    int dead_cells_from_start = (*ordered_grid.cbegin()).x - min_x;
    if (dead_cells_from_start) temp_str.append(biggerThan1(dead_cells_from_start)).push_back('b');
    for (auto iter = ++ordered_grid.begin(); iter != ordered_grid.end(); iter++){
        int cell_width_distance = (*iter).x - (*std::prev(iter)).x;

        if ((*iter).y != (*std::prev(iter)).y){
            // Dumping all the live cells we got until end of line.
            temp_str.append(biggerThan1(live_cell_streak)).push_back('o');
            live_cell_streak = 1;

            // Counting newlines for the '$' sign.
            int cell_height_distance = (*iter).y - (*std::prev(iter)).y;
            temp_str.append(biggerThan1(cell_height_distance)).push_back('$');
            curr_line++;

            // Again, we only store live cells, so if the line starts with dead cells, we need to account for it.
            dead_cells_from_start = (*iter).x - min_x;
            if (dead_cells_from_start) temp_str.append(biggerThan1(dead_cells_from_start)).push_back('b');
        }
        else if (cell_width_distance == 1){
            live_cell_streak++;
        }
        else{
            // If the "live cell streak" is over, we dump the streak we had so far, and also the "dead cell streak" that follows.
            temp_str.append(biggerThan1(live_cell_streak)).push_back('o');
            temp_str.append(biggerThan1(cell_width_distance - 1)).push_back('b');
            live_cell_streak = 1;
        }

        // To enforce limit of 70 chars per line.
        // The limit is more of a rule of thumb, so on edge cases we'll exceed it by a little, and that's okay.
        if (65 < temp_str.size()){
            rle_file << temp_str << "\n";
            temp_str.clear();
        }
    }

    temp_str.append(biggerThan1(live_cell_streak)).push_back('o');
    temp_str.push_back('!');
    rle_file << temp_str;

    rle_file.close();
    return available_file_path;
}


// Takes an .rle file, parse it and fill 'grid' according to its content.
static void RLEToGrid(std::unordered_set<sf::Vector2i, pair_hash, pair_equal>& pattern, const std::string& file_path){
    std::ifstream rle_file; // read-only
    rle_file.open(file_path);

    std::string pattern_str; // We extract pattern's width and height from the first line.
    getline(rle_file, pattern_str); // We only save live cells, so we don't care about first line (pattern's width and height).
    getline(rle_file, pattern_str, '!');

    int curr_x = 0, curr_y = 0;
    std::string curr_num_str;

    for (auto c : pattern_str){
        if ('0' <= c && c <= '9'){
            curr_num_str.push_back(c);
        }
        else if (c == 'o'){
            int curr_num = curr_num_str.empty() ? 1 : std::stoi(curr_num_str);
            for (int i = 0; i < curr_num; i++){
                pattern.insert({curr_x + i, curr_y});
            }
            curr_x += curr_num;
            curr_num_str.clear();
        }
        else if (c == 'b'){
            curr_x += curr_num_str.empty() ? 1 : std::stoi(curr_num_str);
            curr_num_str.clear();
        }
        else if (c == '$'){
            // A number before '$' implies multiple newlines.
            int curr_num = curr_num_str.empty() ? 1 : std::stoi(curr_num_str);
            curr_y += curr_num;
            curr_num_str.clear();
            curr_x = 0;
        }
        else if (!isspace(c)){ // if 'c' is none of the above and not a whitespace, then file formatting is erroneous.
            std::cout << "bad RLE file, terminating..." << std::endl;
            exit(-1);
        }
    }

    rle_file.close();
}

inline sf::Vector2i centerOfMass(const std::unordered_set<sf::Vector2i, pair_hash, pair_equal>& pattern){
    if (pattern.empty()) return {0,0}; // If pattern is empty there is no center of mass.

    int sum_x = 0, sum_y = 0, cell_amount = pattern.size();

    for (auto cell : pattern){
        sum_x += cell.x;
        sum_y += cell.y;
    }

    return {sum_x / cell_amount, sum_y / cell_amount};
}

void putPatternInGrid(std::unordered_set<sf::Vector2i, pair_hash, pair_equal>& grid, const std::string& file_path){
    /* We can't put the pattern in 'grid' and then modify the elements, since unordered_set's elements are const (if they change, the hash changes).
    And we can't settle for erasing and insert when iterating over 'grid', since doing that will invalidate the iterators.
    Sadly, we must create a temp and then copy its modified elements to 'grid'. */
    std::unordered_set<sf::Vector2i, pair_hash, pair_equal> pattern;
    RLEToGrid(pattern, file_path);

    int cells_count_x = GRID_WIDTH / CELL_SIZE, cells_count_y = GRID_HEIGHT / CELL_SIZE;
    sf::Vector2i center_of_mass = centerOfMass(pattern);

    for (const auto& cell : pattern){
        // We shift the pattern to the grid's middle, and shift it to account for center of mass.
        int x = cell.x + cells_count_x / 2 - center_of_mass.x;
        int y = cell.y + cells_count_y / 2 - center_of_mass.y;

        grid.insert({x, y});
    }
}