/*
 * temp_humid.h
 *
 *  Created on: 2026. 10. 28.
 *      Author: kccistc
 */

#ifndef SRC_AP_SERVICE_TEMP_HUMID_TEMP_HUMID_H_
#define SRC_AP_SERVICE_TEMP_HUMID_TEMP_HUMID_H_

#include "../../../driver/dht11/DHT11.h"
#include <stdbool.h>

void TH_init(void);
dht11_status_t TH_measure(void);
dht11_status_t TH_getStatus(void);
bool TH_isMeasured(void);
void TH_getTempHumid(dht11_data_t *tempHumid);

#endif /* SRC_AP_SERVICE_TEMP_HUMID_TEMP_HUMID_H_ */
