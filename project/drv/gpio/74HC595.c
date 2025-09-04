/*
 * 74HC595.c
 *
 *  Created on: Jun 5, 2025
 *      Author: Administrator
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <unistd.h>

#include "74HC595.h"
#include "gpio.h"

int init_gpios(void)
{
	gpio_configure_dir(GPIO_SH_CP, GPIO_DIR_OUT);
	gpio_configure_dir(GPIO_DS0, GPIO_DIR_OUT);
	gpio_configure_dir(GPIO_ST_CP, GPIO_DIR_OUT);

	gpio_write_value(GPIO_SH_CP,GPIO_LOW_VALUE);
	gpio_write_value(GPIO_DS0,GPIO_LOW_VALUE);
	gpio_write_value(GPIO_ST_CP,GPIO_LOW_VALUE);

	return 0;

}

void Clk_En(){
	gpio_write_value(GPIO_SH_CP, HIGH_VALUE);
	usleep(1000);
	gpio_write_value(GPIO_SH_CP, LOW_VALUE);
	usleep(1000);
}

void Write_data(uint8_t data, uint8_t MSBorLSB){
	if (MSBorLSB == LSB){
		for(int i = 7; i >= 0; i--){
			if((data >> i) & 0x01){
				gpio_write_value(GPIO_DS0, HIGH_VALUE);
			}else{
				gpio_write_value(GPIO_DS0, LOW_VALUE);
			}
			Clk_En();
		}
		Latch_En();
	}
	else if (MSBorLSB == MSB){
		for(int i = 0; i < 8; i++){
			if((data >> i) & 0x01){
				gpio_write_value(GPIO_DS0, HIGH_VALUE);
			}else{
				gpio_write_value(GPIO_DS0, LOW_VALUE);
			}
			Clk_En();
		}
		Latch_En();
	}
}

void Latch_En(){
	gpio_write_value(GPIO_ST_CP, HIGH_VALUE);
	usleep(5000);
	gpio_write_value(GPIO_ST_CP, LOW_VALUE);
	usleep(5000);
}