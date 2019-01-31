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


int is_pressed(int row, int col) {
	// all row and col is NC
	DDRC = 0;
	PORTC = 0;
	avr_wait(1);
	
	// set all col to strong 1
	DDRC = 0x01;
	SET_BIT(PORTC, 4);
	SET_BIT(PORTC, 5);
	SET_BIT(PORTC, 6);
	SET_BIT(PORTC, 7);
	avr_wait(1);
	
	// set col to strong 0
	DDRC = 0x01;
	CLR_BIT(PORTC, col + 4);
	avr_wait(1);
	
	// set row to weak 1
	DDRC = 0;
	SET_BIT(PORTC, row);
	avr_wait(1);
	
	
	if (GET_BIT(PINC,row)) {
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
		CLR_BIT(PORTB,0);
		avr_wait(500);
		SET_BIT(PORTB,0);
		avr_wait(500);
	}
}

int main(void)
{
	DDRB = 0x01;
    while (1) 
    {
		int key = get_key();
		if (key == 0) {
			SET_BIT(PORTB, 0);
		}
		else {
			blinkNtimes(key);
		}
		//CLR_BIT(PORTB,0);
		avr_wait(2000);
    }
}

