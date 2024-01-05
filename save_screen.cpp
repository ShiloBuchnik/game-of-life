#include <filesystem>
#include <fstream>
#include "screens.h"

SaveScreen::SaveScreen(): live_cell_diff(128, 0, 0, 192), dead_cell_diff(64, 64, 64, 192), outline_diff(100, 100, 100, 192) {
    save_prompt = sf::Text("Would you like to save this pattern in an .rle file? [Y/N] ", font, OPTION_CHARACTER_SIZE + 10);
    save_prompt.setFillColor(important_color);
}

// If 'x' is bigger than 1, returns its string version. Otherwise, returns empty string.
inline std::string biggerThan1(unsigned int x){
    return 1 < x ? std::to_string(x) : "";
}

inline void writeRulestringToFile(std::ofstream& rle_file, const std::set <short int>& born_digits, const std::set <short int>& survive_digits){
    rle_file << "rule = B";
    for (const auto& digit : born_digits){
        rle_file << std::to_string(digit);
    }
    rle_file << "/S";
    for (const auto& digit : survive_digits){
        rle_file << std::to_string(digit);
    }
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
std::string SaveScreen::gridToRLE(){
    std::ofstream rle_file; // write-only
    // If 'custom' directory doesn't exist, it creates it; otherwise, it does nothing.
    std::filesystem::create_directories("patterns\\custom");
    std::string available_file_path = findAvailableName();
    rle_file.open(available_file_path);

    // Edge case
    if (grid.empty()){
        rle_file << "x = 0, y = 0, ";
        writeRulestringToFile(rle_file, born_digits, survive_digits);
        rle_file << "\n!";
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
    rle_file << "x = " + std::to_string(pattern_width) + ", y = " + std::to_string(pattern_height) + ", ";

    writeRulestringToFile(rle_file, born_digits, survive_digits);
    rle_file << "\n";

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

void SaveScreen::dimOrBrightenScreen() const{
    static bool is_dim = false;

    if (is_dim){
        live_cell_color += live_cell_diff;
        dead_cell_color += dead_cell_diff;
        outline_color += outline_diff;
    }
    else{
        live_cell_color -= live_cell_diff;
        dead_cell_color -= dead_cell_diff;
        outline_color -= outline_diff;
    }

    is_dim = !is_dim;
}

short int SaveScreen::run(){
    save_prompt.setScale(zoom, zoom);
    centerText(save_prompt, left_top_view_pos.y + view.getSize().y / 4 - save_prompt.getGlobalBounds().height / 2);

    dimOrBrightenScreen();

    while (true){
        sf::Event evnt;
        while (window.pollEvent(evnt)){
            switch (evnt.type){
                case sf::Event::Closed:
                    return -1;

                case sf::Event::KeyPressed:
                    if (evnt.key.code == sf::Keyboard::Escape){
                        dimOrBrightenScreen();
                        grid.clear();
                        zoom = 1;
                        return PATTERN_MENU_SCREEN;
                    }
                    else if (evnt.key.code == sf::Keyboard::Y || evnt.key.code == sf::Keyboard::N){
                        if (evnt.key.code == sf::Keyboard::Y) gridToRLE();
                        dimOrBrightenScreen();
                        return GAME_SCREEN;
                    }
                    break;

                case sf::Event::Resized:
                    resize(evnt);
                    centerText(save_prompt, left_top_view_pos.y + view.getSize().y / 4 - save_prompt.getGlobalBounds().height / 2);

                    break;
            }
        }

        window.clear(dead_cell_color);
        drawGrid();
        window.draw(save_prompt);
        window.display();
    }
}