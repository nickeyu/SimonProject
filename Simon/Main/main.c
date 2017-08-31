#include <asf.h>
#include "io.c"
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

volatile unsigned char TimerFlag = 0; // TimerISR() sets this to 1. C programmer should clear to 0.

// Internal variables for mapping AVR's ISR to our cleaner TimerISR model.
unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1 ms.
unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1ms ticks

void set_PWM(double frequency) {// Keeps track of the currently set frequency
	static double current_frequency;
	// Will only update the registers when the frequency changes, otherwise allows
	// music to play uninterrupted.
	if (frequency != current_frequency) {
		if (!frequency) { TCCR3B &= 0x08; } //stops timer/counter
		else { TCCR3B |= 0x03; } // resumes/continues timer/counter
		
		// prevents OCR3A from overflowing, using prescaler 64
		// 0.954 is smallest frequency that will not result in overflow
		if (frequency < 0.954) { OCR3A = 0xFFFF; }
		
		// prevents OCR0A from underflowing, using prescaler 64					// 31250 is largest frequency that will not result in underflow
		else if (frequency > 31250) { OCR3A = 0x0000; }
		
		// set OCR3A based on desired frequency
		else { OCR3A = (short)(8000000 / (128 * frequency)) - 1; }

		TCNT3 = 0; // resets counter
		current_frequency = frequency; // Updates the current frequency
	}
}

void PWM_on() {
	TCCR3A = (1 << COM3A0);
	// COM3A0: Toggle PB3 on compare match between counter and OCR0A
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	// WGM02: When counter (TCNT0) matches OCR0A, reset counter
	// CS01 & CS30: Set a prescaler of 64
	set_PWM(0);
}

void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

void TimerOn() {
	// AVR timer/counter controller register TCCR1
	TCCR1B = 0x0B;// bit3 = 0: CTC mode (clear timer on compare)
	// bit2bit1bit0=011: pre-scaler /64
	// 00001011: 0x0B
	// SO, 8 MHz clock or 8,000,000 /64 = 125,000 ticks/s
	// Thus, TCNT1 register will count at 125,000 ticks/s
	// AVR output compare register OCR1A.
	OCR1A = 125;	// Timer interrupt will be generated when TCNT1==OCR1A
	// We want a 1 ms tick. 0.001 s * 125,000 ticks/s = 125
	// So when TCNT1 register equals 125,
	// 1 ms has passed. Thus, we compare to 125.
	// AVR timer interrupt mask register
	TIMSK1 = 0x02; // bit1: OCIE1A -- enables compare match interrupt

	//Initialize avr counter
	TCNT1=0;

	_avr_timer_cntcurr = _avr_timer_M;
	// TimerISR will be called every _avr_timer_cntcurr milliseconds

	//Enable global interrupts
	SREG |= 0x80; // 0x80: 1000000
}

void TimerOff() {
	TCCR1B = 0x00; // bit3bit1bit0=000: timer off
}

void TimerISR() {
	TimerFlag = 1;
}

