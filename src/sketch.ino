// global vars and config.h
#include "main.h"

// timer and external interrupt setup
//#include "interrupts.h"

// scheduler
#include "SCH_AVR.h"


// TODO: more raw register things for SPI and UART
// UART: http://extremeelectronics.co.in/avr-tutorials/using-the-usart-of-avr-microcontrollers-reading-and-writing-data/

void setup () {
	ballast_init();
	serial_init();
	//status_init();
	//setup_external_interrupts();
	mains_check_init();

	// set up scheduler with tick interval defined in config.h
	SCH_init(TICK_INTERVAL);

	SCH_add_task(ballast_fsm, 1, BALLAST_INTERVAL);
	SCH_add_task(serial_command, 2, 20);
	SCH_add_task(mains_check, 3, 40);

	SCH_start();
}

void loop () {
	SCH_dispatch_tasks();
	SCH_sleep();
}


// MACRO: Interrupt for Left and right encoder
//ISR(INT0_vect) { L_count++; }
//ISR(INT1_vect) { R_count++; }
