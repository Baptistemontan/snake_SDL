#ifndef HEAD_RENDER
#define HEAD_RENDER

    #include "linker.h"
    
    void renderSnake(int map[NB_CASE_WIDTH][NB_CASE_HEIGHT],Coord lastCoord, SDL_Surface* sprites, SDL_Rect* spritesCoord, SDL_Surface* screen);
    void renderMap(int map[NB_CASE_WIDTH][NB_CASE_HEIGHT], SDL_Surface* screen, SDL_Surface* sprites, SDL_Rect* spritesCoord);
    void renderSnakeEnd(int map[NB_CASE_WIDTH][NB_CASE_HEIGHT], SDL_Surface* screen, SDL_Surface* sprites, SDL_Rect* spritesCoord, SDL_Surface* background, Coord* lastCoord);
    void renderSnakeHead(int map[NB_CASE_WIDTH][NB_CASE_HEIGHT], SDL_Surface* screen, SDL_Surface* sprites, SDL_Rect* spritesCoord, SDL_Surface* background, Coord lastCoord, Coord headCoord);

#endif