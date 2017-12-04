/*
 * Hardware.cpp
 *
 *  Created on: 28.06.2017
 *      Author: lechu
 */

#include "Hardware.h"
#include "hdr_bitband.h"
#include "hdr_rcc.h"
#include "Gpio.h"
#include "Pomiar.h"
#include "Fifo.h"
#include "QuickTask.h"

#ifndef DEBUG
#define WATCHDOG_ENABLE 1
#endif

constexpr uint32_t nvicPriority = 0x03U;


//I2C_TypeDef * Hardware::i2ch = I2C1;

// ******************************************

//#define IS_FLAG_SET(__REGPTR, __FLAG) (((__REGPTR) & (__FLAG)) == 0)
//#define IS_FLAG_CLEAR(__REGPTR, __FLAG) (((__REGPTR) & (__FLAG)) != 0)



/*
+=============================================================================+
| local functions
+=============================================================================+
 */


///*------------------------------------------------------------------------*//**
//* \brief Enables FPU
//* \details Enables FPU in Cortex-M4 for both privileged and user mode. This is
//* done by enabling CP10 and CP11 coprocessors in CPACR register (possible only
//* when in privileged mode).
//*//*-------------------------------------------------------------------------*/
//
//static void fpu_enable(void)
//{
//#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
//	SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));	// set CP10 and CP11 Full Access
//#endif
//}


///*------------------------------------------------------------------------*//**
//* \brief Starts the PLL
//* \details Configure and enable PLL to achieve some frequency with some crystal.
//* Before the speed change Flash latency is configured via flash_latency(). PLL
//* parameters are based on both function parameters. The PLL is set up,
//* started and connected. AHB clock ratio is set to 1:1 with core clock, APB1 -
//* 1:4 (max 42MHz), APB2 - 1:2 (max 84MHz). USB OTG, RNG and SDIO clock is set
//* to (VCO output / 9) (max 48MHz).
//*
//* \param [in] crystal is the frequency of the crystal resonator connected to the
//* STM32F407VG
//* \param [in] frequency is the desired target frequency after enabling the PLL
//*
//* \return real frequency that was set
//*//*-------------------------------------------------------------------------*/
//
//static uint32_t pll_start(uint32_t crystal, uint32_t frequency)
//{
//	uint32_t div, mul, div_core, vco_input_frequency, vco_output_frequency, frequency_core;
//	uint32_t best_div = 0, best_mul = 0, best_div_core = 0, best_frequency_core = 0;
//
//	RCC_CR_HSEON_bb = 1;					// enable HSE clock
//	flash_latency(frequency);				// configure Flash latency for desired frequency
//
//	for (div = 2; div <= 63; div++)			// PLLM in [2; 63]
//	{
//		vco_input_frequency = crystal / div;
//
//		if ((vco_input_frequency < 1000000ul) || (vco_input_frequency > 2000000))	// skip invalid settings
//			continue;
//
//		for (mul = 64; mul <= 432; mul++)	// PLLN in [64; 432]
//		{
//			vco_output_frequency = vco_input_frequency * mul;
//
//			if ((vco_output_frequency < 64000000ul) || (vco_output_frequency > 432000000ul))	// skip invalid settings
//				continue;
//
//			for (div_core = 2; div_core <= 8; div_core += 2)	// PLLP in {2, 4, 6, 8}
//			{
//				frequency_core = vco_output_frequency / div_core;
//
//				if (frequency_core > frequency)	// skip values over desired frequency
//					continue;
//
//				if (frequency_core > best_frequency_core)	// is this configuration better than previous one?
//				{
//					best_frequency_core = frequency_core;	// yes - save values
//					best_div = div;
//					best_mul = mul;
//					best_div_core = div_core;
//				}
//			}
//		}
//	}
//
//	RCC->PLLCFGR = (best_div << RCC_PLLCFGR_PLLM_bit) | (best_mul << RCC_PLLCFGR_PLLN_bit) | ((best_div_core / 2 - 1) << RCC_PLLCFGR_PLLP_bit) | RCC_PLLCFGR_PLLQ_DIV9 | RCC_PLLCFGR_PLLSRC_HSE;	// configure PLL factors, always divide USB clock by 9
//
//	RCC->CFGR = RCC_CFGR_PPRE2_DIV2 | RCC_CFGR_PPRE1_DIV4 | RCC_CFGR_HPRE_DIV1;	// AHB - no prescaler, APB1 - divide by 4, APB2 - divide by 2
//
//	while (!RCC_CR_HSERDY_bb);				// wait for stable clock
//
//	RCC_CR_PLLON_bb = 1;					// enable PLL
//	while (!RCC_CR_PLLRDY_bb);				// wait for PLL lock
//
//	RCC->CFGR |= RCC_CFGR_SW_PLL;			// change SYSCLK to PLL
//	while (((RCC->CFGR) & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);	// wait for switch
//
//	return best_frequency_core;
//}


