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
	if ( time.day >= days_in_month[time.month] ) {
		if (is_leap_year(time.year) && time.month == 1) {
			if (time.day == 28) {
				return;
			}
			else {
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
	sprintf(buf, "%02d/%02d/%04d  %s %c", time.month, time.day, time.year, edit_state, edit_position_char);
	lcd_puts2(buf);
	
	lcd_pos(1,0);
	if (military_time) {
		sprintf(buf, "%02d:%02d:%02d", time.hour, time.minute, time.second);
	}
	else{
		char timezone = "";
		if (time.hour >= 12) {
			timezone = "PM";
		} 
		else {
			timezone = "AM";
		}
		sprintf(buf, "%02d:%02d:%02d %s", time.hour % 12, time.minute, time.second, timezone);
	}
	lcd_puts2(buf);
	
}

void add_or_sub_time(int mode) {
	if (edit_position == 0){
		time.month += mode;
	}
	else if (edit_position == 1) {
		time.day += mode;
	}
	else if (edit_position == 2) {
		time.year += mode;
	}
	else if (edit_position == 3) {
		time.hour += mode;
	}
	else if (edit_position == 4) {
		time.minute += mode;
	}
	else if (edit_position == 5) {
		time.second += mode;
	}
					
}


int main(void)
{
	avr_init();
	lcd_init();
    while (1) 
    {
		int key = get_key() - 1;
		
		if (keypad[key] == 'C'){
			military_time = (military_time + 1) % 2;
		}
		else if (keypad[key] == 'D') {
			edit_time = (edit_time + 1) % 2;
		}
		else if (keypad[key] == '#' && edit_time) {
			if (edit_position < 5) {
				edit_position ++;
			}
		}
		else if (keypad[key] == '*' && edit_time) {
			if (edit_position > 0) {
				edit_position --;
			}
		}
		else if (keypad[key] == 'A' && edit_time) {
			add_or_sub_time(1);
		}
		else if (keypad[key] == 'B' && edit_time) {
			add_or_sub_time(-1);
		}
		
		print_time();
		increment_time();
		avr_wait(100);
    }
}

