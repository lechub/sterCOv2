/*
 * STM32F4i2c.cpp
 *
 *  Created on: 6 gru 2017
 *      Author: lechu
 */

#include <STM32F4i2c.h>


Gpio gpioSCL = Gpio(GPIOB,6);
Gpio gpioSDA = Gpio(GPIOB,7);
Gpio gpioLcdReset = Gpio(GPIOC,7);

void Hardware::delayMs(uint32_t milis){
	volatile uint32_t tmp = QuickTask::getCounter();
	while(QuickTask::getCounter() - tmp < milis ){
		i2cTimerJob();
	}
}
// ****************************************************************

//constexpr uint32_t I2C_CLK_FREQ_kHz = 100;	//
//uint8_t i2cBuffer[100];
//Fifo i2cFifo = Fifo(i2cBuffer, 100);
constexpr uint8_t I2C_BLOCK_START = 0xfe;

uint8_t i2cFrameBuffer[50];
Fifo i2cFrame = Fifo(i2cFrameBuffer, 50);

uint8_t i2cDataBuffer[200];
Fifo i2cData = Fifo(i2cDataBuffer, 200);

Hardware::I2Cdefs I2CdefsStr;
Hardware::I2Cdefs * Hardware::i2c = &I2CdefsStr;



// ****************************************************************

//constexpr uint32_t I2C_CLK_FREQ_kHz = 100;	//
//uint8_t i2cBuffer[100];
//Fifo i2cFifo = Fifo(i2cBuffer, 100);
constexpr uint8_t I2C_BLOCK_START = 0xfe;

uint8_t i2cFrameBuffer[50];
Fifo i2cFrame = Fifo(i2cFrameBuffer, 50);

uint8_t i2cDataBuffer[200];
Fifo i2cData = Fifo(i2cDataBuffer, 200);

Hardware::I2Cdefs I2CdefsStr;
Hardware::I2Cdefs * Hardware::i2c = &I2CdefsStr;

Gpio gpioSCL = Gpio(GPIOB,6);
Gpio gpioSDA = Gpio(GPIOB,7);
Gpio gpioLcdReset = Gpio(GPIOC,7);

//HAL_I2C_Master_Transmit()
//bool Hardware::i2cInit(){
//	if (HAL_StatusTypeDef::HAL_OK == HAL_I2C_Init(i2c)) return false;
//	return true;
//}

void Hardware::lcdPinReset(bool newstate){ gpioLcdReset.pinSet(newstate); }


//void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c){
//	i2c_Ongoing = false;
//}

