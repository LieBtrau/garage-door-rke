# Features
* Remote and controller share the same firmware.  Differentiation is done based on compile time parameter and #define structure.

# Library for wireless module
* [RadioLib](https://github.com/jgromes/RadioLib/wiki)
    * Support for some ham-radio protocols like AX.25
    * Limited higher layer2 support (no encapsulation of packets into frames, no flow control, no retransmissions)
* [Arduino-LoRa](https://github.com/sandeepmistry/arduino-LoRa)
    * It inherits from the Stream class, which will make it easier to interface it to other libraries such as [PacketIO](https://packetio.readthedocs.io/en/latest/).
* [RadioHead](http://www.airspayce.com/mikem/arduino/RadioHead/)
    * Supports acknowledgements on frames and re-transmissions, but no support for encapsulation of packets into frames.  The application has to split packets into frames and make sure these arrive.
    * SX1278 : no FSK support, only LoRa.
    * I refuse to add more modules to this library, as Mike McMauley heavily based his MRF89XA driver [on my code](https://github.com/LieBtrau/RadioHead/blob/010059f01bc5f790ef788f355999595e51d3f9b8/RH_MRF89XA.cpp) without owing me credit for it.  Removing a copyright notice in a source code file is not a nice thing to do.
    * If you're not using the default SPI-pins, you'll have to patch RHHardwareSpi.cpp
* [LowPowerLab](https://github.com/LowPowerLab/RFM69)
    * RFM69/SX1231 only
    * OSI-layer2 support thrown together with hardware driver.  This would take some time to separate that nicely.
