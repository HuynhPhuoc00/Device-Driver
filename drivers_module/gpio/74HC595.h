/*
 * 74HC595.h
 *
 *  Created on: Jun 5, 2025
 *      Author: Administrator
 */

#ifndef INC_74HC595_H_
#define INC_74HC595_H_

/*****************************************************
 *	gpio2.2 ->SH_CP
 *	gpio2.7 ->DS0
 *	gpio2.8-ST_CP0
 *	PE3->DS1
 *	PE4-ST_CP1
 *	PE5->DS2
 *	PE6-ST_CP2
 *	PE7->DS3
 *	PE8-ST_CP3
 *****************************************************/
#define GPIO_SH_CP		"gpio2.2"   /*  SH_CP 	*/ 
#define GPIO_DS0		"gpio2.7"   /*  DS0 		*/
#define GPIO_ST_CP		"gpio2.8"   /*  ST_CP0	*/
// #define GPIO_LCD_D4   "gpio2.9"   /*  DS1   	*/
// #define GPIO_LCD_D5   "gpio2.10"  /*  ST_CP1    */
// #define GPIO_LCD_D6   "gpio2.11"  /*  DS2		*/
// #define GPIO_LCD_D7   "gpio2.12"  /*  ST_CP2    */

void Clk_En();
void Write_data(uint8_t data, uint8_t MSBorLSB);
void Latch_En();


enum MSBorLSB{
	LSB	= 0,
	MSB	= 1,
};

#endif /* INC_74HC595_H_ */
