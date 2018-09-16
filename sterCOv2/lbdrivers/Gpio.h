/*
 * Gpio.h
 *
 *  Created on: 23 sty 2014
 *      Author: lechu
 */

#ifndef GPIO_H_
#define GPIO_H_

#include "stm32f4xx.h"
//#include "settings.h" //#include "macros.h"
/*
 * Gpio.h
 *
 *  Created on: 23 sty 2014
 *      Author: lechu
 */

//#define GPIO_bitSet(gpio,bit) (gpio->BSRR = _BV(bit))
//
//#define GPIO_bitClear(gpio,bit) (gpio->BRR = _BV(bit))
//
//#define GPIO_SetInHi(gpio,bit) {gpio->PUPDR = (0b01<<(bit*2)) | (gpio->PUPDR & (~(0b10<<(bit*2))));
//		gpio->MODER &= (~(0b11<<(bit*2)));}
//#define GPIO_SetInHiZ(gpio,bit) {gpio->PUPDR &= (~(0b11<<(bit*2)));
//		gpio->MODER &= (~(0b11<<(bit*2)));}
//#define GPIO_SetOutOD(gpio,bit) {gpio->OTYPER != _BV(bit);
//		gpio->MODER = (0b01<<(bit*2)) | (gpio->MODER & (~(0b10<<(bit*2))));}
//#define GPIO_SetOutPP(gpio,bit) {gpio->OTYPER &= (~_BV(bit));
//		gpio->MODER = (0b01<<(bit*2)) | (gpio->MODER & (~(0b10<<(bit*2))));}
//
//#define GPIO_GetPinState(gpio,bit) ((gpio->IDR) & _BV(bit))

class Gpio{


public:
	//	enum GpioMode{ //MODER
	//		INPUT 					= 0x00,
	//		OUTPUT_PP				= 0x01,
	//		ALTERNATE_FUNCTION_OD	= 0x10,
	//		ANALOG 					= 0x11,
	//		OUTPUT_OD,
	//		ALTERNATE_FUNCTION_PP,
	//	};
	enum GpioMode{ //MODER
		INPUT 			= 0b00,
		OUTPUT			= 0b01,
		ALTERNATE		= 0b10,
		ANALOG 			= 0b11,
	};

	enum GpioOType{	//OTYPER
		PushPull 	= 0b0,
		OpenDrain 	= 0b1,
		NoMatter	= 0b1
	};

	enum GpioPuPd{
		NoPull		= 0b00,
		PullUp		= 0b01,
		PullDown 	= 0b10
	};
	enum GpioSpeed{
		LowSpeed		= 0b00,	//LowSpeed
		MediumSpeed		= 0b01,
		HighSpeed		= 0b10,
		MaximumSpeed	= 0b11	//HighSpeed
	};

protected:
	GPIO_TypeDef *	gpio;
	uint16_t	pinNr;
	GpioMode	Mode;
	GpioPuPd	Pull;
	GpioSpeed	Speed;
	//Alternate

public:


	Gpio(GPIO_TypeDef * gpioDef, uint16_t pin_Nr){
		gpio = gpioDef;
		pinNr = pin_Nr;
		Mode = GpioMode::ANALOG;		//GPIO_MODE_ANALOG;
		Pull = GpioPuPd::NoPull;		//GPIO_NOPULL;
		Speed = GpioSpeed::MediumSpeed;		//GPIO_SPEED_FREQ_MEDIUM;
	}

	Gpio(GPIO_TypeDef * gpioDef, const uint16_t pin_Nr, GpioMode mode, GpioOType oType, GpioPuPd pupd, GpioSpeed speed){
		gpio = gpioDef;
		pinNr = pin_Nr;
		setMODE(mode);
		setOType(oType);
		setPullUpDn(pupd);
		setSpeed(speed);
	}


	inline void pinSetUp(){
		gpio->BSRR = (1<<pinNr);
	}
	inline void pinSetDown(){
		gpio->BSRR = (1<<(pinNr + 16l));
	}

	inline void pinSet(bool newstate){ newstate? pinSetUp() : pinSetDown(); }

	inline bool getOutputValue(){
		return (gpio->ODR & (1<<pinNr));
	}

	inline void toggleOutput(){ getOutputValue()? pinSetDown() : pinSetUp(); }


	inline bool getInputValue(){
		return (gpio->IDR & (1<<pinNr));
	}


	inline void setMODE(const GpioMode mode){
		gpio->MODER  &= ~(GPIO_MODER_MODER0 << (pinNr * 2));
		gpio->MODER |= ((uint32_t)mode << (pinNr * 2));
	}

	inline void setSpeed(const GpioSpeed speed){
		gpio->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR0 << (pinNr * 2));
		gpio->OSPEEDR |= ((uint32_t)(speed) << (pinNr * 2));
	}

	inline void setPullUpDn(const GpioPuPd pupd){
		gpio->PUPDR &= ~(GPIO_PUPDR_PUPDR0 << ((uint16_t)pinNr * 2));
		gpio->PUPDR |= (((uint32_t)pupd) << (pinNr * 2));
	}

	//SetSpeed i setOType tylko dla trybu GPIO_Mode_OUT i GPIO_Mode_AF


	/* Output mode configuration */
	inline void setOType(const GpioOType otype){
		gpio->OTYPER &= ~((GPIO_OTYPER_OT_0) << ((uint16_t)pinNr));
		gpio->OTYPER |= (uint16_t)(((uint16_t)otype) << ((uint16_t)pinNr));
	}


	void setup(GpioMode mode, GpioOType oType, GpioPuPd pupd, GpioSpeed speed){
		setMODE(mode);
		setOType(oType);
		setPullUpDn(pupd);
		setSpeed(speed);
	}


	bool setAlternateFunc(uint8_t alternateFunction){
		if (alternateFunction > 7) return false;
		constexpr uint32_t BitMask = 0b01111;
		uint32_t val = alternateFunction;
		if (pinNr < 8){
			uint8_t offset = uint8_t(pinNr * 4);
			uint32_t tmp = gpio->AFR[0];
			tmp &= ~(BitMask << offset);
			tmp |= val << offset;
			gpio->AFR[0] = tmp;
		}
		if ((pinNr > 8)&&(pinNr < 16)){
			uint8_t offset = uint8_t((pinNr - 8) * 4);
			uint32_t tmp = gpio->AFR[1];
			tmp &= ~(BitMask << offset);
			tmp |= val << offset;
			gpio->AFR[1] = tmp;
		}
		return true;
	}

//	static void setup(GPIO_TypeDef * gpioDef, const uint16_t pin_Nr, GpioMode mode, GpioOType oType, GpioPuPd pupd, GpioSpeed speed){
//
//		setMODE(mode);
//		setOType(oType);
//		setPullUpDn(pupd);
//		setSpeed(speed);
//	}

};

//----------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
//----------------------------------------------------

//void gpio_init(void);
//----------------------------------------------------
#ifdef __cplusplus
}
#endif
//----------------------------------------------------



#endif /* GPIO_H_ */
