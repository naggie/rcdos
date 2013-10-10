// timer and external interrupt setup

//#include "main.h"
// almost RTOS stuff here

/*
// REPLACED BY MJP SCHEDULER
void setup_timers(void) {
	// enable interrupts (match A, too)
	cli();
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1  = 0;

	// set up timer1 @ 16MHz to call ISR every TICK ms
	// will clobber servo lib and pwm 9 and 10

	// SYS clock @ 16Mhz, Prescaler at 64, 16 bit timer1
	// so 250 per ms
	OCR1A = 250*TICK;

	// CTC mode (clear timer on compare match
	TCCR1B |= (1 << WGM12);

	// 64 prescaler
	TCCR1B |= (1 << CS11);
	TCCR1B |= (1 << CS10);

	// enable timer compare interrupt
	TIMSK1 |= (1 << OCIE1A);

	sei();
}
*/

// sets up pins 2 and 3 as external interrupts
//https://sites.google.com/site/qeewiki/books/avr-guide/external-interrupts-on-the-atmega328
void setup_external_interrupts(void) {
	// no interrupts
	cli();

	// input/pullup
	pinMode(2,INPUT);
	digitalWrite(2,HIGH);

	// every transition (leave ISC01 as zero)
	EICRA |= ( 1 << ISC00);
	// Global Enable INT0 interrupt
	EIMSK |= ( 1 << INT0);

	// input/pullup
	pinMode(3,INPUT);
	digitalWrite(3,HIGH);

	// every transition
	EICRA |= ( 1 << ISC10);
	// Global Enable INT1 interrupt
	EIMSK  |= ( 1<< INT1);

	// interrupts
	sei();
}


