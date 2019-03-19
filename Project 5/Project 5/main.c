
#include <avr/io.h>
#include <stdio.h>
#include <math.h>
#include "avr.h"
#include "lcd.h"

struct datetime {
	int hour;
	int minute;
	int second;
	int subsecond;	
} CLOCK_TIME, TIMER_TIME, STOPWATCH_TIME;

char keypad[17] = {
	'1', '2', '3', 'A',
	'4', '5', '6', 'B',
	'7', '8', '9', 'C',
	'*', '0', '#', 'D'
};

#define TOTAL_NOTES 12
int FREQUENCY[TOTAL_NOTES] = {220, 233, 246, 261, 277, 293, 311, 329, 349, 369, 391, 415};
int DURATION_MODIFIER[3] = {1,2,4};
int FREQUENCY_POSITION = 7;
int DURATION_MODIFIER_POSITION = 0;

int EDIT_TIME_CURRENT_TIME = 0;
int EDIT_TIME_POSITION = 0;

enum clock_state {ALARM, STOP_WATCH, TIMER} CLOCK_STATE;
enum edit_clock_state {EDIT_CLOCK_OFF, EDIT_CLOCK_ON} EDIT_CLOCK_STATE;
	
enum alarm_state {OFF, ON} ALARM_SOUND_STATE;
enum stopwatch_state {STOPWATCH_STOPPED, STOPWATCH_RUNNING} STOPWATCH_STATE;
enum timer_state {TIMER_STOPPED, TIMER_RUNNING} TIMER_STATE;
	
enum edit_alarm_state {EDIT_ALARM_OFF, EDIT_ALARM_ON} EDIT_ALARM_STATE;
enum edit_frequency_state {EDIT_FREQUENCY_OFF, EDIT_FREQUENCY_ON} EDIT_FREQUENCY_STATE;
enum edit_time_state {EDIT_TIME_OFF, EDIT_TIME_ON} EDIT_TIME_STATE;

#define MAX_RECORDED_TIMES 5
int MAX_ALARM_NOTES = 50;
int ALARM_NOTES_PLAYED = 0;
struct datetime ALARMS[MAX_RECORDED_TIMES];
int ALARMS_POSITION = 0;

struct datetime STOPWATCH_RECORDED_TIMES[MAX_RECORDED_TIMES];
int STOPWATCH_RECORDED_TIMES_POSITION = 0;

// Keyboard Functions

int is_pressed(int row, int col) {
	DDRC = 0;
	PORTC = 0;
	
	SET_BIT(DDRC, row);
	SET_BIT(PORTC, col + 4);
	avr_wait(1);
	
	if (GET_BIT(PINC,col + 4)) {
		return 0;
	}
	
	return 1;
}

int get_key() {
	int row, col;
	for (row = 0; row < 4; ++row) {
		for (col = 0; col < 4; ++col) {
			if (is_pressed(row,col)) {
				return (row * 4) + col + 1;
			}
		}
	}
	return 0;
}



// Play Note Functions

void play_note(int frequency,int duration_modifier) {
	int subsecond = 2000;
	int total_time = ceil((20000.0 / frequency));
	int TH = total_time/2;
	
	if(TH == total_time) {
		TH--;
	}
	int TL= total_time-TH;
	
	int duration_runs = (subsecond / total_time) / duration_modifier;
	for (int i = 0; i < duration_runs; ++i) {
		SET_BIT(PORTB,3);
		avr_wait2(TH);
		CLR_BIT(PORTB,3);
		avr_wait2(TL);
	}
	avr_wait2(subsecond - (total_time * duration_runs));
}

void play_alarm_note() {
	play_note(FREQUENCY[FREQUENCY_POSITION], DURATION_MODIFIER[DURATION_MODIFIER_POSITION]);
}

void increment_current_frequency() {
	if (FREQUENCY_POSITION < TOTAL_NOTES - 1) {
		FREQUENCY_POSITION ++;
	}
}

void decrement_current_frequency() {
	if (FREQUENCY_POSITION > 0) {
		FREQUENCY_POSITION --;
	}
}


// Shared
void reset_datetime(struct datetime *time) {
	time->hour = 0;
	time->minute = 0;
	time->second = 0;
	time->subsecond = 0;
}

void increment_time(struct datetime *time) {
	time->subsecond ++;
	if (time->subsecond >= 10) {
		time->subsecond = 0;
		time->second ++;
	}
	if (time->second >= 60) {
		time->second = 0;
		time->minute ++;
	}
	if (time->minute >= 60) {
		time->minute = 0;
		time->hour ++;
	}
	if (time->hour >= 24) {
		time->hour = 0;
	}
}

