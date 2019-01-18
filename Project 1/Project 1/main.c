/*
 * Project 1.c
 *
 * Created: 1/16/2019 5:49:38 PM
 * Author : Jackson Tsoi
 */ 

#include <avr/io.h>

int main(void)
{
    /* Replace with your application code */
//     while (1) 
//     {
//     }
	DDRB = 1;
	for( ; ;) {
		PORTB = 1;  /* LED ON*/
		wait_half_sec();
		PORTB = 0;  /* LED OFF */
		wait_half_sec();
	}

}

