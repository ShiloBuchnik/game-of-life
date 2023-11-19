#include <SFML/Graphics.hpp>
#include <iostream>
#include <limits>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <filesystem>
#include "patterns.h"
#include "globals.h"

// Verify that input num is in the range [1, upper).
static unsigned long long int handleMenuInput(unsigned long long int upper){
    unsigned long long int num;

    std::cin >> num;
    while (std::cin.fail() || num < 1 || upper <= num){
        std::cout << "Invalid input, please try again: ";
        std::cin.clear(); // Clears error flag. If it's raised, 'cin' won't get input.
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clears buffer
        std::cin >> num;
    }

    return num;
}

// Given relative path to automaton directory in 'patterns_dir',
// the function prints all .rle filenames to screen, and saves them to 'vec'.
static void iterateOverAutomatonDirectory(const std::string& patterns_dir, std::vector <std::string>& vec){
    unsigned long long int index = 2;

    // 'directory_iterator' is on one hand a container, and on other hand an iterator...very confusing.
    // Anyway, it stores all the files in given 'dir_path'.
    for (const auto& pattern_type_dir : std::filesystem::directory_iterator(patterns_dir)){ // Iterating over pattern *types*.
        // If it's not a directory, or it's an empty directory, we continue.
        if (!std::filesystem::is_directory(pattern_type_dir) || std::filesystem::is_empty(pattern_type_dir)) continue;
        // 'pattern_type_dir.path()' returns relative path, 'relative()' returns it without the dirname at the start,
        // 'stem()' stems the file extension, and 'string()', well, returns the string version of the 'path' object.
        std::cout << relative(pattern_type_dir.path(), patterns_dir).stem().string() << " patterns:" << std::endl;

        for (const auto& entry : std::filesystem::directory_iterator(pattern_type_dir)){ // Iterating over patterns themselves.
            // If it's not an .rle file, we continue.
            if (entry.path().extension().string() != ".rle") continue;

            std:: cout << index << ". " << relative(entry.path(), pattern_type_dir).stem().string() << std::endl;
            vec.push_back(entry.path().string());
            index++;
        }
        std::cout << "\n";
    }
}

// Returns true if user chose "mode 1" (to input a pattern themselves), and false otherwise.
// It also stores the chosen automaton's name in 'automaton_name'.
// *Yes, I know I can identify the chosen automaton with a class enum, but I need the string itself for printing,
// so it doesn't make sense to save both.
bool introduction(std::unordered_set<sf::Vector2i, pair_hash, pair_equal>& grid, std::string& automaton_name){
    // We want to start from 1, so we put garbage in 0th index.
    const std::vector <std::string> num_to_automaton_name = {"", "Game of Life", "Seeds", "Life without Death", "HighLife", "Day and Night"};
    for (int i = 1; i < num_to_automaton_name.size(); i++){
        std::cout << i << ". " << num_to_automaton_name[i] << std::endl;
    }
    std::cout << "Please choose an automaton number: ";
    automaton_name = num_to_automaton_name[ handleMenuInput(num_to_automaton_name.size()) ];
    system("cls");


    std::cout << "Below are available patterns:\n" << std::endl;
    std::cout << "1. Custom Pattern\n" << std::endl;

    std::string patterns_dir = "patterns\\" + automaton_name;
    // If some directories in path doesn't exist, it creates them; otherwise (all exist), it does nothing.
    std::filesystem::create_directories(patterns_dir);
    // We want to start from 2 here, so 0th and 1st indices are garbage.
    std::vector <std::string> num_to_pattern_path = {"", ""};
    iterateOverAutomatonDirectory(patterns_dir, num_to_pattern_path);

    std::cout << "Please choose a pattern number: ";
    unsigned long long int pattern_num = handleMenuInput(num_to_pattern_path.size());
    system("cls");

    if (pattern_num != 1) putPatternInGrid(grid, num_to_pattern_path[pattern_num]);
    return pattern_num == 1;
}

