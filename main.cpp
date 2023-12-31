#include "screens.h"

int main(){
    std::vector<BaseScreen*> screens;
    short int curr_screen = 0;

    AutomatonMenuScreen screen0;
    screens.push_back(&screen0);
    RulestringMenuScreen screen1;
    screens.push_back(&screen1);
    PatternMenuScreen screen2;
    screens.push_back(&screen2);
    PatternInputScreen screen3;
    screens.push_back(&screen3);
    SaveGridScreen screen4;
    screens.push_back(&screen4);
    GameScreen screen5;
    screens.push_back(&screen5);

    while (0 <= curr_screen){
        curr_screen = screens[curr_screen]->run();
    }

    return 0;
}