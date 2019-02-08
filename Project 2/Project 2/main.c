/*
 * Project 2.c
 *
 * Created: 1/30/2019 12:26:36 PM
 * Author : Jackson Tsoi
 */ 

#include <avr/io.h>
#include <stdio.h>
#include "avr.h"
#include "lcd.h"

struct datetime {
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int second;
	int subsecond;	
} time;

char keypad[17] = {
	'1', '2', '3', 'A',
	'4', '5', '6', 'B',
	'7', '8', '9', 'C',
	'*', '0', '#', 'D'
};

int days_in_month[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
int edit_time = 0;
int edit_position = 0;
int military_time = 0;
int is_setting_time = 0;
int time_being_set = 0;
int time_being_set_position = 0;


void increment_time() {
	time.subsecond ++;
	if (time.subsecond >= 10) {
		time.subsecond = 0;
		time.second ++;
	}
	if (time.second >= 60) {
		time.second = 0;
		time.minute ++;
	}
	if (time.minute >= 60) {
		time.minute = 0;
		time.hour ++;
	}
	if (time.hour >= 24) {
		time.hour = 0;
		time.day ++;
	}
	if ( time.day >= days_in_month[time.month]) {
		if (time.year % 4 == 0 && time.month == 1 && (time.year % 100 != 0 || time.year % 400 == 0 ) ) {
			if (time.day == 27) {
				return;
			}
			else if (time.day > 28) {
				time.day = 0;
				time.month ++;
			}
		}
		else {
			time.day = 0;
			time.month ++;
		}
	}						
	if (time.month >= 12) {
		time.month = 0;
		time.year ++;
	}
}

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

void blinkNtimes(int times) {
	for(int i = 0; i < times; ++i) {
		blink();
	}
}

void blink() {
	SET_BIT(PORTB,0);
	avr_wait(500);
	CLR_BIT(PORTB,0);
	avr_wait(500);
}

void print_time() {
	lcd_clr();
	char buf[17];
	
	lcd_pos(0,0);
	char edit_state = "";
	char edit_position_char = 0;
	if (edit_time) {
		edit_state = "E";
		edit_position_char = edit_position + 48 + 1;
	}
	sprintf(buf, "%02d/%02d/%04d  %s %c", time.month + 1, time.day + 1, time.year, edit_state, edit_position_char);
	lcd_puts2(buf);
	
	lcd_pos(1,0);
	int hour = time.hour;
	if (military_time) {
		if (edit_time == 1) {
			sprintf(buf, "%02d:%02d:%02d %04d", hour, time.minute, time.second, time_being_set);
		}
		else {
			sprintf(buf, "%02d:%02d:%02d", hour, time.minute, time.second);
		}
	}
	else{
		int am_pm_hour = hour;
		if (hour % 12 == 0) {
			am_pm_hour = 12;
		}
		else {
			am_pm_hour %= 12;
		}
		
		char timezone = "";
		if (hour >= 12) {
			timezone = "PM";
		} 
		else {
			timezone = "AM";
			
		}
		if (edit_time == 1) {
			sprintf(buf, "%02d:%02d:%02d %s %04d", am_pm_hour , time.minute, time.second, timezone, time_being_set);
		}
		else {
			sprintf(buf, "%02d:%02d:%02d %s", am_pm_hour , time.minute, time.second, timezone);
		}
		
	}
	lcd_puts2(buf);
	
}

void add_or_sub_time(int mode) {
	if (edit_position == 0){
		if (time.month + mode >= 0){
			time.month += mode;
		}
	}
	else if (edit_position == 1) {
		if (time.day + mode >= 0){
			time.day += mode;
		}
	}
	else if (edit_position == 2) {
		if (time.year + mode >= 0){
			time.year += mode;
		}
	}
	else if (edit_position == 3) {
		if (time.hour + mode >= 0){
			time.hour += mode;
		}
	}
	else if (edit_position == 4) {
		if (time.minute + mode >= 0){
			time.minute += mode;
		}
	}
	else if (edit_position == 5) {
		if (time.second + mode >= 0){
			time.second += mode;
		}
	}
					
}

void set_time() {
	if (edit_position == 0){
		if (time_being_set > 0) {
			time.month = time_being_set - 1;
		}
	}
	else if (edit_position == 1) {
		if (time_being_set > 0) {
			time.day = time_being_set - 1;
		}
	}
	else if (edit_position == 2) {
		time.year = time_being_set;
	}
	else if (edit_position == 3) {
		time.hour = time_being_set;
	}
	else if (edit_position == 4) {
		time.minute = time_being_set;
	}
	else if (edit_position == 5) {
		time.second = time_being_set;
	}	
}

void clear_set_time() {
	time_being_set = 0;
	time_being_set_position = 0;
	is_setting_time = 0;
}

int main(void)
{
	avr_init();
	lcd_init();
    while (1) 
    {
		int key = get_key() - 1;
		if (key == -1) {
			// Do Nothing
		}
		else if (keypad[key] == 'C'){
			military_time = (military_time + 1) % 2;
		}
		else if (keypad[key] == 'D') {
			edit_time = (edit_time + 1) % 2;
			is_setting_time = 0;
			time_being_set = 0;
			time_being_set_position = 0;
		}
		else if (edit_time) {
			if (keypad[key] == '#') {
				if (edit_position < 5) {
					edit_position ++;
				}
			}
			else if (keypad[key] == '*') {
				if (edit_position > 0) {
					edit_position --;
				}
			}
			else if (keypad[key] == 'A') {
				add_or_sub_time(1);
			}
			else if (keypad[key] == 'B') {
				add_or_sub_time(-1);
			}
			else {
				is_setting_time = 1;
				time_being_set_position ++;
				int num = keypad[key] - 48;
				time_being_set = time_being_set * 10 + num;
				if (edit_position == 2 && time_being_set_position == 4) { // edit position is on year
					set_time();
					clear_set_time();
				}
				else if (edit_position != 2 && time_being_set_position == 2) { // edit position is on everything else other than year
					set_time();
					clear_set_time();
				}
			}
		}
		print_time();
		increment_time();
		avr_wait(100);
    }
}

