#pragma once

#ifndef TEXT_H
#define TEXT_H

#include "header.h"
#include "IMG.h"

class Text {
   private:
    CIMG* FONT;

    SDL_Rect rCrop;
    SDL_Rect rRect;

    int fontSize;
    int extraLeft, nextExtraLeft;

    // ----- METHODS
    int getPos(int iChar);
    void checkExtra(int iChar);

   public:
    Text(void);
    ~Text(void);

    void Draw(std::string sText, int X, int Y, int fontSize = 16);
    void Draw(std::string sText, int X, int Y, int fontSize, int iR, int iG, int iB);
    void DrawCenterX(std::string sText, int Y, int fontSize = 16, int iR = 255, int iG = 255, int iB = 255);
    void Draw(std::string sText, int X, int Y, int iWidth, int iHeight);
    void DrawWS(std::string sText, int X, int Y, int iR, int iG, int iB, int fontSize = 16);

    int getTextWidth(std::string sText, int fontSize = 16);

    // ----- SET FONT IMG
    void setFont(std::string fileName);
};

#endif