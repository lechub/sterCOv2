/*
 * st7032i.c
 *
 *  Created on: 29.09.2016
 *      Author: lechu
 */

#include "st7032i.h"
//#include "stm32f4xx_hal_i2c.h"

// Functions for ST7032I


//I2C_HandleTypeDef i2c;

//bool ST7032I::sendCmdOrData(bool isCMD, uint8_t byteValue){
//	uint8_t buf[2];
//	buf[0] = isCMD ? CMD : DATA;
//	buf[1] = byteValue;
//	bool ret = Hardware::i2cMasterTransmit(ST7032I_ADDRESS, buf, 2);
//	delayMs(2);
//	return ret;
//}
bool ST7032I::sendCmdOrData(bool isCMD, uint8_t byteValue){
	uint8_t buf[2];
	buf[0] = isCMD ? CMD : DATA;
	buf[1] = byteValue;
	bool ret = Hardware::i2cMasterTransmit(ST7032I_ADDRESS, buf, 2);
	return ret;
}

bool ST7032I::sendCommand(uint8_t cmd){
	return sendCmdOrData(true, cmd);
}

bool ST7032I::sendData(uint8_t data){
	return sendCmdOrData(false, data);
}


// LCD initialization procedure
void ST7032I::init(void){
	//Hardware::i2cInit();
	Hardware::lcdPinReset(false);
	delayMs(10);
	Hardware::lcdPinReset(true);
	delayMs(10);

	sendCommand(0x38);
	delayMs(300);
	sendCommand(0x39);
	delayMs(300);

	sendCommand(0x14);
	delayMs(100);
#ifdef LCD5V
	sendCommand(0x79);
	delayMs(100);
	sendCommand(0x50);
	delayMs(100);
	sendCommand(0x6c);
	delayMs(100);
#else
	sendCommand(0x74);
	delayMs(30);
	sendCommand(0x54);
	delayMs(30);
	sendCommand(0x6f);
	delayMs(30);
#endif
	sendCommand(0x0c);
	delayMs(100);
	sendCommand(0x01);
	delayMs(100);

	sendCommand(0x38);	//cmmd table 0
	delayMs(100);
}


bool ST7032I::print(char znak){	return sendData(znak); }

bool ST7032I::print(const char * str){
	constexpr uint8_t BUFFSIZE = 20;
	uint8_t buf[BUFFSIZE];
	buf[0] = DATA;
	uint8_t i;
	for ( i = 1; i < BUFFSIZE-1 ; i++ ){
		char znak = *str++;
		buf[i] = znak;
		if (znak == '\0') break;
	}
	buf[BUFFSIZE-1] = '\0';
	bool ret = Hardware::i2cMasterTransmit(ST7032I_ADDRESS, buf, (uint16_t)(i));
	return ret;
}

bool ST7032I::gotoXY(uint8_t Row, uint8_t Col){
	switch (Row){
	case 1:
		sendCommand(LCD_DDRAM_WRITE | LCD_L2 | Col); break;
		//	case 2:
		//		sendCommand(LCD_L3 | Col); break;
		//	case 3:
		//		sendCommand(LCD_L4 | Col); break;
	case 0:
	default:
		sendCommand(LCD_DDRAM_WRITE | LCD_L1 | Col); break;
	}
	return true;
}

// Clear LCD Screen
bool ST7032I::clearScreen(void){
	bool result = sendCommand(LCD_CLR);
	//delayMs(5);
	// 2ms delay is Necessary after sending LCD_CLS command !!!
	return result;
}

bool ST7032I::lcd_ON(){
	sendCommand(LCD_ON);
	//delayMs(10);
	return true;
}

bool ST7032I::lcd_OFF(){
	sendCommand(LCD_OFF) ;
	//delayMs(10);
	return true;
}
