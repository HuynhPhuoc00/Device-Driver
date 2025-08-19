/*
 * dht11.c
 *
 *  Created on: Jun 25, 2025
 *      Author: Administrator
 */

#include "dht11.h"

uint8_t start_dht(){
	// MCU Sends out Start Signal to DHT
    gpio_configure_dir(GPIO_DHT, GPIO_DIR_OUT);
	gpio_write_value(GPIO_DHT,GPIO_LOW_VALUE);
	delay_ms(20);

	gpio_write_value(GPIO_DHT, GPIO_HIGH_VALUE);
	usleep(40);
	
	gpio_configure_dir(GPIO_DHT, GPIO_DIR_IN);

	return CheckResponse();
}

uint8_t CheckResponse(){
	if (!(gpio_read_value(GPIO_DHT))){	// Check DHT pin low
		usleep(80);
		if (!gpio_read_value(GPIO_DHT)){	// Check DHT pin high
			// printf("Check DHT pin high error \n");
			return -1;
		}
		while (gpio_read_value(GPIO_DHT));	// Wait DHT pin low
	}
	else{
		// printf("Check DHT pin low error \n");
		return -1;
	}
	return 1;
}

uint8_t Read_DHT(){
	uint8_t Data[5];
	uint64_t tmp = 0;

	for (int i = 0; i < 40; i++){
		while (!(gpio_read_value(GPIO_DHT)));
		usleep(30);
		if (gpio_read_value(GPIO_DHT)){	// data is 1
			tmp = (tmp << 1) | 0x1;
		}
		else{	// data is 0
			tmp = (tmp << 1) | 0x0;
		}
		while (gpio_read_value(GPIO_DHT));
	}

	Data[0] = (tmp >> 32)	& 0xFF;	// Int RH
	Data[1] = (tmp >> 24)	& 0xFF;	// Dec RH
	Data[2] = (tmp >> 16)	& 0xFF;	// Int T
	Data[3] = (tmp >> 8)	& 0xFF;	// Dec T
	Data[4] = tmp			& 0xFF;	// Checksum
	(void)tmp;
	// CheckSum(Data);



	//End
	while(!gpio_read_value(GPIO_DHT));
	// printf("Humidity: %d.%d %%  Temperature: %d.%d °C\n",
    //        Data[0], Data[1], Data[2], Data[3]);
	return 1;
}

uint8_t CheckSum(uint8_t *data){
	uint32_t Sum = 0;
	for(int i = 0; i < 4; i++){
		Sum += data[i];
	}
	if (Sum != data[4]){
		// printf("Check response success\r\n");
		return -1;
	} 
}
