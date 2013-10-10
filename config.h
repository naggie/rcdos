#pragma once


// tick interval in ms, 1-20 recommended, 260 max
// set to max common factor of task periods
#define TICK_INTERVAL           10

#define BALLAST_RELAY_PIN_N     11
#define BALLAST_TEMPERATURE_PIN A1
#define CHARGE_COMPLETE_PIN     5
#define CHARGE_IN_PROGRESS_PIN  6
#define MAINS_ONLINE_PIN        A0

#define COMMAND_THROW           'T'

// number of ticks to enable ballast for
#define BALLAST_INTERVAL        10


#define BAUD_RATE               9600

