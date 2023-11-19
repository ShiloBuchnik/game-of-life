#include <SFML/Graphics.hpp>
#include <iostream>
#include <limits>
#include <unordered_set>
#include <unordered_map>
#include <filesystem>
#include "game_logic.h"
#include "patterns.h"
#include "globals.h"

// Returns number chosen by user.
short int introductionn(){
    std::cout << "Welcome to 'game of life'. Below are starting modes:\n" << std::endl;
    std::cout << "1. Custom pattern\n" << std::endl;

    std::cout << "StillLifes patterns:" << std::endl;
    std::cout << "2. Block" << std::endl;
    std::cout << "3. Bee hive" << std::endl;
    std::cout << "4. Honey farm" << std::endl;
    std::cout << "5. Loaf" << std::endl;
    std::cout << "6. Bakery" << std::endl;
    std::cout << "7. Boat" << std::endl;
    std::cout << "8. Ship" << std::endl;
    std::cout << "9. Tub\n" << std::endl;

    std::cout << "Oscillators patterns:" << std::endl;
    std::cout << "10. Blinker" << std::endl;
    std::cout << "11. Toad" << std::endl;
    std::cout << "12. Beacon" << std::endl;
    std::cout << "13. Pulsar" << std::endl;
    std::cout << "14. Penta_decathlon" << std::endl;
    std::cout << "15. Cross" << std::endl;
    std::cout << "16. Star" << std::endl;
    std::cout << "17. Clock" << std::endl;
    std::cout << "18. Kok's galaxy" << std::endl;
    std::cout << "19. Octagon" << std::endl;
    std::cout << "20. French kiss" << std::endl;
    std::cout << "21. Fumarole" << std::endl;
    std::cout << "22. Pentoad\n" << std::endl;

    std::cout << "Spaceships patterns:" << std::endl;
    std::cout << "23. Glider" << std::endl;
    std::cout << "24. Light weight spaceship" << std::endl;
    std::cout << "25. Middle weight spaceship" << std::endl;
    std::cout << "26. Heavy weight spaceship\n" << std::endl;

    std::cout << "Guns patterns:" << std::endl;
    std::cout << "27. Gosper's glider gun" << std::endl;
    std::cout << "28. Simkin's glider gun\n\n" << std::endl;

    short int mode_num;
    std::cout << "Please enter a mode number: ";
    std::cin >> mode_num;
    while (std::cin.fail() || mode_num < 1 || 28 < mode_num){
        std::cout << "Invalid input, please try again: ";
        std::cin.clear(); // Clears error flag. If it's raised, cin won't get input.
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clears buffer
        std::cin >> mode_num;
    }
    system("cls");

    return mode_num;
}

bool introduction(std::unordered_set<sf::Vector2i, pair_hash, pair_equal>& grid){
    std::cout << "Welcome to 'game of life'. Below are starting modes:\n" << std::endl;
    std::cout << "1. Custom pattern\n" << std::endl;
    std::unordered_map <int, std::string> num_to_pattern_path;
    int index = 2;

    // TODO: Expand this to enable multiple automata. Based on user choice, we assign 'game of life', 'seeds' etc to 'pattern_dir'
    std::string patterns_dir = "patterns\\game of life";
    // 'directory_iterator' is on one hand a container, and on other hand an iterator...very confusing.
    // Anyway, it stores all the files in given 'dir_path'
    for (const auto& pattern_type_dir : std::filesystem::directory_iterator(patterns_dir)){ // Iterating over pattern *types*
        // 'pattern_dir.path()' returns relative path, 'relative()' returns it without the dirname at the start,
        // 'stem()' stems the file extension, and 'string()', well, returns the string version of the 'path' object.
        if (std::filesystem::is_empty(pattern_type_dir)) continue; // We don't handle empty directories.
        std::cout << relative(pattern_type_dir.path(), patterns_dir).stem().string() << " patterns:" << std::endl;

        for (const auto& entry : std::filesystem::directory_iterator(pattern_type_dir)){ // Iterating over patterns themselves
            if (entry.path().extension().string() != ".rle") continue; // If it's not an RLE file we continue

            std:: cout << index << ". " << relative(entry.path(), pattern_type_dir).stem().string() << std::endl;

            num_to_pattern_path[index] = entry.path().string();
            index++;
        }
        std::cout << "\n";
    }

    int mode_num;
    std::cout << "Please enter a mode number: ";
    std::cin >> mode_num;
    while (std::cin.fail() || mode_num < 1 || index <= mode_num){
        std::cout << "Invalid input, please try again: ";
        std::cin.clear(); // Clears error flag. If it's raised, 'cin' won't get input.
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clears buffer
        std::cin >> mode_num;
    }
    system("cls");

    if (mode_num != 1) putPatternInGrid(grid, num_to_pattern_path[mode_num]);

    return mode_num == 1; // If 'mode_num == 1', that means we're now getting input
}

