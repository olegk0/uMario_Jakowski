#include "header.h"
#include "Core.h"
#include "IMG.h"
#include "CFG.h"
#include "Text.h"
#include "SDL_mixer.h"
#include "gl_subs.h"

/* ******************************************** */

Map* CCore::oMap = new Map();
bool CCore::mouseLeftPressed = false;
bool CCore::mouseRightPressed = false;
int CCore::mouseX = 0;
int CCore::mouseY = 0;
bool CCore::quitGame = false;

bool CCore::movePressed = false;
bool CCore::keyMenuPressed = false;
bool CCore::keyDown = false;
bool CCore::keyW = false;
bool CCore::keyA = false;
bool CCore::keyD = false;
bool CCore::keyFire = false;
bool CCore::keyLeftPressed = false;
bool CCore::keyRightPressed = false;

CCore::CCore() {
    this->quitGame = false;
    this->iFPS = 0;
    this->iNumOfFPS = 0;
    this->lFPSTime = 0;

    int ret = GL_Init("uMario - www.LukaszJakowski.pl", SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO);
    if (ret) {
        SDL_Log("Error GL_Init  ret:%d\n", ret);
        quitGame = true;
        return;
    }
    /*
        window = SDL_CreateWindow(, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, CCFG::GAME_WIDTH, CCFG::GAME_HEIGHT, SDL_WINDOW_SHOWN);

        if (window == NULL) {
            SDL_Log("Error SDL_CreateWindow\n");
            quitGame = true;
        }
        */
    // InitTextureAndRender(CCFG::GAME_WIDTH, CCFG::GAME_HEIGHT, &mainSurface, &rR);
    //  gl_texture = InitTexture(mainSurface);
    //   rR = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // ----- ICO
    /*
    SDL_Surface* loadedSurface = SDL_LoadBMP("images/ico.png");
    SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 255, 0, 255));

    SDL_SetWindowIcon(MainWindow, loadedSurface);
    SDL_FreeSurface(loadedSurface);
*/
    mainEvent = new SDL_Event();
    // ----- ICO

    CCFG::getMusic()->Init();

    ret = Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    SDL_Log("Mix_OpenAudio2 ret:%d\n", ret);

    oMap = new Map(true);
    CCFG::getMM()->setActiveOption();
    CCFG::getSMBLOGO()->setIMG("super_mario_bros");

    CCFG::getMusic()->PlayMusic();

    this->keyMenuPressed = this->movePressed = this->keyDown = this->keyW = this->keyA = this->keyD = this->keyFire = false;

    this->keyLeftPressed = this->keyRightPressed = this->firstDir = false;

    this->mouseX = this->mouseY = 0;

    CCFG::keyIDA = SDLK_a;
    CCFG::keyIDS = SDLK_s;
    CCFG::keyIDD = SDLK_d;
    CCFG::keyIDSpace = SDLK_SPACE;
    CCFG::keyIDShift = SDLK_LSHIFT;
}

CCore::~CCore() {
    delete oMap;
    delete mainEvent;
    // SDL_DestroyRenderer(rR);
    // SDL_FreeSurface(mainSurface);
    //  SDL_DestroyWindow(window);
    GL_Free();
}

/* ******************************************** */

void CCore::mainLoop() {
    lFPSTime = SDL_GetTicks();

    while (!quitGame && mainEvent->type != SDL_QUIT) {
        frameTime = SDL_GetTicks();
        SDL_PollEvent(mainEvent);

        CCFG::getMM()->setBackgroundColor();
        // SDL_RenderClear(rR);
        glClear(GL_COLOR_BUFFER_BIT);
        // SDL_RenderFillRect(rR, NULL);

        Input();
        MouseInput();
        Update();
        Draw();

        /*CCFG::getText()->Draw(rR, "FPS:" + std::to_string(iNumOfFPS), CCFG::GAME_WIDTH - CCFG::getText()->getTextWidth("FPS:" + std::to_string(iNumOfFPS), 8) - 8, 5, 8);

        if(SDL_GetTicks() - 1000 >= lFPSTime) {
                lFPSTime = SDL_GetTicks();
                iNumOfFPS = iFPS;
                iFPS = 0;
        }

        ++iFPS;*/

        // BlitScreen(gl_texture, mainSurface);
        SDL_GL_SwapWindow(MainWindow);
        // SDL_RenderPresent(rR);

        if (SDL_GetTicks() - frameTime < MIN_FRAME_TIME) {
            SDL_Delay(MIN_FRAME_TIME - (SDL_GetTicks() - frameTime));
        }
    }
}

