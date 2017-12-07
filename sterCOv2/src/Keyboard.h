/*
 * Keyboard.h
 *
 *  Created on: 15.09.2016
 *      Author: lechu
 */


#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#include "Button.h"
#include "Fifo.h"
#include "QuickTask.h"
#include "Impulsator.h"
#include "Gpio.h"



class Keyboard {

public:
	static constexpr uint16_t BUFFERSIZE = 4;	// rozmiar bufora znakow
	static constexpr uint32_t TIME_PERIOD_KEYB_MS = 10;

	typedef enum{
		NONE = 0,
		ENTER, CANCEL,
		RIGHT, LEFT,
	}Key;


private:
	// bufor znakow - od najstarszego na najmlodszego. Brak znaku to 'Key::NONE'
	uint8_t buffer[BUFFERSIZE];
	Fifo fifo = Fifo(buffer, BUFFERSIZE);

	/**
	 * kasuje ostatni znak, przesuwa wszystkie o jeden i wpisuje najnowszy na koniec
	 */
	void addToBuffer(Key key){
		if (key == NONE) return;
		fifo.put((uint8_t)key);
	}

	Key getFromBuffer(){
		while(fifo.isNotEmpty()){
			Key result = (Key)fifo.get();
			if (result != Key::NONE) return result;	// pomijac przypadkowe NONE az bedzie wlasciwy klawisz
		}
		return Key::NONE;
	}

	/*
SCL	PB6
SDA	PB7
I2C_RST	PC7
Key1	PC6
Key2	PC5
Key3	PC8
Key4	PA12
lewo	PB8
push	PC9
prawo	PB9

	 * */
	//	Gpio gpioKey1 =  Gpio(GPIOC,6, Gpio::GpioMode::INPUT, Gpio::GpioOType::NoMatter, Gpio::GpioPuPd::PullUp, Gpio::GpioSpeed::Speed1);
	//	Gpio gpioKey2 =  Gpio(GPIOC,5, Gpio::GpioMode::INPUT, Gpio::GpioOType::NoMatter, Gpio::GpioPuPd::PullUp, Gpio::GpioSpeed::Speed1);
	//	Gpio gpioKey3 =  Gpio(GPIOC,8, Gpio::GpioMode::INPUT, Gpio::GpioOType::NoMatter, Gpio::GpioPuPd::PullUp, Gpio::GpioSpeed::Speed1);
	//	Gpio gpioKey4 =  Gpio(GPIOA,12, Gpio::GpioMode::INPUT, Gpio::GpioOType::NoMatter, Gpio::GpioPuPd::PullUp, Gpio::GpioSpeed::Speed1);
	//	Gpio gpioImpPush =  Gpio(GPIOC,9, Gpio::GpioMode::INPUT, Gpio::GpioOType::NoMatter, Gpio::GpioPuPd::PullUp, Gpio::GpioSpeed::Speed1);
	//	Gpio gpioImpLeft =  Gpio(GPIOB,8, Gpio::GpioMode::INPUT, Gpio::GpioOType::NoMatter, Gpio::GpioPuPd::PullUp, Gpio::GpioSpeed::Speed1);
	//	Gpio gpioImpRight =  Gpio(GPIOB,9, Gpio::GpioMode::INPUT, Gpio::GpioOType::NoMatter, Gpio::GpioPuPd::PullUp, Gpio::GpioSpeed::Speed1);

	Gpio *key1;// =  Gpio(GPIOC,6, Gpio::GpioMode::INPUT, Gpio::GpioOType::NoMatter, Gpio::GpioPuPd::PullUp, Gpio::GpioSpeed::Speed1);
	Gpio *key2;// =  Gpio(GPIOC,5, Gpio::GpioMode::INPUT, Gpio::GpioOType::NoMatter, Gpio::GpioPuPd::PullUp, Gpio::GpioSpeed::Speed1);
	Gpio *key3;// =  Gpio(GPIOC,8, Gpio::GpioMode::INPUT, Gpio::GpioOType::NoMatter, Gpio::GpioPuPd::PullUp, Gpio::GpioSpeed::Speed1);
	Gpio *key4;// =  Gpio(GPIOA,12, Gpio::GpioMode::INPUT, Gpio::GpioOType::NoMatter, Gpio::GpioPuPd::PullUp, Gpio::GpioSpeed::Speed1);
	Gpio *impPush;// =  Gpio(GPIOC,9, Gpio::GpioMode::INPUT, Gpio::GpioOType::NoMatter, Gpio::GpioPuPd::PullUp, Gpio::GpioSpeed::Speed1);
	Gpio *impLeft;// =  Gpio(GPIOB,8, Gpio::GpioMode::INPUT, Gpio::GpioOType::NoMatter, Gpio::GpioPuPd::PullUp, Gpio::GpioSpeed::Speed1);
	Gpio *impRight;// =  Gpio(GPIOB,9, Gpio::GpioMode::INPUT, Gpio::GpioOType::NoMatter, Gpio::GpioPuPd::PullUp, Gpio::GpioSpeed::Speed1);


	Button buttonPush = Button(0x01);	// = Button(0x01, impPush, Button::ActiveType::ACTIVE_LOW);

	Impulsator imp = Impulsator(); // = Impulsator(&gpioImpLeft, &gpioImpRight, &buttonPush);

	// unbuffered key state
	Key getPressedKey(){
		if (!key1->getInputValue()) return Key::RIGHT;
		if (!key2->getInputValue()) return Key::ENTER;
		if (!key3->getInputValue()) return Key::CANCEL;
		if (!key4->getInputValue()) return Key::LEFT;
		return Key::NONE;
	}

	// Zamiana Move na Key
	Key getImpulsatorKey(){
		Impulsator::Move move = imp.getMove();
		switch(move){
		case Impulsator::Move::LEFT: return Key::LEFT;
		case Impulsator::Move::RIGHT: return Key::RIGHT;
		case Impulsator::Move::PUSH: return Key::ENTER;
		case Impulsator::Move::RELEASE: return Key::CANCEL;
		case Impulsator::Move::NONE: break;
		}
		return Key::NONE;
	}

public:
	Keyboard(Gpio *key_1, Gpio *key_2, Gpio *key_3, Gpio *key_4, Gpio *ileft, Gpio *iright, Gpio *ipush) {
		key1 = key_1;
		key2 = key_2;
		key3 = key_3;
		key4 = key_4;
		impLeft = ileft;
		impRight = iright;
		impPush = ipush;
		buttonPush.setup(impPush, Button::ActiveType::ACTIVE_LOW);
		imp.setup(impLeft, impRight, &buttonPush);

		for (uint16_t i = 0; i<BUFFERSIZE; i++)	buffer[i] = NONE;
	}


	void co10ms(){
		imp.co10ms();	// teraz impulsator
		static Key oldKey = NONE;
		Key newKey = getUnbufferedKey();
		if (oldKey != newKey){
			oldKey = newKey;
			if (newKey != NONE) addToBuffer(newKey);
		}
	}

	bool isImpulsatorPressed(){ return imp.isPressed(); }

	bool haveBufferedKey() {
		return fifo.isNotEmpty();
	}

	Key getKey(){
		return getFromBuffer();
	}

	Key getUnbufferedKey(){
		Key key = getImpulsatorKey();
		if (key != Key::NONE){
			return key;
		}
		return getPressedKey();
	}


};

extern Keyboard keys;

#endif /* KEYBOARD_H_ */
