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
#define delay_blinker_times 10 //this is how many times you want it to blink, assuming: (50% on & 50% off)
#define rejection_time 200 //time in ms before next press is accepted

//-=-=-=-=-=-=-=-=-=-=-=Variables=-=-=-=-=-=-=-=-=-=-=-=-//
PingPongScores score = PingPongScores(led_team1, led_team2, delay_length, delay_blinker, delay_blinker_times); //where leds and score-keeping happen
volatile int rejection_counter = 0;//this is used as an auxillary variable to count delays for rejection
volatile bool acceptInput = true;//this is used as a signal for when input is ready to be accepted or not
volatile bool updateLCD = false;//this is an auxillary variable used to signal when updating the lcd is ready or not
volatile bool serve_selected = false;//this is used to determine which side starts serving when prompted
volatile bool team1_serving = false;//this is auxillary and used once to pass off input to the PingPongScores object
volatile int aux_scrolling = 0;//this is auxillary to count off how many characters have scrolled
const String start_message = "Pick a side to serve and start!";//31 char long const string for prompt message

void setup()
{
//  Serial.begin(9600);
  //this attaches the interrupt function to the interrupt pin when the pin is rising
  attachInterrupt(interruptPin, interrupt, RISING);
  //sets up 4 lines for digital inputs (buttons)
  pinMode(increase_team1, INPUT);
  pinMode(increase_team2, INPUT);
  pinMode(decrease_team1, INPUT);
  pinMode(decrease_team2, INPUT);
  //sets up 2 lines for digital outputs (leds)
  pinMode(led_team1, OUTPUT);
  pinMode(led_team2, OUTPUT);
  
  lcd.init();// initialize the lcd
  
  lcd.backlight();//turns on the lcd
  lcd.setCursor(0,0);//this is the same as lcd.home(); which to use doesn't really matter much
  
  lcd.print(start_message);// Print a message to the LCD
}

void loop()
{ 
  //routine to guide players on how to start the device
  if(!serve_selected)
  {
    if(aux_scrolling > 14)//14 happens to be 31 - 16 - 1... 31 char string, -16 (length of display), -1 because index 0
    {
      aux_scrolling = 0;
      delay(1500);//hold for 1.5 sec at the end of the text (1.5 so we have matching cycles)
      lcd.clear();//clean it
      lcd.home();
      lcd.print(start_message);
      delay(1000);//hold for 1 sec at start of text
    }
    else
    {
      delay(500);//hold for 0.5 per character (including 0 index)
      lcd.scrollDisplayLeft();//shifts all chars 1 over
      ++aux_scrolling;//count our shifts
    }
  }
  else //actuall loop stuff for most of the use case
  {
    score.updateLEDs();//this needs to be called upon each iteration of the loop
    
    if(updateLCD)//checks if the lcd needs an update and does so just once
    {
      updateScoreBoard();
      updateLCD = false;
    }
    
    //delay so that the board doesn't work too hard
    delay(delay_length);
    if(!acceptInput)//helps with hysteresis on buttons
    {
      ++rejection_counter;
      if(rejection_counter * delay_length >= rejection_time)//once the time elapsed, we will enable more inputs
      {
        rejection_counter = 0;
        acceptInput = true;
      }
    }

    //restart the device
    if(score.reset_game)
    {
      score.reset_game = false;
      serve_selected = false;

      //set leds low
      digitalWrite(led_team1, LOW);
      digitalWrite(led_team2, LOW);
      
      lcd.clear();
      lcd.home();
      lcd.print(start_message);// Print a message to the LCD
    }
  }
}

void updateScoreBoard()
{
  lcd.clear();
  if(score.game_over)
  {
    lcd.home();
    lcd.print("     WINNER     ");
    lcd.setCursor(0, 1);
    if(score.game_team1 > 2)
      lcd.print("       T1       ");
    else
      lcd.print("       T2       ");
  }
  else
  {
    writeGames(score.game_team1, score.game_team2);
    writeScores(score.score_team1, score.score_team2);
  }
}

//this simply writes the game score to the lcd
void writeGames(int game_t1, int game_t2)
{

  if(score.sides_switched)//if its flipped, flip the teams
  {
    //since the score will never exceed 2 digits
    //we can simple do check if its greater than 9
    //team 1 is left leading
    lcd.home();
    lcd.setCursor(0, 1);
    lcd.print(String(game_t2));
  
    //team 2 is right leading
    volatile bool extraDigits = game_t1 > 9;
    lcd.setCursor(15 - extraDigits, 1);
    lcd.print(String(game_t1)); 
  }
  else
  {
    lcd.home();
    lcd.setCursor(0, 1);
    lcd.print(String(game_t1));
  
    //team 2 is right leading
    volatile bool extraDigits = game_t2 > 9;
    lcd.setCursor(15 - extraDigits, 1);
    lcd.print(String(game_t2)); 
  }
}

//mostly the same code, just on row 0, instead of row 1
void writeScores(int score_t1, int score_t2)
{
  if(score.sides_switched)
  {
    //since the score will never exceed 2 digits
    //we can simple do check if its greater than 9
    //team 1 is left leading
    lcd.home();
    lcd.setCursor(0, 0);
    lcd.print("T2:" + String(score_t2));
  
    //team 2 is right leading
    volatile bool extraDigits = score_t1 > 9;
    lcd.setCursor(15 - (extraDigits + 3), 0);
    lcd.print(String(score_t1) + ":T1"); 
  }
  else
  {
    //since the score will never exceed 2 digits
    //we can simple do check if its greater than 9
    //team 1 is left leading
    lcd.home();
    lcd.setCursor(0, 0);
    lcd.print("T1:" + String(score_t1));
  
    //team 2 is right leading
    volatile bool extraDigits = score_t2 > 9;
    lcd.setCursor(15 - (extraDigits + 3), 0);
    lcd.print(String(score_t2) + ":T2"); 
  }
}

//some notes about interrupts:
//when in an interrupt rutine, delay will not work
//any variables should also be volatile

//the reason it is structured this way with a trigger variable
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

  //if we are accepting inputs:
  if(acceptInput)
  {
    //comparisons
    //increase scores
    if(inc_team1)
    {
      //special case is at the begining, where we prompt which side to start with
      if(!serve_selected)
      {
        team1_serving = false;
      }
      else // we will use buttons for their typical use
      {
        score.increaseScore(!score.sides_switched);//if sides_switched is false, then we want button1 to be true and opposite if not
                                                   //since sides switching will lead to buttons needing different purposes too
      }
    }
    else if(inc_team2)
    {
      if(!serve_selected)
      {
        team1_serving = true;
      }
      else
      {
        score.increaseScore(score.sides_switched);
      }
    }
    //decreaseScores
    else if(dec_team1)
    {
      if(!serve_selected)
      {
        team1_serving = false;
      }
      else
      {
        score.decreaseScore(!score.sides_switched);//team 1
      }
    }
    if(dec_team2)
    {
      if(!serve_selected)
      {
        team1_serving = true;
      }
      else
      {
        score.decreaseScore(score.sides_switched);//team 2
      }
    }
    //this will be called so long as any button properly registered
    if(triggered)
    {
      if(!serve_selected)//we will determine if this was for serve selections or if this was just for normal use
      {
        score.team1_serving = team1_serving;
        serve_selected = true;
      }
      acceptInput = false;
      rejection_counter = 0;
      updateLCD = true;
      //todo update lcd (CANNOT USE DELAY)
    }
  }
}
