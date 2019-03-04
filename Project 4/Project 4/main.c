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

int STATE = 0;
int HIGH = 0;
int LOW = 10000;
long TOTAL_IV = 0;
int CURRENT_IV = 0;
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


void AD_init() {
	ADMUX=(1<<REFS0);
	ADCSRA=(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
}

unsigned int get_AD() {
	unsigned x;
	ADMUX |= 0;
	ADCSRA |= (1<<ADSC) | (1<<ADEN);
	while(! (ADCSRA & (1<<ADIF) ));
	ADCSRA |= (1<<ADIF) | (0<<ADEN);
	x = ADC;
	return (x);
}

void reset() {
	TOTAL_SAMPLES = 0;
	STATE = 0;
	HIGH = 0;
	TOTAL_IV = 0;
}

void print_lcd() {
	char buf1[16]; char buf2[16];
	
	if (STATE == 0) {
			sprintf(buf1, "IV:---- AV:----");
			sprintf(buf2, "HI:---- LO:----");
	}
	else {
		float high_converted = (float)(HIGH * 5.0/1024);
		float low_converted = (float)(LOW * 5.0/1024);
		float avg_converted = (float)(TOTAL_IV / TOTAL_SAMPLES * 5.0/1024);
		float iv_converted = (float)(CURRENT_IV * 5.0/1024);
		
		sprintf(buf1, "IV:%d.%d AV:%d.%d", (int)(iv_converted),  (int)((iv_converted - (int)(iv_converted)) * 100), 
										   (int)(avg_converted), (int)((avg_converted - (int)(avg_converted)) * 100));
										   
		sprintf(buf2, "HI:%d.%d LO:%d.%d", (int)(high_converted), (int)((high_converted - (int)(high_converted)) * 100), 
										   (int)(low_converted),  (int)((low_converted - (int)(low_converted)) * 100));
	}
	
	//(int)(1.55), (int)((1.55 - (int)(1.55)) * 100)
	
	lcd_clr();
	lcd_puts2(buf1);
	lcd_pos(1,0);
	lcd_puts2(buf2);
}

int main(void)
{
	// MCUCSR |= (1 << JTD);
	// MCUCSR |= (1 << JTD);
	avr_init();
	lcd_init();
	AD_init();
	// GICR = 1<<INT2;
	// sei();
    while (1) 
    {
		print_lcd();
		CURRENT_IV = get_AD();
		TOTAL_IV += CURRENT_IV;
		
		if (CURRENT_IV > HIGH) {
			HIGH = CURRENT_IV;
		}
		if (CURRENT_IV < LOW) {
			LOW = CURRENT_IV;
		}
		
		TOTAL_SAMPLES ++;
		
		int key = get_key() - 1;
		STATE = 1;
		//if (key == -1) {
			//// Do Nothing
		//}
		//else if (keypad[key] == '1') { // START
			//STATE = 1;
			//
		//}
		//else if (keypad[key] == '2') { // RESET
			//STATE = 0;
		//
		//}
		avr_wait(500);
    }
}

