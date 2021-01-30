#ifndef HEAD_RENDER
#define HEAD_RENDER

    #include "linker.h"
    
    void renderSnake(int nbCaseWidth, int nbCaseHeight, int map[nbCaseWidth][nbCaseHeight],Coord lastCoord, SDL_Surface* sprites, SDL_Rect* spritesCoord, SDL_Surface* screen);
    void renderMap(int nbCaseWidth, int nbCaseHeight, int map[nbCaseWidth][nbCaseHeight], SDL_Surface* screen, SDL_Surface* sprites, SDL_Rect* spritesCoord);
    void renderSnakeEnd(int nbCaseWidth, int nbCaseHeight, int map[nbCaseWidth][nbCaseHeight], SDL_Surface* screen, SDL_Surface* sprites, SDL_Rect* spritesCoord, SDL_Surface* background, Coord* lastCoord);
    void renderSnakeHead(int nbCaseWidth, int nbCaseHeight, int map[nbCaseWidth][nbCaseHeight], SDL_Surface* screen, SDL_Surface* sprites, SDL_Rect* spritesCoord, SDL_Surface* background, Coord lastCoord, Coord headCoord);

#endif