/*
 * Project 4.c
 *
 * Created: 2/27/2019 12:24:30 PM
 * Author : Jackson Tsoi
 */ 

#include <avr/io.h>
#include <stdio.h>
#include "avr.h"
#include "lcd.h"

int TOTAL_SAMPLES = 0;

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

void print_lcd() {
	lcd_clr();
	char buf[17]; char buf2[17];
	sprintf(buf, "hello");
	sprintf(buf2, "");
	lcd_puts2(buf);
	lcd_pos(1,0);
	lcd_puts2(buf2);
}

int get_sample() {
	
}

int main(void)
{
	avr_init();
	lcd_init();
    while (1) 
    {
		print_lcd();
		
		int key = get_key() - 1;
		if (key == -1) {
			// Do Nothing
		}
		else if (keypad[key] == '1') {
		}
		else if (keypad[key] == '2') {
		}
		avr_wait(500);
    }
}

