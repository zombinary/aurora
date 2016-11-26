/* vim: set sw=8 ts=8 si et: */
//#define F_CPU 12500000UL  // 12.5 MHz
#define F_CPU 16000000UL
#ifndef ALIBC_OLD
#include <util/delay.h>
#else
#include <avr/delay.h>
#endif

#ifndef TOUT_H
#define TOUT_H

extern void delay_ms(unsigned int ms);


#endif /* TOUT_H */