bool handleYNInput(){
    char YN;
    std::cout << "Would you like to save this pattern in an .rle file? [Y/N] ";
    std::cin >> YN;
    while (YN != 'Y' && YN != 'y' && YN != 'N' && YN != 'n'){
        std::cout << "Invalid input, please try again: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin >> YN;
    }

    if (YN == 'Y' || YN == 'y') return true;
    return false;
}

void handleLeftClick(std::unordered_set<sf::Vector2i, pair_hash, pair_equal>& grid, const sf::Vector2f& view_pos){
    /* Regarding 'pixel_pos' and 'view_pos':
    Even when changing the view, *the objects themselves always remain in the same place in the "world"*.
    It's like a camera in a video game - it changes our perspective, but doesn't change the world.
    For example, if we click on a certain pixel, move the view (without moving the mouse), and click again,
    *SFML would register that as a click on that exact same pixel*.
    So, essentially, we have the location in the real world and the location in the current view.

    What we do below is transforming mouse coordinates (which are always the literal pixels on the screen),
    to the "relative" or "view" coordinates, which compensate for the fact that our view is moved from the real world objects.
    It's easier to understand with an example:
    If our cursor is on cell (1,1), and we move the view by pressing "D" (right) and "S" (down), and then click;
    We need to a way to "tell SFML" to click on cell (2,2), and not cell (1,1) (which is off-screen now).
    So we use 'mapPixelToCoords()', which takes a literal position on the window, fixes it according to the view, and returns it. */

    sf::Vector2i view_pos_integer = static_cast<sf::Vector2i>(view_pos);

    // We only register clicks inside the grid. Note that we use the grid's width and height, and not the window's, as it contains the window.
    if (0 <= view_pos_integer.x && view_pos_integer.x < GRID_WIDTH && 0 <= view_pos_integer.y && view_pos_integer.y < GRID_HEIGHT){
        // Converting raw coordinate to cell location in the grid. This is how we get from an exact coordinate to the cell it's in.
        view_pos_integer.x /= CELL_SIZE;
        view_pos_integer.y /= CELL_SIZE;

        // We allow selecting and deselecting cells
        if (grid.count(view_pos_integer)) grid.erase(view_pos_integer);
        else grid.insert(view_pos_integer);
    }
}

inline void addNeighbors(std::unordered_map<sf::Vector2i, short int, pair_hash, pair_equal>& m, const sf::Vector2i& coordinate){
    for (int k = coordinate.y - 1; k <= coordinate.y + 1; k++){
        for (int p = coordinate.x - 1; p <= coordinate.x + 1; p++){
            // Bound checking, so we don't add a cell beyond bounds.
            if (0 <= k && k <= GRID_HEIGHT / CELL_SIZE && 0 <= p && p <= GRID_WIDTH / CELL_SIZE) m[{p, k}]++;
        }
    }
}

// These maps save, for each automaton, the 'born' and 'survive' group.
// Yes, I know that global variables are gross, but I don't want to construct these containers everytime I call 'applyRules()'.
const std::unordered_map < std::string, std::unordered_set<short int> > map_born =
        { {"Game of Life", {3}}, {"Seeds", {2}}, {"Life without Death", {3}}, {"Day and Night", {3,6,7,8}} };
const std::unordered_map < std::string, std::unordered_set<short int> > map_survive =
        { {"Game of Life", {2,3}}, {"Seeds", {}}, {"Life without Death", {0,1,2,3,4,5,6,7,8}}, {"Day and Night", {3,4,6,7,8}} };

/* Apply rules based on chosen automaton. The rules of every life-like cellular automaton can be broken down into:
Amount of neighbors for a *dead* cell to be "born".
Amount of neighbors for a *live* cell to "survive".
This is all the information we need to generate the next gen.
We describe this 'born'/'survive' logic with the rulestring: "B<nums>S<nums>". So, for example, Game of Life is "B3S23". */
inline void applyRules(const std::unordered_set<sf::Vector2i, pair_hash, pair_equal>& grid,
                       const std::unordered_map<sf::Vector2i, short int, pair_hash, pair_equal>& m,
                       std::unordered_set<sf::Vector2i, pair_hash, pair_equal>& new_set, const std::string& automaton_name){
    const std::unordered_set <short int>& born = map_born.at(automaton_name);
    const std::unordered_set <short int>& survive = map_survive.at(automaton_name);

    for (const auto& map_pair : m){
        // In the second condition we write 'map_pair.second - 1', because a *live* cell also counts itself in 'm', so we subtract 1.
        if ( (grid.count(map_pair.first) == 0 && born.count(map_pair.second) == 1) ||
             (grid.count(map_pair.first) == 1 && survive.count(map_pair.second - 1) == 1) ) new_set.insert(map_pair.first);
    }
}

