/*
 * StopWatch.h
 *
 *  Created on: 2026. 10. 16.
 *      Author: rhoblack
 */

#ifndef SRC_AP_STOPWATCH_STOPWATCH_H_
#define SRC_AP_STOPWATCH_STOPWATCH_H_

#include <stdint.h>

typedef struct {
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
	uint8_t msec;
} stopWatch_t;

typedef enum {STOP, RUN, CLEAR} stopWatch_state_t;
typedef enum {SW_CMD_NONE, SW_CMD_RUN_STOP, SW_CMD_CLEAR} stopWatch_cmd_t;

void StopWatch_init(void);
void StopWatch_task(stopWatch_cmd_t cmd);
void StopWatch_ISR(void);
stopWatch_t * StopWatch_getTimeData(void);
stopWatch_state_t StopWatch_getState(void);

#endif /* SRC_AP_STOPWATCH_STOPWATCH_H_ */
