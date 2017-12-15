/*
 * st7032i.h
 *
 *  Created on: 29.09.2016
 *      Author: lechu
 */


// ------------------------- Head file for st7032i.c ---------------------------
#ifndef ST7032I_H_
#define ST7032I_H_

#include <MyI2C.h>
#include <stdint.h>
#include "Hardware.h"

#define LCD5V 1		//

typedef enum{
	NONE,
	BLINK,
	LINE,
}CURSOR;

class ST7032I{

private:
	// ---------------------- Constant Definition ----------------------------------
	static constexpr  uint16_t ST7032I_ADDRESS = 0x3E;

	static constexpr  uint8_t LCD_CLR		= 0x01;		// LCD Return home
	static constexpr  uint8_t LCD_HOME		= 0x02;		// LCD Return home
	static constexpr  uint8_t LCD_ENTRY 	= 0x06;		// Set LCD Entry Mode
	static constexpr  uint8_t LCD_C2L		= 0b010000;		// Move Cursor to the left
	static constexpr  uint8_t LCD_C2R		= 0b010100;		// Move Cursor to the right
	static constexpr  uint8_t LCD_D2L		= 0x18;		// Move display to the left
	static constexpr  uint8_t LCD_D2R		= 0x1C;		// Move display to the right

	static constexpr  uint8_t LCD_ON		= 0x0E;		// Turn on LCD and Cursor
	static constexpr  uint8_t LCD_OFF		= 0x08;		// Turn off LCD
	static constexpr  uint8_t CURSOR_LINE	= 0b01110;		// Turn on cursor - line
	static constexpr  uint8_t CURSOR_BLINK		= 0b01111;		// Turn on cursor - blink
	static constexpr  uint8_t CURSOR_INVISIBLE	= 0b01100;		// Turn off cursor

	static constexpr  uint8_t LCD_CGRAM_ADDR = 0x40;	// Start address of LCD CGRAM
	static constexpr  uint8_t LCD_CGMAX 	= 64;			// Max CGRAM bytes

	static constexpr  uint8_t LCD_COL 	= 16;
	static constexpr  uint8_t LCD_ROW 	= 2;
	static constexpr  uint8_t LCD_CHAR	= LCD_COL*LCD_ROW;

	static constexpr  uint8_t LCD_DDRAM_WRITE	= 0x80;
	static constexpr  uint8_t LCD_L1	= 0x00;	//0x80;
	static constexpr  uint8_t LCD_L2	= 0x40;	//C0;
	static constexpr  uint8_t LCD_L3	= 0x10;
	static constexpr  uint8_t LCD_L4	= 0x50;

	static constexpr uint8_t CMD = 0x00;
	static constexpr uint8_t DATA = 0b01000000;

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


	bool sendCommand(uint8_t cmd);
	bool sendData(uint8_t cmd);
	bool sendCmdOrData(bool isCMD, uint8_t byteValue);

	inline void delayMs(uint32_t milis){ i2c->dirtyDelayMs(milis); }

	MyI2C * i2c = nullptr;

public:
	//	ST7032I(I2C_HandleTypeDef * i2cHandle){ hi2c = i2cHandle; };

	ST7032I(){;}
//	ST7032I(I2C_HandleTypeDef * i2cPtr){ init(i2cPtr); }
//	void init(I2C_HandleTypeDef * i2cPtr){
//		i2c = i2cPtr;
//		init();
//	}


	void init(MyI2C * i2cPtr);

	void poll(){ i2c->poll(); }

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

	bool cursorSet(CURSOR cursorType){
		uint8_t cmmd = CURSOR_INVISIBLE;
		switch(cursorType){
		case CURSOR::LINE: cmmd = CURSOR_LINE; break;
		case CURSOR::BLINK: cmmd = CURSOR_BLINK; break;
		case CURSOR::NONE:
		default: break;
		}
		sendCommand(cmmd);
		//delayMs(5);
		return true;
	}

};

#endif /* ST7032I_H_ */