// In our approach, the C programmer does not touch this ISR, but rather TimerISR()
ISR(TIMER1_COMPA_vect) {
	// CPU automatically calls when TCNT1 == OCR1 (every 1 ms per TimerOn settings)
	_avr_timer_cntcurr--; // Count down to 0 rather than up to TOP
	if (_avr_timer_cntcurr == 0) { // results in a more efficient compare
		TimerISR(); // Call the ISR that the user uses
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

// Set TimerISR() to tick every M ms
void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

int A0;
int A1;
int A2;
int A3;
const double note_C4 = 523.251;
const double note_E4 = 329.63;
const double note_G4 = 392.00;
const double note_B4 = 493.88;


int main (void)
{
	PWM_on();
	TimerOn();
	TimerSet(50);
	
	DDRA = 0x00; PORTA = 0xFF; //Button inputs
	DDRB = 0xFF; PORTB = 0x00; //LEDS outputs
	DDRC = 0xFF; PORTC = 0x00; // LCD data lines
	DDRD = 0xFF; PORTD = 0x00; // LCD control lines
	
	LCD_init();
	LCD_DisplayString(6, "SIMON       PRESS TO START");
//	int seed = time(NULL);
//	srand(seed);
	
	unsigned char i = 0;
	unsigned char j = 0;
	unsigned char k = 0;
	unsigned char randNum = rand() % 4;
	unsigned char gameStarted = 0;
	unsigned char loopNum = 0;
	unsigned char userInputs = 0;
	unsigned char scoreNum = 0;
	unsigned char lost = 0;
	
	unsigned char levelArray[6] = { 4, 5, 6, 7, 8, 9 };
	unsigned char inputArray[9];
	unsigned char inputArraySize = 0;
	
	while(1){
		
		A0 = ~PINA & 0x01;
		A1 = ~PINA & 0x02;
		A2 = ~PINA & 0x04;
		A3 = ~PINA & 0x08;

		if ( (A0 == 0x01 || A1 == 0x02 || A2 == 0x04 || A3 == 0x08) && (gameStarted == 0) ) {
			LCD_ClearScreen();//start game
			LCD_DisplayString(1, "Score: ");
			LCD_WriteData('0');
			gameStarted = 1;
		}
		
		else if ( gameStarted == 1 ) {
			
			if ( levelArray[i] == 4 ) { 
				loopNum = 0x04;//loop 4 times	
			}
			
			else if ( levelArray[i] == 5 ) { 
				loopNum = 0x05;
			}
			
			else if ( levelArray[i] == 6 ) { 
				loopNum = 0x06;
			}
			
			else if ( levelArray[i] == 7 ) {
				loopNum = 0x07;
			}	
			
			else if ( levelArray[i] == 8 ) {
				loopNum = 0x08;
			}
			
			else if ( levelArray[i] == 9 ) {
				loopNum = 0x09;
			}
			
			/*while(!TimerFlag); //test loopNum
			TimerFlag = 0;
			
			PORTB = loopNum;
			while(!TimerFlag);
			TimerFlag = 0;
		
			PORTB = 0x00;
			while(!TimerFlag);
			TimerFlag = 0;
			i++;*/

				for ( j = 0; j < loopNum; j++) {
					randNum = rand() % 4;
					
					while(!TimerFlag);
					TimerFlag = 0;
					PORTB = 0x00;
					while(!TimerFlag);
					TimerFlag = 0;
					
					if ( randNum == 0 ) {
						PORTB = 0x01;
						inputArray[j] = 0x01;
						set_PWM(note_C4);
						inputArraySize++;
					}
					
					else if  ( randNum == 1 )  {
						PORTB = 0x02;
						inputArray[j] = 0x02;
						set_PWM(note_E4);
						inputArraySize++;
					}
					
					else if ( randNum == 2 ) {
						PORTB = 0x04;
						inputArray[j] = 0x04;
						set_PWM(note_G4);
						inputArraySize++;
					}
					
					else if ( randNum == 3 ) {
						PORTB = 0x08;
						inputArray[j] = 0x08;
						set_PWM(note_B4);
						inputArraySize++;
					}
					
					while(!TimerFlag);
						TimerFlag = 0;
						PORTB = 0x00;
						set_PWM(0);
					while(!TimerFlag);
						TimerFlag = 0;
				}
			
			userInputs = 0;
			
			while( userInputs == 0) {
				
				A0 = ~PINA & 0x01;
				A1 = ~PINA & 0x02;
				A2 = ~PINA & 0x04;
				A3 = ~PINA & 0x08;

				PORTB = 0x00;
				set_PWM(0);
				 
				if ( k < inputArraySize ) { 	
					
					if ( A0 != 0x01 && A1 != 0x02 && A2 != 0x04 && A3 != 0x08) { 
						continue;
					}
					
					else if ( inputArray[k] == 0x01 && A0 == 0x01 ) {
						while(!TimerFlag);
						TimerFlag = 0;
						PORTB = 0x01;
						set_PWM(note_C4);
						while(!TimerFlag);
						TimerFlag = 0;
						k++;
					}
				
					else if ( inputArray[k] == 0x02 && A1 == 0x02 ) {
						while(!TimerFlag);
						TimerFlag = 0;
						PORTB = 0x02;
						set_PWM(note_E4);
						while(!TimerFlag);
						TimerFlag = 0;
						k++;	
					}
				
					else if ( inputArray[k] == 0x04 && A2 == 0x04 ) {
						while(!TimerFlag);
						TimerFlag = 0;
						PORTB = 0x04;
						set_PWM(note_G4);
						while(!TimerFlag);
						TimerFlag = 0;
						k++;	
					}
				
					else if ( inputArray[k] == 0x08 && A3 == 0x08 ) {
						while(!TimerFlag);
						TimerFlag = 0;
						PORTB = 0x08;
						set_PWM(note_B4);
						while(!TimerFlag);
						TimerFlag = 0;
						k++;	
					}
				
					else {
						LCD_DisplayString(1, "Wrong! You lost!");
						lost = 1;
						while(!TimerFlag);
						TimerFlag = 0;
						while(!TimerFlag);
						TimerFlag = 0;
						break;
					}
					
				}
				
				if ( k == inputArraySize ) {
					
					inputArray[9] = 0;
					inputArraySize = 0;
					userInputs = 1;
					i++;
					k = 0;
					scoreNum++;
					
				}
				
			}
			
			if ( lost == 1 ) {
				userInputs = 1;
			/*	if ( i > 0 ) {
					i--;
				}
				else {
					i = 0;
				}*/
				lost = 0;
				inputArray[9] = 0;
				inputArraySize = 0;
				k = 0;
				while(!TimerFlag);
				TimerFlag = 0;
				LCD_DisplayString(1, "Score: ");
				LCD_WriteData('0' + scoreNum);
				while(!TimerFlag);
				TimerFlag = 0;
				set_PWM(0);
				PORTB = 0x00;
				while(!TimerFlag);
				TimerFlag = 0;
			}
			else { 
				PORTB = 0x00;
				set_PWM(0);
				while (!TimerFlag);
					TimerFlag = 0;
				LCD_DisplayString(1, "Score: ");
				LCD_WriteData('0' + scoreNum);
				while(!TimerFlag);
					TimerFlag = 0;
				if ( scoreNum == 6 ) {
					LCD_DisplayString(1, "You Win!");
					break;
				}
			}
			
		}
		
	}
}