void CCore::Input() {
    switch (CCFG::getMM()->getViewID()) {
        case 2:
        case 7:
            if (!oMap->getInEvent()) {
                InputPlayer();
            } else {
                resetMove();
            }
            break;
        default:
            InputMenu();
            break;
    }
}

void CCore::InputMenu() {
    if (mainEvent->type == SDL_KEYDOWN) {
        CCFG::getMM()->setKey(mainEvent->key.keysym.sym);

        switch (mainEvent->key.keysym.sym) {
            case SDLK_s:
            case SDLK_DOWN:
                if (!keyMenuPressed) {
                    CCFG::getMM()->keyPressed(2);
                    keyMenuPressed = true;
                }
                break;
            case SDLK_w:
            case SDLK_UP:
                if (!keyMenuPressed) {
                    CCFG::getMM()->keyPressed(0);
                    keyMenuPressed = true;
                }
                break;
            case SDLK_KP_ENTER:
            case SDLK_RETURN:
                if (!keyMenuPressed) {
                    CCFG::getMM()->enter();
                    keyMenuPressed = true;
                }
                break;
            case SDLK_ESCAPE:
                if (!keyMenuPressed) {
                    CCFG::getMM()->escape();
                    keyMenuPressed = true;
                }
                break;
            case SDLK_LEFT:
            case SDLK_d:
                if (!keyMenuPressed) {
                    CCFG::getMM()->keyPressed(3);
                    keyMenuPressed = true;
                }
                break;
            case SDLK_RIGHT:
            case SDLK_a:
                if (!keyMenuPressed) {
                    CCFG::getMM()->keyPressed(1);
                    keyMenuPressed = true;
                }
                break;
        }
    }

    if (mainEvent->type == SDL_KEYUP) {
        switch (mainEvent->key.keysym.sym) {
            case SDLK_s:
            case SDLK_DOWN:
            case SDLK_w:
            case SDLK_UP:
            case SDLK_KP_ENTER:
            case SDLK_RETURN:
            case SDLK_ESCAPE:
            case SDLK_a:
            case SDLK_RIGHT:
            case SDLK_LEFT:
            case SDLK_d:
                keyMenuPressed = false;
                break;
            default:
                break;
        }
    }
}

