/* Author: Jeffrey Wang jwang619@ucr.edu
 * Lab Section: 22
 * Assignment: Lab # 6  Exercise # 3
 * Exercise Description: [optional - include for your own benefit]
 *
 * I acknowledge all content contained herein, excluding template or example
 * code is my own original work.
 *
 *  Demo Link: https://drive.google.com/file/d/1lYLA5-9as37o4XZTG_mgBdiE41qv6nES/view?usp=sharing
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

enum SMStates {SMStart, Init, Wait, Inc, Reset, Dec, WaitInc, WaitReset, WaitDec} SMState;
unsigned char i = 0x00;
unsigned char per = 100;

void SMFunc() {
        unsigned char inputA = 0x00;
        unsigned char outputB = 0x00;

        outputB = PORTB;
        inputA = ~PINA;
        switch(SMState) {
                case SMStart:
                        SMState = Init;
                        break;
                case Init:
			i = 0x00;
			if((inputA & 0x03) == 0x01) {
				SMState = Inc;
			}
			else if((inputA & 0x03) == 0x03) {
                                SMState = Reset;
                        }
			else if((inputA & 0x03) == 0x02) {
                                SMState = Dec;
                        }
			else {
				SMState = Wait;
			}
			break;
                case Wait:
			i = 0x00;
                        if((inputA & 0x03) == 0x01) {
                                SMState = Inc;
                        }
                        if((inputA & 0x03) == 0x03) {
                                SMState = Reset;
                        }
                        if((inputA & 0x03) == 0x02) {
                                SMState = Dec;
                        }
                        break;
                case Inc:
                        SMState = WaitInc;
                        break;
                case Reset:
                        SMState = WaitReset;
                        break;
                case Dec:
                        SMState = WaitDec;
                        break;
		case WaitInc:
			if((inputA & 0x03) == 0x03) {
                                SMState = Reset;
                        }
			if((inputA & 0x03) == 0x02) {
                                SMState = Dec;
                        }	
                        if((inputA & 0x03) == 0x00) {
                                SMState = Wait;
                        }
			if(!(i < (1000/per))) {
                                SMState = Inc;
                                i = 0x00;
                        }
			++i;
			break;
		case WaitReset:
                        if((inputA & 0x03) == 0x01) {
                                SMState = Inc;
                        }
                        if((inputA & 0x03) == 0x02) {
                                SMState = Dec;
                        }
                        if((inputA & 0x03) == 0x00) {
                                SMState = Wait;
                        }
                        break;
		case WaitDec:
                        if((inputA & 0x03) == 0x03) {
                                SMState = Reset;
                        }
                        if((inputA & 0x03) == 0x01) {
                                SMState = Inc;
                        }
                        if((inputA & 0x03) == 0x00) {
                                SMState = Wait;
                        }
			if(!(i < (1000/per))) {
                                SMState = Dec;
                                i = 0x00;
                        }
			++i;
                        break;
                default:
                        SMState = SMStart;
                        break;

        }
        switch(SMState) {
                case SMStart:
                        break;
                case Init:
                        outputB = 0x07;
                        break;
                case Wait:
                        break;
                case Inc:
			if(outputB < 0x09) {
                                outputB = outputB + 1;
                        }
                        break;
                case Reset:
                        outputB = 0x00;
                        break;
                case Dec:
                        if(outputB > 0x00) {
                                outputB = outputB - 1;
                        }
                        break;
		case WaitInc:
			break;
		case WaitReset:
			break;
		case WaitDec:
			break;
                default:
                        break;
        }
        PORTB = outputB;
}

int main(void) {
    /* Insert DDR and PORT initializations */
        DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
        DDRB = 0xFF; PORTB = 0x00;
	TimerSet(100);
	TimerOn();
    /* Insert your solution below */
        SMState = SMStart;

    while (1) {
        SMFunc();
	while(!TimerFlag);
	TimerFlag = 0;
    }
    return 0;
}
