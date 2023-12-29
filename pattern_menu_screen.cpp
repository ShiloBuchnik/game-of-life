#include <filesystem>
#include <fstream>
#include "screens.h"

PatternMenuScreen::PatternMenuScreen(){
    // 0th item is "custom pattern", so obviously it has no path.
    // Still writing something, so I can identify pattern directories later.
    menu_options_pattern_paths = {"Custom"};
    menu_options.emplace_back("1. Input your own custom pattern", font, CHARACTER_SIZE);

    // If 'patterns' directory doesn't exist, it creates it; otherwise, it does nothing.
    // Calling 'iterateOverPatternDirectory()' in 'run()' (to reflect changes made to files while the program runs) hurts performance,
    // as it's an expensive function.
    // Instead, we call it once (before the menus are showed), and if the user deletes/moves the files, we simply exit
    std::filesystem::create_directories("patterns");
    iterateOverPatternDirectory();
}

sf::Vector2i PatternMenuScreen::centerOfMass(){
    if (pattern.empty()) return {0,0}; // If pattern is empty, then there is no center of mass.

    int sum_x = 0, sum_y = 0, cell_amount = pattern.size();

    for (const auto& cell : pattern){
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
                pattern.insert({curr_x + i, curr_y});
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

    for (const auto& cell : pattern){
        // We shift the pattern to the grid's middle, and shift it to account for center of mass.
        int x = cell.x + cells_count_x / 2 - center_of_mass.x;
        int y = cell.y + cells_count_y / 2 - center_of_mass.y;

        grid.insert({x, y});
    }
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
        menu_options.emplace_back(relative(pattern_type_dir.path(), "patterns").stem().string() + ":", font, CHARACTER_SIZE);
        menu_options.back().setFillColor(sf::Color::Magenta);
        menu_options.back().setStyle(sf::Text::Bold | sf::Text::Underlined);
        truncateFileNameIfTooLong(menu_options.back());
        menu_options_pattern_paths.emplace_back("");

        for (const auto& entry : std::filesystem::directory_iterator(pattern_type_dir)){ // Iterating over patterns themselves.
            // If it's not an .rle file, we continue.
            if (entry.path().extension().string() != ".rle") continue;

            menu_options.emplace_back(std::to_string(index) + ". " + relative(entry.path(), pattern_type_dir).stem().string(), font, CHARACTER_SIZE);
            truncateFileNameIfTooLong(menu_options.back());

            menu_options_pattern_paths.push_back(entry.path().parent_path().string());
            index++;
        }
    }
}


void PatternMenuScreen::truncateFileNameIfTooLong(sf::Text& text){
    std::string text_str = text.getString();
    size_t i = text_str.size() - 1;
    for (; 0 <= i; i--){
        // 100 is an arbitrary number so the filename at screen width won't be allowed
        if (text.findCharacterPos(i + 1).x < window_width - 100) break;
    }

    text_str.erase(i + 1, std::string::npos);
    if (i != text.getString().getSize() - 1) text_str.append("..."); // If we've entered, that means we've truncated
    text.setString(text_str);
}

