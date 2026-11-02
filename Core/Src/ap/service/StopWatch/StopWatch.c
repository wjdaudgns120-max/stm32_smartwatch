/*
 * StopWatch.c
 *
 *  Created on: 2026. 10. 16.
 *      Author: rhoblack
 */

#include "StopWatch.h"

static void StopWatch_stop(stopWatch_cmd_t cmd);
static void StopWatch_run(stopWatch_cmd_t cmd);
static void StopWatch_clear();
static void StopWatch_setState(stopWatch_state_t s);
static void StopWatch_incMSec(void);
static void StopWatch_clearTime(void);

static volatile stopWatch_t stopWatch_time;
static volatile stopWatch_state_t stopWatch_state;

stopWatch_t * StopWatch_getTimeData(void)
{
	return (stopWatch_t *)&stopWatch_time;
}


void StopWatch_ISR(void)
{
	if (stopWatch_state == RUN) {
		StopWatch_incMSec();
	}
}

static void StopWatch_incMSec(void)
{
	if (stopWatch_time.msec < 99) {
		stopWatch_time.msec++;
		return;
	}
	stopWatch_time.msec = 0;
	if (stopWatch_time.sec < 59) {
		stopWatch_time.sec++;
		return;
	}
	stopWatch_time.sec = 0;
	if (stopWatch_time.min < 59) {
		stopWatch_time.min++;
		return;
	}
	stopWatch_time.min = 0;
	if (stopWatch_time.hour < 23) {
		stopWatch_time.hour++;
		return;
	}
	stopWatch_time.hour = 0;
}

static void StopWatch_clearTime(void)
{
	stopWatch_time.hour = 0;
	stopWatch_time.min = 0;
	stopWatch_time.msec = 0;
	stopWatch_time.sec = 0;
}

static void StopWatch_setState(stopWatch_state_t s)
{
	stopWatch_state = s;
}

stopWatch_state_t StopWatch_getState(void)
{
	return stopWatch_state;
}



void StopWatch_init(void)
{
	stopWatch_state = STOP;

	stopWatch_time.hour = 0;
	stopWatch_time.min = 0;
	stopWatch_time.msec = 0;
	stopWatch_time.sec = 0;


}

// cmd : button queue 에서 꺼낸 event 를 ap 에서 변환해서 전달
void StopWatch_task(stopWatch_cmd_t cmd)
{
	switch(stopWatch_state)
	{
	case STOP:
		StopWatch_stop(cmd);
		break;
	case RUN:
		StopWatch_run(cmd);
		break;
	case CLEAR:
		StopWatch_clear();
		break;
	default:
		StopWatch_stop(cmd);
		break;
	}
}

static void StopWatch_stop(stopWatch_cmd_t cmd)
{
	if (cmd == SW_CMD_RUN_STOP) {
		StopWatch_setState(RUN);
	}
	else if (cmd == SW_CMD_CLEAR) {
		StopWatch_setState(CLEAR);
	}
}

static void StopWatch_run(stopWatch_cmd_t cmd)
{
	if (cmd == SW_CMD_RUN_STOP) {
		StopWatch_setState(STOP);
	}
}

static void StopWatch_clear()
{

	StopWatch_clearTime();
	StopWatch_setState(STOP);
}

