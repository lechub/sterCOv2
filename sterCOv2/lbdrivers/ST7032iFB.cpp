/*
 * st7032ifb.cpp
 *
 *  Created on: 12 wrz 2018
 *      Author: lechu
 */

#include <ST7032iFB.h>

ST7032iFB::~ST7032iFB() {
	// TODO Auto-generated destructor stub
}

// Functions for ST7032I


bool ST7032iFB::sendCmdOrData(bool isCMD, uint8_t byteValue){
	uint8_t buf[2];
	buf[0] = isCMD ? CMD : DATA;
	buf[1] = byteValue;
	bool ret = i2c->masterTransmit(buf, 2);
	return ret;
}

bool ST7032iFB::sendCommand(uint8_t cmd){
	return sendCmdOrData(true, cmd);
}

bool ST7032iFB::sendData(uint8_t data){
	return sendCmdOrData(false, data);
}


// LCD initialization procedure
void ST7032iFB::init(STM32F4_i2c * i2cPtr){
	i2c = i2cPtr;
	i2c->setSlaveAdres(ST7032I_ADDRESS);
	//Hardware::i2cInit();
	i2c->setResetPin(false);
	delayMs(10);
	i2c->setResetPin(true);
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


bool ST7032iFB::print(char znak){	return sendData(znak); }

bool ST7032iFB::print(const char * str){
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
	bool ret = i2c->masterTransmit(buf, (uint16_t)(i));
	return ret;
}

bool ST7032iFB::gotoXY(uint8_t Row, uint8_t Col){
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
bool ST7032iFB::clearScreen(void){
	bool result = sendCommand(LCD_CLR);
	//delayMs(5);
	// 2ms delay is Necessary after sending LCD_CLS command !!!
	return result;
}

bool ST7032iFB::lcd_ON(){
	sendCommand(LCD_ON);
	//delayMs(10);
	return true;
}

bool ST7032iFB::lcd_OFF(){
	sendCommand(LCD_OFF) ;
	//delayMs(10);
	return true;
}






void ST7032iFB::irq_job(){
	switch(lcdStage){
	case LcdStage::WAIT_START: break;
	case LcdStage::LINE1_SEND: break;
	case LcdStage::WAIT1: break;
	case LcdStage::LINE2_SEND: break;
	case LcdStage::WAIT2: break;
	case LcdStage::CURSOR_SET: break;
	case LcdStage::WAIT3: break;
	default: break;
	}

}
