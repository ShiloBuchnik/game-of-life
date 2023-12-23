#include <limits>
#include "menu_screen.h"

// Verifies that input num is in the range [1, upper).
unsigned long long int MenuScreen::handleMenuInput(unsigned long long int upper){
    unsigned long long int num;

    std::cin >> num;
    while (std::cin.fail() || num < 1 || upper <= num){
        std::cout << "Invalid input, please try again: ";
        std::cin.clear(); // Clears error flag. If it's raised, 'cin' won't get input.
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clears buffer
        std::cin >> num;
    }

    return num;
}

