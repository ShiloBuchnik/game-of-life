#include "screens.h"

RulestringMenuScreen::RulestringMenuScreen():
initial_born_prompt_str("Enter born-string (sequence of non-repeating digits in range [1,8]): _"),
initial_survive_prompt_str("Enter survive-string (sequence of non-repeating digits in range [0,8]): "),
born_prompt(initial_born_prompt_str, font, OPTION_CHARACTER_SIZE + 5),
survive_prompt(initial_survive_prompt_str, font, OPTION_CHARACTER_SIZE + 5) { }

short int RulestringMenuScreen::run(){
    centerText(born_prompt, window_height / 4 - born_prompt.getGlobalBounds().height / 2);
    centerText(survive_prompt, born_prompt.getGlobalBounds().top + born_prompt.getGlobalBounds().height + DISTANCE * 5);

    // We start filling born_prompt, then move on to survive_prompt
    sf::Text* curr_prompt = &born_prompt;
    short int curr_lower_limit = 1; // Lower limit is 1 for born_prompt, but will be 0 for survive_prompt
    int curr_prompt_initial_size = born_prompt.getString().getSize();
    std::set <short int> curr_prompt_digits;

    while (true) {
        sf::Event evnt;
        while (window.pollEvent(evnt)){
            switch (evnt.type){
                case sf::Event::Closed:
                    return -1;

                case sf::Event::KeyPressed:
                    if (evnt.key.code == sf::Keyboard::Escape){
                        born_prompt.setString(initial_born_prompt_str);
                        survive_prompt.setString(initial_survive_prompt_str);
                        return AUTOMATON_MENU_SCREEN;
                    }
                    break;

                case sf::Event::TextEntered: {
                    std::string curr_prompt_str = curr_prompt->getString();
                    auto last_num_pos = curr_prompt_str.end() - 2;

                    // Inputted digit
                    if ('0' + curr_lower_limit <= evnt.text.unicode && evnt.text.unicode <= '8' &&
                    curr_prompt_digits.count(evnt.text.unicode - '0') == 0) {
                        curr_prompt_digits.insert(evnt.text.unicode - '0');

                        curr_prompt_str.insert(last_num_pos + 1, static_cast<char>(evnt.text.unicode)); // Inserting before '_'
                        curr_prompt->setString(curr_prompt_str);
                    // Inputted backspace, accept only if there's what to delete
                    } else if (evnt.text.unicode == 8 && curr_prompt_initial_size < curr_prompt->getString().getSize()) {
                        curr_prompt_digits.erase(*last_num_pos - '0');

                        curr_prompt_str.erase(last_num_pos); // // Erasing before '_'
                        curr_prompt->setString(curr_prompt_str);
                    // Inputted enter
                    } else if (evnt.text.unicode == 13) {
                        if (curr_lower_limit == 1){ // Enter on born_prompt
                            curr_prompt_str.pop_back(); // Erasing '_'
                            born_prompt.setString(curr_prompt_str);

                            survive_prompt.setString(survive_prompt.getString() + "_"); // Adding '_'
                            curr_prompt = &survive_prompt;
                            curr_lower_limit--;
                            curr_prompt_initial_size = survive_prompt.getString().getSize();

                            born_digits = curr_prompt_digits;
                            curr_prompt_digits.clear();
                        }
                        else{ // Enter on survive_prompt
                            born_prompt.setString(initial_born_prompt_str);
                            survive_prompt.setString(initial_survive_prompt_str);

                            survive_digits = curr_prompt_digits;
                            return PATTERN_MENU_SCREEN;
                        }
                    }
                    break;
                }

                case sf::Event::Resized:
                    resize(evnt);
                    centerText(born_prompt, born_prompt.getPosition().y);
                    centerText(survive_prompt, survive_prompt.getPosition().y);

                    break;
            }
        }

        window.clear();
        window.draw(born_prompt);
        window.draw(survive_prompt);
        window.display();
    }
}