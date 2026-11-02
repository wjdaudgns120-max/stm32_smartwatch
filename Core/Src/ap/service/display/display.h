/*
 * display.h
 *
 *  Created on: 2026. 10. 28.
 *      Author: kccistc
 */

#ifndef SRC_AP_SERVICE_DISPLAY_DISPLAY_H_
#define SRC_AP_SERVICE_DISPLAY_DISPLAY_H_

#include "../TimeClock/TimeClock.h"
#include "../StopWatch/StopWatch.h"
#include "../temp_humid/temp_humid.h"
#include "../distance/distance.h"

void display_init(void);
void display_timeClock(const TimeClock_t *time, const TimeDate_t *date);
void display_tempHumid(dht11_status_t status, const dht11_data_t *th);
void display_distance(distance_status_t status, uint16_t cm);
void display_stopWatch(const stopWatch_t *time, stopWatch_state_t state);

#endif /* SRC_AP_SERVICE_DISPLAY_DISPLAY_H_ */