/*------------------------------------------------------------------------*//**
 * \brief Configures Flash latency.
 * \details Configures Flash latency (wait-states) which allows the chip to run
 * at higher speeds. The constants used for calculations are valid when chip's
 * voltage is in 2.7V to 3.6V range.
 *
 * \param [in] frequency defines the target frequency of the core
 *//*-------------------------------------------------------------------------*/

static void flash_latency(uint32_t frequency)
{
	uint32_t wait_states;

	wait_states = frequency / 30000000ul;	// calculate wait_states (30M is valid for 2.7V to 3.6V voltage range, use 24M for 2.4V to 2.7V, 18M for 2.1V to 2.4V or 16M for  1.8V to 2.1V)
	wait_states &= 7;						// trim to max allowed value - 7

	FLASH->ACR = wait_states;				// set wait_states, disable all caches and prefetch
	FLASH->ACR = FLASH_ACR_DCRST | FLASH_ACR_ICRST | wait_states;	// reset caches
	FLASH->ACR = FLASH_ACR_DCEN | FLASH_ACR_ICEN | FLASH_ACR_PRFTEN | wait_states;	// enable caches and prefetch
}

/*------------------------------------------------------------------------*//**
 * \brief Starts the PLL
 * \details Configure and enable PLL to achieve some frequency with some crystal.
 * Before the speed change Flash latency is configured via flash_latency(). PLL
 * parameters are based on both function parameters. The PLL is set up,
 * started and connected. AHB clock ratio is set to 1:1 with core clock, APB1 -
 * 1:4 (max 42MHz), APB2 - 1:2 (max 84MHz). USB OTG, RNG and SDIO clock is set
 * to (VCO output / 9) (max 48MHz).
 *
 * \param [in] crystal is the frequency of the crystal resonator connected to the
 * STM32F407VG
 * \param [in] frequency is the desired target frequency after enabling the PLL
 *
 * \return real frequency that was set
 *//*-------------------------------------------------------------------------*/

