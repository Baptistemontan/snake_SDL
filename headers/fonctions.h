#ifndef HEAD_FONCTIONS
#define HEAD_FONCTIONS

    #include "linker.h"

    char* editPath(char* folderPath, char* fileName, char* buff);
    SDL_Surface* loadSprite(char* name);
    void checkSurface(SDL_Surface* surface);
    SDL_Rect* creatSnakeCoord();
    void newCoord(int direction, int* x, int* y);
    void updateLastCoord(int carte[NB_CASE_WIDTH][NB_CASE_HEIGHT],Coord *lastCoord);
    void loadLevel(int carte[NB_CASE_HEIGHT][NB_CASE_WIDTH], Coord* lastCoord, Coord* head, int* direction);
    void saveLevel(int carte[NB_CASE_HEIGHT][NB_CASE_WIDTH],Coord lastCoord, Coord head, int direction);
    void blitSnake(int carte[NB_CASE_WIDTH][NB_CASE_HEIGHT],Coord lastCoord, SDL_Surface* snake, SDL_Rect* snakeCoord, SDL_Surface* ecran);
    void* waitEvent(void* arg);

#endif