/* Update the grid to next generation.
 The algorithm is as follows:
 1) Take the set of lives cells, and for every cell:
 Add itself and its neighbors to the map.
 2) Create a new set, and iterate over the map s.t.:
 Every cell that follows the 'born'/'survive' rules gets added to the new set.
 3) The new set holds the live cells of the next gen. Swap it with the old set.
 *In previous versions, when we only had Game of Life, this is what we did on every iteration in step 2.
 I'm saving it because I really like this solution; and also because it's my project, and I can do whatever I want.
 "If a cell appears 3 times (has been added 3 times), add it to the new set.
 If a cell appears 4 times *and is live*, add it to the new set.
 Explanation: in order for a cell to be live in next gen, it needs to be:
 -Live with 2 neighbors or dead with 3 neighbors - in this case it will appear 3 times.
 -Live with 3 neighbors - in this case it will be *live* and appear 4 times."

 Let's denote p = number of live cells. So:
 Time complexity: O(9p + 9p)=O(p). Space complexity: O(9p + 9p)=O(p).
 This is a lot more efficient than our previous algorithm, which was in time O(n^2) and space O(1).
 Although space was O(1) (because it's about *additional* space, and we did it in-place),
 we still had to rely on an O(n^2) matrix, so overall this algorithm is superior*.
 *With the caveat that a matrix is contiguous in memory, so under certain architecture with certain caches, it might be faster. */
void updateGrid(std::unordered_set<sf::Vector2i, pair_hash, pair_equal>& grid, const std::string& automaton_name){
    // Maps a coordinate to amount of times it has been added.
    std::unordered_map<sf::Vector2i, short int, pair_hash, pair_equal> coordinate_to_amount;

    // Note for self: from testing, this is the most expensive operation in this function.
    for (auto coordinate : grid){
        addNeighbors(coordinate_to_amount, coordinate);
    }

    std::unordered_set<sf::Vector2i, pair_hash, pair_equal> next_gen_live_cells;
    applyRules(grid, coordinate_to_amount, next_gen_live_cells, automaton_name);

    // Swap happens in constant time (just rewiring of pointers, not deep copying).
    grid.swap(next_gen_live_cells);
}

// Draws the grid. Returns true iff grid is blank.
// We ONLY draw the visible view, not the entire grid; so the grid can be as big as we want, without extra runtime costs.
void drawGrid(sf::RenderWindow& window, const std::unordered_set<sf::Vector2i, pair_hash, pair_equal>& grid,
              const sf::Vector2i& left_top_view_pos, int window_width, int window_height){
    // We divide by 'CELL_SIZE' to transform view coordinates to grid ones. We add/subtract 1 to be on the safe side.
    int top = left_top_view_pos.y / CELL_SIZE - 1, down = (left_top_view_pos.y + window_height) / CELL_SIZE + 1;
    int left = left_top_view_pos.x / CELL_SIZE - 1, right = (left_top_view_pos.x + window_width) / CELL_SIZE + 1;

    sf::RectangleShape cell(sf::Vector2f(CELL_SIZE, CELL_SIZE));
    cell.setOutlineColor(sf::Color(200, 200, 200)); // Beige
    cell.setOutlineThickness(1.25);

    for (int i = top; i < down ; i++){
        for (int j = left; j < right; j++){
            if (grid.count({j,i})) cell.setFillColor(LIVE_CELL_COLOR);
            else cell.setFillColor(DEAD_CELL_COLOR);

            // Set cell position based on its grid coordinates.
            cell.setPosition(j * CELL_SIZE, i * CELL_SIZE);
            window.draw(cell);
        }
    }
}