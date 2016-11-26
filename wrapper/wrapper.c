/*********************************************
 * wrapper.c
 * author: zombinary
 * copyright: GPL V2
 * see http://www.gnu.org/licenses/gpl.html
 *
 * decode tcp hexstring for ws2812b
 *********************************************/

#include <stdio.h>
#include <string.h>

/* split a string
 * char *c;
 * splitString(c, "hello",1,3);
 * c ==> "ell"
 */
void splitString(char *rslt, char *str,uint8_t start, uint8_t end){
  char *ptr = &str[start];
  uint8_t i=0;
  uint8_t count = 0;

 for(i=start;i<=end;i++){
   if(*ptr == '\0'){break;}
   rslt[count] = *ptr;
   ptr++;
   count++;
  }
  rslt[(end-start)+1] = '\0';
  return;
}
/* convert hex string to integer */
uint8_t hex2int(char *hex) {
	uint8_t val = 0;
    while (*hex) {
    	uint8_t byte = *hex++;
        if (byte >= '0' && byte <= '9') byte = byte - '0';
        else if (byte >= 'a' && byte <='f') byte = byte - 'a' + 10;
        else if (byte >= 'A' && byte <='F') byte = byte - 'A' + 10;
        val = (val << 4) | (byte & 0xF);
    }
    printf("val: %i\n", val);
    return val;
}
