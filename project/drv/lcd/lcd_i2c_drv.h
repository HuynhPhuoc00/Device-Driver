/*
 * lcd2004.h
 *
 *  Created on: Jun 20, 2025
 *      Author: Administrator
 */

#ifndef INC_I2C_LCD_H_
#define INC_I2C_LCD_H_

// #ifdef __cplusplus
// extern "C" {
// #endif

#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/of.h>
#include <linux/delay.h>
#include <linux/types.h>

// void i2c_Config();
void lcd_init(struct i2c_client *client);
void lcd_send_cmd(char data);
void lcd_send_data(char data);
void lcd_send_string(const char *data);
void lcd_put_cur(int row, int col);
void lcd_clear(void);

// #ifdef __cplusplus
// }
// #endif

#endif /* INC_I2C_LCD_H_ */