static uint32_t pll_start(uint32_t crystal, uint32_t frequency)
{
	uint32_t div, mul, div_core, vco_input_frequency, vco_output_frequency, frequency_core;
	uint32_t best_div = 0, best_mul = 0, best_div_core = 0, best_frequency_core = 0;

	RCC_CR_HSEON_bb = 1;					// enable HSE clock
	flash_latency(frequency);				// configure Flash latency for desired frequency

	for (div = 2; div <= 63; div++)			// PLLM in [2; 63]
	{
		vco_input_frequency = crystal / div;

		if ((vco_input_frequency < 1000000ul) || (vco_input_frequency > 2000000))	// skip invalid settings
			continue;

		for (mul = 64; mul <= 432; mul++)	// PLLN in [64; 432]
		{
			vco_output_frequency = vco_input_frequency * mul;

			if ((vco_output_frequency < 64000000ul) || (vco_output_frequency > 432000000ul))	// skip invalid settings
				continue;

			for (div_core = 2; div_core <= 8; div_core += 2)	// PLLP in {2, 4, 6, 8}
			{
				frequency_core = vco_output_frequency / div_core;

				if (frequency_core > frequency)	// skip values over desired frequency
					continue;

				if (frequency_core > best_frequency_core)	// is this configuration better than previous one?
				{
					best_frequency_core = frequency_core;	// yes - save values
					best_div = div;
					best_mul = mul;
					best_div_core = div_core;
				}
			}
		}
	}

	RCC->PLLCFGR = (best_div << RCC_PLLCFGR_PLLM_bit) | (best_mul << RCC_PLLCFGR_PLLN_bit) | ((best_div_core / 2 - 1) << RCC_PLLCFGR_PLLP_bit) | RCC_PLLCFGR_PLLQ_DIV9 | RCC_PLLCFGR_PLLSRC_HSE;	// configure PLL factors, always divide USB clock by 9

	RCC->CFGR = RCC_CFGR_PPRE2_DIV1 | RCC_CFGR_PPRE1_DIV1 | RCC_CFGR_HPRE_DIV1;	// AHB - no prescaler, APB1 - divide by 1, APB2 - divide by 1

	while (!RCC_CR_HSERDY_bb);				// wait for stable clock

	RCC_CR_PLLON_bb = 1;					// enable PLL
	while (!RCC_CR_PLLRDY_bb);				// wait for PLL lock

	RCC->CFGR |= RCC_CFGR_SW_PLL;			// change SYSCLK to PLL
	while (((RCC->CFGR) & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);	// wait for switch

	return best_frequency_core;
}


/*------------------------------------------------------------------------*//**
 * \brief Enables FPU
 * \details Enables FPU in Cortex-M4 for both privileged and user mode. This is
 * done by enabling CP10 and CP11 coprocessors in CPACR register (possible only
 * when in privileged mode).
 *//*-------------------------------------------------------------------------*/

static void fpu_enable(void)
{
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
	SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));	// set CP10 and CP11 Full Access
#endif
}


static void rccInit(void){
	// reset gpio's i obu DMA
	//	RCC->AHB1RSTR = RCC_AHB1RSTR_GPIOARST | RCC_AHB1RSTR_GPIOBRST
	//			|RCC_AHB1RSTR_GPIOCRST | RCC_AHB1RSTR_GPIODRST
	//			|RCC_AHB1RSTR_GPIOERST | RCC_AHB1RSTR_GPIOHRST
	//			|RCC_AHB1RSTR_DMA1RST | RCC_AHB1RSTR_DMA2RST
	//			;

	__IO uint32_t tmpreg = 0x00U;

	do {
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN
				|RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIODEN
				|RCC_AHB1ENR_GPIOEEN | RCC_AHB1ENR_GPIOHEN
				//	|RCC_AHB1ENR_DMA1EN
				| RCC_AHB1ENR_DMA2EN
				;
		tmpreg = RCC->AHB1ENR;
		(void)(tmpreg);
	} while(0U);


	do {
		SET_BIT(RCC->APB1ENR, RCC_APB1ENR_I2C1EN);
		/* Delay after an RCC peripheral clock enabling */
		tmpreg = READ_BIT(RCC->APB1ENR, RCC_APB1ENR_I2C1EN);
		(void)(tmpreg);
	} while(0U);


	do {
		RCC->APB2ENR |= RCC_APB2ENR_ADC1EN
				;
		/* Delay after an RCC peripheral clock enabling */
		tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_ADC1EN);
		(void)(tmpreg);
	} while(0U);

}

void Hardware::init(){

	// Procesor i zegar zainicjowany

	flash_latency(32000000);
	pll_start(8000000, 32000000);
	fpu_enable();

	rccInit();

	SystemCoreClockUpdate();	// na wszelki wypadek

	if (SysTick_Config( SystemCoreClock / SYSTICK_FREQUENCY_HZ)) {
		errorDispatch(Hardware::ErrorCode::Failure);
	}

	NVIC_init();	// przerwania

	gpioInit();

	adcInit();

	i2cInit();

	WDOG_Init();


}

/*------------------------------------------------------------------------*//**
 * \brief GPIO initialization.
 * \details Enables all GPIO ports.
 *//*-------------------------------------------------------------------------*/

void Hardware::gpioInit(){

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN |
			RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIODEN | RCC_AHB1ENR_GPIOEEN |RCC_AHB1ENR_GPIOHEN
			;			// enable all possible GPIO ports

}

