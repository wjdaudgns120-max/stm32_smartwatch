/*
 * temp_humid.c
 *
 *  Created on: 2026. 10. 28.
 *      Author: kccistc
 */

#include "temp_humid.h"

static dht11_data_t TH_data;
static dht11_status_t TH_status = DHT11_ERR_NO_RESPONSE;
static bool TH_measured = false; // button 2 로 한번이라도 측정 했는지

#define dht_port	GPIOC
#define dht_pin		GPIO_PIN_8

void TH_init(void)
{
	dht11_init(dht_port, dht_pin);
}

// 기존 2초 주기 측정 => button 2 를 눌렀을 때 1회 측정
dht11_status_t TH_measure(void)
{
	dht11_data_t data;

	TH_status = dht11_read(&data);
	TH_measured = true;
	if (TH_status == DHT11_OK) {
		TH_data = data; // 정상일 때만 갱신
	}
	return TH_status;
}

bool TH_isMeasured(void)
{
	return TH_measured;
}

dht11_status_t TH_getStatus(void)
{
	return TH_status;
}

void TH_getTempHumid(dht11_data_t *tempHumid)
{
	*tempHumid = TH_data;
}
