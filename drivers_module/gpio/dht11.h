/*
 * dht11.h
 *
 *  Created on: Jun 25, 2025
 *      Author: Administrator
 */

#ifndef INC_DHT11_H_
#define INC_DHT11_H_

#include <gpiod.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#include "gpio.h"

#define CONSUMER "DHT11"
#define LOW 0
#define HIGH 1
#define delay_ms(ms)                            \
	do {                                        \
        for (uint32_t i = 0; i < ms; ++i) {     \
            usleep(1000);                     \
        }                                       \
    } while(0)
/*
 * DHT_Out gpio1_12
 */

#define GPIO_DHT		"gpio1.12"   /*  SH_CP 	*/

uint8_t start_dht();
uint8_t CheckResponse();
uint8_t Read_DHT();
uint8_t CheckSum(uint8_t *data);

#endif /* INC_DHT11_H_ */