void display_lcd() {
	lcd_clr();
	char buf1[16]; char buf2[16];
	
	char edit_clock_state = " ";
	if (EDIT_CLOCK_STATE == EDIT_CLOCK_ON) {
		edit_clock_state = "C";
	}
	
	char time_position[3];
	if (EDIT_TIME_STATE == EDIT_TIME_ON) {
		sprintf(time_position, "T%d", EDIT_TIME_POSITION + 1);
	}
	else if (EDIT_ALARM_STATE == EDIT_ALARM_ON) {
		sprintf(time_position, "A%d", EDIT_TIME_POSITION + 1);
	}
	else {
		sprintf(time_position, "  ");
	}
	
	char freq_pos[3];
	if (EDIT_FREQUENCY_STATE == EDIT_FREQUENCY_ON) {
		sprintf(freq_pos, "F%0d", FREQUENCY_POSITION + 1);
	}
	else {
		sprintf(freq_pos, "   ");
	}
	
	
	sprintf(buf1, "%02d:%02d:%02d %s%s%s", CLOCK_TIME.hour, CLOCK_TIME.minute, CLOCK_TIME.second, edit_clock_state, time_position, freq_pos);
	
	if (CLOCK_STATE == ALARM) {
		struct datetime alarm = ALARMS[ALARMS_POSITION];
		sprintf(buf2, "%02d:%02d:%02d %d ALARM", alarm.hour, alarm.minute, alarm.second, ALARMS_POSITION + 1);
	}
	else if (CLOCK_STATE == STOP_WATCH) {
		struct datetime record = STOPWATCH_RECORDED_TIMES[STOPWATCH_RECORDED_TIMES_POSITION];
		sprintf(buf1, "%02d:%02d:%02d %s WATCH", STOPWATCH_TIME.hour, STOPWATCH_TIME.minute, STOPWATCH_TIME.second, edit_clock_state);
		sprintf(buf2, "%02d:%02d:%02d R%d", record.hour, record.minute, record.second, STOPWATCH_RECORDED_TIMES_POSITION + 1);
	}
	else { // CLOCK_STATE == TIMER
		sprintf(buf2, "%02d:%02d:%02d TIMER", TIMER_TIME.hour, TIMER_TIME.minute, TIMER_TIME.second);
	}
	
	lcd_puts2(buf1); 
	lcd_pos(1,0); 
	lcd_puts2(buf2);
}



void update_edit_time_state() {
	if (EDIT_TIME_STATE == EDIT_TIME_ON) {
		EDIT_TIME_STATE = EDIT_TIME_OFF;
		EDIT_TIME_POSITION = 0;
		EDIT_TIME_CURRENT_TIME = 0;
	}
	else {
		EDIT_TIME_STATE = EDIT_TIME_ON;
	}
}

void set_time(struct datetime *time) {
	if (EDIT_TIME_POSITION == 0) { // hours
		if (EDIT_TIME_CURRENT_TIME < 24) {
			time->hour = EDIT_TIME_CURRENT_TIME;
		}
	}
	else if (EDIT_TIME_POSITION == 1) { // minutes
		if (EDIT_TIME_CURRENT_TIME < 60) {
			time->minute = EDIT_TIME_CURRENT_TIME;
		}
	}
	else if (EDIT_TIME_POSITION == 2) { // seconds
		if (EDIT_TIME_CURRENT_TIME < 60) {
			time->second = EDIT_TIME_CURRENT_TIME;
		}
	}
}

void update_current_time(int key) {
	int num = keypad[key] - 48;
	if (num >= 0 && num <= 9) {
		if (EDIT_TIME_CURRENT_TIME < 100) {
			EDIT_TIME_CURRENT_TIME = EDIT_TIME_CURRENT_TIME * 10 + num;
		}
	}
}

void increment_edit_time_position() {
	EDIT_TIME_CURRENT_TIME = 0;
	if (EDIT_TIME_POSITION < 2) {
		EDIT_TIME_POSITION ++;
	}
}

void decrement_edit_time_position() {
	EDIT_TIME_CURRENT_TIME = 0;
	if (EDIT_TIME_POSITION > 0) {
		EDIT_TIME_POSITION --;
	}
}

