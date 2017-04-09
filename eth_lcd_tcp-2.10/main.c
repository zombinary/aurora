/*********************************************
 * vim:sw=8:ts=8:si:et
 * To use the above modeline in vim you must have "set modeline" in your .vimrc
 * Author: Guido Socher
 * Copyright: GPL V2
 * See http://www.gnu.org/licenses/gpl.html
 *
 * Ethernet remote device with LCD display
 * HTTP interface 
        url looks like this http://10.0.0.25/q&pw=secret&cmd=0
        or http://10.0.0.25/q&pw=secret&cmd=1 
        or http://10.0.0.25/q&pw=secret&cmd=2&l1=Hello&l2=You
 *
 * http://tuxgraphics.org/electronics/
 * Note: there is a version number in the text. Search for "tuxgraphics"
 *********************************************/
#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include <avr/pgmspace.h>
#include "ip_arp_udp_tcp.h"
#include "enc28j60.h"
#include "timeout.h"
#include "avr_compat.h"
#include "net.h"
#include "lcd.h"

// please modify the following two lines. mac and ip have to be unique
// in your local area network. You can not have the same numbers in
// two devices:
static uint8_t mymac[6] = {0x54,0x55,0x58,0x10,0x00,0x65};
// how did I get the mac addr? Translate the first 3 numbers into ascii is: TUX
static uint8_t myip[4] = {10,0,0,25};
// base url (you can put a DNS name instead of an IP addr. if you have
// a DNS server (baseurl must end in "/"):
static char baseurl[]="http://10.11.0.101/";
static uint16_t mywwwport =80; // listen port for tcp/www (max range 1-254)
// or on a different port:
//static char baseurl[]="http://10.0.0.24:88/";
//static uint16_t mywwwport =88; // listen port for tcp/www (max range 1-254)
//

#define BUFFER_SIZE 750
static uint8_t buf[BUFFER_SIZE+1];
#define STR_BUFFER_SIZE 22
static char strbuf[STR_BUFFER_SIZE+1];

// the password string (only the first 5 char checked), (only a-z,0-9,_ characters):
static char password[]="secret"; // must not be longer than 9 char

// 
uint8_t verify_password(char *str)
{
        // a simple password/cookie:
        if (strcmp(password,str)==0){
                return(1);
        }
        return(0);
}

// search for a string of the form key=value in
// a string that looks like q?xyz=abc&uvw=defgh HTTP/1.1\r\n
//
// The returned value is stored in the global var strbuf
uint8_t find_key_val(char *str,char *key)
{
        uint8_t found=0;
        uint8_t i=0;
        char *kp;
        kp=key;
        while(*str &&  *str!=' ' && found==0){
                if (*str == *kp){
                        kp++;
                        if (*kp == '\0'){
                                str++;
                                kp=key;
                                if (*str == '='){
                                        found=1;
                                }
                        }
                }else{
                        kp=key;
                }
                str++;
        }
        if (found==1){
                // copy the value to a buffer and terminate it with '\0'
                while(*str &&  *str!=' ' && *str!='&' && i<STR_BUFFER_SIZE){
                        strbuf[i]=*str;
                        i++;
                        str++;
                }
                strbuf[i]='\0';
        }
        return(found);
}

// convert a single hex digit character to its integer value
unsigned char h2int(char c)
{
        if (c >= '0' && c <='9'){
                return((unsigned char)c - '0');
        }
        if (c >= 'a' && c <='f'){
                return((unsigned char)c - 'a' + 10);
        }
        if (c >= 'A' && c <='F'){
                return((unsigned char)c - 'A' + 10);
        }
        return(0);
}

// decode a url string e.g "hello%20joe" or "hello+joe" becomes "hello joe" 
void urldecode(char *urlbuf)
{
        char c;
        char *dst;
        dst=urlbuf;
        while ((c = *urlbuf)) {
                if (c == '+') c = ' ';
                if (c == '%') {
                        urlbuf++;
                        c = *urlbuf;
                        urlbuf++;
                        c = (h2int(c) << 4) | h2int(*urlbuf);
                }
                *dst = c;
                dst++;
                urlbuf++;
        }
        *dst = '\0';
}

