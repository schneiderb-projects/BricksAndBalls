#include "bricksAndBalls.h"

#define EPSILON 0.00000000000000000001

struct brick bricks[ROWS][COLS];
uint8_t gamescreen_bitmap[(DISPLAY_WIDTH * DISPLAY_HEIGHT) / 8];

int brick_count;

struct ball previous_ball;

struct ball ball;

struct padal padal = {
    0, STANDARD_PADAL_PIXEL_WIDTH
};

void print_special_brick(int row, int col, int x_pixel, int y_pixel) {
    struct brick b = bricks[row][col];

    if (x_pixel < STANDARD_BRICK_PIXEL_WIDTH) {
        printf("s");
    }
    else {
        printf(" ");
    }
}

void print_standard_brick(int row, int col, int x_pixel, int y_pixel) {
    struct brick b = bricks[row][col];
    
    if (b.isActive && x_pixel < STANDARD_BRICK_PIXEL_WIDTH && y_pixel < STANDARD_BRICK_PIXEL_HEIGHT) {
        printf("1");
    }
    else {
        printf("0");
    }
}

void print_bricks_row(int row) {
    for (int y_pixel = 0; y_pixel < ROW_PIXEL_HEIGHT; y_pixel++) {
        for (int col = 0; col < COLS; col++) {
            struct brick b = bricks[row][col];
            for (int x_pixel = 0; x_pixel < COL_PIXEL_WIDTH; x_pixel++) {
                if(b.isActive) {
                    if (b.special) {
                        print_special_brick(row, col, x_pixel, y_pixel);
                    }
                    else {
                        print_standard_brick(row, col, x_pixel, y_pixel);
                    }
                }
                
                else {
                    printf("0");
                }
            }
        }
        printf("\n");
    }
}

void bb_print_bricks() {
    for (int row = 0; row < ROWS; row++) {
        print_bricks_row(row);
    }
}

void bitmap_set_brick_bit(int row, int col, int x_pixel, int y_pixel, int value) {
    int bit_index = (row * ROW_PIXEL_HEIGHT + y_pixel) * (COLS * COL_PIXEL_WIDTH) + (col * COL_PIXEL_WIDTH + x_pixel);
    int index = bit_index / 8;
    int offset = bit_index % 8;

    if (value) {
        gamescreen_bitmap[index] |= 1 << offset;
    }
    else {
        gamescreen_bitmap[index] &= ~(1 << offset);
    }
}

void bitmap_set_bit(int x_pixel, int y_pixel, int value) {
    int bit_index = (y_pixel) * (COLS * COL_PIXEL_WIDTH) + (x_pixel);
    int index = bit_index / 8;
    int offset = bit_index % 8;

    if (value) {
        gamescreen_bitmap[index] |= 1 << offset;
    }

    else {
        gamescreen_bitmap[index] &= ~(1 << offset);
    }
}

void bitmap_add_special_brick(int row, int col, int x_pixel, int y_pixel) {
    struct brick b = bricks[row][col];

    if (b.isActive && x_pixel < STANDARD_BRICK_PIXEL_WIDTH) {
        bitmap_set_brick_bit(row, col, x_pixel, y_pixel, 1);
    }
    else {
        bitmap_set_brick_bit(row, col, x_pixel, y_pixel, 0);
    }
}

void bitmap_add_standard_brick(int row, int col, int x_pixel, int y_pixel) {
    struct brick b = bricks[row][col];
    if (b.isActive && x_pixel < STANDARD_BRICK_PIXEL_WIDTH && y_pixel < STANDARD_BRICK_PIXEL_HEIGHT) {
        bitmap_set_brick_bit(row, col, x_pixel, y_pixel, 1);
    }
    else {
        bitmap_set_brick_bit(row, col, x_pixel, y_pixel, 0);
    }
}

void bitmap_add_bricks_row(int row) {
    for (int y_pixel = 0; y_pixel < ROW_PIXEL_HEIGHT; y_pixel++) {
        for (int col = 0; col < COLS; col++) {
            struct brick b = bricks[row][col];
            for (int x_pixel = 0; x_pixel < COL_PIXEL_WIDTH; x_pixel++) {
                if (b.special) {
                    bitmap_add_special_brick(row, col, x_pixel, y_pixel);
                }
                else {
                    bitmap_add_standard_brick(row, col, x_pixel, y_pixel);
                }
            }
        }
    }
}

void bitmap_add_ball() {
  for (int y = 0; y < ball.radius; y++) {
    for (int x = 0; x < ball.radius; x++) {
      bitmap_set_bit(ball.x + x, ball.y + y, 1);
      bitmap_set_bit(ball.x + x, ball.y - y, 1);
      bitmap_set_bit(ball.x - x, ball.y + y, 1);
      bitmap_set_bit(ball.x - x, ball.y - y, 1);
    }
  }
}

void bitmap_add_bricks() {
  for (int row = 0; row < ROWS; row++) {
    bitmap_add_bricks_row(row);
  }
}

