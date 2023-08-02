# BricksAndBalls
Emulating the old Blackberry Brick-Breaker game using an Arduino Uno.

## Necessary Supplies: 
- Arduino Uno
- Nokia 5110 LCD display
- 16x2 LCD Character display
- 1 Push Button
- 2 Potentiometers

## Wiring

### Nokia 5110 LCD Display
| 5110 | Arduino|
|------|--------|
|SCK | pin 8|
|MOSI | pin 9|
|DC | pin 10 |
|RST | pin 11|
|CS | pin 12|

### 16x2 LCD Character Dipslay
rs = 2, en = 3, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
| 16x2 | Arduino|
|------|--------|
|RS | pin 2|
|EN | pin 3|
|D4 | pin 5|
|D5 | pin 6|
|D7 | pin 7|

### Start Button (push button)
| Push Button | Arduino|
|------|--------|
|Output | pin A2|


### Difficulty Selector (1st potentiometer)
| 1st Pot | Arduino|
|------|--------|
|Output | pin A1|

### Padal Controller (2nd potentiometer)
| 2nd Pot | Arduino|
|------|--------|
|Output | pin A0|

## Game Play
Select your desired difficulty using the Difficulty Selector Potentiometer, press the Start Button to start the game, then use the Padal Controller Potentiometer to move the padal to bounce the ball. Miss the ball with the padal you lose! Destroy all the bricks without missing the ball and you win! 