void Hardware::i2cInit(){
	// RCC
	do {
			SET_BIT(RCC->APB1ENR, RCC_APB1ENR_I2C1EN);
			/* Delay after an RCC peripheral clock enabling */
			tmpreg = READ_BIT(RCC->APB1ENR, RCC_APB1ENR_I2C1EN);
			(void)(tmpreg);
		} while(0U);


	/**I2C1 GPIO Configuration
PB6     ------> I2C1_SCL
PB7     ------> I2C1_SDA
	 */
	{ // GPIO setup
		gpioSCL.setup(Gpio::GpioMode::ALTERNATE, Gpio::GpioOType::OpenDrain, Gpio::GpioPuPd::PullUp, Gpio::GpioSpeed::MaximumSpeed);
		gpioSDA.setup(Gpio::GpioMode::ALTERNATE, Gpio::GpioOType::OpenDrain, Gpio::GpioPuPd::PullUp, Gpio::GpioSpeed::MaximumSpeed);
		gpioSCL.setAlternateFunc(0x04); //AF04 - );
		gpioSDA.setAlternateFunc(0x04); //AF04 - );
		gpioLcdReset.setup(Gpio::GpioMode::OUTPUT, Gpio::GpioOType::PushPull, Gpio::GpioPuPd::NoPull, Gpio::GpioSpeed::MaximumSpeed);
		gpioLcdReset.pinSetUp();

	}


	i2c->instance = I2C1;

	i2c->i2cFreqkHz = 100;
	i2c->frameBuffer = &i2cFrame;
	i2c->dataBuffer = &i2cFrame;

	//	i2c->instance->CR1 |=  I2C_CR1_PE;
	//	while(!(i2c->instance->CR1 & I2C_CR1_PE)){;}	// odczekanie na wlaczenie I2C

	//	i2c->instance->CR1 |= I2C_CR1_SWRST;
	////	while(i2c->instance->CR1 & I2C_CR1_SWRST){;}	// odczekanie na koniec resetu I2C
	//	i2c->instance->CR1 &= ~I2C_CR1_SWRST;

	i2c->instance->CR1 &= ~I2C_CR1_PE;
	while(i2c->instance->CR1 & I2C_CR1_PE){;}	// odczekanie na koniec resetu I2C


	NVIC_SetPriority(IRQn_Type::I2C1_EV_IRQn, NVIC_EncodePriority(nvicPriority, 0,0));
	NVIC_EnableIRQ(IRQn_Type::I2C1_EV_IRQn);

	NVIC_SetPriority(IRQn_Type::I2C1_ER_IRQn, NVIC_EncodePriority(nvicPriority, 0,0));
	NVIC_EnableIRQ(IRQn_Type::I2C1_ER_IRQn);

	uint32_t APBClock = SystemCoreClock;
	uint32_t APB_CLOCKMhz = (APBClock /1000000u) & 0b0111111;
	//APB_CLOCKMhz = (i2c->CR2 & 0b0111111) | APB_CLOCKMhz;

	i2c->instance->CR2 = APB_CLOCKMhz;
	//uint16_t((APB_CLOCKMhz | I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN) & 0x0001111100111111);		// 8 dla 8MHz, 25 dla 25MHz
	i2c->instance->TRISE = 1 + APB_CLOCKMhz; //0x09;	dla 8MHz, 33 dla 32MHz
	i2c->instance->CCR = 160;//(APB_CLOCKMhz * 2000)/(I2C_CLK_FREQ_kHz);  //160;	//40  ;			// 100kHz
	// (((((__PCLK__)/((__SPEED__) << 1U)) & I2C_CCR_CCR) < 4U)? 4U:((__PCLK__) / ((__SPEED__) << 1U)))
	//i2c->CR1 = 0x00;
	i2c->instance->OAR1 = 0x00 + (1UL<<14);
	i2c->instance->OAR2 = 0x00;
	i2c->instance->CR2 |=  I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN;
	i2c->instance->CR1 |=  I2C_CR1_PE;
	while(!(i2c->instance->CR1 & I2C_CR1_PE)){;}	// odczekanie na koniec resetu I2C


	//	if (i2c->instance->SR2 & I2C_SR2_BUSY){
	//		i2c->instance->CR1 |= I2C_CR1_STOP;
	//	}

	i2cFrame.flush();
	i2cData.flush();
	i2c->state = I2CSTATE::IDLE;

}

//uint8_t i2cFrameBuffer[20];
//Fifo i2cFrame = Fifo(i2cFrameBuffer, 20);

//volatile bool i2c_Ongoing  = false;
////volatile bool frameInUse = false;

void Hardware::i2cEvent(){
	volatile uint32_t sr2 = i2c->instance->SR2;
	uint32_t sr1 = i2c->instance->SR1;
	if (sr1 & I2C_SR1_SB){
		i2c->instance->CR1 |= I2C_CR1_ACK;		// ma byc ack
		i2c->instance->DR = Hardware::i2c->slaveAdr << 1;
		i2c->state = I2CSTATE::ADR;
	}else if (sr1 & I2C_SR1_ADDR){
		sr2 = i2c->instance->SR2;
		(void)(sr2 = -sr2);		//		UNUSED(sr2);
		uint32_t cnt = i2cFrame.get_count();
		if (cnt == 0){				// nic do wyslania???
			i2c->instance->CR1 |= I2C_CR1_STOP;
		}else{
			i2c->instance->CR1 |= I2C_CR1_ACK;		// ma byc ack
			i2c->state = I2CSTATE::DATA;
		}
	}else if (sr1 & (I2C_SR1_TXE | I2C_SR1_BTF)){
		if (i2cFrame.isNotEmpty()){
			i2c->instance->CR1 |= I2C_CR1_ACK;		// ma byc ack
			i2c->instance->DR = i2cFrame.get();
			i2c->state = I2CSTATE::DATA;
		}else{
			i2c->instance->CR1 |= I2C_CR1_STOP;
			i2c->state = I2CSTATE::STOP;
			//i2c->state = I2CSTATE::IDLE;
			i2c->timeStamp = QuickTask::getCounter();
		}
	}else{
		if (!(sr2 & I2C_SR2_BUSY)){
			i2c->state = I2CSTATE::IDLE;
			i2c->timeStamp = QuickTask::getCounter();
		}
	}
}


