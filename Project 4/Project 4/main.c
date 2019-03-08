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
int LOW = 1024;
long TOTAL_IV = 0;
int CURRENT_IV = 0;
int TOTAL_SAMPLES = 0;

float MAX_THRESHOLD = 4.5;
float MIN_THRESHOLD = 0.5;

float MAX_VOLTAGE = 5.0;
int MAX_BITS = 1024;


#define TOTAL_NOTES 12
int FREQUENCY[TOTAL_NOTES] = {220, 233, 246, 261, 277, 293, 311, 329, 349, 369, 391, 415};
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

void play_note(int frequency,int duration) {
	float TH_TL_MODIFIER = 1.0;
	int total_time = ceil((20000.0 / frequency));
	int TH = total_time/2 * TH_TL_MODIFIER;
	
	if(TH== total_time) {
		TH--;
	}
	int TL= total_time-TH;
	
	int duration_runs = duration*85 / total_time;
	for (int i = 0; i < duration_runs; ++i) {
		SET_BIT(PORTB,3);
		avr_wait2(TH);
		CLR_BIT(PORTB,3);
		avr_wait2(TL);
	}
}

void AD_init() {
	ADMUX=(1<<REFS0);
	ADCSRA=(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
	ADMUX |= 0;
}

unsigned int get_AD() {
	unsigned x;
	ADCSRA |= (1<<ADSC) | (1<<ADEN);
	while(! (ADCSRA & (1<<ADIF) ));
	ADCSRA |= (1<<ADIF) | (0<<ADEN);
	x = ADC;
	return (x);
}

void reset() {
	STATE = 0;
	HIGH = 0;
	LOW = 1024;
	TOTAL_IV = 0;
	CURRENT_IV = 0;
	TOTAL_SAMPLES = 0;
}

void print_lcd() {
	char buf1[16]; char buf2[16];
	
	if (STATE == 0) {
			sprintf(buf1, "IV:---- AV:----");
			sprintf(buf2, "HI:---- LO:----");
	}
	else {
		int modifier = 1;
		float total_sum = 0;
		if (STATE == 0 || STATE == 1) {
			
		}
		else if (STATE == 2) {
			modifier = -2;
			total_sum = MAX_VOLTAGE;
		} 
		
		float ratio = MAX_VOLTAGE / (MAX_BITS / modifier) ;
		float high_converted = (float)(HIGH * ratio) + total_sum;
		float low_converted = (float)(LOW * ratio) + total_sum;
		float avg_converted = (float)(TOTAL_IV / TOTAL_SAMPLES * ratio) + total_sum;
		float iv_converted = (float)(CURRENT_IV * ratio) + total_sum;
		
		sprintf(buf1, "IV:%d.%02d AV:%d.%02d", (int)(iv_converted),  (int)((iv_converted - (int)(iv_converted)) * 100), 
										   (int)(avg_converted), (int)((avg_converted - (int)(avg_converted)) * 100));
										   
		sprintf(buf2, "HI:%d.%02d LO:%d.%02d", (int)(high_converted), (int)((high_converted - (int)(high_converted)) * 100), 
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
	DDRB = 0x08;
	avr_init();
	lcd_init();
	AD_init();
	
    while (1) 
    {
		if (STATE) {
			CURRENT_IV = get_AD();
			TOTAL_IV += CURRENT_IV;
			
			if (CURRENT_IV > HIGH) {
				HIGH = CURRENT_IV;
			}
			if (CURRENT_IV < LOW) {
				LOW = CURRENT_IV;
			}
			
			float current_iv = (float)(CURRENT_IV * 5.0/1024);
			
			if (current_iv > MAX_THRESHOLD) {
				play_note(FREQUENCY[7], DURATION / DURATION_MODIFIER[1]);
			}
			else if (current_iv < MIN_THRESHOLD) {
				play_note(FREQUENCY[7], DURATION / DURATION_MODIFIER[1]);
			}
			
			TOTAL_SAMPLES ++;
		}
		
		
		int key = get_key() - 1;
		if (key == -1) {
			// Do Nothing
		}
		else if (keypad[key] == '1') { // START
			ADMUX |= 0x0;
			STATE = 1;
		}
		else if (keypad[key] == '2') { // RESET
			ADMUX |= 0x0;
			reset();
		}
		else if (keypad[key] == '3') { // 2 STATE INPUT
			ADMUX |= 0x10;
			STATE = 2;
		}
		
		print_lcd();
		avr_wait(500);
    }
}

