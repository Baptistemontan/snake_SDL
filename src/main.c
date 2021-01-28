#include "../headers/linker.h"

// #define FRAMERATE 10 // max framerate | default : 10 | disabled if set to 0 or commented
#define FRAMECOUNTER // output framerate if defined

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

    // variable init and surfaces loading
    SDL_Surface *screen = SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    checkSurface(screen,SDL_TRUE);
    SDL_Surface* sprites = loadSprite(SPRITESNAME,SDL_TRUE); // load all the sprites at once
    SDL_Surface* background = loadSprite(BACKGROUNDNAME,SDL_FALSE); // load background sprite, does not exit if not found, but error will appear
    if(background == NULL) {
        background = SDL_CreateRGBSurface(SDL_HWSURFACE, WINDOW_WIDTH,WINDOW_HEIGHT,32,0,0,0,0);
        checkSurface(background,SDL_TRUE);
        SDL_FillRect(background,NULL,SDL_MapRGB(background->format,DEFAULTBACKGROUNDCOLOR));
    }
    SDL_Rect* spritesCoord = createSnakeCoord(); // create Rect for every sprite in the surface sprites
    SDL_bool continuerMain = SDL_TRUE; // loop as long as true
    int map[NB_CASE_WIDTH][NB_CASE_HEIGHT];
    Coord last, head; // store the coord of the head and the end of the snake
    int direction, newDirection; // current direction, newDirection is modified by the event thread
    
    // window caption
    SDL_WM_SetCaption("SNAKE", NULL);

    //chargement du niveau
    if(loadLevel(map,&last,&head,&direction) == SDL_FALSE) {
        if(defaultLevel(map,&last,&head,&direction) == SDL_FALSE){
            fprintf(stderr,"board size too small.\n");
            exit(EXIT_FAILURE);
        }
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
    renderMap(map,screen,sprites,spritesCoord);

    // target and score creation
    createTarget(map,screen,sprites,spritesCoord);
    int score = 0;


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
        newCoord(direction,&(head.x),&(head.y));
        if(map[head.x][head.y] == TARGET) {
            // score incrementation when fruit is eaten
            score++;
            fprintf(stderr,"score : %d\n",score);
            createTarget(map,screen,sprites,spritesCoord);
        } else {
            // move the end of the snake
            renderSnakeEnd(map,screen,sprites,spritesCoord,background,&last);
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
        renderSnakeHead(map,screen,sprites,spritesCoord,background,last,head);
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