void Hardware::i2cError(){
	static uint32_t err = 0;
	err++;
	uint32_t sr1 = i2c->instance->SR1;
	//uint32_t sr2 = i2c->instance->SR2;

	//	if (sr1 & I2C_SR1_SMBALERT) i2c->instance->SR1 &= ~I2C_SR1_SMBALERT;
	//	if (sr1 & I2C_SR1_TIMEOUT) i2c->instance->SR1 &= ~I2C_SR1_TIMEOUT;
	//	if (sr1 & I2C_SR1_PECERR) i2c->instance->SR1 &= ~I2C_SR1_PECERR;
	//	if (sr1 & I2C_SR1_OVR) i2c->instance->SR1 &= ~I2C_SR1_OVR;

	i2c->instance->SR1 &= ~(
			I2C_SR1_SMBALERT |
			I2C_SR1_TIMEOUT |
			I2C_SR1_PECERR |
			I2C_SR1_OVR |
			I2C_SR1_AF |
			I2C_SR1_ARLO |
			I2C_SR1_BERR
	);
	i2c->state = I2CSTATE::IDLE;
	i2c->timeStamp = QuickTask::getCounter();
}

bool i2cGetBusyFlag(){
	uint32_t cr1    = Hardware::i2c->instance->CR2; // odczyt CR2, zeby nie skasowac przypadkiem ADDR
	if (cr1 & I2C_CR1_PE){
		return Hardware::i2c->instance->SR2 & I2C_SR2_BUSY;
	}
	return false;
}


void Hardware::i2cTimerJob(){

	if (i2cGetBusyFlag()){
		i2c->timeStamp = QuickTask::getCounter();
		return;
	}
	if (QuickTask::getTimeIntervalMilis(i2c->timeStamp) < 8) return;  // 4 ms odstepu do poprzedniej transmisji/ komendy

	if (i2c->dataBuffer->isEmpty()) return;

	if (i2c->state != I2CSTATE::IDLE){
		if (QuickTask::getTimeIntervalMilis(i2c->timeStamp) > 1000){  	// jesli powyzej sekundy to timeStamp
//			if (i2c->instance->CR1 & I2C_CR1_PE){
//				i2c->instance->CR1 &= ~I2C_CR1_PE;	// wylacz i2c
//			}else{
//				i2c->instance->CR1 |= I2C_CR1_PE;
//				// i2c->instance->CR1 |= I2C_CR1_SWRST;
				i2c->state = I2CSTATE::IDLE;
//			}
		}
		return;
	}
	// jest IDLE
	while (i2c->dataBuffer->get() !=  I2C_BLOCK_START){
		if (i2c->dataBuffer->isEmpty()) return;
	}
	if (i2c->dataBuffer->isEmpty()) return;
	uint8_t amount = i2c->dataBuffer->get();
	for (uint8_t i = 0; i < amount; i++){
		if (i2c->dataBuffer->isEmpty()) return;
		i2c->frameBuffer->put(i2c->dataBuffer->get());
	}
	i2c->state = I2CSTATE::START;
	i2c->instance->CR1 |= I2C_CR1_START;
}


bool Hardware::i2cMasterTransmit(uint16_t slaveAdres, uint8_t * buffer, uint16_t amount){
	if ((i2c->state == I2CSTATE::IDLE)&&(i2c->dataBuffer->isNotEmpty())){
		i2cTimerJob();
	}
	i2c->slaveAdr = slaveAdres;
	if (i2c->dataBuffer->isFull()) return false;
	i2c->dataBuffer->put(I2C_BLOCK_START);
	i2c->dataBuffer->put(amount);
	for (uint32_t i = 0; i < amount; i++){
		if (i2c->dataBuffer->isFull()) return false;
		i2c->dataBuffer->put(buffer[i]);
	}
	if (i2c->state == I2CSTATE::IDLE){
		i2cTimerJob();
	}
	return true;
}

//bool Hardware::i2cMasterTransmit(uint16_t slaveAdres, uint8_t * buffer, uint16_t amount, uint32_t timeout)
//{
//	//return (HAL_StatusTypeDef::HAL_OK == HAL_I2C_Master_Transmit(i2c, slaveAdres << 1, buffer, amount, timeout));
//}

void I2C1_EV_IRQHandler(void){
	Hardware::i2cEvent();
}

void I2C1_ER_IRQHandler(void){
	Hardware::i2cError();
}