// Alarm Functions
void update_edit_alarm_state() {
	if (EDIT_ALARM_STATE ==  EDIT_ALARM_ON) {
		EDIT_ALARM_STATE = EDIT_ALARM_OFF;
		EDIT_TIME_POSITION = 0;
		EDIT_TIME_CURRENT_TIME = 0;
	}
	else {
		EDIT_ALARM_STATE = EDIT_ALARM_ON;
	}
}

void check_alarms() {
	for (int idx = 0; idx < MAX_RECORDED_TIMES; idx ++) {
		if (CLOCK_TIME.hour != 0 || CLOCK_TIME.minute != 0 || CLOCK_TIME.second != 0) {
			if (CLOCK_TIME.hour == ALARMS[idx].hour && CLOCK_TIME.minute == ALARMS[idx].minute && CLOCK_TIME.second == ALARMS[idx].second) {
				ALARM_SOUND_STATE = ON;
				ALARM_NOTES_PLAYED = 0;
			}
		}
	}
}

void  alarm_increment_position() {
	if (ALARMS_POSITION < 4) {
		ALARMS_POSITION ++;
	}
}

void alarm_decrement_position() {
	if (ALARMS_POSITION > 0) {
		ALARMS_POSITION --;
	}
}

// Stopwatch Functions
void stopwatch_record_time(struct datetime time) {
	// Shift all values to end of array
	for (int idx = MAX_RECORDED_TIMES - 1; idx > 0; idx --) {
		STOPWATCH_RECORDED_TIMES[idx] = STOPWATCH_RECORDED_TIMES[idx - 1];
	}
	STOPWATCH_RECORDED_TIMES[0] = time;
}

void  stopwatch_increment_record_position() {
	if (STOPWATCH_RECORDED_TIMES_POSITION < 4) {
		STOPWATCH_RECORDED_TIMES_POSITION ++;
	}
}

void stopwatch_decrement_record_position() {
	if (STOPWATCH_RECORDED_TIMES_POSITION > 0) {
		STOPWATCH_RECORDED_TIMES_POSITION --;
	}
}

void reset_stopwatch_state() {
	STOPWATCH_STATE = STOPWATCH_STOPPED;
	reset_datetime(&STOPWATCH_TIME);
}

// Timer Functions

void reset_timer_state() {
	TIMER_STATE = TIMER_STOPPED;
	reset_datetime(&TIMER_TIME);
}

void decrement_time(struct datetime *time) {
	if (time->subsecond <= 0) {
		if (time->second <= 0) {
			if (time->minute <= 0) {
				if (time->hour <= 0) {
				}
				else { // time.hour > 0
					time->hour --;
					time->minute = 59;
					time->second = 59;
					time->subsecond = 10;
				}
			}
			else { // time.minute > 0
				time->minute --;
				time->second = 59;
				time->subsecond = 10;
			}
		}
		else { // time.second > 0
			time->second --;
			time->subsecond = 10;
		}
	}
	
	if (time->subsecond > 0) {
		time->subsecond --;
	}
	
	// Use this to act as a flag to trigger alarm with subsecond as -1
	if (time->subsecond == 0 && time->second == 0 && time->minute == 0 && time->hour == 0) {
		time->subsecond --;
	}
	
}


void update_edit_frequency_state() {
	if (EDIT_FREQUENCY_STATE ==  EDIT_FREQUENCY_ON) {
		EDIT_FREQUENCY_STATE = EDIT_FREQUENCY_OFF;
	}
	else {
		EDIT_FREQUENCY_STATE = EDIT_FREQUENCY_ON;
	}
}

void update_edit_clock_state() {
	if (EDIT_CLOCK_STATE ==  EDIT_CLOCK_ON) {
		EDIT_CLOCK_STATE = EDIT_CLOCK_OFF;
	}
	else {
		EDIT_CLOCK_STATE = EDIT_CLOCK_ON;
	}
}

