#include "../headers/linker.h"

// render the snake according to the map
// deprecated
void renderSnake(int map[NB_CASE_WIDTH][NB_CASE_HEIGHT],Coord lastCoord, SDL_Surface* sprites, SDL_Rect* spritesCoord, SDL_Surface* screen) {
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
            currentSurface = spritesCoord + SNAKE_HEAD + lastDir;
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

// take a map and render every element other than the snake
void renderMap(int map[NB_CASE_WIDTH][NB_CASE_HEIGHT], SDL_Surface* screen, SDL_Surface* sprites, SDL_Rect* spritesCoord) {
    SDL_Rect pos;
    for(int i = 0; i < NB_CASE_WIDTH;i++) {
        for(int j = 0; j < NB_CASE_HEIGHT;j++) {
            switch(map[i][j]) {
                case WALL:
                    pos.x = i * SPRITE_WIDTH;
                    pos.y = j * SPRITE_HEIGHT;
                    SDL_BlitSurface(sprites,spritesCoord + SPRITE_WALL,screen,&pos);
                    break;
            }
        }
    }
}

void renderSnakeEnd(int map[NB_CASE_WIDTH][NB_CASE_HEIGHT], SDL_Surface* screen, SDL_Surface* sprites, SDL_Rect* spritesCoord, SDL_Surface* background, Coord* lastCoord) {
    SDL_Rect pos ={.w = SPRITE_WIDTH, .h = SPRITE_HEIGHT,.x = lastCoord->x * SPRITE_WIDTH, .y = lastCoord->y * SPRITE_HEIGHT};
    SDL_BlitSurface(background,&pos,screen,&pos);
    updateLastCoord(map,lastCoord);
    SDL_Rect* newPos;
    if(map[lastCoord->x][lastCoord->y] & UP_MASK) {
        newPos = spritesCoord + SNAKE_END + UP;
    } else if(map[lastCoord->x][lastCoord->y] & DOWN_MASK) {
        newPos = spritesCoord + SNAKE_END + DOWN;
    } else if(map[lastCoord->x][lastCoord->y] & LEFT_MASK) {
        newPos = spritesCoord + SNAKE_END + LEFT;
    } else { // RIGHT
        newPos = spritesCoord + SNAKE_END + RIGHT;
    }
    pos.x = lastCoord->x * SPRITE_WIDTH;
    pos.y = lastCoord->y * SPRITE_HEIGHT;
    SDL_BlitSurface(background,&pos,screen,&pos);
    SDL_BlitSurface(sprites,newPos,screen,&pos);
}

void renderSnakeHead(int map[NB_CASE_WIDTH][NB_CASE_HEIGHT], SDL_Surface* screen, SDL_Surface* sprites, SDL_Rect* spritesCoord, SDL_Surface* background, Coord lastCoord, Coord headCoord) {
    Coord last, next = lastCoord;
    int direction, lastDir;
    while(next.x != headCoord.x || next.y != headCoord.y) {
        last = next;
        lastDir = direction;
        if(map[last.x][last.y] & UP_MASK) {
            direction = UP;
        } else if(map[last.x][last.y] & DOWN_MASK) {
            direction = DOWN;
        } else if(map[last.x][last.y] & LEFT_MASK) {
            direction = LEFT;
        } else { // RIGHT
            direction = RIGHT;
        }
        newCoord(direction,&(next.x),&(next.y));
    }
    SDL_Rect* currentSprite;
    switch(lastDir) {
        case UP:
            switch(direction) {
                case UP:
                    currentSprite = spritesCoord + SNAKE_BODY;
                    break;
                case LEFT:
                    currentSprite = spritesCoord + SNAKE_TURN;
                    break;
                case RIGHT:
                    currentSprite = spritesCoord + SNAKE_TURN + 1;
                    break;
            }
            break;
        case DOWN:
            switch(direction) {
                case DOWN:
                    currentSprite = spritesCoord + SNAKE_BODY;
                    break;
                case LEFT:
                    currentSprite = spritesCoord + SNAKE_TURN + UP;
                    break;
                case RIGHT:
                    currentSprite = spritesCoord + SNAKE_TURN + UP + 1;
                    break;
            }
            break;
        case RIGHT:
            switch(direction) {
                case RIGHT:
                    currentSprite = spritesCoord + SNAKE_BODY + 1;
                    break;
                case UP:
                    currentSprite = spritesCoord + SNAKE_TURN + UP;
                    break;
                case DOWN:
                    currentSprite = spritesCoord + SNAKE_TURN;
                    break;
            }
            break;
        case LEFT:
            switch(direction) {
                case LEFT:
                    currentSprite = spritesCoord + SNAKE_BODY + 1;
                    break;
                case UP:
                    currentSprite = spritesCoord + SNAKE_TURN + UP + 1;
                    break;
                case DOWN:
                    currentSprite = spritesCoord + SNAKE_TURN + 1;
                    break;
            }
            break;
    }
    SDL_Rect pos ={.w = SPRITE_WIDTH, .h = SPRITE_HEIGHT,.x = last.x * SPRITE_WIDTH, .y = last.y * SPRITE_HEIGHT};
    SDL_BlitSurface(background,&pos,screen,&pos);
    SDL_BlitSurface(sprites,currentSprite,screen,&pos);
    pos.x = next.x * SPRITE_WIDTH;
    pos.y = next.y * SPRITE_HEIGHT;
    SDL_BlitSurface(background,&pos,screen,&pos);
    SDL_BlitSurface(sprites,spritesCoord + SNAKE_HEAD + direction,screen,&pos);
}
