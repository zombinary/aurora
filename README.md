aurora
==========

Control ws2812b pixel with an atmege328p.
The tcp stack based on the tuxgraphic tcp stack (http://tuxgraphics.org/electronics/).

You can change the color of the pixel with an simple tcp message, like a hex string.
Use an individual tcp-server or node-red.
Also see my other project named 'sphere'. This is a nodejs based http-server to 
control several aurora server.


Hardware
================
 
 * atmega328p nano board
 * enc28J60 nano Ethernet shield
 * ws2812b strip 30

Getting started
================

send tcp response
---------------------

    send hex string to enabel the leds on the rgb string
    "ff00ff" ==> RGB

tcp controller
--------------

 * [sphere](https://github.com/zombinary/sphere)
 

## Supported controller


| __**mcu**__                | __**controller**__    |                           |
|----------------------------|-----------------------|---------------------------|
|  		                        |                       |                           |
|  atmega328p                | ws2812b (30pxl)       |  :white_check_mark:       |
|                            |		                     |                           |

Road-map
=========

  * more pixel min. 255
  * dhcp or non static ip-address
  * expand tcp message to controll single pixel


