#pragma once

#ifndef CORE_H
#define CORE_H

#include "Map.h"

class CCore {
   private:
    // SDL_Window* window;
    // SDL_Renderer* rR;
    SDL_Event* mainEvent;
    // SDL_Surface* mainSurface;
    unsigned int gl_texture;

    unsigned touchKeysMask;

    // ----- FPS -----

    long frameTime;
    static const int MIN_FRAME_TIME = 16;

    unsigned long lFPSTime;
    int iNumOfFPS, iFPS;

    // ----- FPS -----

    // ----- INPUT
    static bool movePressed, keyMenuPressed, keyDownPressed, keyW, keyA, keyD, keyFire;

    static bool keyLeftPressed, keyRightPressed;
    // ----- true = RIGHT, false = LEFT
    bool firstDir;

    // ----- INPUT

    static Map* oMap;

    // ----- Methods

    void makeKeysPR(unsigned keys_mask, bool press);
    void checkFingers();

    void Input();
    void MouseInput();
    void InputPlayer();
    void InputMenu();

   public:
    CCore();
    ~CCore();

    static bool quitGame;

    void mainLoop();

    void Update();
    void Draw();

    void resetMove();
    static void resetKeys();

    static bool mouseLeftPressed, mouseRightPressed;
    static int mouseX, mouseY;

    /* ----- get & set ----- */
    static Map* getMap();
};

#endif
