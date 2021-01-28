#include "../headers/linker.h"

// return an array of Rect going from left to right, up to down, of every sprite of a given size in a given space
// array is malloced
SDL_Rect* createSnakeCoord() {
    SDL_Rect *spritesCoord = malloc(sizeof(SDL_Rect) * NB_SPRITES);
    int k = 0;
    for(int i = 0;i < NB_BASESPRITE_HEIGHT; i++) {
        for(int j = 0; j < NB_BASESPRITE_WIDTH; j++) {
            spritesCoord[k].y = i * SPRITE_HEIGHT;
            spritesCoord[k].x = j * SPRITE_WIDTH;
            spritesCoord[k].w = SPRITE_WIDTH;
            spritesCoord[k].h = SPRITE_HEIGHT;
            k++;
        }
    }
    return spritesCoord;
}

// increment x or y with the given direction
// if x or y go outside boundaries, loop to the other side
void newCoord(int direction, int* x, int* y) {
    switch(direction) {
        case UP:
            (*y)--;
            if(*y < 0) *y = NB_CASE_HEIGHT - 1;
            break;
        case DOWN:
            (*y)++;
            if(*y == NB_CASE_HEIGHT) *y = 0;
            break;
        case RIGHT:
            (*x)++;
            if(*x == NB_CASE_WIDTH) *x = 0;
            break;
        case LEFT:
            (*x)--;
            if(*x < 0) *x = NB_CASE_WIDTH - 1;
            break; 
    }
}

// update end of snake coord and update the maps
void updateLastCoord(int map[NB_CASE_WIDTH][NB_CASE_HEIGHT],Coord *lastCoord) {
    int x = lastCoord->x, y = lastCoord->y;
    if(map[x][y] & UP_MASK) {
        newCoord(UP,&x,&y);
    } else if (map[x][y] & DOWN_MASK) {
        newCoord(DOWN,&x,&y);
    } else if(map[x][y] & RIGHT_MASK) {
        newCoord(RIGHT,&x,&y);
    } else { // LEFT
        newCoord(LEFT,&x,&y);
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
void createTarget(int map[NB_CASE_WIDTH][NB_CASE_HEIGHT],SDL_Surface* screen,SDL_Surface* sprites, SDL_Rect* spritesCoord) {
    int x,y;
    do {
        x = rand() % NB_CASE_WIDTH;
        y = rand() % NB_CASE_HEIGHT;
    } while(map[x][y] != 0);
    map[x][y] = TARGET;
    SDL_Rect pos = {.x = x * SPRITE_WIDTH, .y = y * SPRITE_HEIGHT};
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

SDL_bool defaultLevel(int map[NB_CASE_WIDTH][NB_CASE_HEIGHT],Coord* lastCoord, Coord* head, int* direction) {
    if(NB_CASE_HEIGHT < 3 || NB_BASESPRITE_WIDTH < 3){
        return SDL_FALSE;
    }
    fprintf(stderr,"creating default level...\n");
    for(int i = 0; i < NB_CASE_WIDTH;i++) {
        for(int j = 0; j < NB_CASE_HEIGHT; j++) {
            map[i][j] = EMPTY;
        }
    }
    Coord middle = {.x = NB_CASE_WIDTH / 2, .y = NB_BASESPRITE_HEIGHT / 2};
    lastCoord->x = middle.x;
    lastCoord->y = middle.y + 1;
    head->x = middle.x;
    head->y = middle.y - 1;
    *direction = UP;
    map[lastCoord->x][lastCoord->y] = SNAKE_MASK | UP_MASK;
    map[middle.x][middle.y] = SNAKE_MASK | UP_MASK;
    map[head->x][head->y] = SNAKE_MASK;
    return SDL_TRUE;
}