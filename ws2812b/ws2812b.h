#ifndef WS2812B_H
#define WS2812B_H
/*********************************************
 * ws2812b.h
 * author: zombinary
 * copyright: GPL V2
 * see http://www.gnu.org/licenses/gpl.html
 *
 * setup ws2812b pixel
 *********************************************/

#define COUNT_MAX	61	// To give us a transition every second

// Start of definitions associated with WS2812Bs
#define PIXEL_PORT	PORTB
#define PIXEL_DDR	DDRB
#define PIXEL_PIN	PORTB0
//#define N_PIXELS	5
//#define N_PIXELS	300
#define N_PIXELS	60

uint8_t *pixels;	// Holds GRB values for each WS2812B

volatile uint8_t counter;
volatile uint8_t latchCounter;


void init_WS2812B();
void show_WS2812B();
//void set_WS2812B(uint16_t n, uint8_t red, uint8_t green, uint8_t blue);
void set_WS2812B(uint8_t n, uint8_t red, uint8_t green, uint8_t blue);

#endif
