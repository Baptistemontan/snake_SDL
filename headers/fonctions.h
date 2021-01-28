#ifndef HEAD_FONCTIONS
#define HEAD_FONCTIONS

    #include "linker.h"

    SDL_Rect* createSnakeCoord();
    void newCoord(int direction, int* x, int* y);
    void updateLastCoord(int map[NB_CASE_WIDTH][NB_CASE_HEIGHT],Coord *lastCoord);
    SDL_bool defaultLevel(int map[NB_CASE_WIDTH][NB_CASE_HEIGHT],Coord* lastCoord, Coord* head, int* direction);
    void* waitEvent(void* arg);
    void createTarget(int map[NB_CASE_WIDTH][NB_CASE_HEIGHT],SDL_Surface* screen,SDL_Surface* sprites, SDL_Rect* spritesCoord);
    SDL_bool pauseGame();

#endif