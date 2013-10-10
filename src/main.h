// global vars and config.h
#pragma once
#include "../config.h"
#include <stdint.h>

// MASKS
#define BALLAST_REQ       0b1000
#define BALLAST_ACTIVE    0b0100
#define BALLAST_IDLE      0b0010
#define BALLAST_OVERHEAT  0b0001

// MAPPED TO PINS
#define POWER_FULL        0b100
#define POWER_CHARGING    0b010
#define POWER_MAINS       0b001
#define POWER_DISCHARGING 0b000

bool
	mains_online      = 0,
	charging          = 0,
	charging_conplete = 0;

uint8_t ballast_state = BALLAST_IDLE;

void ballast_init(void) {
	// to read throttle from driver
	pinMode(BALLAST_RELAY_PIN_N,OUTPUT);
	digitalWrite(BALLAST_RELAY_PIN_N,1);
}

void serial_init(void) {
	Serial.begin(BAUD_RATE);
}

void ballast_throw(void) {
	// TODO: check temperature first
	// TODO: check for mains first (maybe in fsm)
	ballast_state = BALLAST_REQ;
}

void ballast_fsm(void) {
	switch(ballast_state) {
		case BALLAST_IDLE:
		break;

		case BALLAST_REQ:
			digitalWrite(BALLAST_RELAY_PIN_N,0);
			ballast_state = BALLAST_ACTIVE;
		break;

		case BALLAST_ACTIVE:
			digitalWrite(BALLAST_RELAY_PIN_N,1);
			ballast_state = BALLAST_IDLE;
		break;
	}
}

void serial_command (void) {
	uint8_t command = 0x00;
	if (Serial.available()) {
		command = Serial.read();
		switch (command) {
			case COMMAND_THROW:
				ballast_throw();
			break;

			default:
				Serial.write("Wut.\n");
		}
	}
}
