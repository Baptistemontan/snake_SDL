#ifndef HEAD_DEFINE
#define HEAD_DEFINE

    #include "linker.h"

    #define SPRITE_WIDTH 40
    #define SPRITE_HEIGHT 40
    #define BASESPRITE_WIDTH 126
    #define BASESPRITE_HEIGHT 126
    #define NB_BASESPRITE_WIDTH 4
    #define NB_BASESPRITE_HEIGHT 6
    #define NB_SPRITE NB_BASESPRITE_HEIGHT * NB_BASESPRITE_WIDTH
    #define NB_CASE_WIDTH 12
    #define NB_CASE_HEIGHT 12
    #define WINDOW_WIDTH SPRITE_WIDTH * NB_CASE_WIDTH
    #define WINDOW_HEIGHT SPRITE_HEIGHT * NB_CASE_HEIGHT
    #define SPRITESPATH "sprites/%s"
    #define ASCII_OFFSET '!'
    #define SAVEFILENAME "carte.lvl"
    #define SAVEFILEPATH "./%s"


    #ifdef __unix__
    #include <unistd.h>
    #define sleep(ms) usleep(ms * 1000)
    #else
    #include <windows.h>
    #define sleep(ms) Sleep(ms)
    #endif

    extern char pathBuff[];
    extern pthread_mutex_t directionMutex;

    
    typedef enum {
        VIDE           = 0x00,
        SNAKE_MASK     = 0x01,
        TARGET         = 0x02,
        WALL           = 0x04,
        //             = 0x08,
        UP_MASK        = 0x10,
        DOWN_MASK      = 0x20,
        RIGHT_MASK     = 0x40,
        LEFT_MASK      = 0x80
    } CASE;

    enum {
        DOWN    = 0,
        LEFT    = 1,
        UP      = 2,
        RIGHT   = 3,
        QUIT
    };

    #define NB_HEAD 3

    enum {
        SNAKE_HEAD          = 0 * NB_BASESPRITE_WIDTH,
        SNAKE_TURN          = 3 * NB_BASESPRITE_WIDTH,
        SNAKE_END           = 4 * NB_BASESPRITE_WIDTH,
        SNAKE_BODY          = 5 * NB_BASESPRITE_WIDTH,
        SPRITE_WALL         = 22,
        SPRITE_TARGET       = 23
    };

    typedef struct {
        int x, y;
    } Coord;

#endif