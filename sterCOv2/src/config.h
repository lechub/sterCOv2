/*
 * config.h
 *
 *  Created on: 4 wrz 2017
 *      Author: lechu
 */

#ifndef CONFIG_H_
#define CONFIG_H_


#define CRYSTAL								8000000ul	///< quartz crystal resonator which is connected to the chip
#define FREQUENCY							8000000ul	///< desired target frequency of the core


// 8MHz HSE -> SystemClockMux, no PLL, CSSEnabled -> SYSCLK = 8MHz
// AHB prescaler = 1 -> AHB clock = 8MHz
// APB1, APB2 x1 -> 8MHz


#endif /* CONFIG_H_ */