short int PatternMenuScreen::run(){
    left_top_view_pos = sf::Vector2i(0,0);
    setInitialView();

    setText();
    float menu_screen_total_height = menu_options.size() * menu_option_rectangle_height;

    bool hovering = false, focus = true, inside_window;
    sf::FloatRect hovered_rectangle_size;
    sf::Text* hovered_menu_option;

    while (true) {
        // Convert mouse position (if it's inside window) to menu option it hovers above
        sf::Vector2i pixel_pos = sf::Mouse::getPosition(window);
        inside_window = 0 <= pixel_pos.x && pixel_pos.x < window_width && 0 <= pixel_pos.y && pixel_pos.y < window_height;
        sf::Vector2f view_pos = window.mapPixelToCoords(pixel_pos);
        // If mouse pos isn't inside window, we assign 'rectangle_index' some arbitrary value as to not segfault
        int rectangle_index = inside_window ? view_pos.y / menu_option_rectangle_height : 0;
        sf::Text* menu_option_from_position = &menu_options[rectangle_index];
        float menu_option_from_position_left = menu_option_from_position->getGlobalBounds().left;
        float menu_option_from_position_width = menu_option_from_position->getGlobalBounds().width;

        sf::Event evnt;
        while (window.pollEvent(evnt)){
            switch (evnt.type){
                case sf::Event::Closed:
                    return -1;

                case sf::Event::KeyPressed:
                    // The program exits when user presses 'esc'.
                    if (evnt.key.code == sf::Keyboard::Escape){
                        // When we escape, we might not hover on an option, so we use 'menu_option_from_position' instead of 'hovered_menu_option'
                        // If cursor is indeed not on an option, then this essentially does nothing
                        // Note that we check in the 'if' condition that it's not on a directory text, since those are not hoverable to begin with
                        if (!menu_options_pattern_paths[rectangle_index].empty()){
                            menu_option_from_position->setFillColor(option_not_chosen_color);
                        }

                        cursor.loadFromSystem(sf::Cursor::Arrow);
                        window.setMouseCursor(cursor);

                        return AUTOMATON_MENU_SCREEN;
                    }
                    break;

                case sf::Event::MouseWheelScrolled:{
                    float delta = 30 * evnt.mouseWheelScroll.delta;

                    if (0 < delta && left_top_view_pos.y - delta < 0){ // Scroll up beyond bound - we don't allow it
                        left_top_view_pos.y = 0;
                    }
                    else if (delta < 0 && menu_screen_total_height < left_top_view_pos.y + window_height - delta){ // Scroll down beyond bound - we don't allow it
                        left_top_view_pos.y = menu_screen_total_height - window_height;
                    }
                    else{ // Legal scroll
                        left_top_view_pos.y -= delta;
                    }

                    view.reset(sf::FloatRect(0, left_top_view_pos.y, window_width, window_height));
                    window.setView(view);
                    break;
                }

                case sf::Event::MouseButtonReleased:{
                    if (evnt.mouseButton.button != sf::Mouse::Left) break;

                    // If it's the empty string - that means it's a folder-name, and we don't register the click.
                    if (menu_options_pattern_paths[rectangle_index].empty()) break;

                    if (menu_option_from_position_left <= view_pos.x && view_pos.x < menu_option_from_position_left + menu_option_from_position_width){
                        cursor.loadFromSystem(sf::Cursor::Arrow);
                        window.setMouseCursor(cursor);

                        std::string pattern_name = menu_option_from_position->getString();
                        pattern_name = pattern_name.substr(pattern_name.find_last_of('.') + 2); // Removes number at the start
                        chosen_file_path = menu_options_pattern_paths[rectangle_index] + "\\" + pattern_name + ".rle";

                        hovered_menu_option->setFillColor(option_not_chosen_color);

                        left_top_view_pos = sf::Vector2i(window_width * (MULTIPLE / 2), window_height * (MULTIPLE / 2));
                        setInitialView();

                        if (rectangle_index != 0){ // Not custom pattern
                            putPatternInGrid();
                            return GAME_SCREEN;
                        }
                        else return PATTERN_INPUT_SCREEN;
                    }
                    break;
                }

                case sf::Event::GainedFocus:
                    focus = true;
                    break;
                case sf::Event::LostFocus:
                    focus = false;
                    break;

                case sf::Event::Resized:
                    resize(evnt);
                    break;
            }
        }

        if (focus && inside_window){
            if (hovering && !hovered_rectangle_size.contains(view_pos)){ // Un-hover
                hovering = false;
                hovered_menu_option->setFillColor(option_not_chosen_color);

                cursor.loadFromSystem(sf::Cursor::Arrow);
                window.setMouseCursor(cursor);
            }
            else if (!menu_options_pattern_paths[rectangle_index].empty() && // Start hovering - only if it's not a directory name
                     menu_option_from_position_left <= view_pos.x && view_pos.x < menu_option_from_position_left + menu_option_from_position_width){
                menu_option_from_position->setFillColor(option_chosen_color);
                hovering = true;
                hovered_rectangle_size = sf::FloatRect(
                        menu_option_from_position_left, rectangle_index * menu_option_rectangle_height, menu_option_from_position_width, menu_option_rectangle_height);
                hovered_menu_option = menu_option_from_position;

                cursor.loadFromSystem(sf::Cursor::Hand);
                window.setMouseCursor(cursor);
            }
        }

        window.clear();
        drawText();
        window.display();
    }
}