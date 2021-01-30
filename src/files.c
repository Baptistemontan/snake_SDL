#include "../headers/linker.h"

// add a filename to a path and store it in a buffer, return the buffer
char* editPath(char* folderPath, const char* fileName, char* buff) {
    sprintf(buff,folderPath,fileName);
    return buff;
}

// check if a surface as been succesfully loaded
// print the error to stderr if not
void checkSurface(SDL_Surface* surface,SDL_bool terminate) {
    if(surface == NULL) {
        fprintf(stderr, "erreur surface: %s\n", SDL_GetError());
        if(terminate) exit(EXIT_FAILURE);
    }
}

// takes a filename in the sprites folder and return the SDL surfaces
SDL_Surface* loadSprite(const char* name, SDL_bool check) {
    char pathBuff[MAX_PATH_LENGTH];
    SDL_Surface* sprite = IMG_Load(editPath(SPRITESPATH,name,pathBuff));
    checkSurface(sprite,check);
    return sprite;
}

// save the level
void saveLevel(int nbCaseWidth, int nbCaseHeight, int map[nbCaseWidth][nbCaseHeight], Coord lastCoord, Coord head, int direction,const char* saveFileName) {
    // does everything as loadLevel but write instead of reading
    char pathBuff[MAX_PATH_LENGTH];
    // save file openings
    FILE* file = fopen(editPath(SAVEFILEPATH,saveFileName,pathBuff),"w+");
    // error checking
    if(file == NULL) {
        fprintf(stderr,"erreur ouverture %s",pathBuff);
        exit(EXIT_FAILURE);
    }
    fprintf(stderr,"saving level...\n");
    // char order is same as loadLevel
    fputc(nbCaseWidth + ASCII_OFFSET,file);
    fputc(nbCaseHeight + ASCII_OFFSET,file);
    fputc(lastCoord.x + ASCII_OFFSET,file);
    fputc(lastCoord.y + ASCII_OFFSET,file);
    fputc(head.x + ASCII_OFFSET,file);
    fputc(head.y + ASCII_OFFSET,file);
    fputc(direction + ASCII_OFFSET,file);
    int i = 0, j = 0;
    while(i < nbCaseWidth) {
        fputc(map[i][j] + ASCII_OFFSET,file);
        j++;
        if(j == nbCaseHeight) {
            j = 0;
            i++;
        }
    }
    fclose(file);
}

// load the level
SDL_bool loadLevel(int nbCaseWidth, int nbCaseHeight,int map[nbCaseWidth][nbCaseHeight],Coord* lastCoord, Coord* head, int* direction,const char* saveFileName) {
    char pathBuff[MAX_PATH_LENGTH];
    // save file loading
    FILE* file = fopen(editPath(SAVEFILEPATH,saveFileName,pathBuff),"r");
    // check for error
    if(file == NULL) {
        fprintf(stderr,"erreur ouverture %s, using default level.\n",pathBuff);
        return SDL_FALSE;
    }
    fprintf(stderr,"loading saved level ...\n");
    // first 2 char are the map dimension
    int width = fgetc(file) - ASCII_OFFSET;
    int height = fgetc(file) - ASCII_OFFSET;
    if(width != nbCaseWidth || height != nbCaseHeight) {
        fprintf(stderr,"save file dimension are not compatible : %d x %d required.\n",width,height);
        fprintf(stderr,"using default level.\n");
        fclose(file);
        return SDL_FALSE;
    }
    // next 2 char are end of snake coord
    lastCoord->x = fgetc(file) - ASCII_OFFSET;
    lastCoord->y = fgetc(file) - ASCII_OFFSET;
    // next 2 are head coord
    head->x = fgetc(file) - ASCII_OFFSET;
    head->y = fgetc(file) - ASCII_OFFSET;
    // next is current direction
    *direction = fgetc(file) - ASCII_OFFSET;
    // the each char goes from left to right, top to bottom
    int i = 0, j = 0;
    while(i < nbCaseWidth) {
        map[i][j] = fgetc(file) - ASCII_OFFSET;
        j++;
        if(j == nbCaseHeight) {
            j = 0;
            i++;
        }
    }
    fclose(file);
    return SDL_TRUE;
}