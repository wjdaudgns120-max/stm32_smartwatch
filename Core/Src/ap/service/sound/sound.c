/*
 * sound.c
 *
 *  Created on: 2026. 10. 30.
 *      Author: kccistc
 */

#include "sound.h"
#include "../../../driver/buzzer/buzzer.h"
#include <stdbool.h>

extern TIM_HandleTypeDef htim1;

#define C4 262 // 도
#define D4 293 // 레
#define E4 330 // 미
#define F4 350 // 파
#define G4 392 // 솔
#define A4 440 // 라
#define B4 494 // 시

#define C5 (C4*2) // 도
#define D5 (D4*2) // 레
#define E5 (E4*2) // 미
#define F5 (F4*2) // 파
#define G5 (G4*2) // 솔
#define A5 (A4*2) // 라
#define B5 (B4*2) // 시

#define C6 (C5*2) // 도
#define D6 (D5*2) // 레
#define E6 (E5*2) // 미
#define F6 (F5*2) // 파
#define G6 (G5*2) // 솔
#define A6 (A5*2) // 라
#define B6 (B5*2) // 시

#define C7 (C6*2) // 도
#define D7 (D6*2) // 레
#define E7 (E6*2) // 미
#define F7 (F6*2) // 파
#define G7 (G6*2) // 솔
#define A7 (A6*2) // 라
#define B7 (B6*2) // 시

#define Db5 554   // 레♭
#define Eb5 622   // 미♭
#define Ab5 831   // 라♭
#define Bb5 932   // 시♭

#define Db6 1109   // 레
#define Eb6 1245   // 미
#define Ab6 1661   // 라
#define Bb6 1865   // 시

#define Db7 (Db6*2)   // 레
#define Eb7 (Eb6*2)   // 미
#define Ab7 (Ab6*2)   // 라
#define Bb7 (Bb6*2)   // 시

#define BTN_BEEP_MS		100	// button 소리 0.1초
#define TICK_BEEP_MS	30	// 초 증가 소리

static bool beep_on = false;
static uint32_t beep_start;
static uint32_t beep_time;

void sound_init(void)
{
	buzzer_init(&htim1, TIM_CHANNEL_4);
}

// HAL_Delay 없이 buzzer on 후 시간만 기록 => sound_task() 에서 off
static void sound_beep(uint32_t freq, uint32_t ms)
{
	buzzer_on(freq);
	beep_start = HAL_GetTick();
	beep_time = ms;
	beep_on = true;
}

void sound_task(void)
{
	if (!beep_on) return;
	if (HAL_GetTick() - beep_start < beep_time) return;

	buzzer_off();
	beep_on = false;
}

void sound_btnBeep(void)
{
	sound_beep(C7, BTN_BEEP_MS);
}

void sound_tick(void)
{
	if (beep_on) return; // button 소리가 우선
	sound_beep(G6, TICK_BEEP_MS);
}


void sound_power(void)
{
	buzzer_on(C6); // C4
	HAL_Delay(100);

	buzzer_on(E6); // E4
	HAL_Delay(100);

	buzzer_on(G6); // G4
	HAL_Delay(100);

	buzzer_on(C7); // C5
	HAL_Delay(200);

	buzzer_off();
}

void sound_button(void)
{
	buzzer_on(G6);
	HAL_Delay(40);
	buzzer_on(C7);
	HAL_Delay(60);
	buzzer_off();
}

void sound_funny(void)
{
	// 천방지축
	buzzer_on(G4);
	HAL_Delay(180);
	buzzer_off();
	buzzer_on(G4);
	HAL_Delay(180);
	buzzer_off();
	buzzer_on(F4);
	HAL_Delay(180);
	buzzer_off();
	buzzer_on(E4);
	HAL_Delay(180);
	buzzer_off();

	// 어리둥절
	buzzer_on(D4);
	HAL_Delay(180);
	buzzer_off();
	buzzer_on(E4);
	HAL_Delay(180);
	buzzer_off();
	buzzer_on(F4);
	HAL_Delay(180);
	buzzer_off();
	buzzer_on(G4);
	HAL_Delay(180);
	buzzer_off();

	// 빙글빙글
	buzzer_on(G4);
	HAL_Delay(180);
	buzzer_off();
	buzzer_on(G4);
	HAL_Delay(180);
	buzzer_off();
	buzzer_on(F4);
	HAL_Delay(180);
	buzzer_off();
	buzzer_on(E4);
	HAL_Delay(180);
	buzzer_off();

	// 돌아가는
	buzzer_on(D4);
	HAL_Delay(180);
	buzzer_off();
	buzzer_on(E4);
	HAL_Delay(180);
	buzzer_off();
	buzzer_on(F4);
	HAL_Delay(180);
	buzzer_off();
	buzzer_on(G4);
	HAL_Delay(180);
	buzzer_off();

	// 짱구의하루
	buzzer_on(A4);
	HAL_Delay(180);
	buzzer_off();
	buzzer_on(G4);
	HAL_Delay(180);
	buzzer_off();
	buzzer_on(F4);
	HAL_Delay(180);
	buzzer_off();
	buzzer_on(E4);
	HAL_Delay(180);
	buzzer_off();
	buzzer_on(D4);
	HAL_Delay(180);
	buzzer_off();

	// 우리의
	buzzer_on(G4);
	HAL_Delay(360);
	buzzer_off();
	buzzer_on(A4);
	HAL_Delay(180);
	buzzer_off();
	buzzer_on(B4);
	HAL_Delay(180);
	buzzer_off();

	// 짱구는
	buzzer_on(C5);
	HAL_Delay(180);
	buzzer_off();
	buzzer_on(D5);
	HAL_Delay(180);
	buzzer_off();
	buzzer_on(E5);
	HAL_Delay(360);
	buzzer_off();

	// 정말 못말려
	buzzer_on(F5);
	HAL_Delay(180);
	buzzer_off();
	buzzer_on(F5);
	HAL_Delay(180);
	buzzer_off();
	buzzer_on(E5);
	HAL_Delay(180);
	buzzer_off();
	buzzer_on(D5);
	HAL_Delay(180);
	buzzer_off();
	buzzer_on(C5);
	HAL_Delay(1100);
	buzzer_off();

}
