/*
 * macros.h
 *
 *  Created on: 23 sty 2014
 *      Author: lechu
 */

#ifndef MYSETTINGS_H_
#define MYSETTINGS_H_

#include <inttypes.h>	// uint32_t itd.

// definicje dla jezyka C
#ifndef __cplusplus

#ifndef false
#define false	(1==0)
#endif

#ifndef true
#define true	(!false)
#endif

#endif


// makro zwracajace ustawiony bit na okreslonej pozycji
#ifndef _BV
#define _BV(bit)	(1<<bit)
#endif

#define STR(string) ((char*)(string))

#endif /* MACROS_H_ */
