# garage-door-rke
Remote keyless entry for garage door

## Problem
1. The Hörmann HS 5 868-BS remote is too heavy.
2. When you forget the remote, you can't open the garage door from the outside.

## Solution

### Hardware
Design files in [EasyEDA](https://oshwlab.com/liebtrau/garage-door-rke)

#### Remote
A light-weight remote has been developed.  It contains :
1. An RFM95W-868S2 LoRa transceiver : set to minimal TX-power, uses very little current.
2. ESP-C3-13U-4M module : available and easy to solder, and includes wifi (not used on the remote).

#### Controller
The controller is mounted near the Hörmann garage door controller, inside the garage.  It consists of:
1. The same PCB transceiver as in the remote (RFM95 + ESP-C3).  So that design only needed to be done once.
2. A 24V to 5V power PCB.  This PCB has an over-sized power supply so that it can also be used in other projects, e.g. for powering a Raspberry Pi.
  * This design was more of an exercise in power supply design than being really practical.
  * Much smaller supplies exist (TPSM84209).
  * If you're after the least effort, then use an 7805 or one of its switching drop-in replacements.
3. A wifi-antenna, connected to the ESP-C3-13U-4M module.

The controller is mounted near the opening of the garage door to increase wireless range.  A 3m UTP-cable connects to an RJ45-terminal socket adapter from [Adafruit](https://www.adafruit.com/product/4981) or [AliExpress](https://www.aliexpress.com/item/1005001493014437.html), which makes connection to the terminal blocks of the Hörmann garage door interface.

### Software

#### LoRa - Authentication

Authentication is based on [Kryptoknight](https://github.com/LieBtrau/arduino-kryptoknight) which is based on a shared secret key between both parties.  It's a light-weight protocol that could also be used on 8-bit MCUs.
This protocol is not vulnerable to rolljam attacks, which is a problem in [rolling code](https://en.wikipedia.org/wiki/Rolling_code) schemes commonly used for this application.

#### WiFi

The controller also includes a web-interface.  By browsing to its fixed local IP-address, a webpage on your smartphone opens.  If you type the correct pin code, the garage door will open.
Security of this web-interface relies on the secrecy of your wifi password and the secret pin.
