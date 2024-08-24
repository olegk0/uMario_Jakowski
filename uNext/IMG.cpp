#include "IMG.h"
#include <SDL2/SDL_image.h>
#include "gl_subs.h"
#include "CFG.h"

/* ******************************************** */

CIMG::CIMG(void) {}

CIMG::CIMG(std::string fileName) {
    setIMG(fileName);
}

CIMG::~CIMG(void) {
    // SDL_DestroyTexture(tIMG);
}

/* ******************************************** */

void CIMG::Draw(int iXOffset, int iYOffset) {
    rRect.x = iXOffset;
    rRect.y = iYOffset;

    // SDL_RenderCopy(rR, tIMG, NULL, &rRect);
    BlitTexture(tIMG, &rRect);
}

void CIMG::Draw(int iXOffset, int iYOffset, bool bRotate) {
    rRect.x = iXOffset;
    rRect.y = iYOffset;

    if (!bRotate) {
        // SDL_RenderCopy(rR, tIMG, NULL, &rRect);
        BlitTexture(tIMG, &rRect);
    } else {
        BlitTextureFlip(tIMG, &rRect, false, true);
        // SDL_RenderCopyEx(rR, tIMG, NULL, &rRect, 180.0, NULL, SDL_FLIP_VERTICAL);
    }
}

void CIMG::DrawVert(int iXOffset, int iYOffset) {
    rRect.x = iXOffset;
    rRect.y = iYOffset;

    BlitTextureFlip(tIMG, &rRect, true, false);
    // SDL_RenderCopyEx(rR, tIMG, NULL, &rRect, 180.0, NULL, SDL_FLIP_HORIZONTAL);
}

void CIMG::DrawCrop(SDL_Rect rCrop, SDL_Rect rRectPart) {
    // rRect.y = CCFG::GAME_HEIGHT - rRect.h;
    BlitTextureCrop(tIMG, &rRectPart, &rRect, &rCrop);
    // BlitTexture(tIMG, &rRect);
    //  SDL_RenderCopy(rR, tIMG, &rCrop, &rRect);
}

/* ******************************************** */

void CIMG::setIMG(std::string fileName) {
    fileName = "images/" + fileName + ".png";
    // SDL_Surface* loadedSurface = SDL_LoadBMP(fileName.c_str());
    // SDL_Log("load img:%s", fileName.c_str());
    SDL_Surface* loadedSurface = IMG_Load(fileName.c_str());
    if (loadedSurface == NULL) {
        SDL_Log("Error load img:%s", fileName.c_str());
        return;
    }
    // SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 255, 0, 255));
#if 0
    tIMG = SDL_CreateTextureFromSurface(rR, loadedSurface);
    int iWidth, iHeight;

    SDL_QueryTexture(tIMG, NULL, NULL, &iWidth, &iHeight);
#else
    tIMG = InitTexture(loadedSurface);
#endif
    rRect.x = 0;
    rRect.y = 0;
    rRect.w = loadedSurface->w;
    rRect.h = loadedSurface->h;
    SDL_FreeSurface(loadedSurface);
}

GLuint CIMG::getIMG() {
    return tIMG;
}

SDL_Rect CIMG::getRect() {
    return rRect;
}