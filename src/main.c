#include "../headers/linker.h"

#define FRAMERATE 10

char pathBuff[50];

pthread_mutex_t directionMutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char const *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);

    // initialisation des variables et chargement
    SDL_Surface *ecran = SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    SDL_WM_SetCaption("SNAKE", NULL);

    SDL_Surface* snake = loadSprite("snake.png");

    SDL_Rect* snakeCoord = creatSnakeCoord();

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
    loadLevel(carte,&last,&head,&direction);
    newDirection = direction;
    // fprintf(stderr,"last: %d %d\nhead: %d %d\ndirection: %d\n",last.x,last.y,head.x,head.y,direction);
    // for(int i = 0; i < NB_CASE_HEIGHT;i++) {
    //     for(int j = 0; j < NB_CASE_WIDTH;j++) {
    //         fprintf(stderr,"%d ",carte[j][i]);
    //     }
    //     fprintf(stderr,"\n");
    // }

    // blitSnake(carte,last,snake,snakeCoord,ecran);
    
    // SDL_Flip(ecran);


    

    void** eventsThreadArgs = malloc(sizeof(void*) * 3);
    eventsThreadArgs[0] = &continuerMain;
    eventsThreadArgs[1] = &direction;
    eventsThreadArgs[2] = &newDirection;
    pthread_t eventsThread;
    pthread_create(&eventsThread,NULL,waitEvent,eventsThreadArgs);


    while(continuerMain) {
        pthread_mutex_lock(&directionMutex);
        direction = newDirection;
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
        updateLastCoord(carte,&last);
        newCoord(direction,&(head.x),&(head.y));
        pthread_mutex_unlock(&directionMutex);
        carte[head.x][head.y] = SNAKE_MASK;
        blitSnake(carte,last,snake,snakeCoord,ecran);
        SDL_Flip(ecran);
        // fprintf(stderr,"%d %d %d %d %d\n",last.x,last.y,head.x,head.y,direction);
        sleep(1000 / FRAMERATE);
    }
    pthread_join(eventsThread,NULL);
    free(snakeCoord);
    SDL_FreeSurface(snake);
    SDL_Quit();
    return EXIT_SUCCESS;
}