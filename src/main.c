#include "../headers/linker.h"

#define FRAMERATE 10

// globals variables
char pathBuff[50];
pthread_mutex_t directionMutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char const *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    srand(time(NULL));

    // initialisation des variables et chargement des surfaces
    SDL_Surface *ecran = SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    SDL_WM_SetCaption("SNAKE", NULL);
    SDL_Surface* sprites = loadSprite("snake.png");
    SDL_Rect* spritesCoord = creatSnakeCoord();
    SDL_bool continuerMain = SDL_TRUE;
    int carte[NB_CASE_WIDTH][NB_CASE_HEIGHT];
    Coord last, head;
    int direction, newDirection;
    // for(int i = 0; i < NB_CASE_HEIGHT;i++) {
    //     for(int j = 0; j < NB_CASE_WIDTH;j++) {
    //         carte[j][i] = VIDE;
    //     }
    // }
    // last.x = 3;
    // last.y = 6;
    // carte[3][6] = SNAKE_MASK | UP_MASK;
    // carte[3][5] = SNAKE_MASK | RIGHT_MASK;
    // carte[4][5] = SNAKE_MASK;
    // head.x = 4;
    // head.y = 5;
    // direction = RIGHT;
    // saveLevel(carte,last,head,direction);

    //chargement du niveau
    loadLevel(carte,&last,&head,&direction);
    newDirection = direction;    

    // creation of a seperate thread for event handling
    void** eventsThreadArgs = malloc(sizeof(void*) * 3); // free directly in the function
    eventsThreadArgs[0] = &continuerMain;
    eventsThreadArgs[1] = &direction;
    eventsThreadArgs[2] = &newDirection;
    pthread_t eventsThread;
    pthread_create(&eventsThread,NULL,waitEvent,eventsThreadArgs);

    // struct timespec time, newTime;
    // clock_gettime(0,&time);
    // int frame = 0;

    createTarget(carte);
    int score = 0;


    while(continuerMain) {
        pthread_mutex_lock(&directionMutex);
        direction = newDirection;
        pthread_mutex_unlock(&directionMutex);
        switch(direction) {
            case UP:
                carte[head.x][head.y] = SNAKE_MASK | UP_MASK;
                break;
            case DOWN:
                carte[head.x][head.y] = SNAKE_MASK | DOWN_MASK;
                break;
            case LEFT:
                carte[head.x][head.y] = SNAKE_MASK | LEFT_MASK;
                break;
            case RIGHT:
                carte[head.x][head.y] = SNAKE_MASK | RIGHT_MASK;
                break;
        }
        newCoord(direction,&(head.x),&(head.y));
        if(carte[head.x][head.y] == TARGET) {
            score++;
            fprintf(stderr,"score : %d\n",score);
            createTarget(carte);
        } else if(carte[head.x][head.y] & SNAKE_MASK || carte[head.x][head.y] == WALL){
            continuerMain = SDL_FALSE;
            break;
        } else {
            updateLastCoord(carte,&last);
        }
        carte[head.x][head.y] = SNAKE_MASK;
        SDL_FillRect(ecran,NULL,SDL_MapRGB(ecran->format,0,0,0));
        renderMap(carte,ecran,sprites,spritesCoord);
        renderSnake(carte,last,sprites,spritesCoord,ecran);
        SDL_Flip(ecran);
        // fprintf(stderr,"%d %d %d %d %d\n",last.x,last.y,head.x,head.y,direction);
        sleep(1000 / FRAMERATE);
        // frame++;
        // clock_gettime(0,&newTime);
        // if(time.tv_sec + 1 <= newTime.tv_sec) {
        //     fprintf(stderr,"%d\n",frame);
        //     frame = 0;
        //     time = newTime;
        // }
    }
    pthread_cancel(eventsThread);
    free(spritesCoord);
    SDL_FreeSurface(sprites);
    SDL_Quit();
    return EXIT_SUCCESS;
}
