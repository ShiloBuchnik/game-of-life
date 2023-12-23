#include <SFML/Graphics.hpp>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include "globals.h"

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

// This function adds the Moore neighborhood of every cell (including itself) to the map.
inline void addNeighbors(const std::unordered_set<sf::Vector2i, pair_hash, pair_equal>& grid,
                         std::unordered_map<sf::Vector2i, short int, pair_hash, pair_equal>& m){
    for (const auto& coordinate : grid){

        for (int k = coordinate.y - 1; k <= coordinate.y + 1; k++){
            for (int p = coordinate.x - 1; p <= coordinate.x + 1; p++){
                // Bound checking, so we don't add a cell beyond bounds.
                if (0 <= k && k <= GRID_HEIGHT / CELL_SIZE && 0 <= p && p <= GRID_WIDTH / CELL_SIZE) m[{p, k}]++;
            }
        }
    }
}

// Apply rules based on chosen automaton's 'born' and 'survive' digits.
inline void applyRules(std::unordered_set<sf::Vector2i, pair_hash, pair_equal>& grid,
                       const std::unordered_map<sf::Vector2i, short int, pair_hash, pair_equal>& m,
                       const std::set<short int>& born_digits,
                       const std::set<short int>& survive_digits){
    for (const auto& map_pair : m){
        bool is_curr_cell_live = grid.count(map_pair.first);
        if (!is_curr_cell_live && born_digits.count(map_pair.second) == 1){ // Cell is dead and can be born.
            grid.insert(map_pair.first);
        }

        //we write 'map_pair.second - 1', because a *live* cell also counts itself in 'm', so we subtract 1.
        if (is_curr_cell_live && survive_digits.count(map_pair.second - 1) == 0){ // Cell is live and can *not* survive.
            grid.erase(map_pair.first);
        }
    }
}

// Global variable that stores previous size of the map allocated in 'updateGrid', so that we can 'reserve()' in the next call.
int reserved_size = 500;

/* Update the grid to next generation. The algorithm is as follows:
 1) Take the set of lives cells (grid), and for every cell:
 Add itself and its neighbors to a map that maps coordinate to number of appearances.
 *Adding *itself* is crucial, for cells without neighbors, for example.
 2) Iterate over the map s.t. for every coordinate with 'k' appearances:
 If it's live and doesn't fulfill the 'survive' rule - delete it from grid.
 If it's dead and does fulfill the 'burn' rule - insert it to grid.
 *In previous versions, when we only had Game of Life, this is what we did:
 (I'm saving it because I really like this solution; and also because it's my project, and I can do whatever I want):
 "If a cell appears 3 times (has been added 3 times), add it to the new set.
 If a cell appears 4 times *and is live*, add it to the new set.
 The new set is the new generation. Swap it with grid.
 Explanation: in order for a cell to be live in next gen, it needs to be:
 -Live with 2 neighbors or dead with 3 neighbors - in this case it will appear 3 times.
 -Live with 3 neighbors - in this case it will be *live* and appear 4 times."

 Let's denote p = number of live cells. So:
 Time complexity: O(9p + 9p)=O(p). Space complexity: O(9p + 9p)=O(p).
 This is a lot more efficient than our previous algorithm, which was in time O(n^2) and space O(1).
 Although space was O(1) (because it's about *additional* space, and we did it in-place),
 we still had to rely on an O(n^2) matrix, so overall this algorithm is superior*.
 *With the caveat that a matrix is contiguous in memory, so under certain architecture with certain caches, it might be faster. */
void updateGrid(std::unordered_set<sf::Vector2i, pair_hash, pair_equal>& grid, const std::set<short int>& born_digits,
                const std::set<short int>& survive_digits){
    // Maps a coordinate to amount of times it has been added.
    std::unordered_map<sf::Vector2i, short int, pair_hash, pair_equal> coordinate_to_amount;
    /* After a shitload of tests and plotting in Matlab, I've come to the conclusion that:
    -reserving in advance 1.015 times the previous size improves performance.
    -Tinkering with max_load_factor doesn't do a whole lot. */
    coordinate_to_amount.reserve(reserved_size);

    // Note for self: from testing, 'addNeighbors()' takes ~75% of function's runtime, while 'applyRules()' takes ~25%.
    // The costly operation is accessing the map, not iterating over all the neighbors.
    addNeighbors(grid, coordinate_to_amount);
    reserved_size = coordinate_to_amount.size() * 1.015;

    applyRules(grid, coordinate_to_amount, born_digits, survive_digits);
}

// Draws the grid. We ONLY draw the visible view, not the entire grid; so the grid can be as big as we want, without extra runtime costs.
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