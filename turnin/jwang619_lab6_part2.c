/* Author: Jeffrey Wang jwang619@ucr.edu
 * Lab Section: 22
 * Assignment: Lab # 6  Exercise # 2
 * Exercise Description: [optional - include for your own benefit]
 *
 * I acknowledge all content contained herein, excluding template or example
 * code is my own original work.
 *
 *  Demo Link: https://drive.google.com/file/d/1lPTXw4etpA-G1DPVy25BYJm4DI0D3NCs/view?usp=sharing
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

enum SMStates {CycleArr, Pause1, Pause2, Return1} SMState;
unsigned char i = 0x00;
unsigned char stateArr[4] = {0x01, 0x02, 0x04, 0x02};

void SMFunc() {
	unsigned char inputA = 0x00;
	unsigned char outB = 0x00;

	inputA = ~PINA;
	outB = PORTB;

	switch(SMState) {
		case CycleArr:
			if((inputA & 0x01) == 0x01) {
				SMState = Pause1;
			}
			break;
		case Pause1:
			if((inputA & 0x01) == 0x00) {
				SMState = Pause2;
			}
			break;
		case Pause2:
			if((inputA & 0x01) == 0x01) {
				SMState = Return1;
			}
			break;
		case Return1:
			if((inputA & 0x01) == 0x00) {
				SMState = CycleArr;
			}
			break;
		default:
			SMState = CycleArr;
			break;
	}
	switch(SMState) {
		case CycleArr:
			outB = stateArr[i];
			if(i == 0x03) {
				i = 0x00;
			}
			else {
				++i;
			}
			break;
		case Pause1:
			break;
		case Pause2:
			break;
		case Return1:
			break;
		default:
			break;
	}
	PORTB = outB;
}

int main(void) {
    /* Insert DDR and PORT initializations */
        DDRA = 0x00; PORTA = 0xFF;
       	DDRB = 0xFF; PORTB = 0x00;
	TimerSet(300);
	TimerOn();
    /* Insert your solution below */
	SMState = CycleArr;
    while (1) {
	SMFunc();
	while(!TimerFlag);
	TimerFlag = 0;
    }
    return 0;
}
