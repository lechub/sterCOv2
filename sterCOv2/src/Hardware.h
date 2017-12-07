/*
 * Hardware.h
 *
 *  Created on: 28.06.2017
 *      Author: lechu
 */

#ifndef HARDWARE_H_
#define HARDWARE_H_

#include "stm32f4xx.h"
#include "STM32F4i2c.h"

#include "Fifo.h"

class Hardware {

public:

	static constexpr uint32_t nvicPriority = 0x03U;
	static constexpr uint32_t SYSTICK_FREQUENCY_HZ = 1000;

	typedef enum{
		OK, Incident, Failure,
	}ErrorCode;


	Hardware(){	}

	static void init();

	static uint32_t getTickMs();
	static void delayMs(uint32_t milis);

	static void WDOG_Init();
	static void WDOG_Reload();
	static void NVIC_init();

	//static inline void I2C_Init(){ i2c_init(i2c, I2C_TIMEOUT_MS, getTickMs); }

//	static bool I2C_Init(I2C_TypeDef *i2cPtr, uint32_t Timeout);
//	inline static bool I2C_MasterTransmit(uint16_t DevAddress, uint8_t *pData, uint16_t Size){
//			return I2C_Result::I2C_OK == i2c_MasterTransmit(DevAddress, pData, Size);
//	}

//	static bool i2cMasterTransmit(uint16_t slaveAdres, uint8_t * buffer, uint16_t amount);
//	static bool i2cMasterTransmit(uint16_t slaveAdres, uint8_t * buffer, uint16_t amount, uint32_t timeout);
//	static void i2cEvent();
//	static void i2cError();
//
//	static void i2cTimerJob();

	static void errorDispatch(ErrorCode errCode){
		switch(errCode){
		case Failure:
			while (true){;}// ToDo mruganie dioda
			break;
		case Incident:
		case OK:
			// Todo zasygnalizowanie
			break;
		}
	}

	//static void lcdPinReset(bool newstate);

private:
	static void gpioInit();
	static void adcInit();
//	static void i2cInit();

//	static bool	waitUntilFlagState(__IO uint32_t *regPtr, uint32_t flag, bool unlockState, uint32_t timeoutMs);
//
//	static bool I2C_WaitNotBusyToTimeout();
//	static I2C_TypeDef * i2c;
//	static uint32_t i2cTimeout;
};

#ifdef __cplusplus
 extern "C" {
#endif

void I2C1_EV_IRQHandler(void) __attribute__ ((interrupt));
void I2C1_ER_IRQHandler(void) __attribute__ ((interrupt));
void DMA1_Stream0_IRQHandler(void) __attribute__ ((interrupt));
void ADC_IRQHandler(void) __attribute__ ((interrupt));

#ifdef __cplusplus
 }
#endif

#endif /* HARDWARE_H_ */