//I2C_TypeDef * i2c = I2C1;

//	IN0	PC0	reg.pok.		ADC_1_10
//	IN1	PC3	czujnikPodaj.	ADC_1_13
//	IN2	PC1	czujnikCWU		ADC_1_11
//	IN3	PC2	CzujnikCo		ADC_1_12
//	IN4	PA4	termik			ADC_1_4
//	IN5	PA1	NC				ADC_1_1
//	in6	Temp_uP				ADC_1_IN18

Gpio analog0 = Gpio(GPIOC, 0);
Gpio analog1 = Gpio(GPIOC, 3);
Gpio analog2 = Gpio(GPIOC, 1);
Gpio analog3 = Gpio(GPIOC, 2);
Gpio analog4 = Gpio(GPIOA, 4);
Gpio analog5 = Gpio(GPIOA, 1);


void Hardware::adcInit(){

	const uint32_t dataLength = Pomiar::Analogi::count;
	uint32_t dataPtr = (uint32_t)Pomiar::getDataTablePtr();

	// ---------- RCC ---------------

	// wlaczenie zegara dla ADC
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
	// wlaczenie zegara dla DMA2
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;

	// ----------GPIO ---------------

	analog0.setup(Gpio::GpioMode::ANALOG, Gpio::GpioOType::NoMatter, Gpio::GpioPuPd::NoPull, Gpio::GpioSpeed::HighSpeed);
	analog1.setup(Gpio::GpioMode::ANALOG, Gpio::GpioOType::NoMatter, Gpio::GpioPuPd::NoPull, Gpio::GpioSpeed::HighSpeed);
	analog2.setup(Gpio::GpioMode::ANALOG, Gpio::GpioOType::NoMatter, Gpio::GpioPuPd::NoPull, Gpio::GpioSpeed::HighSpeed);
	analog3.setup(Gpio::GpioMode::ANALOG, Gpio::GpioOType::NoMatter, Gpio::GpioPuPd::NoPull, Gpio::GpioSpeed::HighSpeed);
	analog4.setup(Gpio::GpioMode::ANALOG, Gpio::GpioOType::NoMatter, Gpio::GpioPuPd::NoPull, Gpio::GpioSpeed::HighSpeed);
	analog5.setup(Gpio::GpioMode::ANALOG, Gpio::GpioOType::NoMatter, Gpio::GpioPuPd::NoPull, Gpio::GpioSpeed::HighSpeed);

	// ----------DMA ---------------
	/* Disable the peripheral */

	DMA2_Stream0->CR  &=  ~((uint32_t)0x00000001);


	DMA2_Stream0->CR  &= (uint32_t)(~DMA_SxCR_DBM);		// reset double buffer

	/* Configure DMA Stream data length */

	DMA2_Stream0->NDTR = dataLength;

	//	   /* Peripheral to Memory */
	DMA2_Stream0->PAR = ADC1->DR;	//Peripheral
	//	     /* Configure DMA Stream source address */
	DMA2_Stream0->M0AR = (uint32_t)dataPtr;


	/* Enable all interrupts */
	//hdma->Instance->CR  |= DMA_IT_TC | DMA_IT_HT | DMA_IT_TE | DMA_IT_DME;

	//	  DMA2_Stream0->CR |= DMA_SxCR_HTIE | DMA_SxCR_TCIE | DMA_SxCR_TEIE | DMA_SxCR_DMEIE;

	//	  DMA2_Stream0->FCR |= DMA_SxFCR_FEIE; //DMA_IT_FE;

	/* Enable the Peripheral */
	DMA2_Stream0->CR |=  DMA_SxCR_EN;	// poszly konie

	//  -------------------------------------



	//  ---------------- ADC ------------------

	/* Set ADC parameters */
	/* Set the ADC clock prescaler */
	ADC->CCR |=
			ADC_CCR_TSVREFE |							// Temperature sensor enabled
			ADC_CCR_ADCPRE_0 | ADC_CCR_ADCPRE_1;		// prescaler PCLK2 /8

	/* Set ADC scan mode */
	ADC1->CR1 |= ADC_CR1_SCAN
			//			  | ADC_CR1_RES_0			// RES = 12bit 15ADCCLK,
			//			| ADC_CR1_EOCIE			// Interrupt enable on end of conversion
			;


	// Sample time register
	uint32_t smp = ADC_SMPR1_SMP10_0 |ADC_SMPR1_SMP10_1; // 56 cycles /sampling time na wszystkich kanalach
	uint32_t val = 0;
	for (int i = 0 ; i < 9; i++){
		val |= smp << (3 * i);
	}
	ADC1->SMPR1 = val;

	val |= smp << (3 * 9);
	ADC1->SMPR2 = val;		// 56 cycles /sampling time na wszystkich kanalach

	// ilosc wejsc
	ADC1->SQR1 |= (dataLength) << 20U;		// 7 - ilosc sekwencji

	//	IN0	PC0	reg.pok.		ADC_1_10
	//	IN1	PC3	czujnikPodaj.	ADC_1_13
	//	IN2	PC1	czujnikCWU		ADC_1_11
	//	IN3	PC2	CzujnikCo		ADC_1_12
	//	IN4	PA4	termik			ADC_1_4
	//	IN5	PA1	NC				ADC_1_1
	//	in6	Temp_uP				ADC_1_IN18

	ADC1->SQR3 =
			(10 << (0*5)) |
			(13 << (1*5)) |
			(11 << (2*5)) |
			(12 << (3*5)) |
			(4 << (4*5)) |
			(1 << (5*5))
			;
	ADC1->SQR2 =
			(18 << (0*5))
			;

	/* Set ADC resolution */
	/* Set ADC data alignment */

	/* Enable or disable ADC continuous conversion mode */
	ADC1->CR2 |=
			//			  ADC_CR2_ALIGN |
//			ADC_CR2_DMA |	// lecimy DMA
			ADC_CR2_DDS |	// wznawiamy DMA po ostatniej konwersji
			ADC_CR2_CONT ;


	/* Enable or disable ADC DMA continuous request */
	ADC1->CR2 |= ADC_CR2_DDS
			|  ADC_CR2_ADON		// ADC wlaczone
			;


	/* Delay for ADC stabilization time */
	/* Compute number of CPU cycles to wait for */
	__IO uint32_t counter = (3U * (SystemCoreClock / 1000000U));
	while(counter != 0U)
	{
		counter--;
	}


	ADC1->SR &= ~ADC_SR_EOC;


	ADC1->CR2 |= ADC_CR2_DMA;


}

