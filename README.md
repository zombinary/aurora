aurora v0.0.2
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

send tcp response
---------------------

    send color value as hex  to enabel the pixel.
    {0xff,0xff,0xff} ==> RGB (color white full light intensity)

tcp controller
---------------------

 * [sphere](https://github.com/zombinary/sphere)
 

## Supported controller


| __**mcu**__                | __**controller**__    |__**versiont**__           |                           |
|----------------------------|-----------------------|---------------------------|---------------------------|
|                    	     |                       |                           |                           |
| atmega328p                 | ws2812b (300pxl)      |  v0.0.1                   |  :white_check_mark:       |
|                            |      		     |  v0.0.2                   |  :white_check_mark:       |

Road-map
=========
  
  * dhcp or non static ip-address
  * add API to use light effects (flash, smooth, chaserlight, ....)
  * add switch mode to control second string on another port
  * add digital input/output


