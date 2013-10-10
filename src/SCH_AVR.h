// Simple co-operative scheduler for Arduino (or regular ATMega328) with 16MHz crystal
// Licensed under GPLv2, originally from http://www.tte-systems.com/books/pttes
// Copyright  Mohammad Attari
// Modified by: Callan Bryant <callan.bryant@gmail.com>
// Modifications Copyright Callan Bryant 2013

// Operation: init -> add tasks -> start --> loop dispatcher + sleep

// Scheduler data structure for storing task data
typedef struct {
	// Pointer to task
	void (* pTask)(void);
	// Initial delay in ticks
	uint16_t delay;
	// Periodic interval in ticks
	uint16_t period;
	// Runme flag (indicating when the task is due to run)
	uint8_t runMe;
} sTask;

// Function prototypes
void SCH_init(const uint8_t);
void SCH_start(void);
// Core scheduler functions
void SCH_dispatch_tasks(void);
uint8_t SCH_add_task(void (*)(void), const uint16_t, const uint16_t);
uint8_t SCH_delete_task(const uint8_t);
void SCH_sleep(void);

// Maximum number of tasks
// MUST BE ADJUSTED FOR EACH NEW PROJECT
#define SCH_MAX_TASKS (10)


//#include <avr/io.h>
//#include <avr/interrupt.h>


// The array of tasks
sTask SCH_tasks_G[SCH_MAX_TASKS];


//  This is the 'dispatcher' function.  When a task (function)
//  is due to run, SCH_Dispatch_Tasks() will run it.
//  This function must be called (repeatedly) from the main loop.
void SCH_dispatch_tasks(void) {
	uint8_t index;

	// Dispatches (runs) the next task (if one is ready)
	for (index = 0; index < SCH_MAX_TASKS; index++) {
		if ((SCH_tasks_G[index].runMe > 0) && (SCH_tasks_G[index].pTask != 0)) {
			(*SCH_tasks_G[index].pTask)();  // Run the task
			SCH_tasks_G[index].runMe -= 1;   // Reset / reduce runMe flag

			// periodic tasks will automatically run again
			// - if this is a 'one shot' task, remove it from the array
			if (SCH_tasks_G[index].period == 0)
				SCH_delete_task(index);
		}
	}
}

//  Causes a task (function) to be executed at regular intervals
//  or after a user-defined delay
//
//  pFunction - The name of the function which is to be scheduled.
//              NOTE: All scheduled functions must be 'void, void' -
//              that is, they must take no parameters, and have
//              a void return type.
//
//  DELAY     - The interval (TICKS) before the task is first executed
//
//  PERIOD    - If 'PERIOD' is 0, the function is only called once,
//              at the time determined by 'DELAY'.  If PERIOD is non-zero,
//              then the function is called repeatedly at an interval
//              determined by the value of PERIOD (see below for examples
//              which should help clarify this).
//
//
//  RETURN VALUE:
//
//  Returns the position in the task array at which the task has been
//  added.  If the return value is SCH_MAX_TASKS then the task could
//  not be added to the array (there was insufficient space).  If the
//  return value is < SCH_MAX_TASKS, then the task was added
//  successfully.
//
//  Note: this return value may be required, if a task is
//  to be subsequently deleted - see SCH_Delete_Task().
//
//  EXAMPLES:
//
//  Task_ID = SCH_Add_Task(Do_X,1000,0);
//  Causes the function Do_X() to be executed once after 1000 sch ticks.
//
//  Task_ID = SCH_Add_Task(Do_X,0,1000);
//  Causes the function Do_X() to be executed regularly, every 1000 sch ticks.
//
//  Task_ID = SCH_Add_Task(Do_X,300,1000);
//  Causes the function Do_X() to be executed regularly, every 1000 ticks.
//  Task will be first executed at T = 300 ticks, then 1300, 2300, etc.
uint8_t SCH_add_task(void (*pFunction)(), const uint16_t DELAY, const uint16_t PERIOD) {
	uint8_t index = 0;

	// First find a gap in the array (if there is one)
	while ((SCH_tasks_G[index].pTask != 0) && (index < SCH_MAX_TASKS))
		index++;

	// Have we reached the end of the list?
	if (index == SCH_MAX_TASKS)
		// Task list is full, return an error code
		return SCH_MAX_TASKS;

	// If we're here, there is a space in the task array
	SCH_tasks_G[index].pTask  = pFunction;
	SCH_tasks_G[index].delay  = DELAY;
	SCH_tasks_G[index].period = PERIOD;
	SCH_tasks_G[index].runMe  = 0;

	// return position of task (to allow later deletion)
	return index;
}

