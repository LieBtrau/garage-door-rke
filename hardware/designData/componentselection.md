# MCU

## STM32WLE5WJC (includes LoRa radio)
### Modules
* [RAK3172](https://store.rakwireless.com/products/wisduo-lpwan-module-rak3172?variant=40014759297222)
* [Seeed Wio-E5 STM32WLE5JC Module](https://wiki.seeedstudio.com/LoRa-E5_STM32WLE5JC_Module/)
### Firmware development
* no support for PlatformIO yet
* [LoRa-E5-mini](https://revspace.nl/LoRa-E5-mini) : Arduino toolchain

## ESP32-C3
* cheap
* low power
* available in stock
* PlatformIO support
### Modules
* [Lolin C3-MINI](https://www.wemos.cc/en/latest/c3/c3_mini.html) : 
    * default SPI pins (GPIO14-17) are not connected.
    * WiFi doesn't work (LDO too weak?)
* ESP32-C3-WROOM-02U-N4
    * Small
    * available
    * cheap
    * powerful enough
    * low power
## Seeeduino XIAO
* PlatformIO support
* internal 3V3 not sufficient to power RFM69HW.  So there's no more point in using the Xiao.  If an external 3V3 LDO is needed, I might as well use a cheaper and smaller ESP32-C3 module.  The ESP32-C3 might serve me better in the future as well, as it has bluetooth and wifi.

## ATmega328
* PlatformIO support
### 328Board 
* https://www.tindie.com/products/kdcircuits/328-board-atmega328p-breakout/
* https://www.kevindarrah.com/wiki/index.php?title=328Board
### bare atmega328pb
* not available anywhere
 
# Radio
## Frequency : 868MHz
Smaller antennas than 433MHz, better building penetration than 2.4GHz.  Deployment in EU, so no 915MHz.
![HopeRF modules versions](../docs/rfm-cheat-sheet.png)
Due to limited space, we'll use the smaller modules : 
* RFM69HCW-868S2 (limited availability)
* RFM95W-868S2 (available at Antratek)
## RFM69HW (used for testing)
* rebranded SX1231 radio
* cheaper than LoRa variant
* Be careful about these HopeRF modules.  There are wide and narrow parts.  The RFM69HW is a wide part.
* RST must be kept low (unconnected at module)
* at least 22uF buffer capacitor is needed on 3V3

# Housing
Ingun needle box.
Internal height = 9mm
## Bottom side
* Li-Ion : 4.7mm high
* JST PH SM4 : 5.5mm high (can be removed)
## PCB
* 1.6mm thick -> can be as low as 0.6mm
## top side
### Push button
* On R1: PTS645SK50SMTR92LFS : 3.75mm high, top is 5mm high, which is at least 1.5mm too low to push in.
* On R2 : 
    * CUI TS04 : 3.5mm + height of studs (unknown)
    * E-switch TL3305 : height not defined
    * E-switch TL3301 : 4.0mm
    * C&K PTS647 : 3.2mm high, 4.5 x 4.5 mm footprint
    * *C&K PTS645 PTS645SK70SMTR92LFS (=260gf)*, PTS645SL70SMTR92LFS (=130gf) : 3.75mm
    * Schurter LSG : 3.9mm high
    * APEM ADTSM : 4.1mm
    * APEM MJTP : 4.0mm
    * Wuerth WS-TASV : 3.4mm + undefined studs height
    
 

