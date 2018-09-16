/*
 * I2C.cpp
 *
 *  Created on: 13 wrz 2018
 *      Author: lechu
 */

#include <I2C.h>

namespace STM32F4xx {

//I2C::~I2C() {
//	// TODO Auto-generated destructor stub
//}


I2C i2cInstance = I2C();

I2C * I2C::getInstance(){ return &i2cInstance; }

///**
// * Aktywny delay - ale musi byc uruchomiony Systick z QuickTask
// */
//void I2C::dirtyDelayMs(uint32_t miliseconds){
//	volatile uint32_t tmp = QuickTask::getCounter();
//	while(QuickTask::getCounter() - tmp < miliseconds ){
//		cyclicJob();
//	}
//}
// ****************************************************************

uint8_t buffer[I2C::MAX_DATA_SIZE];
Fifo dataFrame = Fifo(buffer, I2C::MAX_DATA_SIZE);


//uint8_t frameBuffer1[50];
//Fifo frame1Fifo = Fifo(frameBuffer1, 50);
//
//uint8_t frameBuffer2[50];
//Fifo frame2Fifo = Fifo(frameBuffer1, 50);
//
//uint8_t dataFifo[250];
//Fifo dataStreamFifo = Fifo(dataFifo, 250);

// ****************************************************************
bool I2C::init(DeviceDefs * initDefsPtr){
	i2c = initDefsPtr;
	if (i2c->base == nullptr) return false;
	if (initDefsPtr->sda == nullptr) return false;
	if (initDefsPtr->scl == nullptr) return false;
	if (i2c->i2cFreqkHz == 0) i2c->i2cFreqkHz = 100;
	return init();
}

bool I2C::init(){
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
		i2c->scl->setup(Gpio::GpioMode::ALTERNATE, Gpio::GpioOType::OpenDrain, Gpio::GpioPuPd::PullUp, Gpio::GpioSpeed::MaximumSpeed);
		i2c->sda->setup(Gpio::GpioMode::ALTERNATE, Gpio::GpioOType::OpenDrain, Gpio::GpioPuPd::PullUp, Gpio::GpioSpeed::MaximumSpeed);
		i2c->scl->setAlternateFunc(0x04); //AF04 - );
		i2c->sda->setAlternateFunc(0x04); //AF04 - );
		//		if (rstPin != nullptr){
		//			rstPin->setup(Gpio::GpioMode::OUTPUT, Gpio::GpioOType::PushPull, Gpio::GpioPuPd::NoPull, Gpio::GpioSpeed::MaximumSpeed);
		//			rstPin->pinSetUp();
		//		}
		//		if(bckLightPin != nullptr){
		//			bckLightPin->setup(Gpio::GpioMode::OUTPUT, Gpio::GpioOType::PushPull, Gpio::GpioPuPd::NoPull, Gpio::GpioSpeed::LowSpeed);
		//			bckLightPin->pinSetUp();
		//		}

	}

	i2c->base->CR1 &= ~I2C_CR1_PE;
	while(i2c->base->CR1 & I2C_CR1_PE){;}	// odczekanie na koniec resetu I2C


	NVIC_SetPriority(IRQn_Type::I2C1_EV_IRQn, NVIC_EncodePriority(NVIC_I2C_PRIORITY, 0,0));
	NVIC_EnableIRQ(IRQn_Type::I2C1_EV_IRQn);

	NVIC_SetPriority(IRQn_Type::I2C1_ER_IRQn, NVIC_EncodePriority(NVIC_I2C_PRIORITY, 0,0));
	NVIC_EnableIRQ(IRQn_Type::I2C1_ER_IRQn);

	uint32_t APBClock = SystemCoreClock;
	uint32_t APB_CLOCKMhz = (APBClock /1000000u) & 0b0111111;
	//APB_CLOCKMhz = (CR2 & 0b0111111) | APB_CLOCKMhz;

	i2c->base->CR2 = APB_CLOCKMhz;
	//uint16_t((APB_CLOCKMhz | I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN) & 0x0001111100111111);		// 8 dla 8MHz, 25 dla 25MHz
	i2c->base->TRISE = 1 + APB_CLOCKMhz; //0x09;	dla 8MHz, 33 dla 32MHz
	i2c->base->CCR = 160;//(APB_CLOCKMhz * 2000)/(I2C_CLK_FREQ_kHz);  //160;	//40  ;			// 100kHz
	// (((((__PCLK__)/((__SPEED__) << 1U)) & I2C_CCR_CCR) < 4U)? 4U:((__PCLK__) / ((__SPEED__) << 1U)))
	//CR1 = 0x00;
	i2c->base->OAR1 = 0x00 + (1UL<<14);
	i2c->base->OAR2 = 0x00;
	i2c->base->CR2 |=  I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN;
	i2c->base->CR1 |=  I2C_CR1_PE;
	while(!(i2c->base->CR1 & I2C_CR1_PE)){;}	// odczekanie na koniec resetu I2C


	//	if (base->SR2 & I2C_SR2_BUSY){
	//		base->CR1 |= I2C_CR1_STOP;
	//	}


	data = &dataFrame;

	setState(State::IDLE);

	return true;
}

void I2C::irqEvent(){
	//makeStamp();	// i2c cos wlasnie robi (przerwanie)
	I2C_TypeDef * base = i2c->base;
	uint32_t sr2 = 0xffff;// = base->SR2;
	uint32_t sr1 = base->SR1;
	Fifo * frame = data;
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


void I2C::irqError(){
	static uint32_t err = 0;
	err++;
	uint32_t sr1 = i2c->base->SR1;
	//uint32_t sr2 = base->SR2;

	//	if (sr1 & I2C_SR1_SMBALERT) base->SR1 &= ~I2C_SR1_SMBALERT;
	//	if (sr1 & I2C_SR1_TIMEOUT) base->SR1 &= ~I2C_SR1_TIMEOUT;
	//	if (sr1 & I2C_SR1_PECERR) base->SR1 &= ~I2C_SR1_PECERR;
	//	if (sr1 & I2C_SR1_OVR) base->SR1 &= ~I2C_SR1_OVR;

	i2c->base->SR1 &= ~(
			I2C_SR1_SMBALERT |
			I2C_SR1_TIMEOUT |
			I2C_SR1_PECERR |
			I2C_SR1_OVR |
			I2C_SR1_AF |
			I2C_SR1_ARLO |
			I2C_SR1_BERR
	);
	setState(State::IDLE);
}

bool I2C::masterTransmit(Fifo * frame){

	if (isBusy()) return false;	// sprawdzic czy mozna
	data->putAll(frame);
	setState(State::START);							// znacznik poczatku
	i2c->base->CR1 |= I2C_CR1_START;				// start transmisji i2c
	return true;
}


bool I2C::masterTransmit(uint8_t * dataBuffer, uint8_t amount){

	if (isBusy()) return false;	// sprawdzic czy mozna
	for (uint32_t i = 0; i < amount; i++){			// zaladowac do bufora
		data->put(dataBuffer[i]);
	}
	setState(State::START);							// znacznik poczatku
	i2c->base->CR1 |= I2C_CR1_START;				// start transmisji i2c
	return true;
}



void I2C1_EV_IRQHandler(void){
	i2cInstance.irqEvent();
}

void I2C1_ER_IRQHandler(void){
	i2cInstance.irqError();
}




} /* namespace STM32F4 */
