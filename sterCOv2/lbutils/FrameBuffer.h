/*
 * FrameBuffer.h
 *
 *  Created on: 11 wrz 2018
 *      Author: lechu
 */

#ifndef FRAMEBUFFER_H_
#define FRAMEBUFFER_H_

#include <stdint.h>

class FrameBuffer {
public:

	typedef enum {
		HIDDEN, BLINK, SOLID, DASH} CursorMode;

private:
	uint32_t	cursorPosition;	// 0=lewy gorny, kolejne to
	CursorMode	cursor;

	uint32_t COLUMNS;// = 16;
	uint32_t ROWS;// = 2;
	uint32_t BUFFERSIZE;// = ROWS*COLUMNS;

	//uint8_t bytes[COLUMNS][ROWS];
	uint8_t * bytes;

	void checkCursorPosition(){
		cursorPosition = cursorPosition % BUFFERSIZE;
	}


public:

	FrameBuffer(uint32_t maxColumns, uint32_t maxRows, uint8_t *buffer) {
		COLUMNS = maxColumns;
		ROWS = maxRows;
		bytes = buffer;
		BUFFERSIZE = ROWS*COLUMNS;
		cursorPosition = 0;
		cursor = CursorMode::HIDDEN;
	}

	virtual ~FrameBuffer();

	uint32_t getROWS(){return ROWS;}
	uint32_t getCOLUMNS(){return COLUMNS;}

	uint8_t * getBuffer(){ return bytes;}

	void cursorRight(void){
		if (++cursorPosition >= BUFFERSIZE) cursorPosition = 0;
	}

	void cursorLeft(void){
		cursorPosition = (cursorPosition == 0) ? BUFFERSIZE -1 : cursorPosition -1;
	}

	bool gotoXY(uint32_t col, uint32_t row){
		if (row >= ROWS) return false;
		if (col >= COLUMNS) return false;
		cursorPosition = row * COLUMNS + col;
		return true;
	}

	void print(char znak){
		checkCursorPosition();
		*(bytes + cursorPosition) = znak;
		cursorRight();
	}

	void print(const char * str){
		while(*str != '\0'){
			print(*str);
		}
	}

	void printXY(const char * str, uint32_t col, uint32_t row){
		gotoXY(col, row);
		print(str);
	}

	void clearScreen(){
		for (uint32_t i = 0; i < BUFFERSIZE; i++)
			*(bytes + cursorPosition) = ' ';
	}

	void homeScreen(void){
		cursorPosition = 0;
	}


	void cursorMode(CursorMode mode){
		cursor = mode;
	}
};

#endif /* FRAMEBUFFER_H_ */
