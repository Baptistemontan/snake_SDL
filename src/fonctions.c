#include "../headers/linker.h"

char* editPath(char* folderPath, char* fileName, char* buff) {
    sprintf(buff,folderPath,fileName);
    return buff;
}

SDL_Surface* loadSprite(char* name) {
    SDL_Surface* sprite = IMG_Load(editPath(SPRITESPATH,name,pathBuff));
    checkSurface(sprite);
    return sprite;
}

void checkSurface(SDL_Surface* surface) {
    if(surface == NULL) {
        fprintf(stderr, "erreur surface: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
}

SDL_Rect* creatSnakeCoord() {
    SDL_Rect *spritesCoord = malloc(sizeof(SDL_Rect) * NB_BASESPRITE_HEIGHT * NB_BASESPRITE_WIDTH);

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

void blitSnake(int carte[NB_CASE_WIDTH][NB_CASE_HEIGHT],Coord lastCoord, SDL_Surface* snake, SDL_Rect* snakeCoord, SDL_Surface* ecran) {
    static int head = 0;
    SDL_FillRect(ecran,NULL,SDL_MapRGB(ecran->format,0,0,0));
    int lastDir,x = lastCoord.x,y = lastCoord.y;
    int currentCase = carte[x][y];
    SDL_Rect* currentSurface;
    SDL_bool continuer = SDL_TRUE;
    SDL_Rect coord;
    if(currentCase & UP_MASK) {
        currentSurface = snakeCoord + SNAKE_END + UP;
        lastDir = UP;
    } else if(currentCase & DOWN_MASK) {
        currentSurface = snakeCoord + SNAKE_END + DOWN;
        lastDir = DOWN;
    } else if(currentCase & LEFT_MASK) {
        currentSurface = snakeCoord + SNAKE_END + LEFT;
        lastDir = LEFT;
    } else if(currentCase & RIGHT_MASK) {
        currentSurface = snakeCoord + SNAKE_END + RIGHT;
        lastDir = RIGHT;
    }
    coord.x = x * SPRITE_WIDTH;
    coord.y = y * SPRITE_WIDTH;
    newCoord(lastDir,&x,&y);
    currentCase = carte[x][y];
    SDL_BlitSurface(snake,currentSurface,ecran,&coord);
    while(continuer) {
        if(currentCase & UP_MASK) {
            switch(lastDir) {
                case UP:
                    currentSurface = snakeCoord + SNAKE_BODY;
                    break;
                case LEFT:
                    currentSurface = snakeCoord + SNAKE_TURN + UP + 1;
                    break;
                case RIGHT:
                    currentSurface = snakeCoord + SNAKE_TURN + UP;
                    break;
            }
            lastDir = UP;
        } else if(currentCase & DOWN_MASK) {
            switch(lastDir) {
                case DOWN:
                    currentSurface = snakeCoord + SNAKE_BODY;
                    break;
                case LEFT:
                    currentSurface = snakeCoord + SNAKE_TURN + 1;
                    break;
                case RIGHT:
                    currentSurface = snakeCoord + SNAKE_TURN;
                    break;
            }
            lastDir = DOWN;
        } else if(currentCase & RIGHT_MASK) {
            switch(lastDir) {
                case RIGHT:
                    currentSurface = snakeCoord + SNAKE_BODY + 1;
                    break;
                case DOWN:
                    currentSurface = snakeCoord + SNAKE_TURN + 1 + UP;
                    break;
                case UP:
                    currentSurface = snakeCoord + SNAKE_TURN + 1;
                    break;
            }
            lastDir = RIGHT;
        } else if(currentCase & LEFT_MASK) {
            switch(lastDir) {
                case LEFT:
                    currentSurface = snakeCoord + SNAKE_BODY + 1;
                    break;
                case DOWN:
                    currentSurface = snakeCoord + SNAKE_TURN + UP;
                    break;
                case UP:
                    currentSurface = snakeCoord + SNAKE_TURN;
                    break;
            }
            lastDir = LEFT;
        } else {
            currentSurface = snakeCoord + SNAKE_HEAD + lastDir + head * NB_BASESPRITE_WIDTH;
            head = (head + 1) % NB_HEAD;
            continuer = SDL_FALSE;
        }
        coord.x = x * SPRITE_WIDTH;
        coord.y = y * SPRITE_WIDTH;
        SDL_BlitSurface(snake,currentSurface,ecran,&coord);
        newCoord(lastDir,&x,&y);
        currentCase = carte[x][y];
    } 
}

void loadLevel(int carte[NB_CASE_WIDTH][NB_CASE_HEIGHT],Coord* lastCoord, Coord* head, int* direction) {
    FILE* file = fopen(editPath(SAVEFILEPATH,SAVEFILENAME,pathBuff),"r");
    if(file == NULL) {
        fprintf(stderr,"erreur ouverture %s",SAVEFILENAME);
        exit(EXIT_FAILURE);
    }
    lastCoord->x = fgetc(file) - ASCII_OFFSET;
    lastCoord->y = fgetc(file) - ASCII_OFFSET;
    head->x = fgetc(file) - ASCII_OFFSET;
    head->y = fgetc(file) - ASCII_OFFSET;
    *direction = fgetc(file) - ASCII_OFFSET;
    int i = 0, j = 0;
    while(i < NB_CASE_WIDTH) {
        carte[i][j] = fgetc(file) - ASCII_OFFSET;
        j++;
        if(j == NB_CASE_HEIGHT) {
            j = 0;
            i++;
        }
    }
    fclose(file);
}

void saveLevel(int carte[NB_CASE_WIDTH][NB_CASE_HEIGHT], Coord lastCoord, Coord head, int direction) {
    FILE* file = fopen(editPath(SAVEFILEPATH,SAVEFILENAME,pathBuff),"w+");
    if(file == NULL) {
        fprintf(stderr,"erreur ouverture %s",SAVEFILENAME);
        exit(EXIT_FAILURE);
    }
    fputc(lastCoord.x + ASCII_OFFSET,file);
    fputc(lastCoord.y + ASCII_OFFSET,file);
    fputc(head.x + ASCII_OFFSET,file);
    fputc(head.y + ASCII_OFFSET,file);
    fputc(direction + ASCII_OFFSET,file);
    int i = 0, j = 0;
    while(i < NB_CASE_WIDTH) {
        fputc(carte[i][j] + ASCII_OFFSET,file);
        j++;
        if(j == NB_CASE_HEIGHT) {
            j = 0;
            i++;
        }
    }
    fclose(file);
}

void updateLastCoord(int carte[NB_CASE_WIDTH][NB_CASE_HEIGHT],Coord *lastCoord) {
    int x = lastCoord->x, y = lastCoord->y;
    if(carte[x][y] & UP_MASK) {
        newCoord(UP,&x,&y);
    } else if (carte[x][y] & DOWN_MASK) {
        newCoord(DOWN,&x,&y);
    } else if(carte[x][y] & RIGHT_MASK) {
        newCoord(RIGHT,&x,&y);
    } else { // LEFT
        newCoord(LEFT,&x,&y);
    }
    carte[lastCoord->x][lastCoord->y] = VIDE;
    lastCoord->x = x;
    lastCoord->y = y;
}

void* waitEvent(void* arg) {
    SDL_Event event;
    SDL_bool* continuerMain = ((void**)arg)[0];
    int* direction = ((void**)arg)[1];
    int* newDirection = ((void**)arg)[2];
    while(*continuerMain) {
        SDL_WaitEvent(&event);
        switch (event.type) {
            case SDL_QUIT:
                *continuerMain = SDL_FALSE;
                break;
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym) {
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