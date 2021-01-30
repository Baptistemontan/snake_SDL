#include "../headers/linker.h"

// return an array of Rect going from left to right, up to down, of every sprite of a given size in a given space
// array is malloced
SDL_Rect* createSpritesCoord(SDL_Surface* sprites) {
    int width = sprites->w / NB_BASESPRITE_WIDTH;
    int height = sprites->h / NB_BASESPRITE_HEIGHT;
    SDL_Rect *spritesCoord = malloc(sizeof(SDL_Rect) * NB_SPRITES);
    int k = 0;
    for(int i = 0;i < NB_BASESPRITE_HEIGHT; i++) {
        for(int j = 0; j < NB_BASESPRITE_WIDTH; j++) {
            spritesCoord[k].x = j * width;
            spritesCoord[k].y = i * height;
            spritesCoord[k].w = width;
            spritesCoord[k].h = height;
            k++;
        }
    }
    return spritesCoord;
}

// increment x or y with the given direction
// if x or y go outside boundaries, loop to the other side
void newCoord(int nbCaseWidth, int nbCaseHeight,int direction, int* x, int* y) {
    switch(direction) {
        case UP:
            (*y)--;
            #ifdef NOBOUNDARIES
            if(*y < 0) *y = nbCaseHeight - 1;
            #endif
            break;
        case DOWN:
            (*y)++;
            #ifdef NOBOUNDARIES
            if(*y == nbCaseHeight) *y = 0;
            #endif
            break;
        case RIGHT:
            (*x)++;
            #ifdef NOBOUNDARIES
            if(*x == nbCaseWidth) *x = 0;
            #endif
            break;
        case LEFT:
            (*x)--;
            #ifdef NOBOUNDARIES
            if(*x < 0) *x = nbCaseWidth - 1;
            #endif
            break; 
    }
}

// update end of snake coord and update the maps
void updateLastCoord(int nbCaseWidth, int nbCaseHeight, int map[nbCaseWidth][nbCaseHeight],Coord *lastCoord) {
    int x = lastCoord->x, y = lastCoord->y;
    if(map[x][y] & UP_MASK) {
        newCoord(nbCaseWidth,nbCaseHeight,UP,&x,&y);
    } else if (map[x][y] & DOWN_MASK) {
        newCoord(nbCaseWidth,nbCaseHeight,DOWN,&x,&y);
    } else if(map[x][y] & RIGHT_MASK) {
        newCoord(nbCaseWidth,nbCaseHeight,RIGHT,&x,&y);
    } else { // LEFT
        newCoord(nbCaseWidth,nbCaseHeight,LEFT,&x,&y);
    }
    map[lastCoord->x][lastCoord->y] = EMPTY;
    lastCoord->x = x;
    lastCoord->y = y;
}

// goes in a seperate thread, arg is an array of adress
// first address is the main loop condition
// 2nd is the current direction
// 3rd is the new directions
void* waitEvent(void* arg) {
    SDL_Event event;
    SDL_bool* continuerMain = ((void**)arg)[0];
    // if we use only direction we could modifie it multiple time beetween frames
    // the we could change direction from UP to DOWN, and instantly eat ourself
    int* direction = ((void**)arg)[1];
    int* newDirection = ((void**)arg)[2];
    // directly free the argument array, we don't need it anymore
    free(arg);
    // we use the same condition as the main loop
    while(*continuerMain) {
        // as we are on a different thread, having synchronous event listening is no problem
        SDL_WaitEvent(&event);
        switch (event.type) {
            case SDL_QUIT:
                // quitting is requested, we terminate this thread and the main loop
                *continuerMain = SDL_FALSE;
                break;
                
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym) {
                    // direction update
                    case SDLK_UP:
                        pthread_mutex_lock(&directionMutex);
                        if(*direction != DOWN) *newDirection = UP;
                        pthread_mutex_unlock(&directionMutex);
                        break;
                    case SDLK_DOWN:
                        pthread_mutex_lock(&directionMutex);
                        if(*direction != UP) *newDirection = DOWN;
                        pthread_mutex_unlock(&directionMutex);
                        break;
                    case SDLK_LEFT:
                        pthread_mutex_lock(&directionMutex);
                        if(*direction != RIGHT) *newDirection = LEFT;
                        pthread_mutex_unlock(&directionMutex);
                        break;
                    case SDLK_RIGHT:
                        pthread_mutex_lock(&directionMutex);
                        if(*direction != LEFT) *newDirection = RIGHT;
                        pthread_mutex_unlock(&directionMutex);
                        break;
                }
                break;
        }
    }
    return NULL;
}

// pick a random empty location and put the target in
void createTarget(int nbCaseWidth, int nbCaseHeight, int map[nbCaseWidth][nbCaseHeight],SDL_Surface* screen,SDL_Surface* sprites, SDL_Rect* spritesCoord) {
    int x,y;
    do {
        x = rand() % nbCaseWidth;
        y = rand() % nbCaseHeight;
    } while(map[x][y] != 0);
    map[x][y] = TARGET;
    SDL_Rect pos = {.x = x * spritesCoord->w, .y = y * spritesCoord->h};
    SDL_BlitSurface(sprites,spritesCoord + SPRITE_TARGET,screen,&pos);
}

// stop the process until a key is pressed or the exit cross clicked
// return true is the exit cross is pressed, false for an keypress
SDL_bool pauseGame() {
    fprintf(stderr,"press a key or click the red cross to exit\n");
    SDL_Event event;
    while(1) {
        SDL_WaitEvent(&event);
        switch(event.type) {
            case SDL_QUIT:
                return SDL_TRUE;
                break;
            case SDL_KEYDOWN:
                return SDL_FALSE;
                break;
        }
    }
}

void defaultLevel(int nbCaseWidth, int nbCaseHeight, int map[nbCaseWidth][nbCaseHeight],Coord* lastCoord, Coord* head, int* direction) {
    fprintf(stderr,"creating default level...\n");
    for(int i = 0; i < nbCaseWidth;i++) {
        for(int j = 0; j < nbCaseHeight; j++) {
            map[i][j] = EMPTY;
        }
    }
    Coord middle = {.x = nbCaseWidth / 2, .y = nbCaseHeight / 2};
    lastCoord->x = middle.x;
    lastCoord->y = middle.y + 1;
    head->x = middle.x;
    head->y = middle.y - 1;
    *direction = UP;
    map[lastCoord->x][lastCoord->y] = SNAKE_MASK | UP_MASK;
    map[middle.x][middle.y] = SNAKE_MASK | UP_MASK;
    map[head->x][head->y] = SNAKE_MASK;
}

int initWidthHeight(const char* number,int width) {
    int num = atoi(number);
    if(num < 3) {
        return width ? DEFAULT_NB_CASE_WIDTH : DEFAULT_NB_CASE_HEIGHT;
    }
    return num;
}