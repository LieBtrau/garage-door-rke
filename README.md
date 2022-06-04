# garage-door-rke
Remote keyless entry for garage door

Todo:
- measure power supply voltage using cyclotron 


MCU:
* ESP32-C3
 * cheap
 * low power
 * available in stock
 * Lolin C3-MINI : 
   * default SPI pins (GPIO14-17) are not connected.
   * WiFi doesn't work (LDO too weak?)
 
* Seeeduino XIAO
 * internal 3V3 not sufficient to power RFM69HW.  So there's no more point in using the Xiao.  If an external 3V3 LDO is needed, I might as well use a cheaper and smaller ESP32-C3 module.  The ESP32-C3 might serve me better in the future as well, as it has bluetooth and wifi.
 
* 328Board 
 * https://www.tindie.com/products/kdcircuits/328-board-atmega328p-breakout/
 * https://www.kevindarrah.com/wiki/index.php?title=328Board
 
* bare atmega328pb
 * not available anywhere
 
Radio
* RFM69HW
 * RST must be kept low (unconnected at module)
 * at least 22uF buffer capacitor is needed on 3V3
 
Connection to Hörmann controller
* UTP-cable assembly (about the cheapest and versatile multi-conductor cable one can find)
* RJ45-terminal socket adapter : https://www.adafruit.com/product/4981