void saveInputPattern(const std::unordered_set<sf::Vector2i, pair_hash, pair_equal>& grid){
    char YN;
    std::cout << "Would you like to save this pattern in an .rle file? [Y/N] ";
    std::cin >> YN;
    while (YN != 'Y' && YN != 'y' && YN != 'N' && YN != 'n'){
        std::cout << "Invalid input, please try again: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin >> YN;
    }

    if (YN == 'Y' || YN == 'y'){
        std::string file_path = gridToRLE(grid);
        std::cout << "Saved! file path is: " << file_path;
        sf::sleep(sf::seconds(5)); // Let user see the "Saved!" log before flushing the terminal below.
    }
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

inline void addNeighbors(std::unordered_map<sf::Vector2i, int, pair_hash, pair_equal>& m, const sf::Vector2i& coordinate){
    for (int k = coordinate.y - 1; k <= coordinate.y + 1; k++){
        for (int p = coordinate.x - 1; p <= coordinate.x + 1; p++){
            // Bound checking, so we don't add a cell beyond bounds.
            if (0 <= k && k <= GRID_HEIGHT / CELL_SIZE && 0 <= p && p <= GRID_WIDTH / CELL_SIZE) m[{p, k}]++;
        }
    }
}

inline void applyRules(const std::unordered_set<sf::Vector2i, pair_hash, pair_equal>& grid,
                       const std::unordered_map<sf::Vector2i, int, pair_hash, pair_equal>& m,
                       std::unordered_set<sf::Vector2i, pair_hash, pair_equal>& new_set){
    // Game of life
    for (auto map_pair : m){
        if (map_pair.second == 3 || (map_pair.second == 4 && grid.count(map_pair.first) == 1)) new_set.insert(map_pair.first);
    }

    // Seeds
    //for (auto map_pair : m){
    //    if (map_pair.second == 2 && grid.count(map_pair.first) == 0) new_set.insert(map_pair.first);
    //}

    // Life without death
    //for (auto map_pair : m){
    //    if (grid.count(map_pair.first) == 1 || map_pair.second == 3) new_set.insert(map_pair.first);
    //}
}

/* Update the grid to next generation.
 The algorithm is as follows:
 1) Take the set of lives cells, and for every cell:
 Add itself and its neighbors to the map.
 2) Create a new set, and iterate over the map s.t.:
 If a cell appears 3 times (has been added 3 times), add it to the new set.
 If a cell appears 4 times *and is live*, add it to the new set.
 Explanation: in order for a cell to be live in next gen, it needs to be:
 -Live with 2 neighbors or dead with 3 neighbors - in this case it will appear 3 times.
 -Live with 3 neighbors - in this case it will be *live* and appear 4 times.
 3) The new set holds the live cells of the next gen. Swap it with the old set.

 Let's denote p = number of live cells. So:
 Time complexity: O(9p + 9p)=O(p). Space complexity: O(9p + 9p)=O(p).
 This is a lot more efficient than our previous algorithm, which was in time O(n^2) and space O(1).
 Although space was O(1) (because it's about *additional* space, and we did it in-place),
 we still had to rely on an O(n^2) matrix, so overall this algorithm is superior*.
 *With the caveat that a matrix is contiguous in memory, so under certain architecture with certain caches, it might be faster. */
void updateGrid(std::unordered_set<sf::Vector2i, pair_hash, pair_equal>& grid){
    // Maps a coordinate to amount of times it has been added.
    std::unordered_map<sf::Vector2i, int, pair_hash, pair_equal> coordinate_to_amount;

    for (auto coordinate : grid){
        addNeighbors(coordinate_to_amount, coordinate);
    }

    std::unordered_set<sf::Vector2i, pair_hash, pair_equal> next_gen_live_cells;
    applyRules(grid, coordinate_to_amount, next_gen_live_cells);

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