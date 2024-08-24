#pragma once

#ifndef CHEEP_H
#define CHEEP_H

#include "Minion.h"

class Cheep : public Minion {
   private:
    bool moveY, moveYDIR;

   public:
    Cheep(int iXPos, int iYPos, int minionType, int moveSpeed, bool moveDiretion = false);
    ~Cheep(void);

    void Update();
    void Draw(CIMG* iIMG);

    void minionPhysics();
    void collisionWithPlayer(bool TOP);
};

#endif