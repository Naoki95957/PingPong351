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
#define interruptPin 0//is 2 and 1 is to 3

//-=-=-=-=-=-=-=-=-=-=-=-Buttons-=-=-=-=-=-=-=-=-=-=-=-=-//
#define increase_team1 3
#define decrease_team1 4
#define increase_team2 5
#define decrease_team2 6

//-=-=-=-=-=-=-=-=-=-=-=-=-LEDS=-=-=-=-=-=-=-=-=-=-=-=-=-//
//these are arbitrary
#define led_team1 7
#define led_team2 8

/***
 * SOFTWARE
 */
//-=-=-=-=-=-=-=-=-=-Configuration-=-=-=-=-=-=-=-=-=-=-//
#define delay_length 10
#define delay_blinker 50 //this is such that: x * delay_length = ~1/2 a second
#define delay_blinker_times 5 //this is how many times you want it to blink, assuming: (50% on & 50% off)
#define rejection_time 250 //time in ms before next press is accepted
//-=-=-=-=-=-=-=-=-=-=-=Variables=-=-=-=-=-=-=-=-=-=-=-=-//
PingPongScores score = PingPongScores(led_team1, led_team2, delay_length, delay_blinker, delay_blinker_times);
int rejection_counter = 0;
bool acceptInput = true;


void setup()
{
  Serial.begin(9600);
  //this attaches the interrupt function to the interrupt pin when the pin is rising
  //pinMode(2, INPUT_PULLUP);//puts a resistor on the input to filter some noise
  attachInterrupt(interruptPin, interrupt, RISING);

  pinMode(increase_team1, INPUT);
  pinMode(increase_team2, INPUT);
  pinMode(decrease_team1, INPUT);
  pinMode(decrease_team2, INPUT);
  
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
  updateScoreBoard();//TODO fix this shit
  
  //delay so that the board doesn't work too hard
   delay(delay_length);
   
   if(!acceptInput)//helps with hysteresis on buttons
   {
     rejection_counter++;
     if(rejection_counter * delay_length >= rejection_time)
     {
        rejection_counter = 0;
        acceptInput = true;
     }
   }
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

//the reason it is structured with trigger
//is so that if interrupt is accidentally called
//(ie with static charge), than it will dismiss it
void interrupt()
{
  //check the 4 buttons
  bool inc_team1 = digitalRead(increase_team1);
  bool inc_team2 = digitalRead(increase_team2);
  bool dec_team1 = digitalRead(decrease_team1);
  bool dec_team2 = digitalRead(decrease_team2);
  
  bool triggered = inc_team1 || inc_team2 || dec_team1 || dec_team2;

  //compare
  if(inc_team1)
  {
    score.increaseScore(true);//team 1
  }
  else if(inc_team2)
  {
    score.increaseScore(false);//team 2
  }
  //decreaseScores
  if(dec_team1)
  {
    score.decreaseScore(true);//team 1
  }
  else if(dec_team2)
  {
    score.decreaseScore(false);//team 2
  }
  if(triggered)
  {
  //  Serial.println("INTERRUPTED");
    Serial.println(inc_team1);
    Serial.println(inc_team2);
    Serial.println(dec_team1);
    Serial.println(dec_team2);
    Serial.println("-------------");
    acceptInput = false;
    rejection_counter = 0;
    //todo update lcd (CANNOT USE DELAY)
  }
}
