#pragma once

#ifndef LAKITO_H
#define LAKITO_H

#include "Minion.h"

class Lakito : public Minion {
   private:
    int iMaxXPos;
    bool end;

    bool followPlayer;

    int nextSpikeyFrameID;

   public:
    Lakito(int iXPos, int iYPos, int iMaxXPos);
    ~Lakito(void);

    void Update();
    void Draw(CIMG* iIMG);

    void minionPhysics();

    void collisionWithPlayer(bool TOP);
};

#endif