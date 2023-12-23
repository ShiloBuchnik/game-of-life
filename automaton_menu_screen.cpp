#include "automaton_menu_screen.h"
#include "globals.h"

// Verify that inputted born-string and survive-string are formatted properly. We don't demand for them to be sorted.
// Note that born-strings can't include 0 (like LifeViewer and Golly), since that would light up the entire grid.
std::set <short int> AutomatonMenuScreen::handleBornSurviveInput(short int start){
    std::string bs_string;
    short int max_string_size = 8 - start + 1;
    std::set <short int> bs_digits;

    std::getline(std::cin, bs_string);
    for (int i = 0; i < bs_string.size(); i++){
        short int digit = bs_string[i] - '0';

        // Check digit is in range, that it isn't repeating, and that the born-string isn't longer than allowed.
        if (bs_digits.count(digit) == 0 && i < max_string_size && start <= digit && digit <= 8){
            bs_digits.insert(digit);
        }
        else{
            bs_digits.clear();
            i = -1; // Looping until we get correct input.
            std::cout << "Invalid input, please try again: ";
            std::getline(std::cin, bs_string);
        }
    }

    return bs_digits;
}

AutomatonMenuScreen::AutomatonMenuScreen(){
    // We want to start from 1, so we put garbage in 0th index.
    num_to_automaton_name =
            {"", "Game of Life", "Custom", "Life without Death", "HighLife", "Day and Night", "Seeds", "H-trees", "Replicator", "Live Free or Die",
             "Serviettes", "Iceballs", "DotLife", "Flock", "Maze", "Coral", "Grounded Life", "Pseudo Life", "Morley"};

    map_born =
            {{"Game of Life",       {3}},
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
}

short int AutomatonMenuScreen::run(){
    for (int i = 1; i < num_to_automaton_name.size(); i++){
        std::cout << i << ". " << num_to_automaton_name[i] << std::endl;
    }

    std::cout << "Please choose an automaton number: ";
    automaton_name = num_to_automaton_name[ handleMenuInput(num_to_automaton_name.size()) ];
    if (automaton_name == "Custom"){
        std::cin.get(); // In this point there's '\n' in the buffer, and to call 'getline()' properly later, we have to flush it.
        std::cout << "Please enter born-string (sequence of non-repeating digits in range [1,8]): ";
        map_born[automaton_name] = handleBornSurviveInput(1);
        std::cout << "Please enter survive-string (sequence of non-repeating digits in range [0,8]): ";
        map_survive[automaton_name] = handleBornSurviveInput(0);
    }
    system("cls");

    born_digits = map_born.at(automaton_name);
    survive_digits = map_survive.at(automaton_name);

    return PATTERN_MENU_SCREEN;
}