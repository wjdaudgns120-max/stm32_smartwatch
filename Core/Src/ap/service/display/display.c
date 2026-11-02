/*
 * display.c
 *
 *  Created on: 2026. 10. 28.
 *      Author: kccistc
 */


#include "display.h"
#include "../../../driver/lcd/lcd.h"
#include <stdio.h>
#include <string.h>

#define LCD_COLS	16
#define LCD_ROWS	2

extern I2C_HandleTypeDef hi2c1;

static char lcdLine[LCD_ROWS][LCD_COLS + 1]; // 현재 LCD 에 출력된 내용

// 내용이 바뀐 줄만 LCD 로 전송 (I2C 통신 시간 절약)
static void display_line(uint8_t row, const char *str)
{
	char buff[LCD_COLS + 1];

	snprintf(buff, sizeof(buff), "%-16s", str); // 남는 칸은 공백으로 지움
	if (strcmp(lcdLine[row], buff) == 0) {
		return;
	}
	strcpy(lcdLine[row], buff);
	Lcd_writeStringXY(row, 0, buff);
}

void display_init(void)
{
	Lcd_init(&hi2c1);
	memset(lcdLine, 0, sizeof(lcdLine));
	display_line(0, "Hello STM32!");
	display_line(1, "Smart Watch");
}

void display_timeClock(const TimeClock_t *time, const TimeDate_t *date)
{
	static const char *dayName[8] = {"---", "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
	char buff[30];
	uint8_t day = (date->day <= 7) ? date->day : 0;

	sprintf(buff, "20%02u-%02u-%02u %s", date->year, date->month, date->date, dayName[day]);
	display_line(0, buff);
	sprintf(buff, "    %02u:%02u:%02u", time->hour, time->min, time->sec);
	display_line(1, buff);
}

void display_tempHumid(dht11_status_t status, const dht11_data_t *th)
{
	char buff[30];

	display_line(0, "[Temp & Humid]");
	if (!TH_isMeasured()) {
		display_line(1, "Press BTN2");
		return;
	}
	switch (status)
	{
	case DHT11_OK:
		sprintf(buff, "T:%2u.%uC H:%2u%%", th->t_int, th->t_dec, th->h_int);
		break;
	case DHT11_ERR_CHECKSUM:
		sprintf(buff, "Checksum Error");
		break;
	case DHT11_ERR_TIMEOUT:
		sprintf(buff, "Timeout Error");
		break;
	default:
		sprintf(buff, "No Response");
		break;
	}
	display_line(1, buff);
}

void display_distance(distance_status_t status, uint16_t cm)
{
	char buff[30];

	display_line(0, "[Distance]");
	switch (status)
	{
	case DISTANCE_OK:
		sprintf(buff, "Dist: %3u cm", cm);
		break;
	case DISTANCE_IN_PROGRESS:
		sprintf(buff, "Measuring...");
		break;
	case DISTANCE_ERROR:
		sprintf(buff, "No Echo");
		break;
	default:
		sprintf(buff, "Press BTN2");
		break;
	}
	display_line(1, buff);
}

void display_stopWatch(const stopWatch_t *time, stopWatch_state_t state)
{
	char buff[30];

	sprintf(buff, "[StopWatch] %s", (state == RUN) ? "RUN" : "STP");
	display_line(0, buff);
	sprintf(buff, "  %02u:%02u:%02u.%02u", time->hour, time->min, time->sec, time->msec);
	display_line(1, buff);
}
