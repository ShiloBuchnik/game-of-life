#include "screens.h"

GameScreen::GameScreen(): gen_text("", font, CHARACTER_SIZE) {
    timestep = 325; // By default, we "sleep" for 325ms.
    reserved_size = 500;

    gen_text.setFillColor(sf::Color::Black);
    gen_text.setStyle(sf::Text::Bold);
}

// This function adds the Moore neighborhood of every cell (including itself) to the map.
void GameScreen::addNeighbors(std::unordered_map<sf::Vector2i, short int, pair_hash, pair_equal>& m) const{
    for (const auto& coordinate : grid){

        for (int k = coordinate.y - 1; k <= coordinate.y + 1; k++){
            for (int p = coordinate.x - 1; p <= coordinate.x + 1; p++){
                // Bound checking, so we don't add a cell beyond bounds.
                if (0 <= k && k <= grid_height / CELL_SIZE && 0 <= p && p <= grid_width / CELL_SIZE) m[{p, k}]++;
            }
        }
    }
}

// Apply rules based on chosen automaton's 'born' and 'survive' digits.
void GameScreen::applyRules(const std::unordered_map<sf::Vector2i, short int, pair_hash, pair_equal>& m){
    for (const auto& map_pair : m){
        bool is_curr_cell_live = grid.count(map_pair.first);
        if (!is_curr_cell_live && born_digits.count(map_pair.second) == 1){ // Cell is dead and can be born.
            grid.insert(map_pair.first);
        }

        // We write 'map_pair.second - 1', because a *live* cell also counts itself in 'm', so we subtract 1.
        if (is_curr_cell_live && survive_digits.count(map_pair.second - 1) == 0){ // Cell is live and can *not* survive.
            grid.erase(map_pair.first);
        }
    }
}

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
void GameScreen::updateGrid(){
    // Maps a coordinate to amount of times it has been added.
    std::unordered_map<sf::Vector2i, short int, pair_hash, pair_equal> coordinate_to_amount;
    /* After a shitload of tests and plotting in Matlab, I've come to the conclusion that:
    -reserving in advance 1.015 times the previous size improves performance.
    -Tinkering with max_load_factor doesn't do a whole lot. */
    coordinate_to_amount.reserve(reserved_size);

    // Note for self: from testing, 'addNeighbors()' takes ~75% of function's runtime, while 'applyRules()' takes ~25%.
    // The costly operation is accessing the map, not iterating over all the neighbors.
    addNeighbors(coordinate_to_amount);
    reserved_size = coordinate_to_amount.size() * 1.015;

    applyRules(coordinate_to_amount);
}

short int GameScreen::run(){
    bool clicking = false;
    sf::Vector2i old_pos;
    sf::Clock timestep_clock, key_press_clock;

    unsigned long long gen = 0;
    gen_text.setString("gen: 0");
    gen_text.setPosition(left_top_view_pos.x, left_top_view_pos.y);

    while (true){
        sf::Event evnt;
        while (window.pollEvent(evnt)){
            switch (evnt.type){
                case sf::Event::Closed:
                    return -1;

                case sf::Event::KeyPressed:
                    if (evnt.key.code == sf::Keyboard::Escape){
                        std::cout << "You've stopped the game on the " << gen << " generation" << std::endl;
                        grid.clear();
                        return PATTERN_MENU_SCREEN;
                    }
                    else if (evnt.key.code == sf::Keyboard::Enter){ // Resets game
                        std::cout << "You've stopped the game on the " << gen << " generation" << std::endl;
                        grid.clear();
                        return PATTERN_INPUT_SCREEN;
                    }
                    else if (evnt.key.code == sf::Keyboard::X) timestep = std::max(25, timestep - 25); // Speed up
                    else if (evnt.key.code == sf::Keyboard::Z) timestep = std::min(700, timestep + 25); // Speed down
                    break;

                case sf::Event::MouseButtonPressed:
                    if (evnt.mouseButton.button != sf::Mouse::Left) break;

                    old_pos = sf::Vector2i(evnt.mouseButton.x, evnt.mouseButton.y);
                    clicking = true;
                    break;

                case sf::Event::MouseButtonReleased:{
                    if (evnt.mouseButton.button != sf::Mouse::Left) break;

                    clicking = false;
                    break;
                }

                case sf::Event::MouseMoved:{
                    if (!clicking) break;

                    sf::Vector2i new_pos = sf::Vector2i(evnt.mouseMove.x, evnt.mouseMove.y);
                    handleDrag(old_pos, new_pos);
                    gen_text.setPosition(left_top_view_pos.x, left_top_view_pos.y);
                    break;
                }

                case sf::Event::Resized:
                    resize(evnt);
                    break;
            }
        }

        int delta_time = key_press_clock.restart().asMilliseconds();
        if (window.hasFocus()){
            checkForChangeViewWithKeys(delta_time);
            gen_text.setPosition(left_top_view_pos.x, left_top_view_pos.y);
        }

        /* Using 'sleep' would lag other events like resizing or restarting the grid (since 'sleep' pauses the entire program for its duration).
        A better solution is to use 'sf::clock', which lets us check for elapsed time.
        When that elapsed time is bigger than our 'TIMESTEP' duration, we reset the clock and redraw the grid. */
        if (sf::milliseconds(timestep) <= timestep_clock.getElapsedTime()){
            timestep_clock.restart();
            gen++;
            gen_text.setString("gen: " + std::to_string(gen));

            // Note to self: this is the most expensive function in an iteration, and the only one with runtime dependent on amount of live cells.
            // It takes about 90% of an iteration's runtime.
            updateGrid();
            if (grid.empty()){ // If grid clears itself, we begin to get input again.
                std::cout << "This pattern lived for " << gen << " generations." << std::endl;

                return PATTERN_INPUT_SCREEN;
            }
        }

        window.clear(dead_cell_color);
        drawGrid();
        window.draw(gen_text);
        window.display();
    }
}