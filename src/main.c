#include "../headers/linker.h"

#define FRAMERATE 10 // max framerate | default : 10 | disabled if set to 0 or commented
// #define FRAMECOUNTER // output framerate if defined

// globals variables
pthread_mutex_t directionMutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char const *argv[])
{
    // SDL initialisation
    if(SDL_Init(SDL_INIT_VIDEO) == -1) {
        fprintf(stderr, "erreur initialisation SDL : %s\n",SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // rand functions init
    srand(time(NULL));

    const int nbCaseWidth = argc > ARG_NB_CASE_WIDTH ? initWidthHeight(argv[ARG_NB_CASE_WIDTH],0) : DEFAULT_NB_CASE_WIDTH;
    const int nbCaseHeight = argc > ARG_NB_CASE_HEIGHT ? initWidthHeight(argv[ARG_NB_CASE_WIDTH],1) : DEFAULT_NB_CASE_HEIGHT;

    // sprites loading
    SDL_Surface* sprites = argc > ARG_SPRITES_FILE && strlen(argv[ARG_SPRITES_FILE]) > 0 ? loadSprite(argv[ARG_SPRITES_FILE],SDL_FALSE) : NULL; // load all the sprites at once
    if(sprites == NULL) sprites = loadSprite(DEFAULTSPRITESNAME,SDL_TRUE);
    //sprite cutting
    SDL_Rect* spritesCoord = createSpritesCoord(sprites); // create Rect for every sprite in the surface sprites
    // window creation
    const int windowWidth = nbCaseWidth * spritesCoord->w, windowHeight = nbCaseHeight * spritesCoord->h;
    SDL_Surface *screen = SDL_SetVideoMode(windowWidth, windowHeight, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    checkSurface(screen,SDL_TRUE);
    //background creation
    SDL_Surface* background = argc > ARG_BACKGROUND_FILE && strlen(argv[ARG_BACKGROUND_FILE]) > 0 ? loadSprite(argv[ARG_BACKGROUND_FILE],SDL_FALSE) : NULL; // load all the sprites at once
    if(background == NULL) background = loadSprite(BACKGROUNDNAME,SDL_FALSE);
    if(background == NULL) {
        background = SDL_CreateRGBSurface(SDL_HWSURFACE,windowWidth,windowHeight,32,0,0,0,0);
        checkSurface(background,SDL_TRUE);
        SDL_FillRect(background,NULL,SDL_MapRGB(background->format,DEFAULTBACKGROUNDCOLOR));
    }

    // variable creation
    SDL_bool continuerMain = SDL_TRUE; // loop as long as true
    int map[nbCaseWidth][nbCaseHeight];
    Coord last, head; // store the coord of the head and the end of the snake
    int direction, newDirection; // current direction, newDirection is modified by the event thread
    
    // window caption
    SDL_WM_SetCaption("SNAKE", NULL);

    //level loading
    if(argc < ARG_SAVE_FILE || strlen(argv[ARG_SAVE_FILE]) == 0 || loadLevel(nbCaseWidth,nbCaseHeight,map,&last,&head,&direction,argv[ARG_SAVE_FILE]) == SDL_FALSE) {
        defaultLevel(nbCaseWidth,nbCaseHeight,map,&last,&head,&direction);
    }
    newDirection = direction;


    // enable key press repeating
    // make the input more responsive when switching direction rapidly
    SDL_EnableKeyRepeat(50, 50);

    // creation of a seperate thread for event handling
    void** eventsThreadArgs = malloc(sizeof(void*) * 3); // free directly in the function
    eventsThreadArgs[0] = &continuerMain;
    eventsThreadArgs[1] = &direction;
    eventsThreadArgs[2] = &newDirection;
    pthread_t eventsThread;
    pthread_create(&eventsThread,NULL,waitEvent,eventsThreadArgs);

    #ifdef FRAMECOUNTER
    // basic framerate counter part 1/2
    Uint32 oldTime = SDL_GetTicks(), newTime;
    uint frame = 0;

    unsigned long totalFrame = 0;
    uint nbReset = 0;
    #endif

    // render map elements such as walls
    renderMap(nbCaseWidth,nbCaseHeight,map,screen,sprites,spritesCoord);
    renderSnake(nbCaseWidth,nbCaseHeight,map,last,sprites,spritesCoord,screen);

    // target and score creation
    createTarget(nbCaseWidth,nbCaseHeight,map,screen,sprites,spritesCoord);
    int score = 0;

    SDL_Flip(screen);

    while(continuerMain) {
        // update direction from the event thread
        pthread_mutex_lock(&directionMutex);
        direction = newDirection;
        pthread_mutex_unlock(&directionMutex);
        // update the map according to the direction
        switch(direction) {
            case UP:
                map[head.x][head.y] = SNAKE_MASK | UP_MASK;
                break;
            case DOWN:
                map[head.x][head.y] = SNAKE_MASK | DOWN_MASK;
                break;
            case LEFT:
                map[head.x][head.y] = SNAKE_MASK | LEFT_MASK;
                break;
            case RIGHT:
                map[head.x][head.y] = SNAKE_MASK | RIGHT_MASK;
                break;
        }
        // move the head according to the direction
        newCoord(nbCaseWidth,nbCaseHeight,direction,&(head.x),&(head.y));
        #ifndef NOBOUNDARIES
        if(head.x < 0 || head.y < 0 || head.x >= nbCaseWidth || head.y >= nbCaseHeight) {
            continuerMain = SDL_FALSE;
            break;
        }
        #endif
        if(map[head.x][head.y] == TARGET) {
            // score incrementation when fruit is eaten
            score++;
            fprintf(stderr,"score : %d\n",score);
            createTarget(nbCaseWidth,nbCaseHeight,map,screen,sprites,spritesCoord);
        } else {
            // move the end of the snake
            renderSnakeEnd(nbCaseWidth,nbCaseHeight,map,screen,sprites,spritesCoord,background,&last);
            // collision checks
            if(map[head.x][head.y] & SNAKE_MASK || map[head.x][head.y] == WALL){
                // pauseGame();
                continuerMain = SDL_FALSE; // exiting main loop
                break;
            }
        }
        // map update
        map[head.x][head.y] = SNAKE_MASK;
        //render the head
        renderSnakeHead(nbCaseWidth,nbCaseHeight,map,screen,sprites,spritesCoord,background,last,head);
        // display the screen
        SDL_Flip(screen);
        // force cap the framerate
        #if FRAMERATE != 0
        SDL_Delay(1000 / FRAMERATE);
        #endif
        // basic framerate counter part 2/2
        #ifdef FRAMECOUNTER
        frame++;
        newTime = SDL_GetTicks();
        if(newTime - oldTime >= 1000) {
            oldTime = newTime;
            fprintf(stderr,"framerate : %d\n",frame);
            totalFrame += frame;
            nbReset++;
            frame = 0;
        }
        #endif
    }
    #ifdef FRAMECOUNTER
    fprintf(stderr,"average framerate : %lu\n", totalFrame / nbReset);
    #endif
    // force termination of the event thread
    pthread_cancel(eventsThread);
    // variable freeing
    free(spritesCoord);
    SDL_FreeSurface(sprites);
    SDL_FreeSurface(background);
    // end of programme
    SDL_Quit();
    return EXIT_SUCCESS;
}
