/*
 * st7032ifb.h
 *
 *  Created on: 12 wrz 2018
 *      Author: lechu
 */

#ifndef ST7032IFB_H_
#define ST7032IFB_H_

#include <stdint.h>
#include "Hardware.h"
//#include "STM32F4i2c.h"
#include "FrameBuffer.h"

//using STM32F4xx;

#include "I2C.h"



#define LCD5V 1		//


class ST7032iFB {
public:

	// ---------------------- Constant Definition ----------------------------------
	static constexpr  uint16_t ST7032I_ADDRESS = 0x3E;

	typedef enum {
		LCD_CLR		= 0x01,		// LCD Return home
		LCD_HOME		= 0x02,		// LCD Return home
		LCD_ENTRY 	= 0x06,		// Set LCD Entry Mode
		LCD_C2L		= 0b010000,		// Move Cursor to the left
		LCD_C2R		= 0b010100,		// Move Cursor to the right
		LCD_D2L		= 0x18,		// Move display to the left
		LCD_D2R		= 0x1C,		// Move display to the right

		LCD_ON		= 0x0E,		// Turn on LCD and Cursor
		LCD_OFF		= 0x08,		// Turn off LCD
		CURSOR_LINE	= 0b01110,		// Turn on cursor - line
		CURSOR_BLINK		= 0b01111,		// Turn on cursor - blink
		CURSOR_INVISIBLE	= 0b01100,		// Turn off cursor
	}LcdCommand;

//	static constexpr  uint8_t LCD_CLR		= 0x01;		// LCD Return home
//	static constexpr  uint8_t LCD_HOME		= 0x02;		// LCD Return home
//	static constexpr  uint8_t LCD_ENTRY 	= 0x06;		// Set LCD Entry Mode
//	static constexpr  uint8_t LCD_C2L		= 0b010000;		// Move Cursor to the left
//	static constexpr  uint8_t LCD_C2R		= 0b010100;		// Move Cursor to the right
//	static constexpr  uint8_t LCD_D2L		= 0x18;		// Move display to the left
//	static constexpr  uint8_t LCD_D2R		= 0x1C;		// Move display to the right
//
//	static constexpr  uint8_t LCD_ON		= 0x0E;		// Turn on LCD and Cursor
//	static constexpr  uint8_t LCD_OFF		= 0x08;		// Turn off LCD
//	static constexpr  uint8_t CURSOR_LINE	= 0b01110;		// Turn on cursor - line
//	static constexpr  uint8_t CURSOR_BLINK		= 0b01111;		// Turn on cursor - blink
//	static constexpr  uint8_t CURSOR_INVISIBLE	= 0b01100;		// Turn off cursor
//
	static constexpr  uint8_t LCD_CGRAM_ADDR = 0x40;	// Start address of LCD CGRAM
	static constexpr  uint8_t LCD_CGMAX 	= 64;			// Max CGRAM bytes

	static constexpr  uint8_t LCD_COL 	= 16;
	static constexpr  uint8_t LCD_ROW 	= 2;
	static constexpr  uint8_t LCD_CHARS_AMOUNT	= LCD_COL*LCD_ROW;

	static constexpr  uint8_t LCD_DDRAM_WRITE	= 0x80;
	static constexpr  uint8_t LCD_L1	= 0x00;	//0x80;
	static constexpr  uint8_t LCD_L2	= 0x40;	//C0;
	static constexpr  uint8_t LCD_L3	= 0x10;
	static constexpr  uint8_t LCD_L4	= 0x50;

	static constexpr uint8_t CMD = 0x00;
	static constexpr uint8_t DATA = 0b01000000;

	FrameBuffer * frameBuffer;

	//	static constexpr uint8_t CGRAM[LCD_CGMAX] =
	//	{
	//			// Make eight patterns of 8*5 font
	//			0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,		// 1.Dsiplay All
	//			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,		// 2.White Board
	//			0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,		// 3.Virtical 1
	//			0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,		// 4.Virtical 2
	//			0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,		// 5.Horizon 1
	//			0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,		// 6.Horizon 2
	//			0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,		// 7.Stars
	//			0xFF,0x11,0x11,0x11,0x11,0x11,0x11,0xFF,		// 8.Rectangle
	//	};
	// -----------------------------------------------------------------------------



private:
	typedef enum{
		WAIT_START,
		LINE1_SEND,
		WAIT1,
		LINE2_SEND,
		WAIT2,
		CURSOR_POS_SET,
		WAIT3,
		CURSOR_MODE_SET,
		WAIT4,
	}LcdStage;

	LcdStage lcdStage;

	bool sendCommand(uint8_t cmd);
	bool sendData(uint8_t cmd);
	bool sendCmdOrData(bool isCMD, uint8_t byteValue);

	//inline void delayMs(uint32_t milis){ i2c->dirtyDelayMs(milis); }

	STM32F4xx::I2C * i2c = nullptr;

public:

	ST7032iFB() {

	}
	virtual ~ST7032iFB();

	void init(STM32F4xx::I2C * i2cPort);

	//void poll(){ i2c->poll(); }

	bool gotoXY(uint8_t Row, uint8_t Col);
	bool print(char znak);
	bool print(const char * str);
	bool clearScreen(void);
	bool homeScreen(void){
		sendCommand(0b00000010);
		delayMs(5);
		return true;
	}

	bool lcd_ON();
	bool lcd_OFF();

	bool cursorLeft(void){
		sendCommand(LCD_C2L);
		//delayMs(5);
		return true;
	}

	bool cursorRight(void){
		sendCommand(LCD_C2R);
		//delayMs(5);
		return true;
	}

	bool cursorSet(FrameBuffer::CursorMode cursorMode){
		uint8_t cmmd = CURSOR_INVISIBLE;
		switch(cursorMode){
		case FrameBuffer::CursorMode::DASH: cmmd = CURSOR_LINE; break;
		case FrameBuffer::CursorMode::SOLID: cmmd = CURSOR_LINE; break;
		case FrameBuffer::CursorMode::BLINK: cmmd = CURSOR_BLINK; break;
		case FrameBuffer::CursorMode::HIDDEN:
		default: break;
		}
		sendCommand(cmmd);
		//delayMs(5);
		return true;
	}


	bool sendCommand(LcdCommand cmd);
	bool sendByte(uint8_t dataByte);
	bool sendLine(uint32_t lineNr);
	void process();	// wywolywac cyklicznie

};

#endif /* ST7032IFB_H_ */