void CCore::InputPlayer() {
    if (mainEvent->type == SDL_WINDOWEVENT) {
        switch (mainEvent->window.event) {
            case SDL_WINDOWEVENT_FOCUS_LOST:
                CCFG::getMM()->resetActiveOptionID(CCFG::getMM()->ePasue);
                CCFG::getMM()->setViewID(CCFG::getMM()->ePasue);
                CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cPASUE);
                CCFG::getMusic()->PauseMusic();
                break;
        }
    }

    if (mainEvent->type == SDL_KEYUP) {
        if (mainEvent->key.keysym.sym == CCFG::keyIDD) {
            if (firstDir) {
                firstDir = false;
            }

            keyRightPressed = false;
        }

        if (mainEvent->key.keysym.sym == CCFG::keyIDS) {
            oMap->getPlayer()->setSquat(false);
            keyDown = false;
        }

        if (mainEvent->key.keysym.sym == CCFG::keyIDA) {
            if (!firstDir) {
                firstDir = true;
            }

            keyLeftPressed = false;
        }

        if (mainEvent->key.keysym.sym == CCFG::keyIDSpace) {
            CCFG::keyJump = false;
        }

        if (mainEvent->key.keysym.sym == CCFG::keyIDShift) {
            if (keyFire) {
                oMap->getPlayer()->resetRun();
                keyFire = false;
            }
        }
        switch (mainEvent->key.keysym.sym) {
            case SDLK_KP_ENTER:
            case SDLK_RETURN:
            case SDLK_ESCAPE:
                keyMenuPressed = false;
                break;
        }
    }

    if (mainEvent->type == SDL_KEYDOWN) {
        if (mainEvent->key.keysym.sym == CCFG::keyIDD) {
            keyRightPressed = true;
            if (!keyLeftPressed) {
                firstDir = true;
            }
        }

        if (mainEvent->key.keysym.sym == CCFG::keyIDS) {
            if (!keyDown) {
                keyDown = true;
                if (!oMap->getUnderWater() && !oMap->getPlayer()->getInLevelAnimation()) oMap->getPlayer()->setSquat(true);
            }
        }

        if (mainEvent->key.keysym.sym == CCFG::keyIDA) {
            keyLeftPressed = true;
            if (!keyRightPressed) {
                firstDir = false;
            }
        }

        if (mainEvent->key.keysym.sym == CCFG::keyIDSpace) {
            if (!CCFG::keyJump) {
                oMap->getPlayer()->jump();
                CCFG::keyJump = true;
            }
        }

        if (mainEvent->key.keysym.sym == CCFG::keyIDShift) {
            if (!keyFire) {
                oMap->getPlayer()->startRun();
                keyFire = true;
            }
        }

        switch (mainEvent->key.keysym.sym) {
            case SDLK_KP_ENTER:
            case SDLK_RETURN:
                if (!keyMenuPressed) {
                    CCFG::getMM()->enter();
                    keyMenuPressed = true;
                }
            case SDLK_ESCAPE:
                if (!keyMenuPressed && CCFG::getMM()->getViewID() == CCFG::getMM()->eGame) {
                    CCFG::getMM()->resetActiveOptionID(CCFG::getMM()->ePasue);
                    CCFG::getMM()->setViewID(CCFG::getMM()->ePasue);
                    CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cPASUE);
                    CCFG::getMusic()->PauseMusic();
                    keyMenuPressed = true;
                }
                break;
        }
    }

    if (keyLeftPressed) {
        if (!oMap->getPlayer()->getMove() && firstDir == false && !oMap->getPlayer()->getChangeMoveDirection() && !oMap->getPlayer()->getSquat()) {
            oMap->getPlayer()->startMove();
            oMap->getPlayer()->setMoveDirection(false);
        } else if (!keyRightPressed && oMap->getPlayer()->getMoveSpeed() > 0 && firstDir != oMap->getPlayer()->getMoveDirection()) {
            oMap->getPlayer()->setChangeMoveDirection();
        }
    }

    if (keyRightPressed) {
        if (!oMap->getPlayer()->getMove() && firstDir == true && !oMap->getPlayer()->getChangeMoveDirection() && !oMap->getPlayer()->getSquat()) {
            oMap->getPlayer()->startMove();
            oMap->getPlayer()->setMoveDirection(true);
        } else if (!keyLeftPressed && oMap->getPlayer()->getMoveSpeed() > 0 && firstDir != oMap->getPlayer()->getMoveDirection()) {
            oMap->getPlayer()->setChangeMoveDirection();
        }
    }

    if (oMap->getPlayer()->getMove() && !keyLeftPressed && !keyRightPressed) {
        oMap->getPlayer()->resetMove();
    }
}

void CCore::MouseInput() {
    switch (mainEvent->type) {
        case SDL_MOUSEBUTTONDOWN: {
            switch (mainEvent->button.button) {
                case SDL_BUTTON_LEFT:
                    mouseLeftPressed = true;
                    break;
                case SDL_BUTTON_RIGHT:
                    mouseRightPressed = true;
                    break;
            }
            break;
        }
        case SDL_MOUSEMOTION: {
            SDL_GetMouseState(&mouseX, &mouseY);
            // CCFG::getMM()->getConsole()->print("x:" + std::to_string(mouseX));
            // CCFG::getMM()->getConsole()->print("y:" + std::to_string(mouseY));
            break;
        }
        case SDL_MOUSEBUTTONUP: {
            switch (mainEvent->button.button) {
                case SDL_BUTTON_LEFT:
                    mouseLeftPressed = false;
                    break;
                case SDL_BUTTON_RIGHT:
                    mouseRightPressed = false;
                    break;
            }
            break;
        }
        case SDL_MOUSEWHEEL:
            if (mainEvent->wheel.timestamp > SDL_GetTicks() - 2) {
                // CCFG::getMM()->getLE()->mouseWheel(mainEvent->wheel.y);
            }
            break;
    }
}

void CCore::resetKeys() {
    movePressed = keyMenuPressed = keyDown = keyW = keyA = keyD = CCFG::keyJump = keyFire = keyLeftPressed = keyRightPressed = false;
}

void CCore::Update() {
    CCFG::getMM()->Update();
}

void CCore::Draw() {
    CCFG::getMM()->Draw();
}

/* ******************************************** */

void CCore::resetMove() {
    this->keyLeftPressed = this->keyRightPressed = false;
}

Map* CCore::getMap() {
    return oMap;
}