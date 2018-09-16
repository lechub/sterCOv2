/*
 * STM32F4i2c.h
 *
 *  Created on: 6 gru 2017
 *      Author: lechu
 */

#ifndef STM32F4I2C_H_
#define STM32F4I2C_H_

#include <stdint.h>
#include "stm32f4xx.h"
#include "Gpio.h"
#include "Fifo.h"

class STM32F4_i2c {
public:
	static constexpr uint32_t TIMEOUT_MS = 1000;
	static constexpr uint32_t COMMAND_MIN_DELAY_MS = 8;
	typedef struct{
		I2C_TypeDef * base = nullptr;
		Gpio * sda;
		Gpio * scl;
		Gpio * resetPin;
		Gpio * backLight;
		uint32_t i2cFreqkHz = 100;
	}InitDefs;

private:
	static constexpr uint8_t I2C_BLOCK_START = 0xfe;

	typedef enum{
		IDLE, START, ADR, DATA, STOP
	}State;

	//InitDefs * initDefs;
	I2C_TypeDef * base = nullptr;
	Gpio * sdaPin;
	Gpio * sclPin;
	Gpio * rstPin;
	Gpio * bckLightPin;
	uint32_t i2cFreqkHz = 100;
	Fifo * frameIrq = nullptr;
	Fifo * frame1 = nullptr;
	//Fifo * frame2 = nullptr;
	bool frameLoading = false;
	Fifo * dataStream = nullptr;
	volatile uint32_t timeStamp = 0;
	volatile uint16_t slaveAdr = 0;
	volatile State state = State::IDLE;

	void setState(State newstate){
		if (newstate == State::IDLE){
			state = State::IDLE;
		}else{
			state = newstate;
		}
	}

	bool init();
	void cyclicJob();
	//bool isBusy();
	void makeStamp();

public:
	static STM32F4_i2c * getInstance();

	bool init(InitDefs * initDefsPtr);

	void setSlaveAdres(uint16_t slaveAdres) {slaveAdr = slaveAdres; }

	bool masterTransmit(uint8_t * buffer, uint8_t amount);
	bool masterTransmit(uint16_t slaveAdres, uint8_t * buffer, uint8_t amount){
		setSlaveAdres(slaveAdres);
		return masterTransmit(buffer, amount);
	}

	void setResetPin(bool newstate){ rstPin->pinSet(newstate); }
	void setBackLight(bool newstate){ bckLightPin->pinSet(newstate); }

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
