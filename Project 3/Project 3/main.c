/*
 * Project 3.c
 *
 * Created: 1/30/2019 12:26:36 PM
 * Author : Jackson Tsoi
 */ 

#include <avr/io.h>
#include <stdio.h>
#include <math.h>
#include "avr.h"
#include "lcd.h"

struct Note  {
	int frequency;
	int duration;
};

struct Note MARRY_HAD_A_LITTLE_LAMB[13] = { {7,1}, {5,1}, {3,1}, {5,1}, {7,1}, {7,1}, {7,0}, {5,1}, {5,1} , {5,0}, {7,1} , {10,1}, {10,0} };

#define TOTAL_NOTES 12
int FREQUENCY[TOTAL_NOTES] = {220, 233, 246, 261, 277, 293, 311, 329, 349, 369, 391, 415};
int DURATION_MODIFIER[3] = {1,2,4};
int DURATION = 200;
int SONG_NOTE = 0;
float TH_TL_MODIFIER = 1.0;

char keypad[17] = {
	'1', '2', '3', 'A',
	'4', '5', '6', 'B',
	'7', '8', '9', 'C',
	'*', '0', '#', 'D'
};

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

void play_note(int frequency,int duration) {
	int total_time = ceil((20000.0 / frequency));
	int TH = total_time/2 * TH_TL_MODIFIER;
	
	if(TH== total_time) {
		TH--;
	}
	int TL= total_time-TH;
	
	lcd_clr();
	char buf[17]; char buf2[17];
	sprintf(buf, "F:%03d D:%03d  %d", frequency, duration, DURATION);
	sprintf(buf2, "TH:%d TL:%d, %d", TH, TL, total_time);
	lcd_puts2(buf);
	lcd_pos(1,0);
	lcd_puts2(buf2);
	
	int duration_runs = duration*85 / total_time;
	for (int i = 0; i < duration_runs; ++i) {
		SET_BIT(PORTA,0);
		avr_wait(TH);
		CLR_BIT(PORTA,0);
		avr_wait(TL);
	}
}

int main(void)
{
	avr_init();
	lcd_init();
	DDRA = 0x01;
	SONG_NOTE = 13;
    while (1) 
    {	
		if (SONG_NOTE < 13)
		{
			struct Note note = MARRY_HAD_A_LITTLE_LAMB[SONG_NOTE];
			play_note(FREQUENCY[note.frequency] , DURATION / DURATION_MODIFIER[note.duration]);
			SONG_NOTE ++;
		}
		
		int key = get_key() - 1;
		if (key == -1) {
			// Do Nothing
		}
		else if (keypad[key] == '*') {
			if (TH_TL_MODIFIER > 0.3) {
				TH_TL_MODIFIER -= .25;
			}
		}
		else if (keypad[key] == '0') {
			if (TH_TL_MODIFIER < 1.9) {
				TH_TL_MODIFIER += .25;
			}
		}
		else if (keypad[key] == '#') {
			if (DURATION >= 20)
			{
				DURATION -= 20;
			}
		}
		else if (keypad[key] == 'D') {
			DURATION += 20;
		}

		else {
			SONG_NOTE = 0;
		}
    }
}

