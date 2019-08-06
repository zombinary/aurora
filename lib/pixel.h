#ifndef PIXEL_H
#define PIXEL_H
	void initPixel(uint16_t maxpixel);
	void clearPixel(uint8_t* led);
	void setPixel(uint8_t *led, uint8_t *data);
	void setColor(uint8_t *led, uint8_t *data);
	void setRange(uint8_t *led, uint8_t *data);

	uint16_t setClearPixel_res(uint8_t *buf);
	uint16_t setPixel_res(uint8_t *buf);
	uint16_t setColor_res(uint8_t *buf);
	uint16_t setRange_res(uint8_t *buf);
	uint16_t setError_res(uint8_t *buf, uint8_t errCode);
#endif
