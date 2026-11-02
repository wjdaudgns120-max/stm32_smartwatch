/*
 * distance.h
 *
 *  Created on: 2026. 10. 29.
 *      Author: kccistc
 */

#ifndef SRC_AP_SERVICE_DISTANCE_DISTANCE_H_
#define SRC_AP_SERVICE_DISTANCE_DISTANCE_H_

#include <stdint.h>

typedef enum {
	DISTANCE_NOT_READY,
	DISTANCE_OK,
	DISTANCE_IN_PROGRESS,
	DISTANCE_ERROR
}distance_status_t;

void distance_init(void);
void distance_task(void);
void distance_request(void);
distance_status_t distance_get(uint16_t *distance_cm);
void distance_ISR(void);


#endif /* SRC_AP_SERVICE_DISTANCE_DISTANCE_H_ */
