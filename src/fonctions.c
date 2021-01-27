#include "../headers/linker.h"

// add a filename to a path and store it in a buffer, return the buffer
char* editPath(char* folderPath, char* fileName, char* buff) {
    sprintf(buff,folderPath,fileName);
    return buff;
}

// check if a surface as been succesfully loaded
// print the error to stderr if not
void checkSurface(SDL_Surface* surface) {
    if(surface == NULL) {
        fprintf(stderr, "erreur surface: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
}

// takes a filename in the sprites folder and return the SDL surfaces
SDL_Surface* loadSprite(char* name) {
    char pathBuff[MAX_PATH_LENGTH];
    SDL_Surface* sprite = IMG_Load(editPath(SPRITESPATH,name,pathBuff));
    checkSurface(sprite);
    return sprite;
}

// return an array of Rect going from left to right, up to down, of every sprite of a given size in a given space
// array is malloced
SDL_Rect* creatSnakeCoord() {
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

// render the snake according to the map
void renderSnake(int map[NB_CASE_WIDTH][NB_CASE_HEIGHT],Coord lastCoord, SDL_Surface* sprites, SDL_Rect* spritesCoord, SDL_Surface* screen) {
    // we have 3 sets of head, this variable is increment every render
    static int head = 0;
    // variable declaration
    int lastDir,x = lastCoord.x,y = lastCoord.y;
    int currentCase = map[x][y];
    SDL_Rect* currentSurface;
    SDL_bool continuer = SDL_TRUE;
    SDL_Rect coord;

    // calculate end of the snake direction
    if(currentCase & UP_MASK) {
        currentSurface = spritesCoord + SNAKE_END + UP;
        lastDir = UP;
    } else if(currentCase & DOWN_MASK) {
        currentSurface = spritesCoord + SNAKE_END + DOWN;
        lastDir = DOWN;
    } else if(currentCase & LEFT_MASK) {
        currentSurface = spritesCoord + SNAKE_END + LEFT;
        lastDir = LEFT;
    } else if(currentCase & RIGHT_MASK) {
        currentSurface = spritesCoord + SNAKE_END + RIGHT;
        lastDir = RIGHT;
    }
    coord.x = x * SPRITE_WIDTH;
    coord.y = y * SPRITE_WIDTH;
    // move the current coord according to the given direction
    newCoord(lastDir,&x,&y);
    currentCase = map[x][y];
    // render the end of the snake
    SDL_BlitSurface(sprites,currentSurface,screen,&coord);
    // continuer is set to false when it hit the head
    while(continuer) {
        // UP current direction
        if(currentCase & UP_MASK) {
            // sprite calculation given last direction
            switch(lastDir) {
                case UP:
                    currentSurface = spritesCoord + SNAKE_BODY;
                    break;
                case LEFT:
                    currentSurface = spritesCoord + SNAKE_TURN + UP + 1;
                    break;
                case RIGHT:
                    currentSurface = spritesCoord + SNAKE_TURN + UP;
                    break;
            }
            lastDir = UP;
        } else if(currentCase & DOWN_MASK) {
            switch(lastDir) {
                case DOWN:
                    currentSurface = spritesCoord + SNAKE_BODY;
                    break;
                case LEFT:
                    currentSurface = spritesCoord + SNAKE_TURN + 1;
                    break;
                case RIGHT:
                    currentSurface = spritesCoord + SNAKE_TURN;
                    break;
            }
            lastDir = DOWN;
        } else if(currentCase & RIGHT_MASK) {
            switch(lastDir) {
                case RIGHT:
                    currentSurface = spritesCoord + SNAKE_BODY + 1;
                    break;
                case DOWN:
                    currentSurface = spritesCoord + SNAKE_TURN + 1 + UP;
                    break;
                case UP:
                    currentSurface = spritesCoord + SNAKE_TURN + 1;
                    break;
            }
            lastDir = RIGHT;
        } else if(currentCase & LEFT_MASK) {
            switch(lastDir) {
                case LEFT:
                    currentSurface = spritesCoord + SNAKE_BODY + 1;
                    break;
                case DOWN:
                    currentSurface = spritesCoord + SNAKE_TURN + UP;
                    break;
                case UP:
                    currentSurface = spritesCoord + SNAKE_TURN;
                    break;
            }
            lastDir = LEFT;
        } else {
            // head in this case
            currentSurface = spritesCoord + SNAKE_HEAD + lastDir + head * NB_BASESPRITE_WIDTH;
            head = (head + 1) % NB_HEAD;
            continuer = SDL_FALSE; // loop exit
        }
        coord.x = x * SPRITE_WIDTH;
        coord.y = y * SPRITE_WIDTH;
        // sprit rendering
        SDL_BlitSurface(sprites,currentSurface,screen,&coord);
        // current coord update
        newCoord(lastDir,&x,&y);
        currentCase = map[x][y];
    } 
}

// load the level
void loadLevel(int map[NB_CASE_WIDTH][NB_CASE_HEIGHT],Coord* lastCoord, Coord* head, int* direction) {
    char pathBuff[MAX_PATH_LENGTH];
    // save file loading
    FILE* file = fopen(editPath(SAVEFILEPATH,SAVEFILENAME,pathBuff),"r");
    // check for error
    if(file == NULL) {
        fprintf(stderr,"erreur ouverture %s",pathBuff);
        exit(EXIT_FAILURE);
    }
    // first 2 char are end of snake coord
    lastCoord->x = fgetc(file) - ASCII_OFFSET;
    lastCoord->y = fgetc(file) - ASCII_OFFSET;
    // next 2 are head coord
    head->x = fgetc(file) - ASCII_OFFSET;
    head->y = fgetc(file) - ASCII_OFFSET;
    // next is current direction
    *direction = fgetc(file) - ASCII_OFFSET;
    // the each char goes from left to right, top to bottom
    int i = 0, j = 0;
    while(i < NB_CASE_WIDTH) {
        map[i][j] = fgetc(file) - ASCII_OFFSET;
        j++;
        if(j == NB_CASE_HEIGHT) {
            j = 0;
            i++;
        }
    }
    fclose(file);
}

// save the level
void saveLevel(int map[NB_CASE_WIDTH][NB_CASE_HEIGHT], Coord lastCoord, Coord head, int direction) {
    // does everything as loadLevel but write instead of reading
    char pathBuff[MAX_PATH_LENGTH];
    // save file openings
    FILE* file = fopen(editPath(SAVEFILEPATH,SAVEFILENAME,pathBuff),"w+");
    // error checking
    if(file == NULL) {
        fprintf(stderr,"erreur ouverture %s",SAVEFILENAME);
        exit(EXIT_FAILURE);
    }
    // char order is same as loadLevel
    fputc(lastCoord.x + ASCII_OFFSET,file);
    fputc(lastCoord.y + ASCII_OFFSET,file);
    fputc(head.x + ASCII_OFFSET,file);
    fputc(head.y + ASCII_OFFSET,file);
    fputc(direction + ASCII_OFFSET,file);
    int i = 0, j = 0;
    while(i < NB_CASE_WIDTH) {
        fputc(map[i][j] + ASCII_OFFSET,file);
        j++;
        if(j == NB_CASE_HEIGHT) {
            j = 0;
            i++;
        }
    }
    fclose(file);
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
void createTarget(int map[NB_CASE_WIDTH][NB_CASE_HEIGHT]) {
    int x,y;
    do {
        x = rand() % NB_CASE_WIDTH;
        y = rand() % NB_CASE_HEIGHT;
    } while(map[x][y] != 0);
    map[x][y] = TARGET;
}

// take a map and render every element other than the snake
void renderMap(int map[NB_CASE_WIDTH][NB_CASE_HEIGHT], SDL_Surface* screen, SDL_Surface* sprites, SDL_Rect* spritesCoord) {
    SDL_Rect pos;
    for(int i = 0; i < NB_CASE_WIDTH;i++) {
        for(int j = 0; j < NB_CASE_HEIGHT;j++) {
            switch(map[i][j]) {
                case TARGET:
                    pos.x = i * SPRITE_WIDTH;
                    pos.y = j * SPRITE_HEIGHT;
                    SDL_BlitSurface(sprites,spritesCoord + SPRITE_TARGET,screen,&pos);
                    break;
                case WALL:
                    pos.x = i * SPRITE_WIDTH;
                    pos.y = j * SPRITE_HEIGHT;
                    SDL_BlitSurface(sprites,spritesCoord + SPRITE_WALL,screen,&pos);
                    break;
            }
        }
    }
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