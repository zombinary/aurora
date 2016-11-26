#include "timeout.h"
// just using _delay_ms makes the code very fat. If we encapsulate it in 
// a function then it is OK.
//
void delay_ms(unsigned int ms)
{
        while(ms){
                _delay_ms(0.96);
                ms--;
        }
}

