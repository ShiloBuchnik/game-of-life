#include <filesystem>
#include "pattern_menu_screen.h"
#include "patterns.h"

PatternMenuScreen::PatternMenuScreen(){
    // We want to start from 2 here, so 0th and 1st indices are garbage.
    num_to_pattern_path = {"", ""};
}

short int PatternMenuScreen::run(){
    std::cout << "Below are available patterns:\n" << std::endl;
    std::cout << "1. Custom Pattern\n" << std::endl;

    // If 'patterns' directory doesn't exist, it creates it; otherwise, it does nothing.
    std::filesystem::create_directories("patterns");
    iterateOverPatternDirectory(num_to_pattern_path);

    std::cout << "Please choose a pattern number: ";
    unsigned long long int pattern_num = handleMenuInput(num_to_pattern_path.size());
    system("cls");

    if (pattern_num != 1){
        putPatternInGrid(grid, num_to_pattern_path[pattern_num]);
        return GAME_SCREEN;
    }
    else return PATTERN_INPUT_SCREEN;
}


