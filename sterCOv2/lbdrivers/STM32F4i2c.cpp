/*
 * STM32F4i2c.cpp
 *
 *  Created on: 6 gru 2017
 *      Author: lechu
 */

#include <STM32F4i2c.h>
#include "QuickTask.h"
#include "Hardware.h"
#include "stm32f4xx.h"

STM32F4_i2c i2cInstance = STM32F4_i2c();

STM32F4_i2c * STM32F4_i2c::getInstance(){ return &i2cInstance; }



/**
 * Aktywny delay - ale musi byc uruchomiony Systick z QuickTask
 */
void STM32F4_i2c::dirtyDelayMs(uint32_t miliseconds){
	volatile uint32_t tmp = QuickTask::getCounter();
	while(QuickTask::getCounter() - tmp < miliseconds ){
		cyclicJob();
	}
}
// ****************************************************************


uint8_t frameBuffer1[50];
Fifo frame1Fifo = Fifo(frameBuffer1, 50);

uint8_t frameBuffer2[50];
Fifo frame2Fifo = Fifo(frameBuffer1, 50);

uint8_t dataFifo[250];
Fifo dataStreamFifo = Fifo(dataFifo, 250);

// ****************************************************************
bool STM32F4_i2c::init(InitDefs * initDefsPtr){
	base = initDefsPtr->base;
	if (base == nullptr) base = I2C1;
	if (initDefsPtr->sda == nullptr) Hardware::errorDispatch(Hardware::ErrorCode::Failure);
	if (initDefsPtr->scl == nullptr) Hardware::errorDispatch(Hardware::ErrorCode::Failure);
	sdaPin = initDefsPtr->sda;
	sclPin = initDefsPtr->scl;
	rstPin = initDefsPtr->resetPin;
	bckLightPin = initDefsPtr->backLight;
	i2cFreqkHz = (initDefsPtr->i2cFreqkHz == 0) ? 100 :  initDefsPtr->i2cFreqkHz;
	return init();
}

bool STM32F4_i2c::init(){
	// RCC
	do {
		SET_BIT(RCC->APB1ENR, RCC_APB1ENR_I2C1EN);
		/* Delay after an RCC peripheral clock enabling */
		__IO uint32_t tmpreg = READ_BIT(RCC->APB1ENR, RCC_APB1ENR_I2C1EN);
		(void)(tmpreg);
	} while(0U);


	/**I2C1 GPIO Configuration
PB6     ------> I2C1_SCL
PB7     ------> I2C1_SDA
	 */
	{ // GPIO setup
		sclPin->setup(Gpio::GpioMode::ALTERNATE, Gpio::GpioOType::OpenDrain, Gpio::GpioPuPd::PullUp, Gpio::GpioSpeed::MaximumSpeed);
		sdaPin->setup(Gpio::GpioMode::ALTERNATE, Gpio::GpioOType::OpenDrain, Gpio::GpioPuPd::PullUp, Gpio::GpioSpeed::MaximumSpeed);
		sclPin->setAlternateFunc(0x04); //AF04 - );
		sdaPin->setAlternateFunc(0x04); //AF04 - );
		if (rstPin != nullptr){
			rstPin->setup(Gpio::GpioMode::OUTPUT, Gpio::GpioOType::PushPull, Gpio::GpioPuPd::NoPull, Gpio::GpioSpeed::MaximumSpeed);
			rstPin->pinSetUp();
		}
		if(bckLightPin != nullptr){
			bckLightPin->setup(Gpio::GpioMode::OUTPUT, Gpio::GpioOType::PushPull, Gpio::GpioPuPd::NoPull, Gpio::GpioSpeed::LowSpeed);
			bckLightPin->pinSetUp();
		}

	}

	base->CR1 &= ~I2C_CR1_PE;
	while(base->CR1 & I2C_CR1_PE){;}	// odczekanie na koniec resetu I2C


	NVIC_SetPriority(IRQn_Type::I2C1_EV_IRQn, NVIC_EncodePriority(Hardware::nvicPriority, 0,0));
	NVIC_EnableIRQ(IRQn_Type::I2C1_EV_IRQn);

	NVIC_SetPriority(IRQn_Type::I2C1_ER_IRQn, NVIC_EncodePriority(Hardware::nvicPriority, 0,0));
	NVIC_EnableIRQ(IRQn_Type::I2C1_ER_IRQn);

	uint32_t APBClock = SystemCoreClock;
	uint32_t APB_CLOCKMhz = (APBClock /1000000u) & 0b0111111;
	//APB_CLOCKMhz = (CR2 & 0b0111111) | APB_CLOCKMhz;

	base->CR2 = APB_CLOCKMhz;
	//uint16_t((APB_CLOCKMhz | I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN) & 0x0001111100111111);		// 8 dla 8MHz, 25 dla 25MHz
	base->TRISE = 1 + APB_CLOCKMhz; //0x09;	dla 8MHz, 33 dla 32MHz
	base->CCR = 160;//(APB_CLOCKMhz * 2000)/(I2C_CLK_FREQ_kHz);  //160;	//40  ;			// 100kHz
	// (((((__PCLK__)/((__SPEED__) << 1U)) & I2C_CCR_CCR) < 4U)? 4U:((__PCLK__) / ((__SPEED__) << 1U)))
	//CR1 = 0x00;
	base->OAR1 = 0x00 + (1UL<<14);
	base->OAR2 = 0x00;
	base->CR2 |=  I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN;
	base->CR1 |=  I2C_CR1_PE;
	while(!(base->CR1 & I2C_CR1_PE)){;}	// odczekanie na koniec resetu I2C


	//	if (base->SR2 & I2C_SR2_BUSY){
	//		base->CR1 |= I2C_CR1_STOP;
	//	}


	frame1 = &frame1Fifo;
	//frame2 = &frame2Fifo;
	dataStream = &dataStreamFifo;

	frame1->flush();
	//frame2->flush();
	dataStream->flush();
	frameIrq = frame1;

	setState(State::IDLE);

	return true;
}

