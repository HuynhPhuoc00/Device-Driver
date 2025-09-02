/*
 * lcd.c
 *
 *  Created on: Jun 20, 2025
 *      Author: Administrator
 */

#include "lcd_i2c_drv.h"

struct i2c_client *client_for_drv;

void lcd_init(struct i2c_client *client){
	client_for_drv = client;
	msleep(10);
	lcd_send_cmd (0x33); /* set 4-bits interface */
	lcd_send_cmd (0x32);msleep(1);
//    msleep(1); i2c_send_cmd_lcd (0x20);  // 4bit mode
	msleep(1); lcd_send_cmd (0x28); // Function set --> DL=0 (4 bit mode), N = 1 (2 line display) F = 0 (5x8 characters)

	msleep(1);
	lcd_send_cmd(0x01); msleep(1); // clear display
	lcd_send_cmd(0x06); msleep(2); // entry mode
	lcd_send_cmd(0x0C); msleep(1); // set on display
	lcd_send_cmd(0x02); msleep(1); // move cursor to home

	lcd_clear();

	// i2c_lcd_put_cur(0,0);
	// i2c_send_string("Init LCD1"); // Test

	// i2c_lcd_put_cur(1,0);
	// i2c_send_string("Init LCD2"); // Test

	msleep(500);
	// i2c_lcd_clear();
}


void lcd_send_cmd(char data){
	char data_u, data_l;
	uint8_t data_t[4];
	data_u = (data&0xf0);
	data_l = ((data<<4)&0xf0);
	data_t[0] = data_u|0x0C;  //en=1, rs=0
	data_t[1] = data_u|0x08;  //en=0, rs=0s
	data_t[2] = data_l|0x0C;  //en=1, rs=0
	data_t[3] = data_l|0x08;  //en=0, rs=0
	i2c_master_send(client_for_drv, data_t, 4);
}

void lcd_send_data(char data){
	char data_u, data_l;
	uint8_t data_t[4];
	data_u = (data&0xf0);
	data_l = ((data<<4)&0xf0);
	data_t[0] = data_u|0x0D;  //en=1, rs=1
	data_t[1] = data_u|0x09;  //en=0, rs=1
	data_t[2] = data_l|0x0D;  //en=1, rs=1
	data_t[3] = data_l|0x09;  //en=0, rs=1
	i2c_master_send(client_for_drv, data_t, 4);
}

void lcd_send_string(const char *data){
	while (*data){
		lcd_send_data(*data++);
	}
}

void lcd_clear(void){
	lcd_send_cmd(0x01);msleep(2);
}

void lcd_put_cur(int row, int col){
    uint8_t addr = (row == 0) ? (0x00 + col) : (0x40 + col);
	pr_info("lcd1602: put_cur row=%d col=%d -> cmd=0x%02X\n", row, col, 0x80|addr);
    lcd_send_cmd(0x80 | addr);
    // i2c_send_cmd_lcd (col);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("LCD");
MODULE_DESCRIPTION("LCD module driver");