#include "screens.h"

AutomatonMenuScreen::AutomatonMenuScreen(){
    map_born =
            {{"Game of Life",       {3}},
             {"Custom rulestring",   {}},
             {"Life without Death", {3}},
             {"HighLife",           {3, 6}},
             {"Day and Night",      {3, 6, 7, 8}},
             {"Seeds",              {2}},
             {"H-trees",            {1}},
             {"Replicator",         {1, 3, 5, 7}},
             {"Live Free or Die",   {2}},
             {"Serviettes",         {2, 3, 4}},
             {"Iceballs",           {2, 5, 6, 7, 8}},
             {"DotLife",            {3}},
             {"Flock",              {3}},
             {"Maze",               {3}},
             {"Coral",              {3}},
             {"Grounded Life",      {3, 5}},
             {"Pseudo Life",        {3, 5, 7}},
             {"Morley",             {3, 6, 8}}};

    map_survive =
            {{"Game of Life",       {2, 3}},
             {"Custom rulestring",  {}},
             {"Life without Death", {0, 1, 2, 3, 4, 5, 6, 7, 8}},
             {"HighLife",           {2, 3}},
             {"Day and Night",      {3, 4, 6, 7, 8}},
             {"Seeds",              {}},
             {"H-trees",            {0, 1, 2, 3, 4, 5, 6, 7, 8}},
             {"Replicator",         {1, 3, 5, 7}},
             {"Live Free or Die",   {0}},
             {"Serviettes",         {}},
             {"Iceballs",           {5, 6, 7, 8}},
             {"DotLife",            {0, 2, 3}},
             {"Flock",              {1, 2}},
             {"Maze",               {1, 2, 3, 4, 5}},
             {"Coral",              {4, 5, 6, 7, 8}},
             {"Grounded Life",      {2, 3}},
             {"Pseudo Life",        {2, 3, 8}},
             {"Morley",             {2, 4, 5}}};

    // I want 'Game of life' to be 1st on 'menu_options', and 'Custom' to be 2nd
    // (they're not first and second in 'map_born', so we do it manually)
    menu_options.emplace_back("1. Game of Life", font, CHARACTER_SIZE);
    menu_options.emplace_back("2. Custom rulestring", font, CHARACTER_SIZE);
    int index = 3;
    for (const auto& pair : map_born){
        if (pair.first == "Game of Life" || pair.first == "Custom rulestring") continue;
        menu_options.emplace_back(std::to_string(index) + ". " + pair.first, font, CHARACTER_SIZE);
        index++;
    }
}

short int AutomatonMenuScreen::run(){
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
        // If mouse pos isn't inside window, then we assign 'rectangle_index' some arbitrary value as to not segfault
        int rectangle_index = inside_window ? view_pos.y / menu_option_rectangle_height : 0;
        sf::Text* menu_option_from_position = &menu_options[rectangle_index];
        float menu_option_from_position_left = menu_option_from_position->getGlobalBounds().left;
        float menu_option_from_position_width = menu_option_from_position->getGlobalBounds().width;

        sf::Event evnt;
        while (window.pollEvent(evnt)){
            switch (evnt.type){
                case sf::Event::Closed:
                    return -1;

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

                    if (menu_option_from_position_left <= view_pos.x && view_pos.x < menu_option_from_position_left + menu_option_from_position_width){
                        cursor.loadFromSystem(sf::Cursor::Arrow);
                        window.setMouseCursor(cursor);

                        std::string automaton_name = menu_option_from_position->getString();
                        automaton_name = automaton_name.substr(automaton_name.find_last_of('.') + 2); // Removes number at the start

                        hovered_menu_option->setFillColor(option_not_chosen_color); // Reset color of chosen menu option

                        if (rectangle_index != 1){ // Not custom rulestring
                            born_digits = map_born[automaton_name];
                            survive_digits = map_survive[automaton_name];
                            return PATTERN_MENU_SCREEN;
                        }
                        else return RULESTRING_MENU_SCREEN;
                    }
                    break;
                }

                case sf::Event::GainedFocus: // TODO: use hasFocus()!
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
            // Start hovering - only if it's not a directory name
            else if (menu_option_from_position_left <= view_pos.x && view_pos.x < menu_option_from_position_left + menu_option_from_position_width){
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