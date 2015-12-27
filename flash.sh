#!/bin/bash

# Bernhard Trinnes <bernhard.trinnes@guh.guru>
# based on the work of Osdomotics
echo -n "Enter MAC ADDRESS>"
read mac
make clean
make raspbee EUI64_ADDRESS="$mac"
sudo make flash
