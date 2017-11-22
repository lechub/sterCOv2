/*
 * Button.h
 *
 *  Created on: 15.09.2016
 *      Author: lechu
 */

#ifndef BUTTON_H_
#define BUTTON_H_

#include "Gpio.h"

class Button {

public:
	typedef enum { ACTIVE_HIGH, ACTIVE_LOW } ActiveType;

private:
	Gpio * gpio = nullptr;
	uint16_t id;
	ActiveType activeType = ActiveType::ACTIVE_HIGH;
public:

	Button(uint32_t keyID, Gpio * keyGpio, ActiveType activeTypeInit) {
		id = (uint16_t)keyID;
		setup(keyGpio, activeTypeInit);
//		gpio = keyGpio;
//		setActiveType(activeTypeInit);
	}

	Button(uint16_t keyID){
		id = keyID;
	}

	void setup(Gpio * keyGpio, ActiveType activeTypeInit){
		gpio = keyGpio;
		setActiveType(activeTypeInit);
	}

	void setActiveType(ActiveType newActiveType){
		activeType = newActiveType;
		if (gpio == nullptr) return;
		if (activeType == ACTIVE_LOW)
			gpio->setup(Gpio::GpioMode::INPUT, Gpio::GpioOType::PushPull, Gpio::GpioPuPd::PullUp, Gpio::GpioSpeed::MediumSpeed);
		else
			gpio->setup(Gpio::GpioMode::INPUT, Gpio::GpioOType::PushPull, Gpio::GpioPuPd::PullDown, Gpio::GpioSpeed::MediumSpeed);
	}



	bool isPressed(){
		if (gpio == nullptr) return false;
		return ((activeType == ACTIVE_LOW) xor (gpio->getInputValue()));
	}

	uint16_t getId() const{ return id; }

	ActiveType getActiveType() const { return activeType; }

};

#endif /* BUTTON_H_ */