#define IWDG_KEY_RELOAD                 0x0000AAAAU  /*!< IWDG Reload Counter Enable   */
#define IWDG_KEY_ENABLE                 0x0000CCCCU  /*!< IWDG Peripheral Enable       */
#define IWDG_KEY_WRITE_ACCESS_ENABLE    0x00005555U  /*!< IWDG KR Write Access Enable  */
#define IWDG_KEY_WRITE_ACCESS_DISABLE   0x00000000U  /*!< IWDG KR Write Access Disable */

#define IWDG_PRESCALER_4                0x00000000U                   /*!< IWDG prescaler set to 4   */
#define IWDG_PRESCALER_8                IWDG_PR_PR_0                  /*!< IWDG prescaler set to 8   */
#define IWDG_PRESCALER_16               IWDG_PR_PR_1                  /*!< IWDG prescaler set to 16  */
#define IWDG_PRESCALER_32               (IWDG_PR_PR_1 | IWDG_PR_PR_0) /*!< IWDG prescaler set to 32  */
#define IWDG_PRESCALER_64               IWDG_PR_PR_2                  /*!< IWDG prescaler set to 64  */
#define IWDG_PRESCALER_128              (IWDG_PR_PR_2 | IWDG_PR_PR_0) /*!< IWDG prescaler set to 128 */
#define IWDG_PRESCALER_256              (IWDG_PR_PR_2 | IWDG_PR_PR_1) /*!< IWDG prescaler set to 256 */

