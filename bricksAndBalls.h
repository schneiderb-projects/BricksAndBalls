#ifndef BRICKSANDBALLS_H
#define BRICKSANDBALLS_H

#define useArduino
#ifdef useArduino
#include <Arduino.h>
#include <LibPrintf.h>
#else
#include <math.h>
#include <stdio.h>
#define PI M_PI
#endif

#include "components.h"

int bb_init();
void bb_print_bitmap();
uint8_t* bb_update_bitmap();
void bb_move_padal(int x);
int bb_next(int velocity); //returns 1 if game is lost, 2 if game is one, otherwise 0
#endif