void STM32F4_i2c::irqEvent(){
	makeStamp();	// i2c cos wlasnie robi (przerwanie)
	uint32_t sr2 = 0xffff;// = base->SR2;
	uint32_t sr1 = base->SR1;
	Fifo * frame = frameIrq;
	if (sr1 & I2C_SR1_SB){
		base->CR1 |= I2C_CR1_ACK;		// ma byc ack
		base->DR = slaveAdr << 1;
		setState(State::ADR);
	}else if (sr1 & I2C_SR1_ADDR){
		sr2 = base->SR2;		// odczyt SR2 po SR1 gasi ADDR
		(void)(sr2 = -sr2);		//		UNUSED(sr2);
		if (frame->isEmpty()){				// nic do wyslania???
			base->CR1 |= I2C_CR1_STOP;
			setState(State::IDLE);
		}else{
			base->CR1 |= I2C_CR1_ACK;		// ma byc ack
			setState(State::DATA);
		}
	}else if (sr1 & I2C_SR1_BTF){
			base->CR1 |= I2C_CR1_STOP;
			setState(State::IDLE);
	}else if (sr1 & I2C_SR1_TXE){
		if (frame->isNotEmpty()){
			base->CR1 |= I2C_CR1_ACK;		// ma byc ack
			base->DR = frame->get();
			setState(State::DATA);
		}else{
			base->CR1 |= I2C_CR1_STOP;
			setState(State::STOP);
		}
	}else{
		if (!(sr2 & I2C_SR2_BUSY)){
			setState(State::IDLE);
			sr2++;
		}
	}
	sr1++;
}


void STM32F4_i2c::irqError(){
	static uint32_t err = 0;
	err++;
	uint32_t sr1 = base->SR1;
	//uint32_t sr2 = base->SR2;

	//	if (sr1 & I2C_SR1_SMBALERT) base->SR1 &= ~I2C_SR1_SMBALERT;
	//	if (sr1 & I2C_SR1_TIMEOUT) base->SR1 &= ~I2C_SR1_TIMEOUT;
	//	if (sr1 & I2C_SR1_PECERR) base->SR1 &= ~I2C_SR1_PECERR;
	//	if (sr1 & I2C_SR1_OVR) base->SR1 &= ~I2C_SR1_OVR;

	base->SR1 &= ~(
			I2C_SR1_SMBALERT |
			I2C_SR1_TIMEOUT |
			I2C_SR1_PECERR |
			I2C_SR1_OVR |
			I2C_SR1_AF |
			I2C_SR1_ARLO |
			I2C_SR1_BERR
	);
	setState(State::IDLE);
	timeStamp = QuickTask::getCounter();
}

//bool STM32F4_i2c::isBusy(){
////	uint32_t cr1 = base->CR2; // odczyt CR2, zeby nie skasowac przypadkiem ADDR ?
////	cr1 = base->CR1;
////	if (cr1 & I2C_CR1_PE){
//		return base->SR2 & I2C_SR2_BUSY;
////	}
////	return false;
//}

inline void STM32F4_i2c::makeStamp(){ timeStamp = QuickTask::getCounter(); }

void STM32F4_i2c::cyclicJob(){

	if (dataStream->isEmpty()) return;

	frameIrq = frame1;
	Fifo * frame = frameIrq;

	if (state != State::IDLE){
		if (QuickTask::getTimeIntervalMilis(timeStamp) > TIMEOUT_MS){  	// jesli Cos robi powyzej sekundy to problem
			base->CR1 &= ~I2C_CR1_PE;	// wylacz i2c		// base->CR1 |= I2C_CR1_SWRST;
			setState(State::IDLE);
		}
		return;
	}

	if (!(base->CR1 & I2C_CR1_PE)){		// jesli i2c nie je4st wlaczone, to wlacz
		base->CR1 |= I2C_CR1_PE;
	}

//	if (isBusy()){			// jesli busy
//		timeStamp = QuickTask::getCounter();
//		return;
//	}


	if (QuickTask::getTimeIntervalMilis(timeStamp) < COMMAND_MIN_DELAY_MS) return;  // 4 ms odstepu do poprzedniej transmisji/ komendy

	// jest IDLE

	while (dataStream->get() !=  I2C_BLOCK_START){
		if (dataStream->isEmpty()) return;
	}
	if (dataStream->isEmpty()) return;
	uint8_t amount = uint8_t(dataStream->get());
	for (uint8_t i = 0; i < amount; i++){
		if (dataStream->isEmpty()) return;
		frame->put(uint8_t(dataStream->get()));
	}
	setState(State::START);
	base->CR1 |= I2C_CR1_START;
}


bool STM32F4_i2c::masterTransmit(uint8_t * buffer, uint8_t amount){

	if (dataStream->countFree() < (3ul + amount)) return false; // nie zmiesci sie
	dataStream->put(I2C_BLOCK_START);
	dataStream->put(amount);
	for (uint32_t i = 0; i < amount; i++){			//if (dataStream->isFull()) return false;
		dataStream->put(buffer[i]);
	}
	if (state == State::IDLE){
		cyclicJob();
	}
	return true;
}



void I2C1_EV_IRQHandler(void){
	i2cInstance.irqEvent();
}

void I2C1_ER_IRQHandler(void){
	i2cInstance.irqError();
}

