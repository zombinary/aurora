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
#include <string.h>
#include <stdlib.h>
#include <avr/interrupt.h>

#include "eth_lcd_tcp-2.10/enc28j60.h"
#include "eth_lcd_tcp-2.10/ip_arp_udp_tcp.h"
//#include "eth_lcd_tcp-2.10/lcd.h"
#include "eth_lcd_tcp-2.10/net.h"
#include "eth_lcd_tcp-2.10/timeout.h"

#include "ws2812b/light_ws2812.h"
//#include "wrapper/wrapper.h"
#include "lib/pixel.h"

#define MAX_PIXEL 188

//tcp settings
static uint8_t mac[6] = {0x70,0x65,0x6e,0x69,0x73,0x68};
static uint8_t ip[4] = {10,11,0,104};
static uint16_t port = 80; // listen port for tcp/www (max range 1-254)

#define BUFFER_SIZE 750
static uint8_t buf[BUFFER_SIZE+1];
#define STR_BUFFER_SIZE 22

struct cRGB led[MAX_PIXEL];   //led[0].r=16;led[0].g=16;led[0].b=16;  white

int main(void){
  uint16_t plen;
  uint8_t dat_p;
  uint8_t cmd = 0;
  uint16_t x = 0;


  initPixel(MAX_PIXEL);
  // init strip - create an array with length of ledstrip (pixel)
  for(x=MAX_PIXEL; x>0; x--)
  {
      led[x-1].r=0;led[x-1].g=0;led[x-1].b=0;
  }

   	/*initialize enc28j60*/
    enc28j60Init(mac);
    enc28j60clkout(2); // change clkout from 6.25MHz to 12.5MHz
    _delay_loop_1(50); // 12ms
    enc28j60PhyWrite(PHLCON,0x476);
    _delay_loop_1(50); // 12ms

    /* lcd display: */
    //lcd_init(LCD_DISP_ON);
    //lcd_clrscr();
    //lcd_puts_P("aurora");
    init_ip_arp_udp_tcp(mac,ip,port);

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
         if (buf[IP_PROTO_P]==IP_PROTO_TCP_V&&buf[TCP_DST_PORT_H_P]==0&&buf[TCP_DST_PORT_L_P]==port){
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

                             if (strncmp("GET ",(char *)&(buf[dat_p]),4) == 0){
                            	 plen=fill_tcp_data_p(buf,0,PSTR("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n<h1>aurora	</h1>"));
                            	 make_tcp_ack_from_any(buf); // send ack for http get
                                 make_tcp_ack_with_data(buf,plen); // send data

                             }else{
                            	 cmd = buf[dat_p+2];
                            	 switch(cmd){
					                   case 0x10:
                            	 	   //CMD_INFO
	                            	 	 plen=setDeviceInfo_res(buf);
	                            	    break;
				 	                   case 0x11:
	                            	 	 //CMD_SATUS
	                            	 	 plen=setDeviceStatus_res(buf);
        	                    	   break;
                             case 0x12:
                              		 //CMD_CLEAR
                            	 		 clearPixel((uint8_t *)&led);
	                            	   plen=setClearPixel_res(buf);
        	                    	   break;
                           	 case 0x13:
                            	 		 //CMD_SETPIXEL
                            		 	 setPixel((uint8_t *)&led, (uint8_t *)&buf[dat_p]);
                            		 	 plen=setPixel_res(buf);
                            	 		 break;
                         	 	 case 0x14:
                            	 		//CMD_SETRANGE
                            	 		setRange((uint8_t *)&led, (uint8_t *)&buf[dat_p]);
                            	 		plen=setRange_res(buf);
                            	 		break;
	                       	 	 case 0x15:
                        	 		   //CMD_SETCOLOR
                            	 		setColor((uint8_t *)&led, (uint8_t *)&buf[dat_p]);
                            	 		plen=setColor_res(buf);
                            	 		break;
                            default:
                            	 		 plen=setError_res(buf, 0x01);
                            	 		 break;

                            	 }
                           }
                             make_tcp_ack_from_any(buf); // haelt den socket offe
                             //make_tcp_ack_with_data(buf,plen); // send data und schliesst den socket
                             continue;
                          }
                        }
                        continue;
                 }
         }
  }
  return 0;
}
