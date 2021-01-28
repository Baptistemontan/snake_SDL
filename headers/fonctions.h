#ifndef HEAD_FONCTIONS
#define HEAD_FONCTIONS

    #include "linker.h"

    char* editPath(char* folderPath, char* fileName, char* buff);
    SDL_Surface* loadSprite(char* name, SDL_bool check);
    void checkSurface(SDL_Surface* surface,SDL_bool terminate);
    SDL_Rect* createSnakeCoord();
    void newCoord(int direction, int* x, int* y);
    void updateLastCoord(int map[NB_CASE_WIDTH][NB_CASE_HEIGHT],Coord *lastCoord);
    SDL_bool loadLevel(int map[NB_CASE_HEIGHT][NB_CASE_WIDTH], Coord* lastCoord, Coord* head, int* direction);
    void saveLevel(int map[NB_CASE_HEIGHT][NB_CASE_WIDTH],Coord lastCoord, Coord head, int direction);
    SDL_bool defaultLevel(int map[NB_CASE_WIDTH][NB_CASE_HEIGHT],Coord* lastCoord, Coord* head, int* direction);
    void renderSnake(int map[NB_CASE_WIDTH][NB_CASE_HEIGHT],Coord lastCoord, SDL_Surface* snake, SDL_Rect* snakeCoord, SDL_Surface* screen);
    void renderSnakeHead(int map[NB_CASE_WIDTH][NB_CASE_HEIGHT], SDL_Surface* screen, SDL_Surface* sprites, SDL_Rect* spritesCoord, SDL_Surface* background, Coord lastCoord, Coord headCoord);
    void renderSnakeEnd(int map[NB_CASE_WIDTH][NB_CASE_HEIGHT], SDL_Surface* screen, SDL_Surface* sprites, SDL_Rect* spritesCoord, SDL_Surface* background, Coord* lastCoord);
    void* waitEvent(void* arg);
    void createTarget(int map[NB_CASE_WIDTH][NB_CASE_HEIGHT],SDL_Surface* screen,SDL_Surface* sprites, SDL_Rect* spritesCoord);
    void renderMap(int map[NB_CASE_WIDTH][NB_CASE_HEIGHT], SDL_Surface* screen, SDL_Surface* sprites, SDL_Rect* spritesCoord);
    SDL_bool pauseGame();

#endif