#ifndef PINGPONG_LEDS_H
#define PINGPONG_LEDS_H

class PingPongScores
{

  private:
    //physical pin locations
    int led_team1, led_team2;
    //schedule management
    volatile int delay_length, delay_blinker, blinker_times;
    volatile bool led_blink_team1, led_blink_team2;
    volatile bool led_state_team1, led_state_team2;
    volatile int led_state_counter_team1, led_state_counter_team2;
    volatile int led_state_aux_counter_team1, led_state_aux_counter_team2;
  public:
    //literal score of match
		volatile int score_team1, score_team2;
    //literal score of game
		volatile int game_team1, game_team2;
    volatile bool sides_switched = false;
    volatile bool team1_serving = false;

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
		void decreaseScore(bool team1);
		void increaseScore(bool team1);
		void updateLEDs();
};
#endif
