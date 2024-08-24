#pragma once

#ifndef PAUSEMENU_H
#define PAUSEMENU_H

#include "Menu.h"

class PauseMenu : public Menu {
   private:
    SDL_Rect rPause;

   public:
    PauseMenu(void);
    ~PauseMenu(void);

    void Update();
    void Draw();

    void enter();
    void escape();

    void updateActiveButton(int iDir);
};

#endif