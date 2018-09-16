/*
 * st7032ifb.cpp
 *
 *  Created on: 12 wrz 2018
 *      Author: lechu
 */

#include <ST7032iFB.h>



uint8_t dataTab1[ST7032iFB::LCD_CHARS_AMOUNT +5];
FrameBuffer frame_buffer = FrameBuffer(ST7032iFB::LCD_COL, ST7032iFB::LCD_ROW, dataTab1);

uint8_t dataTab2[ST7032iFB::LCD_CHARS_AMOUNT +5];
Fifo dataFifo = Fifo(dataTab2, ST7032iFB::LCD_CHARS_AMOUNT +5);




ST7032iFB::~ST7032iFB() {
	// TODO Auto-generated destructor stub
}

// Functions for ST7032I



// LCD initialization procedure
void ST7032iFB::init(STM32F4xx::I2C * i2cPort, Gpio * backLightPin, Gpio * resetLCDPin){
	frameBuffer = &frame_buffer;
	fifo = &dataFifo;
	lcdStage = LcdStage::WAIT_START;
	i2c = i2cPort;
	i2c->setSlaveAdres(ST7032I_ADDRESS);


	//Hardware::i2cInit();
	setResetPin(false);
	delayMs(50);
	setResetPin(true);
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



bool ST7032iFB::sendCmdOrData(bool isCMD, uint8_t byteValue){
	fifo->flush();
	fifo->put(isCMD ? CMD : DATA);
	fifo->put(byteValue);
	return i2c->masterTransmit(fifo);
}

bool ST7032iFB::sendCommand(uint8_t cmd){
	return sendCmdOrData(true, cmd);
}

bool ST7032iFB::sendData(uint8_t data){
	return sendCmdOrData(false, data);
}


bool ST7032iFB::sendLine(uint32_t lineNr){
	fifo->flush();
	fifo->put(DATA);
	if (!frameBuffer->getLine(lineNr, fifo)) return false;
	return i2c->masterTransmit(fifo);
}


void ST7032iFB::process(){
	switch(lcdStage){
	case LcdStage::WAIT_START: break;
	case LcdStage::LINE1_SEND:
		sendLine(1);
		break;
	case LcdStage::WAIT1: break;
	case LcdStage::LINE2_SEND:
		sendLine(2);
		break;
	case LcdStage::WAIT2: break;
	case LcdStage::CURSOR_POS_SET:

		break;
	case LcdStage::WAIT3: break;
	case LcdStage::CURSOR_MODE_SET: break;
	case LcdStage::WAIT4: break;
	default: break;
	}

}
