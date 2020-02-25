#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "PingPongScores.h"

/***
 * HARDWARE
 */ 
//-=-=-=-=-=-=-=-=-=-=-LCD DISPLAY-=-=-=-=-=-=-=-=-=-=-=-//
//typical address for i2c is expected to be 0x27, but could be something else
//set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);
//-=-=-=-=-=-=-=-=-=-=-=Interrupt=-=-=-=-=-=-=-=-=-=-=-=-//
#define interruptPin 2

//these are arbitrary
#define led_team1 5
#define led_team2 6

/***
 * SOFTWARE
 */
//-=-=-=-=-=-=-=-=-=-=-=Variables=-=-=-=-=-=-=-=-=-=-=-=-//
PingPongScores score = PingPongScores(led_team1, led_team2, delay_length, delay_blinker, delay_blinker_times);

//-=-=-=-=-=-=-=-=-=-Configuration-=-=-=-=-=-=-=-=-=-=-//
#define delay_length 10
#define delay_blinker 50 //this is such that: x * delay_length = ~1/2 a second
#define delay_blinker_times 5 //this is how many times you want it to blink, assuming: (50% on & 50% off)


void setup()
{
  //this attaches the interrupt function to the interrupt pin when the pin is rising
  attachInterrupt(interruptPin, interrupt, RISING);

  pinMode(led_team1, OUTPUT);
  pinMode(led_team2, OUTPUT);
  
  lcd.init();// initialize the lcd
  
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Hello, world!");
  // Print a message to the LCD
}

void loop()
{
  score.updateLEDs();//this needs to be called into the loop!!!
  updateScoreBoard();
  
  //delay so that the board doesn't work too hard
   delay(delay_length);
}

//this will be called at the end of loop for each update
void updateScoreBoard()
{
  writeGames(score.game_team1, score.game_team2);
  writeScores(score.score_team1, score.score_team2);
}

void writeGames(int game_t1, int game_t2)
{
  //since the score will never exceed 2 digits
  //we can simple do check if its greater than 9
  //team 1 is left leading
  lcd.setCursor(0, 0);
  String score1 = "" + game_t1;
  lcd.print(score1);

  //team 2 is right leading
  volatile bool extraDigits = game_t2 > 9;
  lcd.setCursor(15 - extraDigits, 0);
  String score2 = "" + game_t2;
  lcd.print(score2); 
}

void writeScores(int score_t1, int score_t2)
{
  //since the score will never exceed 2 digits
  //we can simple do check if its greater than 9
  //team 1 is left leading
  lcd.setCursor(0, 1);
  String score1 = "" + score_t1;
  lcd.print(score1);

  //team 2 is right leading
  volatile bool extraDigits = score_t2 > 9;
  lcd.setCursor(15 - extraDigits, 1);
  String score2 = "" + score_t2;
  lcd.print(score2); 
}

//when in interrupt dleay wil not work
//any variables should also be volatile
void interrupt()
{
  //check the 4 buttons
  //if() //++team1
  //else if() //++team2
  //else if() //--team1
  //else //--team2
}
