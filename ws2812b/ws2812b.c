// Program for ATmega328 P to allow control of WS2812B LEDs
// Clock source is intended to be an external 16MHz ceramic oscillator
// This code is based on the Adafruit Neopixel library, but heavily
// stripped down.

// FIXME - tidy this up, remove the blinking LED
// (which is no longer once per 2s anyway!)

#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include "ws2812b.h"

ISR(TIMER2_OVF_vect){
	// We call this ever 16us
	if (latchCounter < 5)
		latchCounter++;
	counter++;
	if (counter == COUNT_MAX){
		counter = 0;
		PORTD ^= (1 << PORTD3);
	}
}

void init_WS2812B(){
	pixels = (uint8_t *)malloc(3 * N_PIXELS);
	PIXEL_DDR |= (1 << PIXEL_PIN);	// Set pin as output
	PIXEL_PORT &= (0 << PIXEL_PIN);	// Pull pin low
	latchCounter = 5;
}

void show_WS2812B(){
	volatile uint16_t
	i   = 3 * N_PIXELS; // Loop counter
	volatile uint8_t
	*port,	// Output port register
	*ptr = pixels,   // Pointer to next byte
	b   = *ptr++,   // Current byte value
	hi,             // PORT w/output bit set high
	lo;             // PORT w/output bit set low

    // WS2811 and WS2812 have different hi/lo duty cycles; this is
    // similar but NOT an exact copy of the prior 400-on-8 code.

    // 20 inst. clocks per bit: HHHHHxxxxxxxxLLLLLLL
    // ST instructions:         ^   ^        ^       (T=0,5,13)

	// Wait to ensure we have latched previous show()
	while (latchCounter < 5);

	// Disable interrupts
	uint8_t oldSREG = SREG;
	cli();

    volatile uint8_t next, bit;

    port = &PIXEL_PORT;
    hi   = *port |  (1 << PIXEL_PIN);
    lo   = *port & ~(1 << PIXEL_PIN);
    next = lo;
    bit  = 8;

    asm volatile(
     "head20:"                   "\n\t" // Clk  Pseudocode    (T =  0)
      "st   %a[port],  %[hi]"    "\n\t" // 2    PORT = hi     (T =  2)
      "sbrc %[byte],  7"         "\n\t" // 1-2  if(b & 128)
       "mov  %[next], %[hi]"     "\n\t" // 0-1   next = hi    (T =  4)
      "dec  %[bit]"              "\n\t" // 1    bit--         (T =  5)
      "st   %a[port],  %[next]"  "\n\t" // 2    PORT = next   (T =  7)
      "mov  %[next] ,  %[lo]"    "\n\t" // 1    next = lo     (T =  8)
      "breq nextbyte20"          "\n\t" // 1-2  if(bit == 0) (from dec above)
      "rol  %[byte]"             "\n\t" // 1    b <<= 1       (T = 10)
      "rjmp .+0"                 "\n\t" // 2    nop nop       (T = 12)
      "nop"                      "\n\t" // 1    nop           (T = 13)
      "st   %a[port],  %[lo]"    "\n\t" // 2    PORT = lo     (T = 15)
      "nop"                      "\n\t" // 1    nop           (T = 16)
      "rjmp .+0"                 "\n\t" // 2    nop nop       (T = 18)
      "rjmp head20"              "\n\t" // 2    -> head20 (next bit out)
     "nextbyte20:"               "\n\t" //                    (T = 10)
      "ldi  %[bit]  ,  8"        "\n\t" // 1    bit = 8       (T = 11)
      "ld   %[byte] ,  %a[ptr]+" "\n\t" // 2    b = *ptr++    (T = 13)
      "st   %a[port], %[lo]"     "\n\t" // 2    PORT = lo     (T = 15)
      "nop"                      "\n\t" // 1    nop           (T = 16)
      "sbiw %[count], 1"         "\n\t" // 2    i--           (T = 18)
       "brne head20"             "\n"   // 2    if(i != 0) -> (next byte)
      : [port]  "+e" (port),
        [byte]  "+r" (b),
        [bit]   "+r" (bit),
        [next]  "+r" (next),
        [count] "+w" (i)
      : [ptr]    "e" (ptr),
        [hi]     "r" (hi),
        [lo]     "r" (lo));

	// Restore SREG
	SREG = oldSREG;

	latchCounter = 0;
}

//void set_WS2812B(uint16_t n, uint8_t red, uint8_t green, uint8_t blue){
void set_WS2812B(uint8_t n, uint8_t red, uint8_t green, uint8_t blue){
	if (n > N_PIXELS) return;

	pixels[3*(n-1)    ] = green;
	pixels[3*(n-1) + 1] = red;
	pixels[3*(n-1) + 2] = blue;
}
