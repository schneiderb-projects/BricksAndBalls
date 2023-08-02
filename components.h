#include <stdint.h>

#define DISPLAY_WIDTH 84
#define DISPLAY_HEIGHT 48

#define ROWS 8
#define COLS 6

#define COL_PIXEL_WIDTH (DISPLAY_WIDTH / COLS)
#define ROW_PIXEL_HEIGHT (DISPLAY_HEIGHT / ROWS)

#define STANDARD_BRICK_PIXEL_WIDTH (COL_PIXEL_WIDTH - 1)
#define STANDARD_BRICK_PIXEL_HEIGHT (ROW_PIXEL_HEIGHT - 1)

#define STANDARD_PADAL_PIXEL_WIDTH (DISPLAY_WIDTH / COLS)
#define STANDARD_PADAL_PIXEL_HEIGHT (DISPLAY_HEIGHT / 9)
#define STANDARD_PADAL_Y (DISPLAY_HEIGHT - STANDARD_PADAL_PIXEL_HEIGHT)

#define STANDARD_BALL_VELOCITY .5
#define BALL_RADIUS 2

struct brick {
    uint8_t row;
    uint8_t col;
    uint8_t width; // dimension along the x axis (width in pixels = width * STANDARD_BRICK_PIXEL_WIDTH)
    uint8_t height; // dimension along the y axis (height in pixels = height * STANDARD_BRICK_PIXEL_HEIGHT)
    uint8_t isActive;
    uint8_t special; 
};

struct ball {
    float x;
    float y;
    float velocity;
    float angle; 
    float radius;
};

struct padal {
    uint8_t x;
    uint8_t width;
};