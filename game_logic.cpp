#include <SFML/Graphics.hpp>
#include <iostream>
#include <limits>
#include "game_logic.h"

// Returns number chosen by user.
short int introduction(){
    std::cout << "Welcome to 'game of life'. Below are starting modes:\n" << std::endl;
    std::cout << "1. Free mode\n" << std::endl;

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
bool drawGrid(sf::RenderWindow& window, std::vector< std::vector<sf::RectangleShape> >& grid, int grid_size){
    bool is_grid_blank = true;

    for (int i = 0; i < grid.size(); i++) {
        for (int j = 0; j < grid[0].size(); j++) {
            // Set cell position based on its grid coordinates
            grid[i][j].setPosition(j * grid_size, i * grid_size);
            window.draw(grid[i][j]);

            if (grid[i][j].getFillColor() == LIVE_CELL_COLOR) is_grid_blank = false;
        }
    }

    return is_grid_blank;
}

static void handleLeftClick(sf::RenderWindow& window, std::vector< std::vector<sf::RectangleShape> >& grid,
                            int window_width, int window_height, int grid_size){
    sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);

    // We only register clicks inside the window
    if (0 <= mouse_pos.x && mouse_pos.x < window_width && 0 <= mouse_pos.y && mouse_pos.y < window_height) {
        // Converting raw coordinate to cell location in the grid
        // This is how we get from an exact coordinate to the cell it's in
        mouse_pos.x /= grid_size;
        mouse_pos.y /= grid_size;

        // We allow selecting and deselecting cells
        if (grid[mouse_pos.y][mouse_pos.x].getFillColor() == DEAD_CELL_COLOR){
            grid[mouse_pos.y][mouse_pos.x].setFillColor(LIVE_CELL_COLOR);
        }
        else{
            grid[mouse_pos.y][mouse_pos.x].setFillColor(DEAD_CELL_COLOR);
        }
    }
}

// This function get initial pattern from user, and then is being called everytime the grid gets blank
void getUserInput(sf::RenderWindow& window, std::vector< std::vector<sf::RectangleShape> >& grid,
                  int window_width, int window_height, int grid_size){
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
                    break;

                    // Getting input from mouse
                case sf::Event::MouseButtonPressed:
                    if (evnt.mouseButton.button == sf::Mouse::Left){
                        system("cls"); // clear screen from previous generations
                        handleLeftClick(window, grid, window_width, window_height, grid_size);
                    }
                    break;
            }
        }

        window.clear();
        drawGrid(window, grid, grid_size);
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
                if (grid.at(k).at(p).getFillColor() == LIVE_CELL_COLOR) count++;
            }
            catch(const std::out_of_range& ex) { }
        }
    }

    return count;
}

// Update the grid to next generation.
// Note that we can't do the changes in-place, so we write all the changes to a boolean matrix, then copy it back to the original grid.
// Essentially, this is like double buffering
void updateGrid(std::vector< std::vector<sf::RectangleShape> >& grid){
    std::vector< std::vector<bool> > new_locations(grid.size(), std::vector<bool>(grid[0].size(), false));

    // Writing changes to boolean matrix
    for (int i = 0; i < grid.size(); i++) {
        for (int j = 0; j < grid[0].size(); j++) {
            short int num_of_neighbors = numOfNeighbors(grid, i, j);

            if (grid[i][j].getFillColor() == LIVE_CELL_COLOR){ // Cell is alive
                if (num_of_neighbors <= 1 || 4 <= num_of_neighbors) new_locations[i][j] = false;
                else new_locations[i][j] = true;
            }
            else{ // Cell is dead
                if (num_of_neighbors == 3) new_locations[i][j] = true;
            }
        }
    }

    // Copying back to original grid
    for (int i = 0; i < grid.size(); i++) {
        for (int j = 0; j < grid[0].size(); j++) {
            grid[i][j].setFillColor(new_locations[i][j] ? LIVE_CELL_COLOR : DEAD_CELL_COLOR);
        }
    }
}