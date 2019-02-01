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

void increment_time() {
	time.subsecond ++;
	if (time.subsecond == 10) {
		time.subsecond = 0;
		time.second ++;
		
		if (time.second == 60) {
			time.second = 0;
			time.minute ++;
			
			if (time.minute == 60) {
				time.minute = 0;
				time.hour ++;
				
				if (time.hour == 24) {
					time.hour = 0;
					time.day ++;
					
					if (time.day ) {
					}
				}
			}
		}
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
int main(void)
{
	DDRB = 0x01;
    while (1) 
    {
		int key = get_key();
		blinkNtimes(key);
    }
}

