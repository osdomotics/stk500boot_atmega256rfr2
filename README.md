# stk500boot_atmega256rfr2

Changes to the standard STK500 bootloader to support the ATMega256RFR2
(used by the guhRF) 

To write the bootloader to the target typ:

./flash.sh
make fuse

# Building

To build the firmware for the RaspBee 

```
make raspbee
```
To build the firmware for guhRF modules

```
make guhRF
```

To write the firmware to the board.

```
make flash
```


# License
Licensed under the GPLv2 license.