//  Removes a task from the scheduler.  Note that this does
//  *not* delete the associated function from memory:
//  it simply means that it is no longer called by the scheduler.
//
//  TASK_INDEX - The task index.  Provided by SCH_Add_Task().
//
//  RETURN VALUE:  RETURN_ERROR or RETURN_NORMAL
uint8_t SCH_delete_task(const uint8_t TASK_INDEX) {
	// Return_code can be used for error reporting, NOT USED HERE THOUGH!
	uint8_t Return_code = 0;

	SCH_tasks_G[TASK_INDEX].pTask  = 0;
	SCH_tasks_G[TASK_INDEX].delay  = 0;
	SCH_tasks_G[TASK_INDEX].period = 0;
	SCH_tasks_G[TASK_INDEX].runMe  = 0;

	return Return_code;
}

//  Scheduler initialisation function.  Prepares scheduler
//  data structures and sets up timer interrupts at required rate.
//  You must call this function before using the scheduler.
//  Set TICK ms for a 16MHz crystal
void SCH_init(const uint8_t tick) {
	uint8_t i;

	// disable interrupts -- re-enable with SCH_start()
	cli();

	for (i = 0; i < SCH_MAX_TASKS; i++)
		SCH_delete_task(i);

	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1  = 0;

	// set up timer1 @ 16MHz to call ISR every TICK ms
	// will clobber servo lib and pwm 9 and 10

	// SYS clock @ 16Mhz, Prescaler at 64, 16 bit timer1
	// so 250 per ms
	OCR1A = 250*tick;

	// CTC mode (clear timer on compare match
	TCCR1B |= (1 << WGM12);

	// 64 prescaler
	TCCR1B |= (1 << CS11);
	TCCR1B |= (1 << CS10);

	// enable timer compare interrupt
	TIMSK1 |= (1 << OCIE1A);
}

//  Starts the scheduler, by enabling interrupts.
//
//  NOTE: Usually called after all regular tasks are added,
//  to keep the tasks synchronised.
//
//  NOTE: ONLY THE SCHEDULER INTERRUPT SHOULD BE ENABLED!!!
void SCH_start(void) {
	sei();
}

//  This is the scheduler ISR.  It is called at a rate
//  determined by the timer settings in SCH_Init_T1().
ISR(TIMER1_COMPA_vect) {
	uint8_t index;
	for (index = 0; index < SCH_MAX_TASKS; index++) {
		// Check if there is a task at this location
		if (SCH_tasks_G[index].pTask) {
			if (SCH_tasks_G[index].delay == 0) {
				// The task is due to run, Inc. the 'runMe' flag
				SCH_tasks_G[index].runMe += 1;

				if (SCH_tasks_G[index].period) {
					// Schedule periodic tasks to run again
					SCH_tasks_G[index].delay = SCH_tasks_G[index].period;
					SCH_tasks_G[index].delay -= 1;
				}
			} else
				// Not yet ready to run: just decrement the delay
				SCH_tasks_G[index].delay -= 1;
		}
	}
}

// call after dispatcher has finished dispatching
void SCH_sleep(void) {
	// standby mode with timer1, ext interrupts active and CPU off
	SMCR |= 0b1101;
}

