#include <iostream>
#include <filesystem>
#include <fstream>
#include "screens.h"

void PatternMenuScreen::truncateFileNameIfTooLong(sf::Text& text){
    std::string text_str = text.getString();
    size_t i = text_str.size() - 1;
    for (; 0 <= i; i--){
        // 100 is an arbitrary number so filenames at screen width wouldn't be allowed
        if (text.findCharacterPos(i + 1).x < window.getSize().x - 100) break;
    }

    text_str.erase(i + 1, std::string::npos);
    if (i != text.getString().getSize() - 1) text_str.append("..."); // If we've entered, that means we've truncated
    text.setString(text_str);
}

// The function saves all .rle filenames in 'pattern' directory in 'menu_options',
// and saves the corresponding path to father directory in 'num_to_pattern_path'.
void PatternMenuScreen::iterateOverPatternDirectory(){
    unsigned long long int index = 2;

    // 'directory_iterator' is on one hand a container, and on other hand an iterator...very confusing.
    // Anyway, it stores all the files in given 'dir_path'.
    for (const auto& pattern_type_dir : std::filesystem::directory_iterator("patterns")){ // Iterating over pattern *types*.
        // If it's not a directory, or it's an empty directory, we continue.
        if (!std::filesystem::is_directory(pattern_type_dir) || std::filesystem::is_empty(pattern_type_dir)) continue;
        // 'pattern_type_dir.path()' returns relative path, 'relative()' returns it without the dirname at the start,
        // 'stem()' stems the file extension, and 'string()', well, returns the string version of the 'path' object.
        menu_options.emplace_back(relative(pattern_type_dir.path(), "patterns").stem().string() + ":", font, OPTION_CHARACTER_SIZE);
        menu_options.back().setFillColor(important_color);
        menu_options.back().setStyle(sf::Text::Bold);
        truncateFileNameIfTooLong(menu_options.back());
        menu_options_pattern_paths.emplace_back("");

        for (const auto& entry : std::filesystem::directory_iterator(pattern_type_dir)){ // Iterating over patterns themselves.
            // If it's not an .rle file, we continue.
            if (entry.path().extension().string() != ".rle") continue;

            menu_options.emplace_back(std::to_string(index) + ". " + relative(entry.path(), pattern_type_dir).stem().string(), font, OPTION_CHARACTER_SIZE);
            truncateFileNameIfTooLong(menu_options.back());

            menu_options_pattern_paths.push_back(entry.path().parent_path().string());
            index++;
        }
    }
}

PatternMenuScreen::PatternMenuScreen(){
    // 0th item is "custom pattern", so obviously it has no path.
    // Still writing something, so I can identify pattern directories later.
    menu_options_pattern_paths = {"Custom"};
    menu_options.emplace_back("1. Input your own custom pattern", font, OPTION_CHARACTER_SIZE);

    // If 'patterns' directory doesn't exist, it creates it; otherwise, it does nothing.
    // Calling 'iterateOverPatternDirectory()' in 'run()' (to reflect changes made to files while the program runs) hurts performance,
    // as it's an expensive function.
    // Instead, we call it once (before the menus are showed), and if the user deletes/moves the files, we simply exit
    std::filesystem::create_directories("patterns");
    iterateOverPatternDirectory();

    menu_title.setString("Pattern Menu");
    menu_screen_total_height = menu_title_rectangle_height + menu_options.size() * menu_option_rectangle_height;
}

sf::Vector2i PatternMenuScreen::centerOfMass() const{
    if (chosen_pattern.empty()) return {0,0}; // If pattern is empty, then there is no center of mass.

    int sum_x = 0, sum_y = 0, cell_amount = chosen_pattern.size();

    for (const auto& cell : chosen_pattern){
        sum_x += cell.x;
        sum_y += cell.y;
    }

    return {sum_x / cell_amount, sum_y / cell_amount};
}

