# garage-door-rke
Remote keyless entry for garage door

Todo:
- measure power supply voltage using cyclotron 


MCU:
* ESP32-C3
 * cheap
 * low power
 * available in stock
 
* Seeeduino XIAO
 * internal 3V3 not sufficient to power RFM69HW
 
* 328Board 
 * https://www.tindie.com/products/kdcircuits/328-board-atmega328p-breakout/
 * https://www.kevindarrah.com/wiki/index.php?title=328Board
 
* bare atmega328pb
 * not available anywhere
 
Radio
* RFM69HW
 * RST must be kept low (unconnected at module)
