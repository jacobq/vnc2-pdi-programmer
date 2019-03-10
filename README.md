# vnc2-pdi-programmer

:warning: work in progress

## Introduction

This program demonstrates a way to load compiled [AVR][avr-home] code onto  from a file (`avr-app.bin`) on a USB stick
attached to an [FTDI VNC2][vnc-home] with some of its I/O pins connected to the AVR MCU's [PDI][sigrok-pdi] lines.
I am making this project to help a colleague.


## Hardware connections

PDI requires 2 signals (clock & data + GND), but the data signal is bidirection.
This project follows the example illustrated in the [AVR1612 application note][AVR1612-pdf]
in that it connects two external pins (thru isolating resistors) to the PDI data signal.
This simplifies initial development and also might make it possible to eventually replace
the ["bit banging"][bit-banging-wiki] approach with hardware peripherals, e.g. [SPI][spi-wiki]
(see section 6.4 of [the VNC2 datasheet][vnc-datasheet]).


| VNC2 name | VNC2 pin | VNC2 function  | Name    | Comments           |
|-----------|----------|----------------|---------|--------------------|
| IO8       | 29       | GPIO B0 output | PDI_CLK |                    |
| IO9       | 30       | GPIO B1 output | PDI_TX  |                    |
| IO10      | 31       | GPIO B2 input  | PDI_RX  |                    |


## Design overview



## Other notes



## License

This is an open source project, licensed under the terms of the "MIT" license, as stated in the [`LICENSE`](./LICENSE) file.
Note, however, that in order to function, it relies on third party products that are not necessarily open source.
For example, this project depends on the libraries provided by FTDI to support reading FAT file system data from a USB mass storage device. 
The toolchain distributed by FTDI also appears to [have its own license terms][vnc-ide-license].
Although the author believes this project to be in compliance with all applicable license terms,
it is ultimately the user's responsibilty to check that their usage is not in violation.



[avr-home]: https://www.microchip.com/design-centers/8-bit/avr-mcus
[avr-wiki]: https://en.wikipedia.org/wiki/AVR_microcontrollers
[AVR1612]: https://www.microchip.com/wwwAppNotes/AppNotes.aspx?appnote=en591295
[AVR1612-pdf]: https://ww1.microchip.com/downloads/en/AppNotes/doc8282.pdf
[sigrok-pdi]: https://sigrok.org/wiki/Protocol_decoder:Avr_pdi

[bit-banging-wiki]: https://en.wikipedia.org/wiki/Bit_banging
[spi-wiki]: https://en.wikipedia.org/wiki/Serial_Peripheral_Interface

[vnc-home]: https://www.ftdichip.com/Products/ICs/VNC2.htm
[vnc-datasheet]: https://www.ftdichip.com/Support/Documents/DataSheets/ICs/DS_Vinculum-II.pdf
[vnc-ide]: https://www.ftdichip.com/Firmware/VNC2tools.htm#VNC2Toolchain
[vnc-ide-license]: https://www.ftdichip.com/Firmware/vnc2toolchain/License.txt
