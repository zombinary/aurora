#include <stdio.h>
#include <string.h>
#include "../ws2812b/light_ws2812.h"
#include "../eth_lcd_tcp-2.10/net.h"

uint16_t setDeviceInfo_res(uint8_t *buf){
	uint8_t *d = buf;
	uint8_t response[] = {0x04,0x00,0x15,*(d+3)};
	uint8_t pos = 0;

	while (pos < 4) {
		buf[TCP_CHECKSUM_L_P+3+pos]=response[pos];
		pos++;
	}
	return pos;
}

uint16_t setDeviceStatus_res(uint8_t *buf){
	uint8_t *d = buf;
	uint8_t response[] = {0x04,0x00,0x15,*(d+3)};
	uint8_t pos = 0;

	while (pos < 4) {
		buf[TCP_CHECKSUM_L_P+3+pos]=response[pos];
		pos++;
	}
	return pos;
}