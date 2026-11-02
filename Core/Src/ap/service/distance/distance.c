/*
 * distance.c
 *
 *  Created on: 2026. 10. 29.
 *      Author: kccistc
 */

#include "distance.h"
#include <stdbool.h>
#include "../../../driver/hcsr04/HCSR04.h"

extern TIM_HandleTypeDef htim10;

static uint32_t echo_us = 0;
static hcsr04_status_t hcsr04_status = HCSR04_ERR_NOT_STARTED;
static bool start_flag = false;
static bool ready = false;

void distance_init(void)
{
	HCSR04_init(GPIOC, GPIO_PIN_5, GPIOC, GPIO_PIN_6, &htim10);
//	ready = true;
}

void distance_task(void)
{
	if (start_flag != true) return;
	if (hcsr04_status != HCSR04_IN_PROGRESS) return;

	hcsr04_status_t s = HCSR04_getResult(&echo_us);
	if (s != HCSR04_IN_PROGRESS) {
		hcsr04_status = s; // OK 또는 timeout(NO_ECHO)
	}
}

void distance_request(void)
{
	if (HCSR04_start() != HCSR04_OK) {
		return; // 측정 중(busy) 이면 무시
	}
	start_flag = true;
	hcsr04_status = HCSR04_IN_PROGRESS; // 이전 결과 초기화
}

distance_status_t distance_get(uint16_t *distance_cm)
{
	if (start_flag != true) {
		return DISTANCE_NOT_READY;
	}

	if (hcsr04_status == HCSR04_IN_PROGRESS) {
		return DISTANCE_IN_PROGRESS;
	}

	start_flag = false;
	if (hcsr04_status != HCSR04_OK) {
		return DISTANCE_ERROR;
	}

	*distance_cm = (echo_us * 17) / 1000;
	return DISTANCE_OK;
}

void distance_ISR(void)
{
//	if (!ready) return;
	HCSR04_echoISR();
}