void bitmap_add_padal() {
  for (int x = 0; x < padal.width; x++) {
    for (int y = 0; y < STANDARD_PADAL_PIXEL_HEIGHT; y++) {
      bitmap_set_bit(padal.x + x, STANDARD_PADAL_Y + y, 1);
    }
  }
}

uint8_t* bb_update_bitmap() {
    bitmap_add_bricks();
    bitmap_add_ball();
    bitmap_add_padal();
    return gamescreen_bitmap;
}

long my_map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float my_float_map(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void bb_move_padal(int x) { // x E [0, 100]
  x = my_map(x, 0, 100, 0, 84 - padal.width);
  if (x > DISPLAY_WIDTH - padal.width) {
    padal.x = DISPLAY_WIDTH - padal.width;
  } else if (x < 0) {
    padal.x = 0;
  } else {
    padal.x = x;
  }
}

void  bb_print_bitmap() {
    for (int x = 0; x < (DISPLAY_HEIGHT * DISPLAY_WIDTH) / 8; x++) {
        for (int i = 0; i < 8; i++) {
            printf("%d", (gamescreen_bitmap[x] & (1 << i)) >> i);
            if(((x * 8 + i) % DISPLAY_WIDTH) == (DISPLAY_WIDTH - 1)) {
                printf("\n");
            }
        }
    }
}

int check_ball_missed() { // returns 1 if the ball has missed the padal, otherwise 0
  if ((ball.y + ball.radius > STANDARD_PADAL_Y) && ((ball.x + ball.radius < padal.x) || (ball.x - ball.radius > padal.x + padal.width))) {
    return 1;
  }

  return 0;
}

void update_ball() {
    memcpy(&previous_ball, &ball, sizeof(struct ball));
    float x_velocity = ball.velocity * cos(ball.angle);
    float y_velocity = ball.velocity * sin(ball.angle);
    float new_x = ball.x + x_velocity;
    float new_y = ball.y + y_velocity;
    ball.x = new_x;
    ball.y = new_y;
}

float sigmoid(float x, float y_min, float y_max, float strech_factor) {
        float total_range = y_max - y_min;
        float offset = y_max - total_range;
        return total_range / (1 + exp(strech_factor*x)) + offset;
}

struct brick* brick_from_x_y(float x, float y) {
  int col = x / COL_PIXEL_WIDTH;
  int row = y / ROW_PIXEL_HEIGHT;
  return &bricks[row][col];
}

float get_slope(float x1, float y1, float x2, float y2) {
  return (y2 - y1) / (x2 - x1 + EPSILON);
}

float get_intercept(float x, float y, float slope) {
  // y = mx + b
  // y - mx = b
  return y - (slope * x + EPSILON);
}
float my_max(float a, float b) {
  if (a < b) {
    return b;
  }
  return a;
}

float my_min(float a, float b) {
  if (a < b) {
    return a;
  }
  return b;
}

struct coordinate {
  float x;
  float y;
};

int is_line_segs_intersect(float x11, float y11, float x12, float y12, float x21, float y21, float x22, float y22, struct coordinate* to_return) {
  float m1 = get_slope(x11, y11, x12, y12);
  float m2 = get_slope(x21, y21, x22, y22);
  float b1 = get_intercept(x11, y11, m1);
  float b2 = get_intercept(x21, y21, m2);
  float x = (b2 - b1) / (m1 - m2);
  float y = m1 * x + b1;
  //printf("intersect: %f, %f, line1: %fx + %f, line2: %fx + %f\n", x, y, m1, b1, m2, b2);
  //printf("%f >= %f && %f <= %f && %f >= %f && %f <= %f", x, my_min(x11, x12), x, my_max(x11, x12), x, my_min(x21, x22), x, my_max(x21, x22));
  if (x >= my_min(x11, x12) - EPSILON && x <= my_max(x11, x12) + EPSILON && x >= my_min(x21, x22) - EPSILON && x <= my_max(x21, x22) + EPSILON) {
    to_return->x = x;
    to_return->y = y;
    return 1;
  }
  //printf("no intersect\n");
  return 0;
}

float angle_reflect_x(float angle) {
  return PI - angle;
}

float angle_reflect_y(float angle) {
  return -angle;
}

int manage_special_brick_collision() { // return 1 if the ball should be deflected
 printf("manage_special_brick_collision unimplemented");
 return 1;
}

int calc_quadrant_from_angle(float angle) {
  while (angle > 2 * PI) {angle -= (2 * PI);}
  while (angle <= 0) {angle += 2 * PI;}
  if (angle < PI / 2) {
    return 1;
  } else if (angle < PI) {
    return 2;
  } else if (angle < 3 * PI / 2) {
    return 3;
  } else {
    return 4;
  }
}

float distance(float x1, float y1, float x2, float y2) {
  return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

int manage_brick_collisions(struct ball* bll, struct ball* prev_bll) {
  struct brick * br_new;
  struct brick * br_prev;
  int collision = 0; 
  int quadrant = calc_quadrant_from_angle(bll->angle);
  float x = 0;
  float y = 0;
  float distance = 0;
  if (quadrant != 4) {
    br_new = brick_from_x_y(bll->x - bll->radius, bll->y - bll->radius);
    br_prev = brick_from_x_y(prev_bll->x - prev_bll->radius, prev_bll->y - prev_bll->radius);
    if (br_new != br_prev && br_new->isActive) {
      collision = 1;
    }
  } 
  if (collision == 0  && quadrant != 3) {
    br_new = brick_from_x_y(bll->x + bll->radius, bll->y - bll->radius);
    br_prev = brick_from_x_y(prev_bll->x + prev_bll->radius, prev_bll->y - prev_bll->radius);
    if (br_new != br_prev && br_new->isActive) {
      collision = 1;
     }
  } 
  if (collision == 0 && quadrant != 1) {
    br_new = brick_from_x_y(bll->x - bll->radius, bll->y + bll->radius);
    br_prev = brick_from_x_y(prev_bll->x - prev_bll->radius, prev_bll->y + prev_bll->radius);
    if (br_new != br_prev && br_new->isActive) {
      collision = 1;
    }
  } 
  if (collision == 0 && quadrant != 2) {
    br_new = brick_from_x_y(bll->x + bll->radius, bll->y + bll->radius);
    br_prev = brick_from_x_y(prev_bll->x + prev_bll->radius, prev_bll->y + prev_bll->radius);
    if (br_new != br_prev && br_new->isActive) {
      collision = 1;
    }
  }
  if (collision) {
    if (br_new->col != br_prev->col) {
      bll->angle = angle_reflect_x(bll->angle);
    }
    if (br_new->row != br_prev->row) {
      bll->angle = angle_reflect_y(bll->angle);
    } 

    br_new->isActive = 0;
    brick_count--;
  }
  return collision;
}


int detect_ball_collision() {
  int collision = 0;
  if ((ball.y + ball.radius > STANDARD_PADAL_Y) && (ball.x + ball.radius >= padal.x) && (ball.x - ball.radius <= padal.x + padal.width)) { // padal deflection
    float x = my_float_map(ball.x, padal.x - ball.radius, padal.x + padal.width + ball.radius, 1, -1); // map the hit spot on the padal from -1 to 1
    ball.angle = sigmoid(x, -PI / 16, -15 * PI / 16, -5); // map padal hit to a deflection angle with sigmoid mapping from -pi/16 to -15 * -pi / 16
    ball.y = STANDARD_PADAL_Y - ball.radius; // set the ball to bounce directly off the padal
    collision = 1;
  }

  if (ball.x - ball.radius < 0) { // if the ball hits the left wall
    ball.angle = angle_reflect_x(ball.angle);
    ball.x = ball.radius; // set the ball to bounce directly off the wall
    collision = 1;
  } else if ((ball.x + ball.radius) > DISPLAY_WIDTH - 1) { // if it hits the right wall
    ball.angle = angle_reflect_x(ball.angle);
    ball.x = DISPLAY_WIDTH - ball.radius - 1; // set the ball to bounce directly off the wall
    collision = 1;
  }

  if (ball.y + ball.radius > DISPLAY_HEIGHT - 1) { // if it hits bottom
    ball.angle = angle_reflect_y(ball.angle);
    ball.y = DISPLAY_HEIGHT - ball.radius - 1; // set the ball to bounce directly off the wall
    collision = 1;
  } else if (ball.y - ball.radius < 0) { // if it hits top 
    ball.angle = angle_reflect_y(ball.angle);
    ball.y = ball.radius + 1; // set the ball to bounce directly off the wall
    collision = 1;
  }

  collision = collision || manage_brick_collisions(&ball, &previous_ball);
  return collision;
}

int bb_next(int velocity) { //returns 1 if game is lost, 2 if game is won, otherwise 0
  int collision = 0;
  int game_lost = 0;
  int game_won = 0;
  for (int i = 0; !collision && !game_lost &&!game_won && i < velocity; i++) {
    update_ball();
    collision = detect_ball_collision();
    game_lost = check_ball_missed();
    game_won = !brick_count;
  }

  if (game_lost) {
    return -1;
  } 
  if (game_won) {
    return 0;
  }
  return brick_count;
}
int bb_init() {
  previous_ball = {
    padal.x + padal.width / 2, DISPLAY_HEIGHT - STANDARD_PADAL_PIXEL_HEIGHT, STANDARD_BALL_VELOCITY, -PI/4, 3
  };

  ball = {    
      padal.x + padal.width / 2, DISPLAY_HEIGHT - STANDARD_PADAL_PIXEL_HEIGHT, STANDARD_BALL_VELOCITY, -PI/4, 3
  };

  brick_count = 0;

  for (int i = 0; i < ROWS; i++) {
      for (int j = 0; j < COLS; j++) {
          bricks[i][j].row = i;
          bricks[i][j].col = j;
          bricks[i][j].width = 1; 
          bricks[i][j].height = 1; 
          bricks[i][j].special = 0;
          if (i < 4) {
            brick_count++;
            bricks[i][j].isActive = 1; 
          }
          else {
            bricks[i][j].isActive = 0; 
          }
      }
  }

  return 0;
}
