#ifndef HEAD_DEFINE
#define HEAD_DEFINE

    #include "linker.h"

    #define SPRITE_WIDTH 40 // width of a sprite bloc in pxl
    #define SPRITE_HEIGHT 40 // height of a sprite bloc in pxl
    #define SPRITESNAME "snake.png"
    // #define SPRITE_WIDTH 64 //40 // width of a sprite bloc in pxl
    // #define SPRITE_HEIGHT 64 //40 // height of a sprite bloc in pxl
    // #define SPRITESNAME "snake_2.png"

    #define NB_BASESPRITE_WIDTH 4 // nb of sprite in a row of the base sprite image
    #define NB_BASESPRITE_HEIGHT 4 // nb of sprite in a coll of the base sprite image
    #define NB_SPRITES NB_BASESPRITE_HEIGHT * NB_BASESPRITE_WIDTH // total number of sprites
    #define NB_CASE_WIDTH 12 // NB of case in a row of the window | min 3
    #define NB_CASE_HEIGHT 12 // NB of case in a col of the window | min 3
    #define WINDOW_WIDTH SPRITE_WIDTH * NB_CASE_WIDTH // window width in pxl
    #define WINDOW_HEIGHT SPRITE_HEIGHT * NB_CASE_HEIGHT // window height in pxl
    #define MAX_PATH_LENGTH 100 // max size of the path buffer
    #define SPRITESPATH "sprites/%s" // path to the sprites folder
    #define BACKGROUNDNAME "background.png"
    #define ASCII_OFFSET '!' // ascii offset, can be 0 but the lvl file will be hard to modifie by hand
    #define SAVEFILENAME "map.lvl" // name of the lvl save map
    #define SAVEFILEPATH "./saves/%s" // path to the save folder
    #define DEFAULTBACKGROUNDCOLOR 0x00,0x00,0x00 //RGB

    // mutex for the direction variable
    extern pthread_mutex_t directionMutex;

    
    // enum of the map elements
    enum {
        EMPTY          = 0x00,
        SNAKE_MASK     = 0x01,
        TARGET         = 0x02,
        WALL           = 0x04,
        //             = 0x08,
        UP_MASK        = 0x10,
        DOWN_MASK      = 0x20,
        RIGHT_MASK     = 0x40,
        LEFT_MASK      = 0x80
    };
    // enum of direction
    enum {
        DOWN    = 0,
        LEFT    = 1,
        UP      = 2,
        RIGHT   = 3
    };

    // sprite coord offset of every snake element
    enum {
        SNAKE_HEAD          = 0 * NB_BASESPRITE_WIDTH,
        SNAKE_TURN          = 1 * NB_BASESPRITE_WIDTH,
        SNAKE_END           = 2 * NB_BASESPRITE_WIDTH,
        SNAKE_BODY          = 3 * NB_BASESPRITE_WIDTH,
        SPRITE_TARGET       = SNAKE_BODY + 2,
        SPRITE_WALL         = SPRITE_TARGET + 1
    };

    // store coords
    typedef struct {
        int x, y;
    } Coord;

#endif