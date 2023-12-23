#ifndef GAME_OF_LIFE_MENU_SCREEN_H
#define GAME_OF_LIFE_MENU_SCREEN_H

#include "base_screen.h"

class MenuScreen: public BaseScreen{
protected:
    // TODO: maybe give up on defining two menu classes, and just create two menu objects?


    static unsigned long long int handleMenuInput(unsigned long long int upper);

public:

};

#endif
