/*
 * ap_main.c
 *
 *  Created on: 2026. 10. 16.
 *      Author: rhoblack
 */

#include "ap_main.h"
#include "service/StopWatch/StopWatch.h"
#include "service/TimeClock/TimeClock.h"
#include "service/temp_humid/temp_humid.h"
#include "service/distance/distance.h"
#include "service/display/display.h"
#include "service/sound/sound.h"
#include "service/btn_event/btn_event.h"
#include <stddef.h>

/*
 * Smart Watch state (button 1 을 누를 때마다 위에서 아래로 이동)
 *
 *   TIME_CLOCK_STATE   (달력/시계)
 *         |  BTN1
 *   TEMP_HUMID_STATE   (온습도, BTN2 : 측정)
 *         |  BTN1
 *   DISTANCE_STATE     (초음파, BTN2 : 측정)
 *         |  BTN1
 *   STOP_WATCH_STATE   (BTN2 : run/stop, BTN3 : clear)
 *         |  BTN1
 *   -> TIME_CLOCK_STATE
 *
 * 각 state 함수 : 1. button event -> 2. service 처리 -> 3. LCD 출력 (App -> Service -> Driver)
 */
typedef enum {
	TIME_CLOCK_STATE,
	TEMP_HUMID_STATE,
	DISTANCE_STATE,
	STOP_WATCH_STATE
} ap_state_t;

#define SEC_NONE	0xff

extern TIM_HandleTypeDef htim4;

static ap_state_t apState = TIME_CLOCK_STATE;

static uint8_t clockPrevSec = SEC_NONE;		// watch 초 증가 sound 용
static uint8_t stopWatchPrevSec = SEC_NONE;	// stopwatch 초 증가 sound 용


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM4) { // 10ms
		TimeClock_ISR();
		StopWatch_ISR();
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == GPIO_PIN_6) { // HC-SR04 echo (rising/falling)
		distance_ISR();
	}
}

static void ap_init(void)
{
	sound_init();
	display_init();
	btn_event_init();

	StopWatch_init();
	TimeClock_init();
	TH_init();
	distance_init();

	HAL_TIM_Base_Start_IT(&htim4);

	sound_power();
}

static void ap_changeState(ap_state_t next)
{
	apState = next;

	// state 진입 직후 tick sound 가 잘못 나지 않도록 초기화
	clockPrevSec = SEC_NONE;
	stopWatchPrevSec = SEC_NONE;
}

// queue 에서 button event 1개를 꺼냄. 모든 button 은 0.1s sound
static btn_event_t *ap_getEvent(btn_event_t *evt)
{
	if (!btn_event_get(evt)) {
		return NULL;
	}
	sound_btnBeep();
	return evt;
}

/* ---------- 1. TIME_CLOCK_STATE ---------- */
static void ap_timeClockState(const btn_event_t *evt)
{
	// 1. button event
	if ((evt != NULL) && (evt->id == BTN_1)) {
		ap_changeState(TEMP_HUMID_STATE);
		return;
	}

	// 2. service
	TimeClock_t *pTime = TimeClock_getTimeData();
	TimeDate_t *pDate = TimeClock_getDateData();

	if ((clockPrevSec != SEC_NONE) && (pTime->sec != clockPrevSec)) {
		sound_tick(); // 초 증가 sound
	}
	clockPrevSec = pTime->sec;

	// 3. LCD
	display_timeClock(pTime, pDate);
}

/* ---------- 2. TEMP_HUMID_STATE ---------- */
static void ap_tempHumidState(const btn_event_t *evt)
{
	dht11_data_t thData;

	// 1. button event
	if (evt != NULL) {
		if (evt->id == BTN_1) {
			ap_changeState(DISTANCE_STATE);
			return;
		}
		if (evt->id == BTN_2) {
			TH_measure(); // button 2 를 눌렀을 때만 측정
		}
	}

	// 2. service
	TH_getTempHumid(&thData);

	// 3. LCD
	display_tempHumid(TH_getStatus(), &thData);
}

/* ---------- 3. DISTANCE_STATE ---------- */
static void ap_distanceState(const btn_event_t *evt)
{
	static distance_status_t distStatus = DISTANCE_NOT_READY;
	static uint16_t distance_cm = 0;

	// 1. button event
	if (evt != NULL) {
		if (evt->id == BTN_1) {
			ap_changeState(STOP_WATCH_STATE);
			return;
		}
		if (evt->id == BTN_2) {
			distance_request(); // button 2 를 눌렀을 때만 측정
		}
	}

	// 2. service
	distance_task();
	distance_status_t s = distance_get(&distance_cm);
	if (s != DISTANCE_NOT_READY) {
		distStatus = s; // 측정중 / OK / ERROR
	}

	// 3. LCD
	display_distance(distStatus, distance_cm);
}

/* ---------- 4. STOP_WATCH_STATE ---------- */
static void ap_stopWatchState(const btn_event_t *evt)
{
	stopWatch_cmd_t cmd = SW_CMD_NONE;

	// 1. button event
	if (evt != NULL) {
		if (evt->id == BTN_1) {
			ap_changeState(TIME_CLOCK_STATE);
			return;
		}
		if (evt->id == BTN_2) cmd = SW_CMD_RUN_STOP;
		else if (evt->id == BTN_3) cmd = SW_CMD_CLEAR;
	}

	// 2. service
	StopWatch_task(cmd);

	stopWatch_t *pStopWatchTime = StopWatch_getTimeData();
	stopWatch_state_t stopWatchState = StopWatch_getState();

	if ((stopWatchState == RUN) && (stopWatchPrevSec != SEC_NONE)
			&& (pStopWatchTime->sec != stopWatchPrevSec)) {
		sound_tick(); // 초 증가 sound
	}
	stopWatchPrevSec = pStopWatchTime->sec;

	// 3. LCD
	display_stopWatch(pStopWatchTime, stopWatchState);
}

int ap_main(void)
{
	btn_event_t evt;
	btn_event_t *pEvt;

	ap_init();

	while(1) // super loop
	{
		// 공통 처리
		btn_event_scan();			// button -> queue
		sound_task();				// 시간 지난 sound off
		TimeClock_task();			// RTC read
		pEvt = ap_getEvent(&evt);	// queue -> event

		// state 처리
		switch (apState)
		{
		case TIME_CLOCK_STATE:
			ap_timeClockState(pEvt);
			break;
		case TEMP_HUMID_STATE:
			ap_tempHumidState(pEvt);
			break;
		case DISTANCE_STATE:
			ap_distanceState(pEvt);
			break;
		case STOP_WATCH_STATE:
			ap_stopWatchState(pEvt);
			break;
		default:
			ap_changeState(TIME_CLOCK_STATE);
			break;
		}
	}
	return 0;
}
