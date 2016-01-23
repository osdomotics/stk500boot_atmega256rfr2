#!/bin/bash

# Bernhard Trinnes <bernhard.trinnes@guh.guru>
# based on the work of Osdomotics

BASH_GREEN="\e[1;32m"
BASH_RED="\e[1;31m"
BASH_NORMAL="\e[0m"

function printGreen() {
    echo -e "${BASH_GREEN}$1${BASH_NORMAL}"
}

function printRed() {
    echo -e "${BASH_RED}$1${BASH_NORMAL}"
}

if [ "$UID" -ne 0 ]; then
    printRed "start the script as root"
    exit
fi


echo -n -e "${BASH_GREEN}Enter MAC Address>${BASH_NORMAL}"
read mac
make clean

PS3='Please enter your platform: '
options=("Merkur Board" "guhRF" "RaspBee" "Quit")
select opt in "${options[@]}"
do
    case $opt in
        "Merkur Board")
            printGreen "you chose Merkur Board"
            TARGET='osd'
            break
            ;;
        "guhRF")
            printGreen "you chose guhRF"
            TARGET='guhRF'
            break
            ;;
        "RaspBee")
            printGreen "you chose RaspBee"
            TARGET='RaspBee'
            break
            ;;
        "Quit")
            exit $?
            break
            ;;
        *) printRed "invalid option";;
    esac
done

printGreen "Building with Target: ${TARGET}"
make ${TARGET} EUI64_ADDRESS="$mac"
printGreen "start flashing"
sudo make flash
