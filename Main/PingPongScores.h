#ifndef PINGPONG_LEDS_H
#define PINGPONG_LEDS_H

class PingPongScores
{

  private:
    //physical pin locations
    int led_team1, led_team2;
    //schedule management
    volatile int delay_length, delay_blinker, blinker_times;
    //this determines when an led should be blinking
    volatile bool led_blink_team1, led_blink_team2;
    //this determines the state while it is blinking
    volatile bool led_state_team1, led_state_team2;
    //this counts how many times it has blinked
    volatile int led_state_counter_team1, led_state_counter_team2;
    //this counts how many cycles has passed per state in a blink-state (so counts cycles of when an led was on/off)
    volatile int led_state_aux_counter_team1, led_state_aux_counter_team2;
  public:
    //literal score of match
		volatile int score_team1, score_team2;
    //literal score of game
		volatile int game_team1, game_team2;
    //switches phyical sides for the players
    volatile bool sides_switched = false;
    //switches who is serving
    volatile bool team1_serving = false;
    //bool flag to determine when the game is over
    volatile bool game_over = false;
    //this will be used to inform the main script to reset itself
    volatile bool reset_game = false;

    //constructor basically copies all vars and inits a few variables
		PingPongScores(int led_team1, int led_team2, int delay_length,
			int delay_blinker, int blinker_times) :
			led_team1(led_team1), led_team2(led_team2),
			delay_length(delay_length), delay_blinker(delay_blinker),
			blinker_times(blinker_times)
		{
			led_blink_team1 = false;
			led_blink_team2 = false;
			led_state_aux_counter_team1 = 0;
			led_state_aux_counter_team2 = 0;
			led_state_counter_team1 = 0;
			led_state_counter_team2 = 0;
		};
    //decrease the score
    //@params team1 - if true, decrement team1, else team 2
		void decreaseScore(bool team1);
    //increase the score
    //@params team1 - if true, increment team1, else team 2
		void increaseScore(bool team1);
    //meant to be constantly called, this is used in part with
    //the loop where counting the cycles with delays schedules
    //how long an led should be on or off
		void updateLEDs();
};
#endif
