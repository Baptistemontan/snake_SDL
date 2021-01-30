#ifndef HEAD_FONCTIONS
#define HEAD_FONCTIONS

    #include "linker.h"

    SDL_Rect* createSpritesCoord(SDL_Surface* sprites);
    void newCoord(int nbCaseWidth, int nbCaseHeight,int direction, int* x, int* y);
    void updateLastCoord(int nbCaseWidth, int nbCaseHeight, int map[nbCaseWidth][nbCaseHeight],Coord *lastCoord);
    void defaultLevel(int nbCaseWidth, int nbCaseHeight, int map[nbCaseWidth][nbCaseHeight],Coord* lastCoord, Coord* head, int* direction);
    void* waitEvent(void* arg);
    void createTarget(int nbCaseWidth, int nbCaseHeight, int map[nbCaseWidth][nbCaseHeight],SDL_Surface* screen,SDL_Surface* sprites, SDL_Rect* spritesCoord);
    SDL_bool pauseGame();
    int initWidthHeight(const char* number,int width);

#endif