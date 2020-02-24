#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

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
//this will be where the scores are saved
//these need to be volatile since there will be modification inside of the intterupt
volatile int score_team1, score_team2;

//these will be used to blink the leds
volatile bool led_blink_team1, led_blink_team2;
//these will be used to blink the leds
volatile bool led_state_team1, led_state_team2;
//counts the overall state of the led blinks
volatile int led_state_counter_team1, led_state_counter_team2; 
//so that the led blinks several times during it's overall
volatile int led_state_aux_counter_team1, led_state_aux_counter_team2; 

String name_team1 = "TEAM RED";
String name_team2 = "TEAM BLUE";
//this will be used to keep track of what to write when scrolling names
volatile int pos_name_team1, pos_name_team2;
//this will be used to delay the scroll names during the scroll period
volatile int aux_scroll_team1,aux_scroll_team2;
//this will be used to hold names during start/stop of scrolling
volatile int aux_hold_team1, aux_hold_team2;


//-=-=-=-=-=-=-=-=-=-Configuration-=-=-=-=-=-=-=-=-=-=-//
#define delay_length 10
#define delay_microSeconds false
#define delay_blinker 50 //this is such that: x * delay_length = ~1/2 a second
#define delay_blinker_times 5 //this is how many times you want it to blink, assuming: (0.5s on + 0.5s off)

#define scrolling_speed 35 //this is x * delay_length = time a char is displayed
#define hold_start_stop 1000 //this is in miliseconds and says how long until scroll starts 

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
  updateLeds();
  updateScoreBoard();
  
  //delay or delayMicroseconds so that the board doesn't work too hard
  if (delay_microSeconds)
    delayMicroseconds(delay_length);
  else
    delay(delay_length);
}

//this will be called at the end of loop for each update
void updateScoreBoard()
{
  //buffers for toCharArray call
  char * buff_1 = new char[7];
  char * buff_2 = new char[7];
  
  if(name_team1.length() > 7)
  {
    if(pos_name_team1 == 0)//string 1 started
    {
      if(aux_hold_team1 * delay_length > hold_start_stop)
      {
        ++pos_name_team1;
      }
      ++aux_hold_team1;
    }
    else if(pos_name_team1 >= name_team1.length() - 7)//string 1 ended
    {
      if(aux_hold_team1 * delay_length > hold_start_stop)
      {
        pos_name_team1 = 0;
      }
      ++aux_hold_team1;
    }
    else
    {
      pos_name_team1 %= 7;
      if(aux_scroll_team1 >= scrolling_speed)
      {
        aux_scroll_team1 = 0;
        ++pos_name_team1;
      }
      ++aux_scroll_team1;

      //this should technically never exceed the lenght of the string
      name_team1.substring(pos_name_team1, pos_name_team1 + 7).toCharArray(buff_1, 7);
    }
  }
  else
  {
    name_team1.toCharArray(buff_1, 7);
  }


  if(name_team2.length() > 7)
  {
    if(pos_name_team2 == 0)//string 1 started
    {
      if(aux_hold_team2 * delay_length > hold_start_stop)
      {
        ++pos_name_team2;
      }
      ++aux_hold_team2;
    }
    else if(pos_name_team2 >= name_team2.length() - 7)//string 1 ended
    {
      if(aux_hold_team2 * delay_length > hold_start_stop)
      {
        pos_name_team2 = 0;
      }
      ++aux_hold_team2;
    }
    else
    {
      pos_name_team2 %= 7;
      if(aux_scroll_team2 >= scrolling_speed)
      {
        aux_scroll_team2 = 0;
        ++pos_name_team2;
      }
      ++aux_scroll_team2;

      //this should technically never exceed the lenght of the string
      name_team2.substring(pos_name_team2, pos_name_team2 + 7).toCharArray(buff_2, 7);
    }
  }
  else
  {
    name_team2.toCharArray(buff_2, 7);
  }
  
  writeTeamNames(buff_1, buff_2);
  writeScores(score_team1, score_team2);
}

//should only be passed two 7-char srings
void writeTeamNames(char * name_t1, char * name_t2)
{
  lcd.setCursor(0,0);
  lcd.print(name_t1);
  lcd.setCursor(9, 0);//9 cause the first 7 are written and the middle 2 are skipped
  lcd.print(name_t2);
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

void updateLeds()
{
   if(led_blink_team1)//blinking
  {
    if(led_state_counter_team1 < delay_blinker * delay_blinker_times)//not done (done when overall counter exceeds)
    {
      if(led_state_aux_counter_team1 < delay_blinker)//0-50 for example
      {
        digitalWrite(led_team1, HIGH);
      }
      else if(led_state_aux_counter_team1 < delay_blinker * 2)//50-100
      {
        digitalWrite(led_team1, LOW);
      }
      ++led_state_aux_counter_team1;
      led_state_aux_counter_team1 %= delay_blinker * 2;//reset upon 100
    }
    else
    {
      led_blink_team1 = false;
      led_state_counter_team1 = -1;
      led_state_aux_counter_team1 = 0;
    }
    ++led_state_counter_team1;
  }
  
  if(led_blink_team2)//same thing but with other led
  {
    if(led_state_counter_team2 < delay_blinker * delay_blinker_times)
    {
      if(led_state_aux_counter_team2 < delay_blinker)
      {
        digitalWrite(led_team2, HIGH);
      }
      else if(led_state_aux_counter_team2 < delay_blinker * 2)
      {
        digitalWrite(led_team1, LOW);
      }
      ++led_state_aux_counter_team2;
      led_state_aux_counter_team2 %= delay_blinker * 2;
    }
    else
    {
      led_blink_team2 = false;
      led_state_counter_team2 = -1;
      led_state_aux_counter_team2 = 0;
    }
    ++led_state_counter_team2;
  }
}

void increaseScore(bool team1)
{
  if(team1)
  {
    led_blink_team1 = true;
    ++score_team1;
  }
  else
  {
    led_blink_team2 = true;
    ++score_team2;
  }
}

void decreaseScore(bool team1)
{
  if(team1)
  {
    --score_team1;
  }
  else
  {
    --score_team2;
  }
}
