#ifndef HEAD_FILES
#define HEAD_FILES

    #include "linker.h"

    char* editPath(char* folderPath, const char* fileName, char* buff);
    void checkSurface(SDL_Surface* surface,SDL_bool terminate);
    SDL_Surface* loadSprite(const char* name, SDL_bool check);
    void saveLevel(int nbCaseWidth, int nbCaseHeight,int map[nbCaseWidth][nbCaseHeight], Coord lastCoord, Coord head, int direction,const char* saveFileName);
    SDL_bool loadLevel(int nbCaseWidth, int nbCaseHeight,int map[nbCaseWidth][nbCaseHeight],Coord* lastCoord, Coord* head, int* direction,const char* saveFileName);
#endif