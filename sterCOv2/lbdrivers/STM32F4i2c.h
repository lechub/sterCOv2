/*
 * STM32F4i2c.h
 *
 *  Created on: 6 gru 2017
 *      Author: lechu
 */

#ifndef STM32F4I2C_H_
#define STM32F4I2C_H_

#include "Hardware.h"

class STM32F4_i2c {
	static constexpr uint32_t I2C_TIMEOUT_MS = 1000;


	typedef enum{
		IDLE, START, ADR, DATA, STOP
	}I2CSTATE;

	typedef struct{
		volatile I2C_TypeDef * instance;
		Fifo * frameBuffer;
		Fifo * dataBuffer;
		volatile uint32_t timeStamp;
		uint32_t i2cFreqkHz;
		volatile uint16_t slaveAdr = 0;
		volatile I2CSTATE state;
	}I2Cdefs;

	static I2Cdefs * i2c;


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
