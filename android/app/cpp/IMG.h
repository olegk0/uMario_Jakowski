#pragma once

#ifndef IMG_H
#define IMG_H

#include "header.h"
#include <SDL_opengles2.h>

class CIMG {
   private:
    GLuint tIMG;
    SDL_Rect rRect;

   public:
    CIMG(void);
    CIMG(std::string fileName);
    ~CIMG(void);

    void Draw(int iXOffset, int iYOffset);
    void Draw(int iXOffset, int iYOffset, bool bRotate);
    void DrawVert(int iXOffset, int iYOffset);
    void DrawCrop(SDL_Rect rCrop, SDL_Rect rRect);

    /* ----- get & set ----- */
    GLuint getIMG();
    void setIMG(std::string fileName);
    SDL_Rect getRect();
};

#endif