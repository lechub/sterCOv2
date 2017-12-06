/*
 * STM32F4i2c.h
 *
 *  Created on: 6 gru 2017
 *      Author: lechu
 */

#ifndef STM32F4I2C_H_
#define STM32F4I2C_H_

#include "stm32f4xx.h"
#include "Gpio.h"
#include "Fifo.h"

class STM32F4_i2c {
public:
	static constexpr uint32_t I2C_TIMEOUT_MS = 1000;
	typedef struct{
		I2C_TypeDef * base = nullptr;
		Gpio * sda;
		Gpio * scl;
		Gpio * resetPin;
		uint32_t i2cFreqkHz = 100;
	}InitDefs;

private:
	typedef enum{
		IDLE, START, ADR, DATA, STOP
	}State;

	//InitDefs * initDefs;
	I2C_TypeDef * base = nullptr;
	Gpio * sdaPin;
	Gpio * sclPin;
	Gpio * rstPin;
	uint32_t i2cFreqkHz = 100;
	Fifo * frameBuffer = nullptr;
	Fifo * dataBuffer = nullptr;
	volatile uint32_t timeStamp = 0;
	volatile uint16_t slaveAdr = 0;
	volatile State state = State::IDLE;

	bool init();
	void cyclicJob();
	bool isBusy();

public:
	static STM32F4_i2c * getInstance();

	bool init(InitDefs * initDefsPtr);

//	bool init(I2C_TypeDef * i2cBasePtr, Gpio * sdaPtr, Gpio * sclPtr){
//		base = i2cBasePtr;
//		sda = sdaPtr;
//		scl = sclPtr;
//		init();
//		return true;
//	}

	bool i2cMasterTransmit(uint16_t slaveAdres, uint8_t * buffer, uint16_t amount);

	void setResetPin(bool newstate){ rstPin->pinSet(newstate); }

	void dirtyDelayMs(uint32_t miliseconds);

	inline void poll() { cyclicJob(); }

	void irqEvent();

	void irqError();

};


#ifdef __cplusplus
extern "C" {
#endif

void I2C1_EV_IRQHandler(void) __attribute__ ((interrupt));
void I2C1_ER_IRQHandler(void) __attribute__ ((interrupt));

#ifdef __cplusplus
}
#endif

#endif /* STM32F4I2C_H_ */
