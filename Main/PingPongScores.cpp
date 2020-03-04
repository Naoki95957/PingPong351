#include <Arduino.h>
#include "PingPongScores.h"

void PingPongScores::increaseScore(bool team1)
{
  if(score_team1 > 9 && score_team2 > 9)
  { 
    team1_serving = !team1_serving;
  }
  else if((score_team1 + score_team2) > 0 && (score_team1 + score_team2) % 2 == 1)
  {
    team1_serving = !team1_serving;
  }
	if (team1)
	{
		++score_team1;
	}
	else
	{
		++score_team2;
	}

	if (score_team1 > 10 && score_team1 - score_team2 > 1)//team1 wins a game
	{
    sides_switched = !sides_switched;
    if(sides_switched)
    {
      led_blink_team2 = true;
    }
    else
    {
      led_blink_team1 = true;
    }
		++game_team1;
		score_team1 = 0;
		score_team2 = 0;
	}

	if (score_team2 > 10 && score_team2 - score_team1 > 1)//team2 wins a game
	{
    sides_switched = !sides_switched;
    if(sides_switched)
    {
      led_blink_team1 = true;
    }
    else
    {
      led_blink_team2 = true;
    }
		++game_team2;
		score_team1 = 0;
		score_team2 = 0;
	}

 //for debugging purposes
//  Serial.println("-/-/-/-/-/-/-/-/-");
//  Serial.println("Team 1 score: " + String(score_team1));
//  Serial.println("Team 1 score: " + String(score_team2));
//  Serial.println("Switch serves: " + String(team1_serving));
//  Serial.println("Switched sides: " + String(sides_switched));
};
void PingPongScores::decreaseScore(bool team1)
{
	if (team1)
	{
		--score_team1;
	}
	else
	{
		--score_team2;
	}

  if (score_team1 < 0)//if score was already 0
  {
    if (game_team1 > 0)//sub from games then
    {
      --game_team1;
    }
    score_team1 = 0;
  }
  
	if (score_team2 < 0)
	{
		if (game_team2 > 0)
		{
			--game_team2;
		}
		score_team2 = 0;
	}
};
void PingPongScores::updateLEDs()
{
  if(led_blink_team1 || led_blink_team2)
  {
  	if (led_blink_team1)//blinking
  	{
  		if (led_state_counter_team1 < delay_blinker * blinker_times)//not done (done when overall counter exceeds)
  		{
  			if (led_state_aux_counter_team1 < delay_blinker / 2)//0-50 for example
  			{
          //leds are in reverse since the switch is determined prior or during the blinking
  				digitalWrite(led_team2, HIGH);
  			}
  			else if (led_state_aux_counter_team1 < delay_blinker)//50-100
  			{
  				digitalWrite(led_team2, LOW);
  			}
  			++led_state_aux_counter_team1;
  			led_state_aux_counter_team1 %= delay_blinker;//reset upon 100
  		}
  		else
  		{
  			led_blink_team1 = false;
  			led_state_counter_team1 = -1;
  			led_state_aux_counter_team1 = 0;
  		}
  		++led_state_counter_team1;
  	}
    else
    {
      digitalWrite(led_team2, LOW);
    }
  	if (led_blink_team2)//same thing but with other led
  	{
  		if (led_state_counter_team2 < delay_blinker * blinker_times)
  		{
  			if (led_state_aux_counter_team2 < delay_blinker / 2)
  			{
  				digitalWrite(led_team1, HIGH);
  			}
  			else if (led_state_aux_counter_team2 < delay_blinker)
  			{
  				digitalWrite(led_team1, LOW);
  			}
  			++led_state_aux_counter_team2;
  			led_state_aux_counter_team2 %= delay_blinker;
  		}
  		else
  		{
  			led_blink_team2 = false;
  			led_state_counter_team2 = -1;
  			led_state_aux_counter_team2 = 0;
  		}
  		++led_state_counter_team2;
  	}
    else
    {
      digitalWrite(led_team1, LOW);
    }
  }
  else
  {
    if(sides_switched)
    {
      digitalWrite(led_team1, team1_serving);
      digitalWrite(led_team2, !team1_serving);
    }
    else
    {
      digitalWrite(led_team1, !team1_serving);
      digitalWrite(led_team2, team1_serving);
    }
  }
};
