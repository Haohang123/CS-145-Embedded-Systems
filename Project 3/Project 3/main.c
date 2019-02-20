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

int FREQUENCY[12] = {220, 233, 246, 261, 277, 293, 311, 329, 349, 369, 391, 415};

#define TOTAL_NOTES 12
//struct Note NOTES[TOTAL_NOTES] = {};
int DURATION_MODIFIER[3] = {1,2,4};
int DURATION = 200;

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

void play_song() {
	for (int i = 0; i < 13; ++i) {
		struct Note note = MARRY_HAD_A_LITTLE_LAMB[i];
		play_note(FREQUENCY[note.frequency] , DURATION / DURATION_MODIFIER[note.duration]);
	}
}

void play_note(int frequency,int duration) {
	int TH = (20000.0 / frequency) / 2;
	int TL= ceil((20000.0 / frequency)) / 2;
	
	lcd_clr();
	char buf[17];
	sprintf(buf, "F:%03d D:%03d %d", frequency, duration, TH);
	lcd_puts2(buf);
	
	for (int i = 0; i < duration; ++i) {
		SET_BIT(PORTA,0);
		avr_wait(TH);
		CLR_BIT(PORTA,0);
		avr_wait(TL);
	}
}

//void notes_init() {
	//int frequency = 220;
	//for (int i = 0; i < TOTAL_NOTES; ++i){
		//NOTES[i].frequency = pow(2.0, (double)i / TOTAL_NOTES ) * frequency;
		//NOTES[i].duration = DURATION; //(1/NOTES[i].frequency) / 2
	//}
//}

//void print_note(int key) {
	//lcd_clr();
	//char buf[17];
	//lcd_pos(0,0);
	//
	//sprintf(buf, "%02d F:%03d D:%03d", key, NOTES[key].frequency, NOTES[key].duration);
	//lcd_puts2(buf);
//}

int main(void)
{
	avr_init();
	lcd_init();
	//notes_init();
	DDRA = 0x01;
	int prevkey = -1;
    while (1) 
    {
		int key = get_key() - 1;
		if (key == -1) {
			// Do Nothing
		}
		else
		{
			//if(prevkey != key){
				//print_note(key);
				//prevkey = key;
			//}
			play_song();
		}
		//avr_wait(1);
    }
}

