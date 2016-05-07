#include "pk_pid.h"
#include "pk_positioning.h"

int targetValue;
int lastReading;
int currentReading;
int timeInterval;

int err_p;
int err_i;
int err_d;

int output;

void pk_pid_init() {
	targetValue = 0;
	lastReading = 0;
	currentReading = 0;
	timeInterval = 0;
	err_p = 0;
	err_i = 0;
	err_d = 0;
}

void setTargetValue(int target) {
	targetValue = target;
}

void takeReading(int reading) {
	lastReading = currentReading;
	currentReading = reading;
}

void takeTimeElapsed(int time) {
	timeInterval = time;
}

void generateErrP() {
	err_p = PID_P * (targetValue - currentReading);
}

void generateErrI() {
	err_i = err_i + PID_I * (targetValue - currentReading);
}

void generateErrD() {
	err_d = PID_D * ((targetValue - currentReading) - (targetValue - lastReading)) / timeInterval;
}

int getOutput() {
	return output;
}
