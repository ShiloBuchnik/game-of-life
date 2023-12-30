#include "screens.h"

AutomatonMenuScreen::AutomatonMenuScreen():
automaton_name_to_born_and_survive_sets{{"Game of Life",           {{3}, {2, 3}} },
                                        {"Custom rulestring",      {{}, {}} },
                                        {"Life without Death",     {{3}, {0, 1, 2, 3, 4, 5, 6, 7, 8}} },
                                        {"HighLife",               {{3, 6}, {2, 3}} },
                                        {"Day and Night",          {{3, 6, 7, 8}, {3, 4, 6, 7, 8}} },
                                        {"Seeds",                  {{2}, {}} },
                                        {"H-trees",                {{1}, {0, 1, 2, 3, 4, 5, 6, 7, 8}} },
                                        {"Replicator",             {{1, 3, 5, 7}, {1, 3, 5, 7}} },
                                        {"Live Free or Die",       {{2}, {0}} },
                                        {"Serviettes",             {{2, 3, 4}, {}} },
                                        {"Iceballs",               {{2, 5, 6, 7, 8}, {5, 6, 7, 8}} },
                                        {"DotLife",                {{3}, {0, 2, 3}} },
                                        {"Flock",                  {{3}, {1, 2}} },
                                        {"Maze",                   {{3}, {1, 2, 3, 4, 5}} },
                                        {"Coral",                  {{3}, {4, 5, 6, 7, 8}} },
                                        {"Grounded Life",          {{3, 5}, {2, 3}} },
                                        {"Pseudo Life",            {{3, 5, 7}, {2, 3, 8}} },
                                        {"Morley",                 {{3, 6, 8}, {2, 4, 5}} }} {
    // I want 'Game of life' to be 1st on 'menu_options', and 'Custom' to be 2nd
    // (they're not first and second in 'map_born', so we do it manually)
    menu_options.emplace_back("1. Game of Life", font, CHARACTER_SIZE);
    menu_options.emplace_back("2. Custom rulestring", font, CHARACTER_SIZE);

    int index = 3;
    for (const auto& pair : automaton_name_to_born_and_survive_sets){
        if (pair.first == "Game of Life" || pair.first == "Custom rulestring") continue;
        menu_options.emplace_back(std::to_string(index) + ". " + pair.first, font, CHARACTER_SIZE);
        index++;
    }

    setText();
    menu_screen_total_height = menu_options.size() * menu_option_rectangle_height;
}

short int AutomatonMenuScreen::run(){
    left_top_view_pos = sf::Vector2i(0,0);
    setInitialView();

    bool hovering = false;
    int rectangle_index;
    sf::FloatRect hovered_rectangle_bounds;
    sf::Text* hovered_menu_option;

    while (true) {
        handleHover(hovering, rectangle_index, nullptr, hovered_menu_option, hovered_rectangle_bounds);

        sf::Event evnt;
        while (window.pollEvent(evnt)){
            switch (evnt.type){
                case sf::Event::Closed:
                    return -1;

                case sf::Event::MouseWheelScrolled:{
                    handleScroll(evnt.mouseWheelScroll.delta);
                    break;
                }

                case sf::Event::MouseButtonReleased:{
                    if (evnt.mouseButton.button != sf::Mouse::Left) break;
                    if (!hovering) break;

                    cursor.loadFromSystem(sf::Cursor::Arrow);
                    window.setMouseCursor(cursor);

                    std::string automaton_name = textToString(hovered_menu_option);

                    hovered_menu_option->setFillColor(option_not_chosen_color); // Reset color of chosen menu option

                    if (rectangle_index != 1){ // Not custom rulestring
                        born_digits = automaton_name_to_born_and_survive_sets.at(automaton_name).first;
                        survive_digits = automaton_name_to_born_and_survive_sets.at(automaton_name).second;
                        return PATTERN_MENU_SCREEN;
                    }
                    else return RULESTRING_MENU_SCREEN;
                }

                case sf::Event::Resized:
                    resize(evnt);
                    setText();
                    break;
            }
        }

        window.clear();
        drawText();
        window.display();
    }
}