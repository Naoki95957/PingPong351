#include <Arduino.h>
#include "PingPongScores.h"

void PingPongScores::increaseScore(bool team1)
{
	if (team1)
	{
		led_blink_team1 = true;
		++score_team1;
	}
	else
	{
		led_blink_team2 = true;
		++score_team2;
	}

	if (score_team1 > 11 && score_team1 - score_team2 > 1)//team1 wins a game
	{
		++game_team1;
		score_team1 = 0;
		score_team2 = 0;
	}

	if (score_team2 > 11 && score_team2 - score_team1 > 1)//team2 wins a game
	{
		++game_team2;
		score_team1 = 0;
		score_team2 = 0;
	}
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
		else //else leave as 0
		{
			score_team1 = 0;
		}
	}

	if (score_team2 < 0)
	{
		if (game_team2 > 0)
		{
			--game_team2;
		}
		else
		{
			score_team2 = 0;
		}
	}
};
void PingPongScores::updateLEDs()
{
	if (led_blink_team1)//blinking
	{
		if (led_state_counter_team1 < delay_blinker * blinker_times)//not done (done when overall counter exceeds)
		{
			if (led_state_aux_counter_team1 < delay_blinker)//0-50 for example
			{
				digitalWrite(led_team1, HIGH);
			}
			else if (led_state_aux_counter_team1 < delay_blinker * 2)//50-100
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

	if (led_blink_team2)//same thing but with other led
	{
		if (led_state_counter_team2 < delay_blinker * blinker_times)
		{
			if (led_state_aux_counter_team2 < delay_blinker)
			{
				digitalWrite(led_team2, HIGH);
			}
			else if (led_state_aux_counter_team2 < delay_blinker * 2)
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
};
