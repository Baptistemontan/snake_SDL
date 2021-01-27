#include "../headers/linker.h"

#define FRAMERATE 10

// globals variables
pthread_mutex_t directionMutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char const *argv[])
{
    // SDL initialisation
    SDL_Init(SDL_INIT_VIDEO);

    // rand functions init
    srand(time(NULL));

    // variable init and surfaces loading
    SDL_Surface *screen = SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    SDL_Surface* sprites = loadSprite("snake.png"); // load all the sprites at once
    SDL_Rect* spritesCoord = creatSnakeCoord(); // create Rect for every sprite in the surface sprites
    SDL_bool continuerMain = SDL_TRUE; // loop as long as true
    int map[NB_CASE_WIDTH][NB_CASE_HEIGHT];
    Coord last, head; // store the coord of the head and the end of the snake
    int direction, newDirection; // current direction, newDirection is modified by the event thread
    
    // window caption
    SDL_WM_SetCaption("SNAKE", NULL);

    // level creation
    // for(int i = 0; i < NB_CASE_HEIGHT;i++) {
    //     for(int j = 0; j < NB_CASE_WIDTH;j++) {
    //         map[j][i] = EMPTY;
    //     }
    // }
    // last.x = 3;
    // last.y = 6;
    // map[3][6] = SNAKE_MASK | UP_MASK;
    // map[3][5] = SNAKE_MASK | RIGHT_MASK;
    // map[4][5] = SNAKE_MASK;
    // head.x = 4;
    // head.y = 5;
    // direction = RIGHT;
    // saveLevel(map,last,head,direction);

    //chargement du niveau
    loadLevel(map,&last,&head,&direction);
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


    // basic framerate counter part 1/2
    // struct timespec time, newTime;
    // clock_gettime(0,&time);
    // int frame = 0;

    // target and score creation
    createTarget(map);
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
            createTarget(map);
        } else {
            // move the end of the snake
            updateLastCoord(map,&last);
            // collision checks
            if(map[head.x][head.y] & SNAKE_MASK || map[head.x][head.y] == WALL){
                // pauseGame();
                continuerMain = SDL_FALSE; // exiting main loop
                break;
            }
        }
        // map update
        map[head.x][head.y] = SNAKE_MASK;
        // screen reset
        SDL_FillRect(screen,NULL,SDL_MapRGB(screen->format,0,0,0));
        // re-render map elements
        renderMap(map,screen,sprites,spritesCoord);
        // re-render the snake
        renderSnake(map,last,sprites,spritesCoord,screen);
        // display the screen
        SDL_Flip(screen);
        // force cap the framerate
        sleep(1000 / FRAMERATE);

        // basic framerate counter part 2/2
        // frame++;
        // clock_gettime(0,&newTime);
        // if(time.tv_sec + 1 <= newTime.tv_sec) {
        //     fprintf(stderr,"%d\n",frame);
        //     frame = 0;
        //     time = newTime;
        // }
    }
    // force termination of the event thread
    pthread_cancel(eventsThread);
    // variable freeing
    free(spritesCoord);
    SDL_FreeSurface(sprites);
    // end of programme
    SDL_Quit();
    return EXIT_SUCCESS;
}
