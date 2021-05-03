/* Author: Jeffrey Wang jwang619@ucr.edu
 * Lab Section: 22
 * Assignment: Lab # 6  Exercise # 1
 * Exercise Description: [optional - include for your own benefit]
 *
 * I acknowledge all content contained herein, excluding template or example
 * code is my own original work.
 *
 *  Demo Link: https://drive.google.com/file/d/1lNo-wDU0tr0HKyWEHvWh-_0ASDt7d-KQ/view?usp=sharing
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn() {
	TCCR1B = 0x0B;

	OCR1A = 125;

	TIMSK1 = 0x02;

	TCNT1 = 0;

	_avr_timer_cntcurr = _avr_timer_M;

	SREG |= 0x80;
}

void TimerOff() {
	TCCR1B = 0x00;
}

void TimerISR() {
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if(_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

enum SMStates {Light1, Light2, Light3} SMState;

void SMFunc() {
	unsigned char outB = 0x00;

	switch(SMState) {
		case Light1:
			SMState = Light2;
			break;
		case Light2:
			SMState = Light3;
			break;
		case Light3:
			SMState = Light1;
			break;
		default:
			SMState = Light1;
			break;
	}
	switch(SMState) {
		case Light1:
			outB = 0x01;
			break;
		case Light2:
			outB = 0x02;
			break;
		case Light3:
			outB = 0x04;
			break;
		default:
			break;
	}
	PORTB = outB;
}

int main(void) {
    /* Insert DDR and PORT initializations */
        DDRB = 0xFF; PORTB = 0x00;
	TimerSet(1000);
	TimerOn();
    /* Insert your solution below */
	SMState = Light1;
    while (1) {
	SMFunc();
	while(!TimerFlag);
	TimerFlag = 0;
    }
    return 0;
}
