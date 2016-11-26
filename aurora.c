/*********************************************
 * aurora.c
 * author: zombinary
 * copyright: GPL V2
 * see http://www.gnu.org/licenses/gpl.html
 *
 * tcp client, to control ws2812b pixel.
 *
 * http://tuxgraphics.org/electronics/
 * Note: there is a version number in the text. Search for "tuxgraphics"
 *********************************************/


#include <avr/io.h>
//#include <avr/iom328p.h>

#include <stdint.h>
//#include <util/delay.h>
#include <avr/interrupt.h>

#include "eth_lcd_tcp-2.10/enc28j60.h"
#include "eth_lcd_tcp-2.10/ip_arp_udp_tcp.h"
#include "eth_lcd_tcp-2.10/lcd.h"
#include "eth_lcd_tcp-2.10/net.h"
#include "eth_lcd_tcp-2.10/timeout.h"

#include "ws2812b/ws2812b.h"
#include "wrapper/wrapper.h"

#define MAX_PIXEL 30

//tcp settings
static uint8_t mymac[6] = {0x54,0x55,0x58,0x10,0x00,0x25};
static uint8_t myip[4] = {10,11,0,100};
static uint16_t mywwwport =80; // listen port for tcp/www (max range 1-254)

#define BUFFER_SIZE 750
static uint8_t buf[BUFFER_SIZE+1];
#define STR_BUFFER_SIZE 22


int main(void){
	uint16_t plen;
	uint16_t dat_p;

	char  byte[3];
	char  *cmd;
	int8_t led_pos;
	int8_t red;
	int8_t green;
	int8_t blue;

	// pixel krams
	TCCR2B &= ~(1 << CS22) & ~(1 << CS21) & ~(1 << CS20);			// Stop the timer during setup
	TIFR2 |= (1 << TOV2);	// Clear the overflow interrupt flag
	TIMSK2 |= (1 << TOIE2);	// Enable interrupts on overflow of timer2
	ASSR &= ~(1 << AS2);	// Clock timer / counter 2 from the I/O clock
	TCCR2A &= ~(1 << COM2B1); TCCR2A &= ~(1 << COM2B0);			// OC2B disconnected
	TCCR2B &= ~(1 << WGM22); TCCR2A &= ~(1 << WGM21); TCCR2A &= ~(1 << WGM20);	// 'Normal' mode
	TCCR2B &= ~(1 << CS22) & ~(1 << CS21);
	TCCR2B |= (1 << CS20);			// Prescale the clock by 1, switching it on

    /*initialize enc28j60*/
    enc28j60Init(mymac);
    enc28j60clkout(2); // change clkout from 6.25MHz to 12.5MHz
    _delay_loop_1(50); // 12ms
    enc28j60PhyWrite(PHLCON,0x476);
    _delay_loop_1(50); // 12ms
    // lcd display:
    lcd_init(LCD_DISP_ON);
    lcd_clrscr();
    lcd_puts_P("=ok=");
    //init the ethernet/ip layer:
    init_ip_arp_udp_tcp(mymac,myip,mywwwport);


	// pixel INIT
	//counter = 0;
	sei();	// enable interrupts
	init_WS2812B();
	set_WS2812B(3, 0, 0, 255); // RED
	show_WS2812B();
	cli();	// enable interrupts

	while(1){
        // get the next new packet:
         plen = enc28j60PacketReceive(BUFFER_SIZE, buf);

         // plen will ne unequal to zero if there is a valid
         // packet (without crc error)
         if(plen==0){
                 continue;
         }

         // arp is broadcast if unknown but a host may also
         // verify the mac address by sending it to
         // a unicast address.
         if(eth_type_is_arp_and_my_ip(buf,plen)){
                 make_arp_answer_from_request(buf);
                 continue;
         }

         // check if ip packets are for us:
         if(eth_type_is_ip_and_my_ip(buf,plen)==0){
                 continue;
         }

         if(buf[IP_PROTO_P]==IP_PROTO_ICMP_V && buf[ICMP_TYPE_P]==ICMP_TYPE_ECHOREQUEST_V){
                 // a ping packet, let's send pong
                 make_echo_reply_from_request(buf,plen);
                 continue;
         }

         // tcp port www start, compare only the lower byte
         if (buf[IP_PROTO_P]==IP_PROTO_TCP_V&&buf[TCP_DST_PORT_H_P]==0&&buf[TCP_DST_PORT_L_P]==mywwwport){
                 if (buf[TCP_FLAGS_P] & TCP_FLAGS_SYN_V){
                         make_tcp_synack_from_syn(buf);
                         // make_tcp_synack_from_syn does already send the syn,ack
                         continue;
                 }
                 if (buf[TCP_FLAGS_P] & TCP_FLAGS_ACK_V){
                         init_len_info(buf); // init some data structures
                         // we can possibly have no data, just ack:
                         dat_p=get_tcp_data_pointer();
                         if (dat_p==0){
                         	if (buf[TCP_FLAGS_P] & TCP_FLAGS_FIN_V){
                         		// finack, answer with ack
                                  make_tcp_ack_from_any(buf);
                             }
                             // just an ack with no data, wait for next packet
                         	continue;
                         }
                         if(buf[TCP_FLAGS_P] & TCP_FLAGS_PSHACK_V){
                         	lcd_clrscr();
                         	lcd_puts_P("PUSH/ACK");
                         	make_tcp_ack_from_any(buf);

                         	cmd = (char *)&(buf[dat_p]);
							lcd_gotoxy(0,1);
							lcd_puts(cmd);

							splitString(byte, cmd,0,1);
							red = hex2int(byte);

							splitString(byte, cmd,2,3);
							green = hex2int(byte);

							splitString(byte, cmd,4,5);
							blue = hex2int(byte);

							for(led_pos=0;led_pos<MAX_PIXEL;led_pos++){
								set_WS2812B(led_pos, red, green,blue); // RED
							}
							sei();	// enable interrupts
  					 	    show_WS2812B();
							cli();	// enable interrupts

                        }
                        continue;
                 }
         }
	}
	return 0;
}