int main(void)
{
	DDRB = 0x08;
	avr_init();
	lcd_init();
	
    while (1)  {
		int key = get_key() - 1;
		if (key == -1) {
			// Do Nothing
		}
		else if (keypad[key] == 'A') { // edit clock state
			update_edit_clock_state();
		}
		else if (keypad[key] == 'B') { // edit frequency state
			update_edit_frequency_state();
		}
		
		// UPDATE CLOCK STATE
		if (EDIT_CLOCK_STATE == EDIT_CLOCK_ON) {
			if (keypad[key] == '1'){
				CLOCK_STATE = ALARM;
			}
			else if (keypad[key] == '2'){
				CLOCK_STATE = STOP_WATCH;
			}
			else if (keypad[key] == '3'){
				CLOCK_STATE = TIMER;
			}
		}
		
		// UPDATE FREQUENCY STATE
		if (EDIT_FREQUENCY_STATE == EDIT_FREQUENCY_ON) {
			if (keypad[key] == '*'){
				decrement_current_frequency();
			}
			else if (keypad[key] == '#'){
				increment_current_frequency();
			}
		}
		
		
		// UPDATE ALARM
		if (CLOCK_STATE == ALARM) {
			if (keypad[key] == 'C') { 
				update_edit_alarm_state(); // ADD ALARM
			}
			else if (keypad[key] == 'D') {
				update_edit_time_state(); // EDIT TIME
			}

			if (EDIT_TIME_STATE == EDIT_TIME_ON) {
				if (key != -1) {
					if (keypad[key] == '*') {
						decrement_edit_time_position();
					}
					else if (keypad[key] == '#') {
						increment_edit_time_position();
					}
					else {
						update_current_time(key);
						set_time(&CLOCK_TIME);
					}
				}
			}
			else if (EDIT_ALARM_STATE == EDIT_ALARM_ON) {
				if (key != -1) {
					if (keypad[key] == '*') {
						decrement_edit_time_position();
					}
					else if (keypad[key] == '#') {
						increment_edit_time_position();
					}
					else {
						update_current_time(key);
						set_time(&ALARMS[ALARMS_POSITION]);
					}
				}
			}
			else {
				if (EDIT_FREQUENCY_STATE == EDIT_FREQUENCY_OFF) {
					if (keypad[key] == '*') {
						alarm_decrement_position();; // decrement position
					}
					else if (keypad[key] == '#') {
						alarm_increment_position();; // increment position
					}
				}

			}
		}
		
		
		// UPDATE STOPWATCH
		if (CLOCK_STATE == STOP_WATCH) {
			if (keypad[key] == '1') {
				STOPWATCH_STATE = STOPWATCH_RUNNING; // START/RESUME
			}
			else if (keypad[key] == '2') {
				STOPWATCH_STATE = STOPWATCH_STOPPED; // PAUSE
			}
			else if (keypad[key] == '3') {
				reset_stopwatch_state(); // RESET
			}
			else if (keypad[key] == '4') {
				stopwatch_record_time(STOPWATCH_TIME); // RECORD
			}
			else if (keypad[key] == '*') {
				stopwatch_decrement_record_position();; // decrement position
			}
			else if (keypad[key] == '#') {
				stopwatch_increment_record_position();; // increment position
			}
		}
		
		
		
		// UPDATE TIMER
		if (CLOCK_STATE == TIMER){
			
			if (keypad[key] == 'D') {
				update_edit_time_state(); // EDIT TIME
			}
			
			if (EDIT_TIME_STATE == EDIT_TIME_ON) {
				if (key != -1) {
					if (keypad[key] == '*') {
						decrement_edit_time_position();
					}
					else if (keypad[key] == '#') {
						increment_edit_time_position();
					}
					else {
						update_current_time(key);
						set_time(&TIMER_TIME);
					}
				}
			}
			else { // EDIT_TIME_OFF
				if (keypad[key] == '1') {
					TIMER_STATE = TIMER_RUNNING; // START/RESUME
				}
				else if (keypad[key] == '2') {
					TIMER_STATE = TIMER_STOPPED; // PAUSE
					ALARM_SOUND_STATE = OFF;
				}
				else if (keypad[key] == '3') {
					reset_timer_state(); // RESET
					ALARM_SOUND_STATE = OFF;
				}
			}
		}
		
		
		
		display_lcd();
		
		if (ALARM_SOUND_STATE == ON) {
			play_alarm_note();
			ALARM_NOTES_PLAYED ++;
			if (ALARM_NOTES_PLAYED == MAX_ALARM_NOTES || keypad[key] == '1') {
				ALARM_SOUND_STATE = OFF;
				ALARM_NOTES_PLAYED = 0;
			}
			else if (keypad[key] == '2') {
				
			}
		}
		else {
			avr_wait(100);
		}
		
		increment_time(&CLOCK_TIME);
		
		check_alarms();
		
		if (STOPWATCH_STATE == STOPWATCH_RUNNING) {
			increment_time(&STOPWATCH_TIME);
		}
		
		if (TIMER_STATE == TIMER_RUNNING) {
			decrement_time(&TIMER_TIME);
			if (TIMER_TIME.subsecond == -1) {
				ALARM_SOUND_STATE = ON;
			}
		}
    }
}