// RLE parser: Takes an .rle file and fill 'pattern' according to its content.
void PatternMenuScreen::RLEToGrid(){
    std::ifstream rle_file; // read-only
    rle_file.open(chosen_file_path);
    if (rle_file.fail()){
        std::cerr << "File has been deleted or moved since start of program" << std::endl;
        exit(-1);
    }

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
                chosen_pattern.insert({curr_x + i, curr_y});
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
            std::cerr << "bad RLE file, terminating..." << std::endl;
            exit(-1);
        }
    }

    rle_file.close();
}

// Takes the pattern from the .rle file specified in 'chosen_file_path', and put in center of grid.
void PatternMenuScreen::putPatternInGrid(){
    /* We can't put the pattern in 'grid' and then modify the elements, since unordered_set's elements are const (if they change, the hash changes).
    And we can't settle for erasing and insert when iterating over 'grid', since doing that will invalidate the iterators.
    Sadly, we must create a temp and then copy its modified elements to 'grid'. */
    RLEToGrid();

    int cells_count_x = grid_width / CELL_SIZE, cells_count_y = grid_height / CELL_SIZE;
    sf::Vector2i center_of_mass = centerOfMass();

    for (const auto& cell : chosen_pattern){
        // We shift the pattern to the grid's center, and shift it to account for its center of mass.
        int x = cell.x + cells_count_x / 2 - center_of_mass.x;
        int y = cell.y + cells_count_y / 2 - center_of_mass.y;

        grid.insert({x, y});
    }
}

short int PatternMenuScreen::run(){
    chosen_pattern.clear();
    left_top_view_pos = sf::Vector2f(0.f,0.f);
    setInitialView();
    setText();
    setArrows();

    bool hovering = false;
    int rectangle_index;
    sf::IntRect hovered_rectangle_bounds;
    sf::Text* hovered_menu_option = &menu_options[0];

    while (true) {
        handleHover(hovering, rectangle_index, &menu_options_pattern_paths, hovered_menu_option, hovered_rectangle_bounds);

        sf::Event evnt;
        while (window.pollEvent(evnt)){
            switch (evnt.type){
                case sf::Event::Closed:
                    return -1;

                case sf::Event::KeyPressed:
                    if (evnt.key.code == sf::Keyboard::Escape){
                        // When we escape, if we hover, we want to un-hover. If we don't hover, this essentially does nothing
                        hovered_menu_option->setFillColor(option_not_chosen_color);
                        cursor.loadFromSystem(sf::Cursor::Arrow);
                        window.setMouseCursor(cursor);

                        return AUTOMATON_MENU_SCREEN;
                    }
                    break;

                case sf::Event::MouseWheelScrolled:{
                    handleScroll(evnt.mouseWheelScroll.delta);
                    break;
                }

                case sf::Event::MouseButtonReleased:{
                    if (evnt.mouseButton.button != sf::Mouse::Left) break;
                    if (!hovering) break;

                    cursor.loadFromSystem(sf::Cursor::Arrow);
                    window.setMouseCursor(cursor);

                    std::string pattern_name = textToString(hovered_menu_option);
                    chosen_file_path = menu_options_pattern_paths[rectangle_index] + "\\" + pattern_name + ".rle";

                    hovered_menu_option->setFillColor(option_not_chosen_color);

                    // Setting so that the exact center of the grid is in the center of the window
                    left_top_view_pos = sf::Vector2f(grid_width / 2 - window.getSize().x / 2, grid_height / 2 - window.getSize().y / 2);
                    setInitialView();

                    if (rectangle_index != 0){ // Not custom pattern
                        putPatternInGrid();
                        return GAME_SCREEN;
                    }
                    else return PATTERN_INPUT_SCREEN;
                }

                case sf::Event::Resized:
                    resize(evnt, menu_screen_total_height);
                    setText();
                    arrow_down_sprite.setPosition(0, left_top_view_pos.y + window.getSize().y - arrow_down_sprite.getGlobalBounds().height);
                    break;
            }
        }

        window.clear();

        drawText();
        window.draw(menu_title);
        drawArrows();

        window.display();
    }
}