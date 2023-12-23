#include <SFML/Graphics.hpp>
#include "globals.h"
#include "screens.h"

int main(){
    std::vector<BaseScreen*> screens(SCREENS_AMOUNT, nullptr);
    short int curr_screen = 0;

    AutomatonMenuScreen screen0;
    screens[screens_names::AUTOMATON_MENU_SCREEN] = &screen0;
    PatternMenuScreen screen1;
    screens[screens_names::PATTERN_MENU_SCREEN] = &screen1;
    PatternInputScreen screen2;
    screens[screens_names::PATTERN_INPUT_SCREEN] = &screen2;
    GameScreen screen3;
    screens[screens_names::GAME_SCREEN] = &screen3;

    // TODO: maybe need to write for each class a function that resets certain resources?
    //Main loop
    while (0 <= curr_screen){
        curr_screen = screens[curr_screen]->run();
    }

    return 0;
}