// takes a string of the form q?pw=secret&cmd=1
// or q?pw=secret&cmd=2&l1=line+one&l2=two and analyse it
// The command number is 3 if the lcd data should be written.
// 
// return values: -1 invalid password, -2 no password
//                -3 no command given but password valid
//                any other number= command number
int8_t analyse_cmd(char *str)
{
        int8_t r=-3;
        if (find_key_val(str,"pw")){
                if (verify_password(strbuf)==0){
                        return(-1);
                }
        }else{
                // no pw found
                return(-2);
        }
        if (find_key_val(str,"cmd")){
                if (*strbuf < 0x3a && *strbuf > 0x2f){
                        // is a ASCII number, return it
                        r=(*strbuf-0x30);
                }
        }
        // cmd=1/0 is for the relay
        // cmd=2 is for the lcd display, read the l1 and l2 strings
        if (r==2){
                // read the lcd lines
                lcd_clrscr();
                if (find_key_val(str,"l1")){
                        urldecode(strbuf);
                        lcd_puts(strbuf);
                }
                if (find_key_val(str,"l2")){
                        lcd_gotoxy(0,1);
                        urldecode(strbuf);
                        lcd_puts(strbuf);
                }
        }
        return(r);
}

// prepare the login webpage by writing the data to the tcp send buffer
uint16_t print_loginpage(uint8_t *buf)
{
        uint16_t plen;
        plen=fill_tcp_data_p(buf,0,PSTR("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n"));
        plen=fill_tcp_data_p(buf,plen,PSTR("<center><form METHOD=\"get\" action=\""));
        plen=fill_tcp_data(buf,plen,baseurl);
        plen=fill_tcp_data_p(buf,plen,PSTR("q\">\npassword: <input size=8 type=\"password\" name=\"pw\"><input type=submit value=\"ok\"></form></center>"));
        return(plen);
}

// prepare the webpage by writing the data to the tcp send buffer
uint16_t print_webpage(uint8_t *buf,uint8_t on_off)
{
        uint16_t plen;
        plen=fill_tcp_data_p(buf,0,PSTR("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n"));
        plen=fill_tcp_data_p(buf,plen,PSTR("<center><p>Output is: "));
        if (on_off){
                plen=fill_tcp_data_p(buf,plen,PSTR("<font color=\"#00FF00\"> ON </font>"));
        }else{
                plen=fill_tcp_data_p(buf,plen,PSTR("OFF"));
        }
        plen=fill_tcp_data_p(buf,plen,PSTR(" <a href=\""));
        plen=fill_tcp_data(buf,plen,baseurl);
        plen=fill_tcp_data_p(buf,plen,PSTR("q?pw="));
        plen=fill_tcp_data(buf,plen,password);
        plen=fill_tcp_data_p(buf,plen,PSTR("\">[refresh status]</a></p>\n<p><a href=\""));
        // the url looks like this http://baseurl/q?pw=secret&cmd=1
        plen=fill_tcp_data(buf,plen,baseurl);
        plen=fill_tcp_data_p(buf,plen,PSTR("q?pw="));
        plen=fill_tcp_data(buf,plen,password);
        if (on_off){
                plen=fill_tcp_data_p(buf,plen,PSTR("&cmd=0\">Switch off</a><p>"));
        }else{
                plen=fill_tcp_data_p(buf,plen,PSTR("&cmd=1\">Switch on</a><p>"));
        }
        plen=fill_tcp_data_p(buf,plen,PSTR("<hr><br><form METHOD=get action=\""));
        plen=fill_tcp_data(buf,plen,baseurl);
        plen=fill_tcp_data_p(buf,plen,PSTR("q\">\n<input size=20 type=text name=l1>\n<br><input size=20 type=text name=l2>\n<input type=hidden name=cmd value=2>\n<input type=hidden name=pw value=\""));
        plen=fill_tcp_data(buf,plen,password);
        plen=fill_tcp_data_p(buf,plen,PSTR("\">\n<br><input type=submit value=\"write to LCD\"></form>\n"));
        plen=fill_tcp_data_p(buf,plen,PSTR("</center><hr><br>version 2.10, tuxgraphics.org\n"));
        return(plen);
}


