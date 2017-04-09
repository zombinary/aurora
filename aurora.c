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

#include <stdint.h>
#include <avr/interrupt.h>

#include "eth_lcd_tcp-2.10/enc28j60.h"
#include "eth_lcd_tcp-2.10/ip_arp_udp_tcp.h"
//#include "eth_lcd_tcp-2.10/lcd.h"
#include "eth_lcd_tcp-2.10/net.h"
#include "eth_lcd_tcp-2.10/timeout.h"

#include "ws2812b/light_ws2812.h"
#include "wrapper/wrapper.h"

#define MAX_PIXEL 60

//tcp settings
static uint8_t mymac[6] = {0x54,0x55,0x58,0x10,0x00,0x65};
static uint8_t myip[4] = {10,11,0,101};
static uint16_t mywwwport =80; // listen port for tcp/www (max range 1-254)

#define BUFFER_SIZE 750
static uint8_t buf[BUFFER_SIZE+1];
#define STR_BUFFER_SIZE 22

struct cRGB led[2];   //led[0].r=16;led[0].g=16;led[0].b=16;  white

int main(void){
  uint16_t plen;
  uint8_t dat_p;

  uint16_t pxl=0;
  uint16_t i;

   	/*initialize enc28j60*/
    enc28j60Init(mymac);
    enc28j60clkout(2); // change clkout from 6.25MHz to 12.5MHz
    _delay_loop_1(50); // 12ms
    enc28j60PhyWrite(PHLCON,0x476);
    _delay_loop_1(50); // 12ms

    /* lcd display: */
    //lcd_init(LCD_DISP_ON);
    //lcd_clrscr();
    //lcd_puts_P("aurora");
    init_ip_arp_udp_tcp(mymac,myip,mywwwport);

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

         // tcp port start, compare only the lower byte
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

                          led[0].r = buf[dat_p];
                          led[0].g = buf[++dat_p];
                          led[0].b = buf[++dat_p];

                          cli();
                          for (i=0; i<(MAX_PIXEL-pxl); i++){
                       	   ws2812_sendarray((uint8_t *)&led[0],3);
                          }
                          sei();

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

                             //if (strncmp("GET ",(char *)&(buf[dat_p]),4)!=0){
                                 //plen=fill_tcp_data_p(buf,0,PSTR("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n<h1>200 aurora</h1>"));
                             //}else if(0){
                             //	 //write hier aurora light functions
                             //}

                             if (strncmp("GET ",(char *)&(buf[dat_p]),4)!=0){
                            	 plen=fill_tcp_data_p(buf,0,PSTR("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n<h1>200 aurora</h1>"));
                             }else{

                             }


                             plen=fill_tcp_data_p(buf,0,PSTR("unicorn"));

                             make_tcp_ack_from_any(buf); // send ack for http get
                             make_tcp_ack_with_data(buf,plen); // send data
                             //continue;
                          }
                        }
                        continue;
                 }
         }
  }
  return 0;
}
