#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include "global_macros.h"

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

// Draws a single cell in a grid
inline void drawCell(sf::RenderWindow& window, std::vector< std::vector<sf::RectangleShape> >& grid, int x, int y, int grid_size){
    grid[y][x].setPosition(x * grid_size, y * grid_size);
    window.draw(grid[y][x]);
}

// Draws the grid. Returns true iff grid is blank.
bool drawGrid(sf::RenderWindow& window, std::vector< std::vector<sf::RectangleShape> >& grid, int grid_size){
    bool is_grid_blank = true;

    for (int i = 0; i < grid.size(); i++) {
        for (int j = 0; j < grid[0].size(); j++) {
            // Set cell position based on its grid coordinates
            drawCell(window, grid, j, i, grid_size);

            if (grid[i][j].getFillColor() == LIVE_CELL_COLOR) is_grid_blank = false;
        }
    }

    return is_grid_blank;
}

void blankGrid(std::vector< std::vector<sf::RectangleShape> >& grid){
    for (int i = 0; i < grid.size(); i++) {
        for (int j = 0; j < grid[0].size(); j++) {
            grid[i][j].setFillColor(DEAD_CELL_COLOR);
        }
    }
}

void handleLeftClick(sf::RenderWindow& window, std::vector< std::vector<sf::RectangleShape> >& grid,
                     int window_width, int window_height, int grid_size){
    sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);

    if (0 <= mouse_pos.x && mouse_pos.x < window_width && 0 <= mouse_pos.y && mouse_pos.y < window_height) {
        mouse_pos.x /= grid_size;
        mouse_pos.y /= grid_size;

        if (grid[mouse_pos.y][mouse_pos.x].getFillColor() == DEAD_CELL_COLOR){
            grid[mouse_pos.y][mouse_pos.x].setFillColor(LIVE_CELL_COLOR);
        }
        else{
            grid[mouse_pos.y][mouse_pos.x].setFillColor(DEAD_CELL_COLOR);
        }
    }

    drawCell(window, grid, mouse_pos.x, mouse_pos.y, grid_size);
    window.display();
}

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
                    // Pressing 'escape' closes window and terminates program
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