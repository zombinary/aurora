aurora v0.0.4
============ 

Control ws2812b pixel with an atmege328p.
The tcp stack based on the tuxgraphic tcp stack (http://tuxgraphics.org/electronics/).

You can change the color of the pixel with an simple tcp message, like a hex string.
Use an individual tcp-server or node-red.
Also see my other project named 'sphere'. This is a nodejs based http-server to 
control several aurora server.

Externals
============ 
 * light_ws2812 V2.4 (light_ws2812_AVR)
 * eth_lcd_tcp-2.10

Hardware
================
 
 * atmega328p nano board
 * enc28J60 nano Ethernet shield
 * ws2812b 300pxl
 * lcd - 2x16 hd44780
 
wiring
---------------------

| __**atmega328p**__         | __**enc28J60**__      | __**lcd 2x16**__          | __**ws2812b**__      |
|----------------------------|-----------------------|---------------------------|----------------------|
|                            |                       |                           |                      |
| PD2                        |                       | PIN4  - RS (v0.0.1 only)  |                      |
| PD3                        |                       | PIN6  - E  (v0.0.1 only)  |                      |
| PD4                        |                       | PIN11 - D4 (v0.0.1 only)  |                      |
| PD5                        |                       | PIN12 - D5 (v0.0.1 only)  |                      |
| PD6                        |                       | PIN13 - D6 (v0.0.1 only)	 |                      |
| PD7                        |                       | PIN14 - D7 (v0.0.1 only)	 |                      |
| PD8                        |                       |                           | DIN                  |
|                            |                       |                           |                      |
| PD10                       | SS                    |                           |                      |
| PD11                       | MOSI                  |                           |                      |
| PD12                       | MISO                  |                           |                      |
| PD12                       | SCK                   |                           |                      |
|                            |                       |                           |                      |



Getting started
================

 You can use the make file after git clone.
 The default ipAddress is 10.11.0.100, change this in aurora.c
 `static uint8_t myip[4] = {10,11,0,100};      //10.11.0.100`

 gcc version 4.8.2 (GCC) 

API description:
---------------------

	
# standard request:
| __**byte**__  | __**description**__   |
|---------------|-----------------------|
| 0-1           | 4	(low byte first)	  |
| 2             | CMD								    |
| 3             | port								  |


# error request:

| __**byte**__  | __**description**__   |
|---------------|-----------------------|
| 0-1           | 4	(low byte first)	  |
| 2             | 0xff								  |
| 3             | error code					  |


# commands

# clearPixel:

clear the internal buffer of led matrix

| __**byte**__  | __**description**__ 	    |
|---------------|---------------------------|
| 0-1           | 3	length (low byte first) |
| 2             | CMD_CLEARPIXEL = 0x12	    |


# setColor:

set the color of all pixel. 
Pixels hardcoded as define 'MAX_PIXEL'.

| __**byte**__  | __**description**__ 	    |
|---------------|---------------------------|
| 0-1           | 7	length (low byte first) |
| 2             | CMD_SETCOLOR = 0x15	      |
| 3             | PIN									      |
| 4             | color value red 		      |
| 5             | color value green 	      |
| 6             | color value blue		      |

# setPixel:

set the color of an specific pixel.
 
| __**byte**__  | __**description**__   |
|---------------|-----------------------|
| 0-1           | 9	(low byte first)	  |
| 2             | CMD_SETPIXEL = 0x13	  |
| 3             | PIN									  |
| 4-5           | pixel (low byte first)|
| 6             | color value red 		  |
| 7             | color value green 	  |
| 8             | color value blue		  |

# setRange:

set the color of an specific range.
 
| __**byte**__  | __**description**__   |
|---------------|-----------------------|
| 0-1           | 10	(low byte first)  |
| 2             | CMD_SETRANGE = 0x14	  |
| 3             | PIN									  |
| 4-5           | first pixel (LBGF)    |
| 6-7           | last pixel (LBGF)     |
| 8             | color value red 		  |
| 9             | color value green 	  |
| 10            | color value blue		  |

controller
---------------------

 * [sphere](https://github.com/zombinary/sphere), browser based gui
 * [gravity-aurora](https://github.com/zombinary/gravity-aurora), npm package
 

# Supported Hardware


| __**mcu**__                | __**controller**__    |__**versiont**__           |                           |
|----------------------------|-----------------------|---------------------------|---------------------------|
|                    	     |                       |                           |                           |
| atmega328p                 | ws2812b (300pxl)      |  v0.0.1                   |  :white_check_mark:       |
|                            |      		     |  v0.0.2                   |  :white_check_mark:       |

Road-map
=========
  
  * * add digital input/output


