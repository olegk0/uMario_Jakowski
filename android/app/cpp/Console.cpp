#include "Console.h"
#include "CFG.h"
#include "gl_subs.h"

/* ******************************************** */

Console::Console(void) {
    rRect.x = 5;
    rRect.y = 5;
    rRect.w = 175;
    rRect.h = 105;

    this->iMax = (rRect.h - rRect.y) / 12;
}

Console::~Console(void) {
}

/* ******************************************** */

void Console::Draw() {
    if (vPrint.size() > 0) {
        // SDL_SetRenderDrawBlendMode(SDL_BLENDMODE_BLEND);
        // SDL_SetRenderDrawColor(4, 4, 4, 128);
        BlitSquareNorm(&rRect, 4, 4, 4, 128);
        // SDL_RenderFillRect(&rRect);
        // SDL_SetRenderDrawColor(255, 255, 255, 128);
        // SDL_RenderDrawRect(&rRect);
        BlitSquareNorm(&rRect, 155, 155, 155, 128);
        // SDL_SetRenderDrawColor(255, 255, 255, 255);

        for (int i = vPrint.size() - 1, j = 0; i >= 0; i--, j++) {
            CCFG::getText()->Draw(vPrint[i], rRect.x + rRect.w - 5 - CCFG::getText()->getTextWidth(vPrint[i], 8), rRect.y + rRect.h - 5 - 8 * (j + 1) - 4 * j, 8);
        }

        //      SDL_SetRenderDrawBlendMode(SDL_BLENDMODE_NONE);
    }
}

/* ******************************************** */

void Console::print(int iPrint) {
    print(std::to_string(iPrint));
}

void Console::print(std::string sPrint) {
    std::string nPrint;
    for (unsigned int i = 0; i < sPrint.size(); i++) {
        if (sPrint[i] >= 97 && sPrint[i] < 123) {
            nPrint += sPrint[i] - 32;
        } else {
            nPrint += sPrint[i];
        }
    }

    if (vPrint.size() >= iMax) {
        vPrint.erase(vPrint.begin());
    }

    vPrint.push_back(nPrint);
}