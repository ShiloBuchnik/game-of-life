#include <SFML/Graphics.hpp>
#include <iostream>
#include <limits>
#include <unordered_set>
#include <unordered_map>
#include "game_logic.h"

// Returns number chosen by user.
short int introduction() {
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
    while (std::cin.fail() || mode_num < 1 || 28 < mode_num) {
        std::cout << "Invalid input, please try again: ";
        std::cin.clear(); // Clears error flag. If it's raised, cin won't get input.
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clears buffer
        std::cin >> mode_num;
    }
    system("cls");

    return mode_num;
}

// Draws the grid. Returns true iff grid is blank.
void drawGrid(sf::RenderWindow& window, std::unordered_set<sf::Vector2i, pair_hash, pair_equal>& grid) {
    sf::RectangleShape cell(sf::Vector2f(CELL_SIZE, CELL_SIZE));
    cell.setOutlineColor(sf::Color(200, 200, 200)); // Beige
    cell.setOutlineThickness(1.25);
    for (int i = 0; i < GRID_HEIGHT / CELL_SIZE; i++) {
        for (int j = 0; j < GRID_WIDTH / CELL_SIZE; j++) {
            if (grid.count({j,i})) cell.setFillColor(LIVE_CELL_COLOR);
            else cell.setFillColor(DEAD_CELL_COLOR);

            // Set cell position based on its grid coordinates.
            cell.setPosition(j * CELL_SIZE, i * CELL_SIZE);
            window.draw(cell);
        }
    }
}

static void handleLeftClick(sf::RenderWindow& window, std::unordered_set<sf::Vector2i, pair_hash, pair_equal>& grid) {
    /* Even when changing the view, *the objects themselves always remain in the same place in the "world"*.
    It's literally like a camera in a video game - it changes our perspective, but doesn't change the world.
    For example, if we click on a certain pixel, move the view (without moving the mouse), and click again,
    *SFML would register that as a click on that exact same pixel*.
    That is because it remained in the same place in the world, and our view is the only thing that changed.
    So, essentially, we have the location in the real world and location in the current view.

    What we do below is transforming click coordinates (which are always the literal pixels on the screen),
    to the "relative" or "view" coordinates, which compensate for the fact that our view is moved from the real world objects.
    It's easier to understand with an example:
    If our cursor is on cell (1,1), and we move the view by pressing "D" (right) and "S" (down), and then click;
    We need to a way to "tell SFML" to click on cell (2,2), and not cell (1,1) (which is off-screen now).
    So we use 'mapPixelToCoords()', which takes a literal position on the window, fixes it according to the view, and returns it. */
    sf::Vector2i pixel_pos = sf::Mouse::getPosition(window); // Pixels are integers
    sf::Vector2i view_pos = static_cast<sf::Vector2i> (window.mapPixelToCoords(pixel_pos)); // Coordinates can be floats, but we cast to ints

    // We only register clicks inside the grid. Note that we use the grid's width and height, and not the window's, as it contains the window.
    if (0 <= view_pos.x && view_pos.x < GRID_WIDTH && 0 <= view_pos.y && view_pos.y < GRID_HEIGHT) {
        // Converting raw coordinate to cell location in the grid
        // This is how we get from an exact coordinate to the cell it's in
        view_pos.x /= CELL_SIZE;
        view_pos.y /= CELL_SIZE;

        // We allow selecting and deselecting cells
        if (grid.count(view_pos)) grid.erase(view_pos);
        else grid.insert(view_pos);
    }
}

// This function get initial pattern from user, and then is being called everytime the grid gets blank
void getUserInput(sf::RenderWindow& window, sf::View& view, std::unordered_set<sf::Vector2i, pair_hash, pair_equal>& grid) {
    while (window.isOpen()) {
        sf::Event evnt;
        while (window.pollEvent(evnt)) {
            switch(evnt.type) {
                case sf::Event::Closed:
                    window.close();
                    exit (0);
                    break;

                case sf::Event::KeyPressed:
                    if (evnt.key.code == sf::Keyboard::Escape) {
                        window.close();
                        exit(0);
                    }

                    // End of user input stage
                    if (evnt.key.code == sf::Keyboard::Enter) return;

                    if (evnt.key.code == sf::Keyboard::W || evnt.key.code == sf::Keyboard::A || evnt.key.code == sf::Keyboard::S || evnt.key.code == sf::Keyboard::D) {
                        changeView(window, view, evnt);
                    }
                    break;

                // Getting input from mouse
                case sf::Event::MouseButtonPressed:
                    if (evnt.mouseButton.button == sf::Mouse::Left) {
                        system("cls"); // clear terminal screen from previous generations
                        handleLeftClick(window, grid);
                    }
                    break;

                case sf::Event::Resized: {
                    sf::Vector2i top_left_view_pos = static_cast<sf::Vector2i> (window.mapPixelToCoords(sf::Vector2i(0, 0)));
                    view.reset(sf::FloatRect(top_left_view_pos.x, top_left_view_pos.y, evnt.size.width, evnt.size.height));
                    window.setView(view);
                    break;
                }
            }
        }

        window.clear();
        drawGrid(window, grid);
        window.display();
    }
}

static void addNeighbors(std::unordered_set<sf::Vector2i, pair_hash, pair_equal>& grid,
                         std::unordered_map<sf::Vector2i, int, pair_hash, pair_equal>& m, const sf::Vector2i& coordinate) {
    for (int k = coordinate.y - 1; k <= coordinate.y + 1; k++) {
        for (int p = coordinate.x - 1; p <= coordinate.x + 1; p++) {
            // Bound checking, so we don't add a cell beyond bounds.
            if (0 <= k && k <= GRID_HEIGHT / CELL_SIZE && 0 <= p && p <= GRID_WIDTH / CELL_SIZE) m[{p, k}]++;
        }
    }
}

// Update the grid to next generation.
// We're doing it in-place. Since the board is updated simultaneously, we need a way to know whether a cell was dead or alive before we changed it.
// If a live cell becomes dead, we color it white; and if a dead cell becomes alive, we color it black.
// Now we know - a live cell is either red or white, and a dead cell is either grey or black.
// Based on that we can reassign the red and grey values to the grid (which happens in the second 'for' loop).

// Note that we can't use a sparse matrix (set or map), because we need to display every cell - dead or alive.
void updateGrid(std::unordered_set<sf::Vector2i, pair_hash, pair_equal>& grid) {
    std::unordered_map<sf::Vector2i, int, pair_hash, pair_equal> live_and_neighbors;

    for (auto coordinate : grid) {
        addNeighbors(grid, live_and_neighbors, coordinate);
    }

    std::unordered_set<sf::Vector2i, pair_hash, pair_equal> next_gen_live_cells;
    for (auto map_pair : live_and_neighbors) {
        if (map_pair.second == 3 || (map_pair.second == 4 && grid.count(map_pair.first))) next_gen_live_cells.insert(map_pair.first);
    }

    grid.swap(next_gen_live_cells);
}