void Hardware::WDOG_Init(){
#ifdef WATCHDOG_ENABLE
	// Enable write access to IWDG_PR and IWDG_RLR registers by writing 0x5555 in KR
	IWDG->KR = IWDG_KEY_WRITE_ACCESS_ENABLE;

	// Write to IWDG registers the Prescaler & Reload values to work with
	IWDG->PR = IWDG_PRESCALER_256;		// 32kHz/256 = 125Hz == 8ms
	IWDG->RLR = 125 * 8 * 2;	// 2 sekundy

	// Enable IWDG. LSI is turned on automaticaly
	IWDG->KR =  IWDG_KEY_ENABLE;	// aktywacja IWDG
#endif	//WATCHDOG_ENABLE
}

void Hardware::WDOG_Reload(){
#ifdef WATCHDOG_ENABLE
	IWDG->KR =  IWDG_KEY_RELOAD;	// przeladowanie IWDG
#endif	//WATCHDOG_ENABLE
}



void Hardware::NVIC_init(){
	NVIC_SetPriorityGrouping(nvicPriority);
}


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

uint8_t i2cFrameBuffer[30];
Fifo i2cFrame = Fifo(i2cFrameBuffer, 30);

uint8_t i2cDataBuffer[100];
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
	uint32_t sr1 = i2c->instance->SR1;
	if (sr1 & I2C_SR1_SB){
		i2c->instance->DR = Hardware::i2c->slaveAdr << 1;
		i2c->state = I2CSTATE::ADR;
	}else if (sr1 & I2C_SR1_ADDR){
		uint32_t sr2 = i2c->instance->SR2;
		(void)(sr2 == sr2);		//		UNUSED(sr2);
		i2c->state = I2CSTATE::DATA;
	}else if (sr1 & (I2C_SR1_TXE | I2C_SR1_BTF)){
		if (i2cFrame.isNotEmpty()){
			i2c->instance->DR = i2cFrame.get();
			i2c->state = I2CSTATE::DATA;
		}else{
			i2c->instance->CR1 |= I2C_CR1_STOP;
			//i2c->state = I2CSTATE::STOP;
			i2c->state = I2CSTATE::IDLE;
			i2c->timeStamp = QuickTask::getCounter();
		}
	}else{
		i2c->state = I2CSTATE::IDLE;
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
	return Hardware::i2c->instance->SR2 & I2C_SR2_BUSY;
}


void Hardware::i2cTimerJob(){

	if (QuickTask::getTimeIntervalMilis(i2c->timeStamp) < 4) return;  // 4 ms odstepu do poprzedniej transmisji/ komendy

//	if (QuickTask::getTimeIntervalMilis(timeStamp) > 1000){  // jesli powyzej sekundy to timeStamp
//		if (i2c->instance->SR2 & I2C_SR2_BUSY){ // jesli tak dlugo busy, to reset I2C
//			i2c->instance->CR1 |= I2C_CR1_SWRST;
//			i2c->instance->CR1 &= ~I2C_CR1_SWRST;
//			i2cInit();
//		}
//		i2c->state = I2CSTATE::IDLE;
//	}

	if (i2cGetBusyFlag()) return;
	if (i2c->state != I2CSTATE::IDLE) return;
	if (i2c->dataBuffer->isEmpty()) return;

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

bool Hardware::i2cMasterTransmit(uint16_t slaveAdres, uint8_t * buffer, uint16_t amount, uint32_t timeout)
{
	//return (HAL_StatusTypeDef::HAL_OK == HAL_I2C_Master_Transmit(i2c, slaveAdres << 1, buffer, amount, timeout));
}

void DMA2_Stream0_IRQHandler(void){
	static volatile uint32_t dmairq = 1;
	dmairq++;
}

void DMA1_Stream0_IRQHandler(void){
	static volatile uint32_t dmairq = 1;
	dmairq++;
}

void ADC_IRQHandler(void){
	static volatile uint32_t adcirq = 1;
	adcirq++;
}

void I2C1_EV_IRQHandler(void){
	static volatile uint32_t i2cev = 1;
	i2cev++;
	Hardware::i2cEvent();
}

void I2C1_ER_IRQHandler(void){
	static volatile uint32_t i2cer = 1;
	i2cer++;
	Hardware::i2cError();
}




