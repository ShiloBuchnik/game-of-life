#ifndef GAME_OF_LIFE_BASE_SCREEN_H
#define GAME_OF_LIFE_BASE_SCREEN_H

#include "screens_settings.h"

class BaseScreen{
protected:
    static BaseGlobalSettings& base_settings;

public:
    virtual short int run() = 0;
};

#endif