int main(void){

        
        uint16_t plen;
        uint16_t dat_p;
        uint8_t i=0;
        int8_t cmd;
        
        // set the clock speed to "no pre-scaler" (8MHz with internal osc or 
        // full external speed)
        // set the clock prescaler. First write CLKPCE to enable setting of clock the
        // next four instructions.
        CLKPR=(1<<CLKPCE); // change enable
        CLKPR=0; // "no pre-scaler"
        _delay_loop_1(50); // 12ms
        /* enable PB0, reset as output */
        //DDRB|= (1<<DDB0);

        /* enable PD2/INT0, as input */
        DDRD&= ~(1<<DDD2);

        
        /*initialize enc28j60*/
        enc28j60Init(mymac);
        enc28j60clkout(2); // change clkout from 6.25MHz to 12.5MHz
        _delay_loop_1(50); // 12ms
        

        // LED
        /* enable PB1, LED as output */
        DDRB|= (1<<DDB1);

        /* set output to Vcc, LED off */
        PORTB|= (1<<PORTB1);

        // the transistor on PD7
        DDRD|= (1<<DDD7);
        PORTD &= ~(1<<PORTD7);// transistor off
        
        /* Magjack leds configuration, see enc28j60 datasheet, page 11 */
        // LEDB=yellow LEDA=green
        //
        // 0x476 is PHLCON LEDA=links status, LEDB=receive/transmit
        // enc28j60PhyWrite(PHLCON,0b0000 0100 0111 01 10);
        enc28j60PhyWrite(PHLCON,0x476);
        _delay_loop_1(50); // 12ms
        
        /* set output to GND, red LED on */
        PORTB &= ~(1<<PORTB1);
        i=1;

        // lcd display:
        lcd_init(LCD_DISP_ON);
        lcd_clrscr();
        lcd_puts_P("=ok=");
        //init the ethernet/ip layer:
        init_ip_arp_udp_tcp(mymac,myip,mywwwport);

        while(1){
                // get the next new packet:
                plen = enc28j60PacketReceive(BUFFER_SIZE, buf);

                /*plen will ne unequal to zero if there is a valid 
                 * packet (without crc error) */
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
                // led----------
                if (i){
                        /* set output to Vcc, LED off */
                        PORTB|= (1<<PORTB1);
                        i=0;
                }else{
                        /* set output to GND, LED on */
                        PORTB &= ~(1<<PORTB1);
                        i=1;
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
                                if (strncmp("GET ",(char *)&(buf[dat_p]),4)!=0){
                                        // head, post and other methods:
                                        //
                                        // for possible status codes see:
                                        // http://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html
                                        plen=fill_tcp_data_p(buf,0,PSTR("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n<h1>200 OK</h1>"));
                                        goto SENDTCP;
                                }
                                if (strncmp("/ ",(char *)&(buf[dat_p+4]),2)==0){
                                        plen=print_loginpage(buf);
                                        goto SENDTCP;
                                }
                                cmd=analyse_cmd((char *)&(buf[dat_p+5]));
                                // for possible status codes see:
                                // http://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html
                                if (cmd==-1){
                                        plen=fill_tcp_data_p(buf,0,PSTR("HTTP/1.0 401 Unauthorized\r\nContent-Type: text/html\r\n\r\n<h1>401 Unauthorized</h1>"));
                                        goto SENDTCP;
                                }
                                if (cmd==-2){
                                        plen=print_loginpage(buf);
                                        goto SENDTCP;
                                }
                                if (cmd==1){
                                        PORTD|= (1<<PORTD7);// transistor on
                                }
                                if (cmd==0){
                                        PORTD &= ~(1<<PORTD7);// transistor off
                                }
                                // just display the web-form:
                                plen=print_webpage(buf,(PORTD & (1<<PORTD7)));
                                //
SENDTCP:
                                make_tcp_ack_from_any(buf); // send ack for http get
                                make_tcp_ack_with_data(buf,plen); // send data
                                continue;
                        }

                }
                // tcp port www end
                //
                // udp is not supported
        }
        return (0);
}
