#ifndef HEAD_FILES
#define HEAD_FILES

    #include "linker.h"

    char* editPath(char* folderPath, char* fileName, char* buff);
    void checkSurface(SDL_Surface* surface,SDL_bool terminate);
    SDL_Surface* loadSprite(char* name, SDL_bool check);
    void saveLevel(int map[NB_CASE_WIDTH][NB_CASE_HEIGHT], Coord lastCoord, Coord head, int direction);
    SDL_bool loadLevel(int map[NB_CASE_WIDTH][NB_CASE_HEIGHT],Coord* lastCoord, Coord* head, int* direction);
#endif