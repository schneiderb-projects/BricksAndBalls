// NOKIA 5110
//      SCK  - Pin 8
//      MOSI - Pin 9
//      DC   - Pin 10
//      RST  - Pin 11
//      CS   - Pin 12
//

#include <LiquidCrystal.h>
#include <LCD5110_Graph.h>
#include "bricksAndBalls.h"

#define HEIGHT 48
#define WIDTH 84

#define STATE_GAME_START (-2)
#define STATE_NO_GAME (-1)
#define STATE_GAME 0
#define STATE_GAME_LOST 1
#define STATE_GAME_WON 2

#define PUSH_BUTTON_PIN A2
#define VELOCITY_POT_PIN A1
#define PADAL_POT_PIN A0

#define STATE_LCD_INIT    0
#define STATE_LCD_DEFAULT 1
#define STATE_LCD_GAME    4
#define STATE_LCD_WINNER  2
#define STATE_LCD_LOSER   3

int slider_pot_upper_bound = 519;
int slider_pot_lower_bound = 10;

long start_time = 0; 
long flash_start_time = 0; 
int game_state = STATE_NO_GAME;
int lcd_state = STATE_LCD_INIT;

int last_score = 10;
int score = 10;

LCD5110 myGLCD(8,9,10,11,12);

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 2, en = 3   , d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void draw_bitmap() {
  uint8_t* bm = bb_update_bitmap();

  int y = 0;
  for (int x = 0; x < (HEIGHT * WIDTH) / 8; x++) {
    for (int i = 0; i < 8; i++) {
      // Serial.print(((x * 8) % 84) + i); Serial.print(", "); Serial.print(y); Serial.print(": "); Serial.print((*(bm + x) & (1 << i)) >> i);
      if((*(bm + x) & (1 << i)) >> i) {
        // Serial.println(" set");
        myGLCD.setPixel((x * 8 + i) % WIDTH, (HEIGHT - y));
      }
      else {
        // Serial.println(" clr");
        myGLCD.clrPixel((x * 8 + i) % WIDTH, (HEIGHT - y));
      }
      if(((x * 8 + i) % WIDTH) == (WIDTH - 1)) {
        y++;
      }
    }
  }
}

int log_base(int val, float base) {
  return log(val) / log(base);
}

int val = 0; 
int read_padal_input() {
  val = constrain(analogRead(PADAL_POT_PIN), slider_pot_lower_bound, slider_pot_upper_bound);
  val = log_base(val, 1.01);
  val = map(val, log_base(slider_pot_lower_bound, 1.01), log_base(slider_pot_upper_bound, 1.01), 100, 0);
  return val;
}

void draw_screen() {
  myGLCD.clrScr();
  draw_bitmap();
  myGLCD.update();
}

void lcd_handle_no_game() {
  if (lcd_state != STATE_LCD_DEFAULT) {
    lcd.clear();
    lcd.noAutoscroll();
    lcd_state = STATE_LCD_DEFAULT;
    lcd.setCursor(0, 0);
    lcd.print("Bricks & Balls");
  }

  lcd.setCursor(0, 1);
  char str[18];
  char* difficulty;
  int v = get_velocity();
  if (v < 6) {
    difficulty = "EASY     ";
  } else if (v < 12) {
    difficulty = "MEDIUM   ";
  } else if (v < 13) {
    difficulty = "HARD     ";
  } else {
    difficulty = "VERY HARD";
  }
  sprintf(str, "Level: %s", difficulty);
  lcd.print(str);
}

void lcd_handle_winner() {
  if (lcd_state != STATE_LCD_WINNER) {
    lcd_state = STATE_LCD_WINNER;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WINNER! WINNER!");
    lcd.setCursor(0, 1);
    lcd.print(" WINNER! WINNER!");
  }
}

void lcd_handle_loser() {
  if (lcd_state != STATE_LCD_LOSER) {
    lcd_state = STATE_LCD_LOSER;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("LOSER! LOSER!");
    lcd.setCursor(0, 1);
    lcd.print("YOU SUCK, PUSSY!");
  }
}

void lcd_handle_game() {
  if (lcd_state != STATE_LCD_GAME) {
    lcd_state = STATE_LCD_GAME; 
    lcd.clear();
    lcd.noAutoscroll();
    lcd.setCursor(0, 0);
    lcd.print("Bricks & Balls");
    lcd.setCursor(0, 1);
    lcd.print("Score: ");
    lcd.setCursor(7, 1);
    char str[16];
    lcd.print(score);
  }
  if (last_score != score) {
    lcd.setCursor(7, 1);
    lcd.print(score);
    lcd.print("  ");
  }
}

int velocity = 6;
int velocity_updated = 0;
int get_velocity() {
  if (game_state == STATE_NO_GAME || game_state == STATE_GAME_START) {
    velocity = map(analogRead(VELOCITY_POT_PIN), 23, 1015, 16, 2);
    velocity_updated = 1;
    return velocity;
  } else {
    velocity_updated = 0;
  }

  return velocity;
}

int mode = 0;
void handle_game_lost() {
  lcd_handle_loser();
  if (start_time == 0) {
      start_time = millis();
  } 
  if (millis() - start_time < 3000) {
    if (millis() - flash_start_time > 500) {
      mode = !mode; 
      myGLCD.invert(mode);
      flash_start_time = millis();
    }
  } else {
    myGLCD.invert(1);
    start_time = 0;
    game_state = STATE_NO_GAME;
  }
}

void handle_no_game() {    
  lcd_handle_no_game();
  if (digitalRead(PUSH_BUTTON_PIN)) {
    game_state = STATE_GAME_START;
  }
  get_velocity();
}

void handle_game() {
  last_score = score;
  bb_move_padal(read_padal_input());
  int state = bb_next(get_velocity());
  if (state == 0) {
    game_state = STATE_GAME_WON;
  } else if (state == -1) {
    game_state = STATE_GAME_LOST;
  } else {
    score = state;
  }
  draw_screen();
  lcd_handle_game();
}

void handle_game_start() {
  get_velocity();
  myGLCD.invert(0);
  bb_move_padal(read_padal_input());
  score = bb_init();
  game_state = STATE_GAME;
}

void handle_game_won() {
  lcd_handle_winner();
  if (start_time == 0) {
    start_time = millis();
  } 
  if (millis() - start_time < 5000) {
    if (millis() - flash_start_time > 500) {
      mode = !mode; 
      myGLCD.invert(mode);
      flash_start_time = millis();
    }
  } else {
    start_time = 0;
    game_state = STATE_NO_GAME;
  }
}

void loop() {
  switch (game_state) {
    case STATE_NO_GAME: handle_no_game(); break; 
    case STATE_GAME_START: handle_game_start(); break; 
    case STATE_GAME: handle_game(); break; 
    case STATE_GAME_LOST: handle_game_lost(); break; 
    case STATE_GAME_WON: handle_game_won(); break; 
  }
}

void setup()
{
  Serial.begin(9600);
  myGLCD.InitLCD();
  pinMode(PUSH_BUTTON_PIN, INPUT);
  bb_move_padal(read_padal_input());
  bb_init();
  myGLCD.invert(1);
  draw_screen();
  lcd.begin(16, 2);
}

