#ifndef GAME_OF_LIFE_MENU_SCREEN_H
#define GAME_OF_LIFE_MENU_SCREEN_H

#include "base_screen.h"

class MenuScreen: public BaseScreen{
protected:
    // TODO fit in view here. Think if you need left_top_view, or just the view object.

    static unsigned long long int handleMenuInput(unsigned long long int upper);

public:

};

#endif
