#include <stdio.h>
#include <string.h>
#include "../ws2812b/light_ws2812.h"
#include "../eth_lcd_tcp-2.10/net.h"

uint16_t MAXpxl = 0;

//void read_aurora_command(char *request){
//	char buf[] = {0x75,0x6e,0x69,0x63,0x6f,0x72,0x6e,0x00};
//	strcpy (request, buf);
//	return;
//
//};

void initPixel(uint16_t maxpixel){
	return MAXpxl = maxpixel;
}

void clearPixel(uint8_t *led){
	uint16_t l;
	uint8_t *ptr = led;
	for (l=MAXpxl*3; l>0;l--){
		*ptr = 0x00;
		ptr++;
	}
	return;
}
uint16_t setClearPixel_res(uint8_t *buf){
	uint8_t response[] = {0x03,0x00,0x12};
	uint8_t pos = 0;

	while (pos < 4) {
		buf[TCP_CHECKSUM_L_P+3+pos]=response[pos];
		pos++;
	}
	return pos;
}

void setRange(uint8_t *led, uint8_t *data){
	uint8_t *d = data;
	uint16_t start = *(d+5);
			 start = (start << 8) | *(d+4);
	uint16_t end = *(d+7);
			 end = (end << 8) | *(d+6);
	uint8_t	 port = (1<<*(d+3));
	uint8_t *ptr = led;
	uint16_t l = 0;
	uint16_t pos = 0;

	for (l=MAXpxl; l>0;l--){
		if(pos>=start && pos<=end){
		//if(l<end && l>start){
			*ptr = *(d+9);	//green
			ptr++;
			*ptr = *(d+8);	//red
			ptr++;
			*ptr = *(d+10); //blue
			ptr++;

		}else{
			ptr+=3;
		}
		pos++;
	}

	cli();
		//ws2812_sendarray(led,MAXpxl*3);
		ws2812_sendarray_mask(led,MAXpxl*3,port);
	sei();
	return;
}

uint16_t setRange_res(uint8_t *buf){
	uint8_t *d = buf;
	uint8_t response[] = {0x04,0x00,0x14,*(d+3)};
	uint8_t pos = 0;

	while (pos < 4) {
		buf[TCP_CHECKSUM_L_P+3+pos]=response[pos];
		pos++;
	}
	return pos;
}

void setPixel(uint8_t *led, uint8_t *data){
	// CMD_SETPIXEL
	uint8_t *d = data;
	uint16_t pixelpos = *(d+5);
			 pixelpos = (pixelpos << 8) | *(d+4);

	uint8_t	 port = (1<<*(d+3));
	uint16_t pos = 0;
	uint8_t *ptr = led;

	while(pos<pixelpos){
		ptr+=3; // +3 = RGB value
		pos++;
	}

	*ptr   = *(d+7); // green value from data ptr
	ptr++;
	*ptr = *(d+6); // red value from data ptr
	ptr++;
	*ptr = *(d+8); // blue value from data ptr

	cli();
		//ws2812_sendarray(led,MAXpxl*3);
		ws2812_sendarray_mask(led,MAXpxl*3,port);
	sei();
  return;
}

uint16_t setPixel_res(uint8_t *buf){
	//uint8_t request[] = {0x75,0x6e,0x69,0x63,0x6f,0x72,0x6e,0x00}; //unicorn
	uint8_t *d = buf;
	uint8_t response[] = {0x04,0x00,0x13,*(d+3)};
	uint8_t pos = 0;
	while (pos < 4) {
		buf[TCP_CHECKSUM_L_P+3+pos]=response[pos];
		pos++;
	}
	return pos;
}

void setColor(uint8_t *led, uint8_t *data){
	uint8_t *d = data;
	uint8_t	port = (1<<*(d+3));
	uint16_t pos = 0;
	uint8_t *ptr = led;

	while(pos<MAXpxl){
		*ptr   = *(d+5); // green value from data ptr
		ptr++;
		*ptr = *(d+4); // red value from data ptr
		ptr++;
		*ptr = *(d+6); // blue value from data ptr
		ptr++;
		pos++;
	}
	cli();
		//ws2812_sendarray(led,MAXpxl*3);
		ws2812_sendarray_mask(led,MAXpxl*3,port);	//0x01 = PB0 //_BV(3) => (1 >> 3) = 0x08;
	sei();
	return;
}

uint16_t setColor_res(uint8_t *buf){
	uint8_t *d = buf;
	uint8_t response[] = {0x04,0x00,0x15,*(d+3)};
	uint8_t pos = 0;

	while (pos < 4) {
		buf[TCP_CHECKSUM_L_P+3+pos]=response[pos];
		pos++;
	}
	return pos;
}

uint16_t setError_res(uint8_t *buf, uint8_t errCode){
	uint8_t response[] = {0x04,0x00,0xff,errCode};
	uint8_t pos = 0;
	while (pos < 4) {
		buf[TCP_CHECKSUM_L_P+3+pos]=response[pos];
		pos++;
	}
	return pos;
}

