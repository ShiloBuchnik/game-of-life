#include <SFML/Graphics.hpp>
#include <iostream>
#include <limits>
#include "game_logic.h"

// Returns number chosen by user.
short int introduction(){
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

// Draws the grid. Returns true iff grid is blank.
bool drawGrid(sf::RenderWindow& window, std::vector< std::vector<sf::RectangleShape> >& grid){
    bool is_grid_blank = true;

    for (int i = 0; i < grid.size(); i++) {
        for (int j = 0; j < grid[0].size(); j++) {
            // Set cell position based on its grid coordinates
            grid[i][j].setPosition(j * CELL_SIZE, i * CELL_SIZE);
            window.draw(grid[i][j]);

            if (grid[i][j].getFillColor() == LIVE_CELL_COLOR) is_grid_blank = false;
        }
    }

    return is_grid_blank;
}

static void handleLeftClick(sf::RenderWindow& window, std::vector< std::vector<sf::RectangleShape> >& grid) {
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
        if (grid[view_pos.y][view_pos.x].getFillColor() == DEAD_CELL_COLOR){
            grid[view_pos.y][view_pos.x].setFillColor(LIVE_CELL_COLOR);
        }
        else{
            grid[view_pos.y][view_pos.x].setFillColor(DEAD_CELL_COLOR);
        }
    }
}

// This function get initial pattern from user, and then is being called everytime the grid gets blank
void getUserInput(sf::RenderWindow& window, sf::View& view, std::vector< std::vector<sf::RectangleShape> >& grid){
    while (window.isOpen()) {
        sf::Event evnt;
        while (window.pollEvent(evnt)) {
            switch(evnt.type) {
                case sf::Event::Closed:
                    window.close();
                    exit (0);
                    break;

                case sf::Event::KeyPressed:
                    if (evnt.key.code == sf::Keyboard::Escape){
                        window.close();
                        exit(0);
                    }

                    // End of user input stage
                    if (evnt.key.code == sf::Keyboard::Enter) return;

                    if (evnt.key.code == sf::Keyboard::W || evnt.key.code == sf::Keyboard::A || evnt.key.code == sf::Keyboard::S || evnt.key.code == sf::Keyboard::D){
                        changeView(window, view, evnt);
                    }
                    break;

                    // Getting input from mouse
                case sf::Event::MouseButtonPressed:
                    if (evnt.mouseButton.button == sf::Mouse::Left){
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

// Blanking the grid
void blankGrid(std::vector< std::vector<sf::RectangleShape> >& grid){
    for (int i = 0; i < grid.size(); i++) {
        for (int j = 0; j < grid[0].size(); j++) {
            grid[i][j].setFillColor(DEAD_CELL_COLOR);
        }
    }
}

// Returns number of neighbors in the cell's Moore's environment
inline short int numOfNeighbors(std::vector< std::vector<sf::RectangleShape> >& grid, int i, int j){
    short int count = 0;

    for (int k = i - 1; k <= i + 1; k++){
        for (int p = j - 1; p <= j + 1; p++){
            if (k == i && p == j) continue; // We don't count the (i,j) cell

            // We use 'at()' to not go out of bounds when we check the environment
            try{
                const sf::Color& color = grid.at(k).at(p).getFillColor();
                if (color == LIVE_CELL_COLOR || color == sf::Color::White) count++;
            }
            catch(const std::out_of_range& ex) { }
        }
    }

    return count;
}

// Update the grid to next generation.
// We're doing it in-place. Since the board is updated simultaneously, we need a way to know whether a cell was dead or alive before we changed it.
// If a live cell becomes dead, we color it white; and if a dead cell becomes alive, we color it black.
// Now we know - a live cell is either red or white, and a dead cell is either grey or black.
// Based on that we can reassign the red and grey values to the grid (which happens in the second 'for' loop).

// Note that we can't use a sparse matrix (set or map), because we need to display every cell - dead or alive.
void updateGrid(std::vector< std::vector<sf::RectangleShape> >& grid){
    // Writing changes to boolean matrix
    for (int i = 0; i < grid.size(); i++) {
        for (int j = 0; j < grid[0].size(); j++) {
            short int num_of_neighbors = numOfNeighbors(grid, i, j);

            if (grid[i][j].getFillColor() == LIVE_CELL_COLOR || grid[i][j].getFillColor() == sf::Color::White){ // Cell is alive
                if (num_of_neighbors <= 1 || 4 <= num_of_neighbors) grid[i][j].setFillColor(sf::Color::White); // Cell becomes dead
            }
            else{ // Cell is dead
                if (num_of_neighbors == 3) grid[i][j].setFillColor(sf::Color::Black); // Cell becomes alive
            }
        }
    }

    for (int i = 0; i < grid.size(); i++) {
        for (int j = 0; j < grid[0].size(); j++) {
            const sf::Color& color = grid[i][j].getFillColor();
            if (color == sf::Color::White) grid[i][j].setFillColor(DEAD_CELL_COLOR);
            else if (color == sf::Color::Black) grid[i][j].setFillColor(LIVE_CELL_COLOR);
        }
    }
}